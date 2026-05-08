#include "utils.h"
#include "logger.h"
#include <stdlib.h>
#include <stdio.h>

u32 FNV(const void* key,u32 h)
{
    //https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    h ^= 2166136261UL;
    const u8* data = (const u8*)key;
    for(i32 i = 0;data[i] != '\0';i++)
    {
        h ^= data[i];
        h *= 16777619;
    }
    return h;
}
u8* read_tga(const char* filename,i32* out_width,i32* out_height)
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
        u8 id_length;
        u8 color_map_type;
        u8 image_type;
        // color map specs
        u16 first_cmap_entry;
        u16 cmap_length;
        u8 cmap_bpp;
        // image specs
        u16 y_origin;
        u16 x_origin;
        u16 width;
        u16 height;
        u8 bpp;
        u8 image_descriptor;
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
    u32 bytes_per_pixel = header.bpp / 8;
    u32 image_data_size = bytes_per_pixel * header.height * header.width;
    u8* raw_image_data = malloc(image_data_size);
    fseek(file,header.id_length + header.cmap_length * (header.cmap_bpp / 8),SEEK_CUR);
    fread(raw_image_data,1,image_data_size,file);
    fclose(file);

    u8* p_temp,*p_temp_2;
    // flip image on y axis
    if((header.image_descriptor & (1 << 5)) != 0)
    {
        for(i32 y = 0;y < header.height / 2 - 1;y++)
        {
            for(i32 x = 0;x < header.width;x++)
            {
                p_temp = &raw_image_data[(y * header.width + x) * bytes_per_pixel];
                p_temp_2 = &raw_image_data[((header.height - y - 1) * header.width + x) * bytes_per_pixel];
                for(i32 b = 0;b < bytes_per_pixel;b++)
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
