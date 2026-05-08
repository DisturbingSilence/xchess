#include "graphics.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include "embedded_files.h"
#include "gfx_common.h"
#include "utils.h"

static i32 compile_shader(const char* src,u32 type,u32* out_shader)
{
	u32 shader = glCreateShader(type);
	glShaderSource(shader,1,&src,nullptr);
	glCompileShader(shader);

	i32 success;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
	if(!success)
	{
		i32 len = 512;
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&len);
		char* text_buffer = malloc(len + 1);
		glGetShaderInfoLog(shader,len,nullptr,text_buffer);
		text_buffer[len] = '\0';
		glDeleteShader(shader);
		LOG_WARNING(text_buffer);
		free(text_buffer);
		return -1;
	}
	if(out_shader) *out_shader = shader;
	return 0;
}
static i32 link_program(u32 program)
{
    glLinkProgram(program);
	i32 success;
	glGetProgramiv(program,GL_LINK_STATUS,&success);
	if(!success)
	{
		i32 len;
		glGetProgramiv(program,GL_INFO_LOG_LENGTH,&len);
		char* text_buffer = malloc(len + 1);
		glGetProgramInfoLog(program,len,nullptr,text_buffer);
		text_buffer[len] = '\0';
		LOG_WARNING(text_buffer);
		glDeleteProgram(program);
		return -1;
	}
	return 0;
}
[[maybe_unused]]static i32 compile_compute_pipeline(u32 comp,u32* out_program)
{
    u32 program = glCreateProgram();
	glAttachShader(program,comp);
	if(link_program(program)) return -1;
	if(out_program) *out_program = program;
	return 0;
}
static i32 compile_graphics_pipeline(u32 vert,u32 frag,u32* out_program)
{
	u32 program = glCreateProgram();
	glAttachShader(program,vert);
	glAttachShader(program,frag);
	if(link_program(program)) return -1;
	if(out_program) *out_program = program;
	return 0;
}
static i32 create_shaders(GraphicsContext* ctx)
{
    u32 frag,vert;
    i32 error = 0;
    {
        error += compile_shader(BOARD_VERTEX_SOURCE,GL_VERTEX_SHADER,&vert);
        error += compile_shader(BOARD_FRAGMENT_SOURCE,GL_FRAGMENT_SHADER,&frag);
        error += compile_graphics_pipeline(vert,frag,&ctx->pipelines[BOARD_PIPELINE]);
    }
    glDeleteShader(vert);glDeleteShader(frag);
    {
        error += compile_shader(UI_VERTEX_SOURCE,GL_VERTEX_SHADER,&vert);
        error += compile_shader(UI_FRAGMENT_SOURCE,GL_FRAGMENT_SHADER,&frag);
        error += compile_graphics_pipeline(vert,frag,&ctx->pipelines[UI_PIPELINE]);
    }
    glDeleteShader(vert);glDeleteShader(frag);
    {
        error += compile_shader(TEXT_VERTEX_SOURCE,GL_VERTEX_SHADER,&vert);
        error += compile_shader(TEXT_FRAGMENT_SOURCE,GL_FRAGMENT_SHADER,&frag);
        error += compile_graphics_pipeline(vert,frag,&ctx->pipelines[TEXT_PIPELINE]);
    }
    glDeleteShader(vert);glDeleteShader(frag);
    {
        error += compile_shader(FULLSCREEN_VERTEX_SOURCE,GL_VERTEX_SHADER,&vert);
        error += compile_shader(FULLSCREEN_FRAGMENT_SOURCE,GL_FRAGMENT_SHADER,&frag);
        error += compile_graphics_pipeline(vert,frag,&ctx->pipelines[FS_PIPELINE]);
    }
    glDeleteShader(vert);glDeleteShader(frag);
    return error;
}
static i32 create_textures(GraphicsContext* ctx)
{
    glCreateTextures(GL_TEXTURE_2D,1,&ctx->textures[BOARD_TEXTURE]);
    glTextureParameteri(ctx->textures[BOARD_TEXTURE],GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTextureParameteri(ctx->textures[BOARD_TEXTURE],GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTextureParameteri(ctx->textures[BOARD_TEXTURE],GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTextureParameteri(ctx->textures[BOARD_TEXTURE],GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    i32 width,height;
    u8* pieces_texture_raw = read_tga("../assets/pieces.tga",&width,&height);
    glTextureStorage2D(ctx->textures[BOARD_TEXTURE],1,GL_RGBA8,width,height);
    glTextureSubImage2D(ctx->textures[BOARD_TEXTURE],0,0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,pieces_texture_raw);
    free(pieces_texture_raw);
    return 1;
}
static void fill_ortho_matrix(float* m,float left,float right,float bottom,float top)
{
    for(i32 i = 0; i < 16; i++) m[i] = 0.0f;

    float near = -1,far = 1;
    float rl = right -  left;
    float tb = top - bottom;
    float fn = far -  near;

    m[ 0] =  2.0f / rl;
    m[ 5] =  2.0f / tb;
    m[10] = -2.0f / fn;
    m[12] = -(left + right) / rl;
    m[13] = -(top + bottom) / tb;
    m[14] = -(far + near)  / fn;
    m[15] = 1.0f;
}
static i32 create_buffers(GraphicsContext* ctx)
{
    glCreateBuffers(5,ctx->buffers);
    ctx->params.white_tile_color = PACK_COLOR(222,227,230,255);
    ctx->params.black_tile_color = PACK_COLOR(140,162,173,255);
    glNamedBufferStorage(ctx->buffers[SHADER_PARAMS_BUFFER],sizeof(ShaderParameters) + BOARD_SIZE_BYTES,&ctx->params,GL_DYNAMIC_STORAGE_BIT);

    float vertices[] = {0,0, 0,1, 1,0, 1,1};
    u8 indices[] = {0,1,3, 0,2,3};
    glNamedBufferStorage(ctx->buffers[VERTEX_BUFFER],sizeof(vertices),vertices,0);
    glNamedBufferStorage(ctx->buffers[INDEX_BUFFER],sizeof(indices),indices,0);
    glNamedBufferStorage(ctx->buffers[INSTANCE_BUFFER],sizeof(RectInstanceData) * UI_MAX_RECTS + sizeof(WordInstanceData) * MAX_GLYPHS,NULL,GL_DYNAMIC_STORAGE_BIT);
    return 0;
}
static void debug_callback(
    u32 source,u32 type,
    u32 id,u32 severity,
    i32 length,const char* message,const void* userParam)
{
    if (id == 131185) return; // buffer will use vram
    LOG_CRITICAL(message);
}
GraphicsContext* gfx_startup(GLFWwindow* win,u32 width,u32 height)
{
    if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        LOG_CRITICAL("Failed to initialize opengl");
        return NULL;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2,0.2,0.2,0.0);
    char text_buffer[100];
    glDebugMessageCallback(debug_callback, nullptr);
    sprintf(text_buffer,"Loaded opengl %s",glGetString(GL_VERSION));
    LOG_INFO(text_buffer);
    GraphicsContext* ctx = malloc(sizeof(GraphicsContext));
    *ctx = (GraphicsContext){0};
    ctx->mouse_x = ~0;
    ctx->mouse_y = ~0;
    ctx->window = win;
    create_shaders(ctx);
    create_buffers(ctx);
    create_textures(ctx);

    glCreateFramebuffers(1,&ctx->main_fbo);
    glCreateTextures(GL_TEXTURE_2D,1,&ctx->textures[FBO_TEXTURE]);
    glTextureStorage2D(ctx->textures[FBO_TEXTURE],1,GL_RGBA8,width,height);
    glNamedFramebufferTexture(ctx->main_fbo,GL_COLOR_ATTACHMENT0,ctx->textures[FBO_TEXTURE],0);
    glCreateVertexArrays(1,&ctx->vao);
    gfx_on_resize(ctx,width,height);
    return ctx;
}
void gfx_on_piece_move(GraphicsContext* ctx,const u32* board)
{
    glNamedBufferSubData(ctx->buffers[SHADER_PARAMS_BUFFER],sizeof(ShaderParameters),BOARD_SIZE_BYTES,board);
}
static void toolbar_callback(GraphicsContext* ctx)
{
    return;
}
void gfx_render(GraphicsContext* ctx)
{
    glBindFramebuffer(GL_FRAMEBUFFER,ctx->main_fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(ctx->vao);
    glUseProgram(ctx->pipelines[BOARD_PIPELINE]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,ctx->buffers[SHADER_PARAMS_BUFFER]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,ctx->buffers[VERTEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ctx->buffers[INDEX_BUFFER]);
    glBindTextureUnit(0,ctx->textures[BOARD_TEXTURE]);
    glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0,128);

    gfx_ui_begin_frame(ctx);
    float wh = ctx->params.height * 0.8f;
    uvec4 r = {0,wh,wh,ctx->params.height - wh};
    UI_WINDOW(ctx,"Evaluation",r,UI_FLAG_BORDER | UI_FLAG_TITLE) {}

    r = (uvec4){wh,50,ctx->params.width - wh,ctx->params.height};
    UI_WINDOW(ctx,"main_menu",r,UI_FLAG_BORDER)
    {
        r.x = r.z * 0.5 - 100;
        r.y = r.w * 0.5 - 60 + 100;
        r.z = 200;
        r.w = 60;
        if(UI_BUTTON(ctx,"Challenge friend",r))
        {
            LOG_INFO("Challenge friend[not implemented]");
        }
        r.y -= 100;
        if(UI_BUTTON(ctx,"Play against computer",r))
        {
            LOG_INFO("Play against computer[not implemented]");
        }
    }
    r = (uvec4){wh,0,ctx->params.width - wh,50};
    UI_TOOLBAR(ctx,r,PACK_COLOR(30,30,30,255),toolbar_callback)
    {
        r = (uvec4){ctx->params.width - wh - 50,5,40,40};
        if(UI_BUTTON(ctx,"X",r))
        {
            glfwSetWindowShouldClose(ctx->window,GLFW_TRUE);
        }
    }
    gfx_ui_end_frame(ctx);
    glUseProgram(ctx->pipelines[UI_PIPELINE]);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER,2,ctx->buffers[INSTANCE_BUFFER],0,sizeof(RectInstanceData) * UI_MAX_RECTS);
    glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0,ctx->ui.rect_count);

    glEnable(GL_BLEND);
    glUseProgram(ctx->pipelines[TEXT_PIPELINE]);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER,2,ctx->buffers[INSTANCE_BUFFER],sizeof(RectInstanceData) * UI_MAX_RECTS,sizeof(WordInstanceData) * MAX_GLYPHS);
    glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0,ctx->ui.word_count);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glClearColor(1,1,1,0);
    glBindTextureUnit(1,ctx->textures[FBO_TEXTURE]);
    glUseProgram(ctx->pipelines[FS_PIPELINE]);
    glUniform1f(1,glfwGetTime());
    glDrawArrays(GL_TRIANGLES,0,3);

    glDisable(GL_BLEND);
}
void gfx_on_resize(GraphicsContext* ctx,i32 width,i32 height)
{
    glViewport(0,0,width,height);
    ctx->params.width = width;
    ctx->params.height = height;

    glDeleteTextures(1,&ctx->textures[FBO_TEXTURE]);
    glCreateTextures(GL_TEXTURE_2D,1,&ctx->textures[FBO_TEXTURE]);
    glTextureStorage2D(ctx->textures[FBO_TEXTURE],1,GL_RGBA8,width,height);
    glNamedFramebufferTexture(ctx->main_fbo,GL_COLOR_ATTACHMENT0,ctx->textures[FBO_TEXTURE],0);

    float aspect = (float)width / height;
    float wh = 8/.8f;
    fill_ortho_matrix(ctx->params.proj_mat,0,wh * aspect,wh,0);
    fill_ortho_matrix(ctx->params.ui_proj_mat,0.f,width,height,0.f);
    glNamedBufferSubData(ctx->buffers[SHADER_PARAMS_BUFFER],0,sizeof(ShaderParameters),&ctx->params);
}
void gfx_on_mouse_click(GraphicsContext* ctx,i32 button,i32 action,i32 mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        ctx->mouse_button = 0;
    }
    else if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        ctx->mouse_button = 1;
    }
    else
    {
        return;
    }
    if (action == GLFW_RELEASE)
    {
        ctx->is_moving_window = 0;
    }

    if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // ...
    }
    ctx->mouse_action = action;
}
void gfx_on_mouse_move(GraphicsContext* ctx,double x,double y)
{

    if (ctx->is_moving_window)
    {
        int winX, winY;
        glfwGetWindowPos(ctx->window, &winX, &winY);

        int deltaX = (int)(x - ctx->mouse_x);
        int deltaY = (int)(y - ctx->mouse_y);

        glfwSetWindowPos(ctx->window, winX + deltaX, winY + deltaY);

        return;
    }
    ctx->mouse_x = x;
    ctx->mouse_y = y;
}
void gfx_shutdown(GraphicsContext* ctx)
{
    if(!ctx) return;
    free(ctx);
}
