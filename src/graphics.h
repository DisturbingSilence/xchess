#ifndef XCHESS_GRAPHICS_H
#define XCHESS_GRAPHICS_H

typedef struct GraphicsContext GraphicsContext;
GraphicsContext* graphics_startup();
void graphics_shutdown(GraphicsContext* ctx);
void graphics_render(GraphicsContext* ctx);
void graphics_on_piece_move(GraphicsContext* ctx,const unsigned int* A);
void graphics_on_resize(GraphicsContext* ctx,int width,int height);

// standalone functions
unsigned char* read_tga(const char* filename,int* width,int* height);
#endif
