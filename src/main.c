#include <stdio.h>
#include "graphics.h"
#include <GLFW/glfw3.h>
#include "logger.h"

#define row 8
#define column 8

// Зручно винести номери фігур в окремі константи
enum {
    TYPE_EMPTY = 0,
    TYPE_PAWN = 1,
    TYPE_KNIGHT = 2,
    TYPE_BISHOP = 3,
    TYPE_ROOK = 4,
    TYPE_QUEEN = 5,
    TYPE_KING = 6
};

typedef enum : unsigned int
{
    EMPTY = 0,

    WHITE_PAWN   = (TYPE_PAWN << 1) | 1,
    WHITE_KNIGHT = (TYPE_KNIGHT << 1) | 1,
    WHITE_BISHOP = (TYPE_BISHOP << 1) | 1,
    WHITE_ROOK   = (TYPE_ROOK << 1) | 1,
    WHITE_QUEEN  = (TYPE_QUEEN << 1) | 1,
    WHITE_KING   = (TYPE_KING << 1) | 1,

    BLACK_PAWN   = (TYPE_PAWN << 1) | 0,
    BLACK_KNIGHT = (TYPE_KNIGHT << 1) | 0,
    BLACK_BISHOP = (TYPE_BISHOP << 1) | 0,
    BLACK_ROOK   = (TYPE_ROOK << 1) | 0,
    BLACK_QUEEN  = (TYPE_QUEEN << 1) | 0,
    BLACK_KING   = (TYPE_KING << 1) | 0,
} ChessPiece;

ChessPiece A[64] = {
    BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK,
    BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
    WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK
};

char getPieceChar(ChessPiece p) {
    if (p == EMPTY) return '.';
    int type = (p >> 1);
    int isBlack = p & 1;

    char c;
    switch(type) {
        case TYPE_PAWN:
            c = 'P';
            break;
        case TYPE_KNIGHT:
            c = 'N';
            break;
        case TYPE_BISHOP:
            c = 'B';
            break;
        case TYPE_ROOK:
            c = 'R';
            break;
        case TYPE_QUEEN:
            c = 'Q';
            break;
        case TYPE_KING:
            c = 'K';
            break;
        default:
            return '-';
    }
    return c;
}

void out()
{
    printf("\n   ");
    for(char i='a'; i<='h'; i++) printf("%c ", i);
    printf("\n");

    for(int i=0; i<row; i++)
    {
        printf("%i  ", 8-i);
        for(int j=0; j<column; j++)
        {
            printf("%c ", getPieceChar(A[i*row+j]));
        }
        printf(" %i\n", 8-i);
    }

    printf("   ");
    for(char i='a'; i<='h'; i++) printf("%c ", i);
    printf("\n\n");
}
static void resize_callback(GLFWwindow* window,int width,int height)
{
    GraphicsContext* ctx = (GraphicsContext*)glfwGetWindowUserPointer(window);
    graphics_on_resize(ctx,width,height);
}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800,800,"XCHESS",NULL,NULL);
    if(!window)
    {
        LOG_CRITICAL("Failed to create window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,resize_callback);
    GraphicsContext* gfx_ctx = graphics_startup();
    glfwSetWindowUserPointer(window,gfx_ctx);
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // here will be game logic

        graphics_on_piece_move(gfx_ctx,A);
        graphics_render(gfx_ctx);
        glfwSwapBuffers(window);
    }
    graphics_shutdown(gfx_ctx);
    glfwDestroyWindow(window);
    glfwTerminate();
}
