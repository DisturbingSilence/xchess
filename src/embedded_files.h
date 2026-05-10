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
    mat4 board_proj;
    mat4 ui_proj;
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
    uint is_figure = uint(step(64, gl_InstanceID));
    vec2 pos = vec2(vertices[v_id], vertices[v_id + 1]);
    uvec2 p = uvec2(i_id % 8, i_id / 8);
    tile_index = ((p.x + p.y) % 2) | (is_figure << 7);
    uint piece = board[i_id] & 0x7F;
    vec2 scale = (is_figure == 1) ? vec2(0.4, 0.7) : vec2(1.0);
    vec2 center_offset = (vec2(1.0) - scale) * 0.5;
    if(is_figure == 1) center_offset += vec2(0, 0.02);
    gl_Position = board_proj * vec4((vec2(p) + pos * scale + center_offset),0,1);
    uv = vec2((pos.x + ((piece & 0x3E) >> 1) + (piece & 1) * 6) / 13.0, 1.0 - pos.y);
})";
static const char BOARD_FRAGMENT_SOURCE[] =
R"(
#version 460
layout(location = 0) in flat uint tile_index;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec4 frag_color;
layout(binding = 0) uniform sampler2D uFigures;
layout(std430, binding = 0) readonly buffer ssbo1
{
    uint tile_colors[2];
    vec2 resolution;
    mat4 board_proj;
    mat4 ui_proj;
    uint board[64];
};
void main()
{
    vec3 bg = unpackUnorm4x8(tile_colors[tile_index & 0x3F]).rgb;
    uint is_fig = tile_index >> 7;
    if (is_fig == 1)
    {
        vec4 f = texture(uFigures,uv);
        frag_color = vec4(mix(bg,f.bgr,f.a), 1.0);
    }
    else
    {
        frag_color = vec4(bg, 1.0);
    }
})";
static const char UI_VERTEX_SOURCE[] =
R"(
#version 460
layout(std430, binding = 0) readonly buffer ssbo1
{
    uint tile_colors[2];
    vec2 resolution;
    mat4 board_proj;
    mat4 ui_proj;
    uint board[64];
};
layout(std430, binding = 1) readonly buffer ssbo2
{
    float vertices[];
};
struct Rect
{
    uvec4 rect;
    uvec4 color;
};
layout(std430, binding = 2) readonly buffer ssbo3
{
    Rect instances[];
};
layout(location = 0) out flat uint o_color;
void main()
{
    Rect d = instances[gl_InstanceID];
    o_color = d.color.x;
    vec2 wpos = vec2(vertices[gl_VertexID*2],vertices[gl_VertexID * 2 + 1]) * d.rect.zw + d.rect.xy;
    gl_Position = ui_proj * vec4(wpos,0,1);
})";
static const char UI_FRAGMENT_SOURCE[] =
R"(
#version 460
layout(location = 0) in flat uint i_color;
layout(location = 0) out vec4 frag_color;
void main()
{
    frag_color = unpackUnorm4x8(i_color);
})";
// https://poniesandlight.co.uk/reflect/debug_print_text/
static const char TEXT_VERTEX_SOURCE[] =
R"(
#version 460
struct per_word_data
{
    float pos_and_scale[4];
    uint word;
    uint color;
    uint d[2];
};
layout(std430, binding = 0) readonly buffer ssbo1
{
    uint tile_colors[2];
    vec2 resolution;
    mat4 board_proj;
    mat4 ui_proj;
    uint board[64];
};
layout(std430, binding = 1) readonly buffer ssbo2
{
    float vertices[];
};
layout(std430,binding = 2) readonly buffer ssbo3
{
    per_word_data msgs[];
};
layout (location = 0) out vec2 o_uv;
layout (location = 1) out flat uvec2 o_msg;
void main()
{
    per_word_data data = msgs[gl_InstanceID];
    o_msg = uvec2(data.word,data.color);
    vec2 scale_factor = vec2(2.,4.)/(resolution);
    scale_factor *= vec2(1,0.5);
    o_uv = vec2(vertices[gl_VertexID*2],vertices[gl_VertexID*2+1]);
    vec2 pixel_pos = vec2(data.pos_and_scale[0],data.pos_and_scale[1]);
    vec2 vertex_offset = vec2(vertices[gl_VertexID*2], vertices[gl_VertexID*2+1]) * data.pos_and_scale[2];
    vec2 final_pixel_coords = pixel_pos + vertex_offset;
    vec2 position;
    position.x = (final_pixel_coords.x * scale_factor.x) - 1.0;
    position.y = 1.0 - (final_pixel_coords.y * scale_factor.y);
    gl_Position = vec4(position,0,1);
})";
static const char TEXT_FRAGMENT_SOURCE[] =
R"(
#version 460
const uvec4 font_data[96] =
{
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00001010, 0x10101010, 0x00001010, 0x00000000 },
    { 0x00242424, 0x24000000, 0x00000000, 0x00000000 },
    { 0x00000024, 0x247E2424, 0x247E2424, 0x00000000 },
    { 0x00000808, 0x1E20201C, 0x02023C08, 0x08000000 },
    { 0x00000030, 0x494A3408, 0x16294906, 0x00000000 },
    { 0x00003048, 0x48483031, 0x49464639, 0x00000000 },
    { 0x00101010, 0x10000000, 0x00000000, 0x00000000 },
    { 0x00000408, 0x08101010, 0x10101008, 0x08040000 },
    { 0x00002010, 0x10080808, 0x08080810, 0x10200000 },
    { 0x00000000, 0x0024187E, 0x18240000, 0x00000000 },
    { 0x00000000, 0x0808087F, 0x08080800, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00001818, 0x08081000 },
    { 0x00000000, 0x0000007E, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00001818, 0x00000000 },
    { 0x00000202, 0x04040808, 0x10102020, 0x40400000 },
    { 0x0000003C, 0x42464A52, 0x6242423C, 0x00000000 },
    { 0x00000008, 0x18280808, 0x0808083E, 0x00000000 },
    { 0x0000003C, 0x42020204, 0x0810207E, 0x00000000 },
    { 0x0000007E, 0x04081C02, 0x0202423C, 0x00000000 },
    { 0x00000004, 0x0C142444, 0x7E040404, 0x00000000 },
    { 0x0000007E, 0x40407C02, 0x0202423C, 0x00000000 },
    { 0x0000001C, 0x2040407C, 0x4242423C, 0x00000000 },
    { 0x0000007E, 0x02040408, 0x08101010, 0x00000000 },
    { 0x0000003C, 0x4242423C, 0x4242423C, 0x00000000 },
    { 0x0000003C, 0x4242423E, 0x02020438, 0x00000000 },
    { 0x00000000, 0x00181800, 0x00001818, 0x00000000 },
    { 0x00000000, 0x00181800, 0x00001818, 0x08081000 },
    { 0x00000004, 0x08102040, 0x20100804, 0x00000000 },
    { 0x00000000, 0x00007E00, 0x007E0000, 0x00000000 },
    { 0x00000020, 0x10080402, 0x04081020, 0x00000000 },
    { 0x00003C42, 0x02040810, 0x00001010, 0x00000000 },
    { 0x00001C22, 0x414F5151, 0x51534D40, 0x201F0000 },
    { 0x00000018, 0x24424242, 0x7E424242, 0x00000000 },
    { 0x0000007C, 0x4242427C, 0x4242427C, 0x00000000 },
    { 0x0000001E, 0x20404040, 0x4040201E, 0x00000000 },
    { 0x00000078, 0x44424242, 0x42424478, 0x00000000 },
    { 0x0000007E, 0x4040407C, 0x4040407E, 0x00000000 },
    { 0x0000007E, 0x4040407C, 0x40404040, 0x00000000 },
    { 0x0000001E, 0x20404046, 0x4242221E, 0x00000000 },
    { 0x00000042, 0x4242427E, 0x42424242, 0x00000000 },
    { 0x0000003E, 0x08080808, 0x0808083E, 0x00000000 },
    { 0x00000002, 0x02020202, 0x0242423C, 0x00000000 },
    { 0x00000042, 0x44485060, 0x50484442, 0x00000000 },
    { 0x00000040, 0x40404040, 0x4040407E, 0x00000000 },
    { 0x00000041, 0x63554949, 0x41414141, 0x00000000 },
    { 0x00000042, 0x62524A46, 0x42424242, 0x00000000 },
    { 0x0000003C, 0x42424242, 0x4242423C, 0x00000000 },
    { 0x0000007C, 0x4242427C, 0x40404040, 0x00000000 },
    { 0x0000003C, 0x42424242, 0x4242423C, 0x04020000 },
    { 0x0000007C, 0x4242427C, 0x48444242, 0x00000000 },
    { 0x0000003E, 0x40402018, 0x0402027C, 0x00000000 },
    { 0x0000007F, 0x08080808, 0x08080808, 0x00000000 },
    { 0x00000042, 0x42424242, 0x4242423C, 0x00000000 },
    { 0x00000042, 0x42424242, 0x24241818, 0x00000000 },
    { 0x00000041, 0x41414149, 0x49495563, 0x00000000 },
    { 0x00000041, 0x41221408, 0x14224141, 0x00000000 },
    { 0x00000041, 0x41221408, 0x08080808, 0x00000000 },
    { 0x0000007E, 0x04080810, 0x1020207E, 0x00000000 },
    { 0x00001E10, 0x10101010, 0x10101010, 0x101E0000 },
    { 0x00004040, 0x20201010, 0x08080404, 0x02020000 },
    { 0x00007808, 0x08080808, 0x08080808, 0x08780000 },
    { 0x00001028, 0x44000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00000000, 0x00FF0000 },
    { 0x00201008, 0x04000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x003C0202, 0x3E42423E, 0x00000000 },
    { 0x00004040, 0x407C4242, 0x4242427C, 0x00000000 },
    { 0x00000000, 0x003C4240, 0x4040423C, 0x00000000 },
    { 0x00000202, 0x023E4242, 0x4242423E, 0x00000000 },
    { 0x00000000, 0x003C4242, 0x7E40403E, 0x00000000 },
    { 0x00000E10, 0x107E1010, 0x10101010, 0x00000000 },
    { 0x00000000, 0x003E4242, 0x4242423E, 0x02023C00 },
    { 0x00004040, 0x407C4242, 0x42424242, 0x00000000 },
    { 0x00000808, 0x00380808, 0x0808083E, 0x00000000 },
    { 0x00000404, 0x001C0404, 0x04040404, 0x04043800 },
    { 0x00004040, 0x40444850, 0x70484442, 0x00000000 },
    { 0x00003808, 0x08080808, 0x0808083E, 0x00000000 },
    { 0x00000000, 0x00774949, 0x49494949, 0x00000000 },
    { 0x00000000, 0x007C4242, 0x42424242, 0x00000000 },
    { 0x00000000, 0x003C4242, 0x4242423C, 0x00000000 },
    { 0x00000000, 0x007C4242, 0x4242427C, 0x40404000 },
    { 0x00000000, 0x003E4242, 0x4242423E, 0x02020200 },
    { 0x00000000, 0x002E3020, 0x20202020, 0x00000000 },
    { 0x00000000, 0x003E4020, 0x1804027C, 0x00000000 },
    { 0x00000010, 0x107E1010, 0x1010100E, 0x00000000 },
    { 0x00000000, 0x00424242, 0x4242423E, 0x00000000 },
    { 0x00000000, 0x00424242, 0x24241818, 0x00000000 },
    { 0x00000000, 0x00414141, 0x49495563, 0x00000000 },
    { 0x00000000, 0x00412214, 0x08142241, 0x00000000 },
    { 0x00000000, 0x00424242, 0x4242423E, 0x02023C00 },
    { 0x00000000, 0x007E0408, 0x1020407E, 0x00000000 },
    { 0x000E1010, 0x101010E0, 0x10101010, 0x100E0000 },
    { 0x00080808, 0x08080808, 0x08080808, 0x08080000 },
    { 0x00700808, 0x08080807, 0x08080808, 0x08700000 },
    { 0x00003149, 0x46000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};
