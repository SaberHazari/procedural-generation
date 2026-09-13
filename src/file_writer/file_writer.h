#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "utils/utils.h"

void bmp_writer(const char *filename, i32 width, 
    i32 height, u8 *rgb_data);

#endif // FILE_WRITER_H