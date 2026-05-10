#include "graphics.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include "embedded_files.h"

#define BOARD_SIZE_BYTES (8 * 8 * sizeof(unsigned int))
typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;
typedef struct
{
    Color white_tile_color;
    Color black_tile_color;
    float resolution_x;
    float resolution_y;
} BoardParameters;
#define BOARD_PARAMS_BUFFER 0
#define VERTEX_BUFFER 1
#define INDEX_BUFFER 2
struct GraphicsContext
{
    unsigned int vao;
    unsigned int pipelines[5];
    unsigned int textures;
    unsigned int buffers[3];

    BoardParameters board_params;
};
static int compile_shader(const char* src,unsigned int type,unsigned int* out_shader)
{
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader,1,&src,nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
	if(!success)
	{
		int len = 512;
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
static int link_program(unsigned int program)
{
    glLinkProgram(program);
	int success;
	glGetProgramiv(program,GL_LINK_STATUS,&success);
	if(!success)
	{
		int len;
		glGetProgramiv(program,GL_INFO_LOG_LENGTH,&len);
		char* text_buffer = malloc(len + 1);
		glGetProgramInfoLog(program,len,nullptr,text_buffer);
		text_buffer[len] = '\0';
		LOG_WARNING(text_buffer);
		glDeleteProgram(program);
		return 0;
	}
	return 1;
}
static int compile_compute_pipeline(unsigned int comp,unsigned int* out_program)
{
    unsigned int program = glCreateProgram();
	glAttachShader(program,comp);
	if(!link_program(program)) return -1;
	if(out_program) *out_program = program;
	return 0;
}
static int compile_graphics_pipeline(unsigned int vert,unsigned int frag,unsigned int* out_program)
{
	unsigned int program = glCreateProgram();
	glAttachShader(program,vert);
	glAttachShader(program,frag);
	if(!link_program(program))  return -1;
	if(out_program) *out_program = program;
	return 0;
}
static int create_shaders(GraphicsContext* ctx)
{
    unsigned int board_vert,board_frag,figures_compute;
    int error = 0;
    error += compile_shader(BOARD_VERTEX_SOURCE,GL_VERTEX_SHADER,&board_vert);
    error += compile_shader(BOARD_FRAGMENT_SOURCE,GL_FRAGMENT_SHADER,&board_frag);
    error += compile_graphics_pipeline(board_vert,board_frag,&ctx->pipelines[0]);
    return error;
}
static int create_textures(GraphicsContext* ctx)
{
    glCreateTextures(GL_TEXTURE_2D,1,&ctx->textures);
    glTextureParameteri(ctx->textures,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTextureParameteri(ctx->textures,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTextureParameteri(ctx->textures,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTextureParameteri(ctx->textures,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    int width,height;
    unsigned char* pieces_texture_raw = read_tga("../assets/pieces.tga",&width,&height);
    glTextureStorage2D(ctx->textures,1,GL_RGBA8,width,height);
    glTextureSubImage2D(ctx->textures,0,0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,pieces_texture_raw);
    free(pieces_texture_raw);
}
static int create_buffers(GraphicsContext* ctx)
{
    glCreateBuffers(3,ctx->buffers);
    ctx->board_params.white_tile_color = (Color){222,227,230,255};
    ctx->board_params.black_tile_color = (Color){140,162,173,255};
    glNamedBufferStorage(ctx->buffers[BOARD_PARAMS_BUFFER],sizeof(BoardParameters) + BOARD_SIZE_BYTES,&ctx->board_params,GL_DYNAMIC_STORAGE_BIT);

    float vertices[] = {0,0, 0,1, 1,0, 1,1};
    unsigned char indices[] = {0,1,3, 0,2,3};
    glNamedBufferStorage(ctx->buffers[VERTEX_BUFFER],sizeof(vertices),vertices,0);
    glNamedBufferStorage(ctx->buffers[INDEX_BUFFER],sizeof(indices),indices,0);
    return 0;
}
static void debug_callback(
    GLenum source,GLenum type,
    GLuint id,GLenum severity,
    GLsizei length,const char* message,const void* userParam)
{
    if (id == 131185) return; // buffer will use vram
    LOG_CRITICAL(message);
}
GraphicsContext* graphics_startup()
{
    if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        LOG_CRITICAL("Failed to initialize opengl");
        return NULL;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    char text_buffer[100];
    glDebugMessageCallback(debug_callback, nullptr);
    sprintf(text_buffer,"Loaded opengl %s",glGetString(GL_VERSION));
    LOG_INFO(text_buffer);
    GraphicsContext* ctx = malloc(sizeof(GraphicsContext));
    create_shaders(ctx);
    create_buffers(ctx);
    create_textures(ctx);
    glCreateVertexArrays(1,&ctx->vao);
    return ctx;
}
void graphics_on_piece_move(GraphicsContext* ctx,const unsigned int* board)
{
    glNamedBufferSubData(ctx->buffers[BOARD_PARAMS_BUFFER],sizeof(BoardParameters),BOARD_SIZE_BYTES,board);
}
void graphics_render(GraphicsContext* ctx)
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(ctx->vao);
    glUseProgram(ctx->pipelines[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,ctx->buffers[BOARD_PARAMS_BUFFER]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,ctx->buffers[VERTEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ctx->buffers[INDEX_BUFFER]);
    glBindTextureUnit(0,ctx->textures);
    glDrawElementsInstanced(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0,128);
}
void graphics_on_resize([[maybe_unused]]GraphicsContext* ctx,int width,int height)
{
    glViewport(0,0,width,height);
}
void graphics_shutdown(GraphicsContext* ctx)
{
    free(ctx);
}
unsigned char* read_tga(const char* filename,int* out_width,int* out_height)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    {
        char text_buffer[200];
        sprintf(text_buffer,"%s:failed to open",filename);
        LOG_WARNING(text_buffer);
        return NULL;
    }
    typedef struct __attribute__((packed))
    {
        unsigned char id_length;
        unsigned char color_map_type;
        unsigned char image_type;
        // color map specs
        unsigned short first_cmap_entry;
        unsigned short cmap_length;
        unsigned char cmap_bpp;
        // image specs
        unsigned short y_origin;
        unsigned short x_origin;
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char image_descriptor;
    } TGAHeader;
    TGAHeader header;
    fread(&header,sizeof(header),1,file);
    if(header.image_type < 2 || header.image_type > 3)
    {
        char text_buffer[200];
        sprintf(text_buffer,"%s:unsupported image type",filename);
        LOG_WARNING(text_buffer);
        return NULL;
    }
    unsigned int bytes_per_pixel = header.bpp / 8;
    unsigned int image_data_size = bytes_per_pixel * header.height * header.width;
    unsigned char* raw_image_data = malloc(image_data_size);
    fseek(file,header.id_length + header.cmap_length * (header.cmap_bpp / 8),SEEK_CUR);
    fread(raw_image_data,1,image_data_size,file);
    fclose(file);

    unsigned char* p_temp,*p_temp_2;
    // flip image on y axis
    if((header.image_descriptor & (1 << 5)) != 0)
    {
        for(int y = 0;y < header.height / 2 - 1;y++)
        {
            for(int x = 0;x < header.width;x++)
            {
                p_temp = &raw_image_data[(y * header.width + x) * bytes_per_pixel];
                p_temp_2 = &raw_image_data[((header.height - y - 1) * header.width + x) * bytes_per_pixel];
                for(int b = 0;b < bytes_per_pixel;b++)
                {
                    p_temp[b] ^= p_temp_2[b];
                    p_temp_2[b] ^= p_temp[b];
                    p_temp[b] ^= p_temp_2[b];
                }
            }
        }
    }
    if(out_width) *out_width = header.width;
    if(out_height) *out_height = header.height;
    return raw_image_data;
}
