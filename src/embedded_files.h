#ifndef XCHESS_SHADERS_H
#define XCHESS_SHADERS_H
static const char BOARD_VERTEX_SOURCE[] =
R"(
#version 460
layout(location = 0) out flat uint tile_index;
layout(location = 1) out vec2 uv;
layout(std430, binding = 0) readonly buffer ssbo1
{
    uint tile_colors[2];
    vec2 resolution;
    uint board[64];
};
layout(std430, binding = 1) readonly buffer ssbo2
{
    float vertices[];
};
void main()
{
    uint v_id = gl_VertexID * 2;
    uint i_id = gl_InstanceID % 64;
    uint is_figure = uint(step(64,gl_InstanceID));
    vec2 pos = vec2(vertices[v_id], vertices[v_id + 1]);
    uvec2 p = uvec2(i_id % 8, i_id / 8);
    tile_index = ((p.x + p.y) % 2) | (is_figure << 7);
    uint piece = board[i_id] & 0x7F;
    vec2 scale,center_offset;
    if(is_figure == 1)
    {
        scale = vec2(0.4,0.7);
        center_offset = (vec2(1.0) - scale) * 0.5 + vec2(0,0.02);
    }
    else
    {
        scale = vec2(1.0);
        center_offset = (vec2(1.0) - scale) * 0.5;
    }
    vec2 wpos = (vec2(p) + (pos * scale) + center_offset) * 0.25 - 1.0;
    gl_Position = vec4(wpos.x,-wpos.y,0.0,1.0);
    uv = vec2((pos.x + ((piece & 0x3E) >> 1) + (piece & 1) * 6) / 13.0,-pos.y);
})";
static const char BOARD_FRAGMENT_SOURCE[] =
R"(
#version 460
layout(location = 0) in flat uint tile_index;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec4 frag_color;
layout(binding = 0) uniform sampler2D figures;
layout(std430, binding = 0) readonly buffer ssbo1
{
    uint tile_colors[2];
    vec2 resolution;
    uint board[64];
};
void main()
{
    vec3 bg = unpackUnorm4x8(tile_colors[tile_index & 0x3F]).rgb;
    uint is_fig = tile_index >> 7;
    if (is_fig == 1)
    {
        vec4 f = texture(figures,uv);
        frag_color = vec4(mix(bg,f.bgr,f.a), 1.0);
    }
    else
    {
        frag_color = vec4(bg, 1.0);
    }
})";
#endif
