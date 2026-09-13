#include "file_writer.h"
#include <stdlib.h>
#include <stdio.h>

void bmp_writer(const char *filename, i32 width, 
    i32 height, u8 *rgb_data) {
    i32 row_size = ((width * 3 + 3) / 4) * 4;
    i32 pixel_data_size = row_size * height;
    i32 file_size = 54 + pixel_data_size;
    
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        fprintf(stderr, "Failed to open %s\n", filename);
        exit(1);
    }
    
    u8 header[54] = {0};
    header[0] = 'B';
    header[1] = 'M';
    header[2] = (file_size >> 0) & 0xFF;
    header[3] = (file_size >> 8) & 0xFF;
    header[4] = (file_size >> 16) & 0xFF;
    header[5] = (file_size >> 24) & 0xFF;
    header[10] = 54;
    header[14] = 40;
    header[18] = (width >> 0) & 0xFF;
    header[19] = (width >> 8) & 0xFF;
    header[20] = (width >> 16) & 0xFF;
    header[21] = (width >> 24) & 0xFF;
    header[22] = (height >> 0) & 0xFF;
    header[23] = (height >> 8) & 0xFF;
    header[24] = (height >> 16) & 0xFF;
    header[25] = (height >> 24) & 0xFF;
    header[26] = 1;
    header[28] = 24;
    header[34] = (pixel_data_size >> 0) & 0xFF;
    header[35] = (pixel_data_size >> 8) & 0xFF;
    header[36] = (pixel_data_size >> 16) & 0xFF;
    header[37] = (pixel_data_size >> 24) & 0xFF;
    header[38] = 0x23; header[39] = 0x2E;
    header[42] = 0x23; header[43] = 0x2E;
    
    fwrite(header, 1, 54, fp);
    
    i32 pad_size = row_size - width * 3;
    u8 pad[3] = {0, 0, 0};
    for(i32 y = height - 1; y >= 0; --y) {
        for(i32 x = 0; x < width; ++x) {
            i32 idx = (y * width + x) * 3;
            u8 bgr[3] = { rgb_data[idx + 2], rgb_data[idx + 1], rgb_data[idx + 0] };
            fwrite(bgr, 1, 3, fp);
        }
        if(pad_size > 0) { fwrite(pad, 1, pad_size, fp); }
    }
    
    fclose(fp);
}