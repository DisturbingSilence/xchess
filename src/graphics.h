#ifndef XCHESS_GRAPHICS_H
#define XCHESS_GRAPHICS_H
#include "common.h"

#define PACK_COLOR(r,g,b,a) ((u32)a << 24 | (u32)b << 16 | (u32)g << 8 | (u32)r)

typedef struct GraphicsContext GraphicsContext;
struct GLFWwindow;
GraphicsContext* gfx_startup(struct GLFWwindow* win,u32 width,u32 height);
void gfx_shutdown(GraphicsContext* ctx);
void gfx_render(GraphicsContext* ctx);
void gfx_on_piece_move(GraphicsContext* ctx,const unsigned int* A);

// for glfw callbacks
void gfx_on_resize(GraphicsContext* ctx,int width,int height);
void gfx_on_mouse_click(GraphicsContext* ctx,int button,int action,int mods);
void gfx_on_mouse_move(GraphicsContext* ctx,double x,double y);


typedef struct
{
    u32 x,y,z;
} uvec3;
typedef struct
{
    u32 x,y,z,w;
} uvec4;
typedef struct
{
    u32 x,y;
} uvec2;
#endif