layout (location = 0) in vec2 i_uv;
layout (location = 1) in flat uvec2 i_msg;
layout (location = 0) out vec4 frag_color;
void main()
{
    const uint MSG_LEN = 4;
    ivec2 char_coord = ivec2(floor(i_uv.xy * vec2(8 * MSG_LEN,16)));
    uint char_code = i_msg.x >> 8 * (char_coord.x/8) & 0xff;
    if (char_code == 0x00)discard;
    char_code -= 0x20;
    char_coord.x = char_coord.x % 8;
    uint four_lines = font_data[char_code][char_coord.y/4];
    uint current_line  = (four_lines >> (8*(3-(char_coord.y)%4))) & 0xff;
    uint current_pixel = (current_line >> (7-char_coord.x)) & 0x01;
    frag_color = mix(vec4(0),unpackUnorm4x8(i_msg.y),current_pixel);
})";

static char FULLSCREEN_VERTEX_SOURCE[] =
R"(
#version 460
layout(location = 0) out vec2 o_uv;
out vec2 v_texcoord;
void main()
{
    const vec4 positions[3] = vec4[3](
        vec4(-1.0, -1.0, 0.0, 1.0),
        vec4( 3.0, -1.0, 0.0, 1.0),
        vec4(-1.0,  3.0, 0.0, 1.0)
    );
    const vec2 texcoords[3] = vec2[3](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );
    gl_Position = positions[gl_VertexID];
    o_uv  = texcoords[gl_VertexID];
})";
// https://www.shadertoy.com/view/wfGBWm
static char FULLSCREEN_FRAGMENT_SOURCE[] =
R"(
#version 460
layout(location = 0) in vec2 i_uv;
layout(location = 0) out vec4 frag_color;
layout(binding = 1) uniform sampler2D uImage;
layout(location = 1) uniform float uTime;
layout(std430, binding = 0) readonly buffer ssbo1
{
    uint tile_colors[2];
    vec2 resolution;
    mat4 board_proj;
    mat4 ui_proj;
    uint board[64];
};
float hash(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p,p + 45.32);
    return fract(p.x * p.y);
}
float noise(vec2 p)
{
    vec2 i = floor(p),f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x),
               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}
