#ifndef XCHESS_GFX_COMMON_H
#define XCHESS_GFX_COMMON_H
#include "graphics.h"

#define BOARD_SIZE_BYTES (8 * 8 * sizeof(u32))

#define UNIQUE(name) (name ## __LINE__)
#define UI_WINDOW(ctx,title,rect,flags)\
    for(char UNIQUE(_guard) = 1,UNIQUE(_open) = _impl_begin_window(ctx,title,rect,flags);\
    UNIQUE(_guard) && UNIQUE(_open);\
    (UNIQUE(_guard)=0))

#define UI_TOOLBAR(ctx,rect,color,clbk)\
    for(char UNIQUE(_guard) = 1,UNIQUE(_open) = _impl_begin_toolbar(ctx,rect,color,clbk);\
    UNIQUE(_guard) && UNIQUE(_open);\
    (UNIQUE(_guard)=0))

#define UI_BUTTON(ctx,title,rect) _impl_button(ctx,title,rect)

typedef struct
{
    uvec4 pos;
    u32 color;
    u32 _pad[3];
} RectInstanceData;

typedef struct
{
    float pos_and_scale[4];
    u32 word;
    u32 color;
    u32 d[2];
} WordInstanceData;

typedef struct ShaderParameters
{
    u32 white_tile_color;
    u32 black_tile_color;
    float width;
    float height;
    float proj_mat[16];
    float ui_proj_mat[16];
} ShaderParameters;

enum
{
    UI_FLAG_BORDER = (1 << 0),
    UI_FLAG_TITLE = (1 << 1)
};

// indices into ctx->buffers[]
#define SHADER_PARAMS_BUFFER 0
#define VERTEX_BUFFER 1
#define INDEX_BUFFER 2
#define INSTANCE_BUFFER 3

// indices into ctx->pipelines[]
#define BOARD_PIPELINE 0
#define UI_PIPELINE 1
#define TEXT_PIPELINE 2
#define FS_PIPELINE 3

// indices into ctx->textures[]
#define BOARD_TEXTURE 0
#define FBO_TEXTURE 1

// general
#define UI_MAX_RECTS 1024
#define MAX_NODES 2048
#define MAX_ROOT_NODES 8
#define MAX_GLYPHS_PER_NODE 64
#define MAX_GLYPHS 1024

typedef enum
{
    UNODE_TYPE_WINDOW,
    UNODE_TYPE_BUTTON,
    UNODE_TYPE_TOOLBAR
} UINodeType;
typedef struct
{
    i32 parent;
    i32 first_child;
    i32 next_sibling;
    i32 last_sibling;
} UINode;
typedef struct
{
    uvec4 rect;
    char text[MAX_GLYPHS_PER_NODE];
} ButtonNodeData;
typedef struct
{
    uvec4 rect;
    uvec4 content_rect;
    char title[MAX_GLYPHS_PER_NODE];
    u32 flags;
} WindowNodeData;
typedef void(*TOOLBAR_CALLBACK_FUN)(GraphicsContext* ctx);
typedef struct
{
    uvec4 rect;
    u32 color;
    TOOLBAR_CALLBACK_FUN callback;
} ToolbarNodeData;
typedef struct
{
    u32 id;
    UINodeType type;
    union
    {
        ButtonNodeData button;
        WindowNodeData window;
        ToolbarNodeData toolbar;
    } data;
} UINodeData;
typedef struct
{
    u32 text_color;
    u32 text_height;
    struct
    {
        u8 edge_width;
        u32 edge_color;
        u32 bg_color;
        u32 title_bg_color;
        u32 title_fg_color;
        u32 title_height;
    } window;
    struct
    {
        u32 color;
        u32 hover_color;
    } button;
} UIStyle;
struct GLFWwindow;
struct GraphicsContext
{
    struct GLFWwindow* window;

    // gl data
    u32 vao;
    u32 pipelines[5];
    u32 textures[2];
    u32 buffers[5];
    u32 main_fbo;

    // mouse data
    double mouse_x,mouse_y;
    double anchor_x,anchor_y;
    u8 mouse_button : 1; // 0 - lmb, 1 - rmb
    u8 mouse_action : 1; // 0 - release,1 - press
    u8 last_mouse_action : 1;// same as previous
    u8 is_moving_window : 1;
    u8 is_resizing_window : 1;
    u8 _pad : 3;
    struct
    {
        u32 root_nodes[MAX_ROOT_NODES];
        u8 root_node_count;
        UINode nodes[MAX_NODES];
        u32 node_count;
        i32 current_parent;
        UINodeData node_data[MAX_NODES];

        RectInstanceData rect_instances[UI_MAX_RECTS];
        u32 rect_count;
        WordInstanceData text_instances[MAX_GLYPHS];
        u32 word_count;

        uvec2 cursor;
        u32 active_item;
        u32 hot_item;
    } ui;
    ShaderParameters params;
};
void gfx_ui_begin_frame(GraphicsContext* ctx);
void gfx_ui_end_frame(GraphicsContext* ctx);
i32 _impl_begin_toolbar(GraphicsContext* ctx,uvec4 rect,u32 color,TOOLBAR_CALLBACK_FUN clbck);
i32 _impl_button(GraphicsContext* ctx,const char* text,uvec4 rect);
i32 _impl_begin_window(GraphicsContext* ctx,const char* title,uvec4 rect,u32 flags);
#endif
