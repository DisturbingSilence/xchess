#include "gfx_common.h"
#include "graphics.h"
#include <glad/glad.h>
#include <assert.h>
#include <string.h>
#include "utils.h"

static UIStyle DEFAULT_STYLE =
{
    .text_color = PACK_COLOR(255,255,255,255),
    .text_height = 32,
    .window =
    {
        .edge_width     = 2,
        .edge_color     = PACK_COLOR(19,39,36,255),
        .bg_color       = PACK_COLOR(22,23,27,255),
        .title_bg_color = PACK_COLOR(31,33,37,255),
        .title_fg_color = PACK_COLOR(255,255,255,255),
        .title_height   = 36
    },
    .button =
    {
        .color = PACK_COLOR(34,154,115,255),
        .hover_color = PACK_COLOR(24, 122, 153,255)
    }
};
static i32 point_rect_collision(uvec4 rect,float px,float py)
{
    return
    (px >= rect.x && px <= rect.x + rect.z) &&
    (py >= rect.y && py <= rect.y + rect.w);
}
static void add_text(GraphicsContext* ctx,const char* text,uvec3* pos_and_scale,u32 color)
{
    u64 len = strlen(text);
    int num_words = (len + 3) / 4;
    for(int i = 0; i < num_words; i++)
    {
        if(ctx->ui.word_count < MAX_GLYPHS)
        {
            u32 word = 0;
            u64 offset = i * 4;
            u64 bytes_to_copy = (len - offset >= 4) ? 4 : (len - offset);
            memcpy(&word,text + offset,bytes_to_copy);
            ctx->ui.text_instances[ctx->ui.word_count++] = (WordInstanceData){
                .word = word,
                .pos_and_scale = {
                    pos_and_scale->z * i + pos_and_scale->x,
                    pos_and_scale->y,
                    pos_and_scale->z},
                .color = color
            };
        }
    }
}
static void add_rect(GraphicsContext* ctx,uvec4* rect,u32 color)
{
    if (ctx->ui.rect_count < UI_MAX_RECTS)
    {
        ctx->ui.rect_instances[ctx->ui.rect_count++] = (RectInstanceData){
            .pos = *rect,
            .color = color
        };
    }
}
static i32 add_ui_node(GraphicsContext* ctx,i32 parent)
{
    const i32 node = ctx->ui.node_count++;
    assert(node < MAX_NODES && "node overflow");
    ctx->ui.nodes[node] = (UINode){.parent = parent,.first_child = -1,.next_sibling = -1,.last_sibling = -1};
    if (parent > -1)
    {
        const i32 s = ctx->ui.nodes[parent].first_child;
        if (s == -1)
        {
            ctx->ui.nodes[parent].first_child = node;
            ctx->ui.nodes[node].last_sibling = node;
        }
        else
        {
            i32 dest = ctx->ui.nodes[s].last_sibling;
            if (dest <= -1)
            {
                dest = s;
                while(ctx->ui.nodes[dest].next_sibling != -1) dest = ctx->ui.nodes[dest].next_sibling;
            }
            ctx->ui.nodes[dest].next_sibling = node;
            ctx->ui.nodes[s].last_sibling = node;
        }
    }
    else
    {
        assert(ctx->ui.root_node_count < MAX_ROOT_NODES && "root node overflow");
        ctx->ui.root_nodes[ctx->ui.root_node_count++] = node;
    }
    return node;
}
i32 _impl_begin_toolbar(GraphicsContext* ctx,uvec4 rect,u32 color,TOOLBAR_CALLBACK_FUN clbk)
{
    u32 id = FNV("TOOLBAR",0);
    i32 node_id = add_ui_node(ctx,ctx->ui.current_parent);
    UINodeData* node = &ctx->ui.node_data[node_id];
    node->id = id;
    node->type = UNODE_TYPE_TOOLBAR;
    node->data.toolbar = (ToolbarNodeData){
        .rect = rect,
        .color = color,
        .callback = clbk
    };
    ctx->ui.current_parent = node_id;
    return 1;
}
void _impl_end_toolbar(GraphicsContext* ctx)
{
    if(ctx->ui.current_parent != -1)
    {
        ctx->ui.current_parent = ctx->ui.nodes[ctx->ui.current_parent].parent;
    }
}
void process_ui_tree(GraphicsContext* ctx,i32 node_id)
{
    if (node_id == -1) return;
    UINodeData* node = &ctx->ui.node_data[node_id];
    if(node->type == UNODE_TYPE_WINDOW)
    {
        uvec4 bg_rect;
        if(node->data.window.flags & UI_FLAG_BORDER)
        {
            uvec4* rect = &node->data.window.rect;
            add_rect(ctx,rect,DEFAULT_STYLE.window.edge_color);
            bg_rect =(uvec4){
                rect->x + DEFAULT_STYLE.window.edge_width,
                rect->y + DEFAULT_STYLE.window.edge_width,
                rect->z - (DEFAULT_STYLE.window.edge_width * 2),
                rect->w - (DEFAULT_STYLE.window.edge_width * 2)
            };
        }
        else
        {
            bg_rect = node->data.window.rect;
        }
        add_rect(ctx,&bg_rect,DEFAULT_STYLE.window.bg_color);
        bg_rect.w = DEFAULT_STYLE.window.title_height;
        if(node->data.window.flags & UI_FLAG_TITLE)
        {
            add_rect(ctx,&bg_rect,DEFAULT_STYLE.window.title_bg_color);
            uvec3 pos_and_scale = {bg_rect.x + 5,bg_rect.y,bg_rect.w};
            add_text(ctx,node->data.window.title,&pos_and_scale,DEFAULT_STYLE.window.title_fg_color);
        }
        ctx->ui.cursor = (uvec2){node->data.window.content_rect.x,node->data.window.content_rect.y};
    }
    else if(node->type == UNODE_TYPE_BUTTON)
    {
        if(point_rect_collision(node->data.button.rect,ctx->mouse_x, ctx->mouse_y))
        {
            ctx->ui.hot_item = node->id;
            if(ctx->mouse_action == 1 && ctx->last_mouse_action == 0)
            {
                ctx->ui.active_item = node->id;
            }
        }
        u64 len = strlen(node->data.button.text);
        uvec4* rect = &node->data.button.rect;
        uvec3 text_pos = {rect->x + (rect->z - len * 8)/2,rect->y + (rect->w - DEFAULT_STYLE.text_height)/2,DEFAULT_STYLE.text_height};
        add_rect(ctx,rect,PACK_COLOR(10,10,10,255));
        rect->x -= 5;
        rect->y -= 5;
        add_rect(ctx,rect,ctx->ui.hot_item == node->id ? DEFAULT_STYLE.button.hover_color : DEFAULT_STYLE.button.color);
        add_text(ctx,node->data.button.text,&text_pos,DEFAULT_STYLE.text_color);
    }
    else if(node->type == UNODE_TYPE_TOOLBAR)
    {
        if (point_rect_collision(node->data.toolbar.rect, ctx->mouse_x, ctx->mouse_y))
        {
            if (ctx->mouse_action == 1 && ctx->last_mouse_action == 0)
            {
                ctx->is_moving_window = 1;
            }
        }
        add_rect(ctx, &node->data.toolbar.rect, node->data.toolbar.color);
    }
    process_ui_tree(ctx,ctx->ui.nodes[node_id].first_child);
    process_ui_tree(ctx,ctx->ui.nodes[node_id].next_sibling);
}
void gfx_ui_begin_frame(GraphicsContext* ctx)
{
    ctx->ui.node_count = 0;
    ctx->ui.root_node_count = 0;
    ctx->ui.current_parent = -1;
    ctx->ui.rect_count = 0;
    ctx->ui.word_count = 0;
}
void gfx_ui_end_frame(GraphicsContext* ctx)
{
    if(ctx->ui.node_count == 0) return;
    ctx->ui.hot_item = 0;
    ctx->ui.active_item = 0;
    for (i32 i = 0;i < ctx->ui.root_node_count;i++) process_ui_tree(ctx,ctx->ui.root_nodes[i]);

    u64 offset = 0;
    glNamedBufferSubData(ctx->buffers[INSTANCE_BUFFER],offset,ctx->ui.rect_count * sizeof(RectInstanceData),ctx->ui.rect_instances);
    offset += UI_MAX_RECTS * sizeof(RectInstanceData);
    glNamedBufferSubData(ctx->buffers[INSTANCE_BUFFER],sizeof(RectInstanceData) * UI_MAX_RECTS,ctx->ui.word_count * sizeof(WordInstanceData),ctx->ui.text_instances);
    ctx->last_mouse_action = ctx->mouse_action;
}
i32 _impl_button(GraphicsContext* ctx,const char* text,uvec4 rect)
{
    u32 id = FNV(text,ctx->ui.current_parent);
    i32 node_id = add_ui_node(ctx,ctx->ui.current_parent);
    if(ctx->ui.current_parent != -1)
    {
        rect.x += ctx->ui.cursor.x;
        if(ctx->ui.node_data[ctx->ui.current_parent].type == UNODE_TYPE_WINDOW)
        {

            rect.y += ctx->ui.cursor.y;
        }
    }
    UINodeData* node = &ctx->ui.node_data[node_id];
    node->id = id;
    node->type = UNODE_TYPE_BUTTON;
    node->data.button.rect = rect;
    strncpy(node->data.button.text,text,63);
    node->data.button.text[63] = '\0';
    return ctx->ui.active_item == id;
}
i32 _impl_begin_window(GraphicsContext* ctx,const char* title,uvec4 rect,u32 flags)
{
    u32 id = FNV(title,0);
    i32 node_id = add_ui_node(ctx,ctx->ui.current_parent);
    UINodeData* node = &ctx->ui.node_data[node_id];
    node->id = id;
    node->type = UNODE_TYPE_WINDOW;
    node->data.window = (WindowNodeData){
        .rect = rect,
        .content_rect = (uvec4){
            rect.x + 10,rect.y + (flags & UI_FLAG_TITLE ? DEFAULT_STYLE.window.title_height : 0) + 10,
            rect.z - 10,rect.w - (flags & UI_FLAG_TITLE ? DEFAULT_STYLE.window.title_height : 0) + 10
        },
        .flags = flags
    };
    ctx->ui.current_parent = node_id;
    strncpy(node->data.window.title,title,63);
    node->data.window.title[63] = '\0';
    return 1;
}
void _impl_end_window(GraphicsContext* ctx)
{
    if(ctx->ui.current_parent != -1)
    {
        ctx->ui.current_parent = ctx->ui.nodes[ctx->ui.current_parent].parent;
    }
}