float fbm(vec2 p, int octaves)
{
    float v = 0,amp = 0.5;
    for (int i = 0; i < octaves; i++)
    {
        v += noise(p) * amp;
        p *= 2.5; amp *= 0.5;
    }
    return v;
}
void main()
{
    vec2 p = (i_uv - 0.5) * vec2(resolution.x / resolution.y, 1.0);
    float progress = uTime * 0.25;
    progress = progress * progress * (3.0 - 2.0 * progress);
    if(uTime > 3)
    {
        vec4 tex = texture(uImage,i_uv);
        frag_color = tex;
        return;
    }
    float threshold = progress * 4.0;
    float smudgeField = length(p) * (0.4 + fbm(p * 1.8 + 10.0, 4) * 3.2);
    float feather = 0.15;
    float revealMask = smoothstep(threshold - feather, threshold + feather, smudgeField);
    revealMask = 1.0 - revealMask;
    float waveProfile = exp(-pow(abs((smudgeField - threshold) / 0.15), 2.0));
    vec2 warpVec = normalize(p) * waveProfile * (0.25 * (1.0 - progress));
    vec3 spectral = vec3(1.5, 1.0, 0.5);
    vec4 texOld = vec4(0);
    vec4 texNew;
    texNew.r = texture(uImage, i_uv - warpVec * spectral.r).r;
    texNew.g = texture(uImage, i_uv - warpVec * spectral.g).g;
    texNew.b = texture(uImage, i_uv - warpVec * spectral.b).b;
    texNew.a = texture(uImage, i_uv - warpVec).a;
    vec4 finalColor = mix(texOld, texNew, revealMask);
    float highFreqNoise = fbm(p * 30.0 + uTime * 0.1, 2);
    float filaments = pow(highFreqNoise, 4.0) * pow(waveProfile, 20.0) * 30.0;
    finalColor.rgb += mix(texOld.rgb, texNew.rgb, 0.5) * pow(waveProfile,40.0) * 5.0;
    finalColor.rgb += vec3(1.0) * filaments;
    frag_color = vec4(finalColor.rgb, finalColor.a);
})";
#endif
