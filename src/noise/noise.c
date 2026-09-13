#include "noise.h"
#include <math.h>

static const u8 PERM[256] = {
    151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
    140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
    247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
     57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
     74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
     60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
     65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
    200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
     52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
    207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
    119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
    129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
    218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
     81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
    184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
    222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};

static u8 permutation[512];
static b32 perlin_initialized = false;

static void init_perlin(void) {
    if(perlin_initialized) return;
    for(int i = 0; i < 256; ++i) {
        permutation[i] = PERM[i];
        permutation[256 + i] = PERM[i];
    }
    perlin_initialized = true;
}

static const f32 GRAD_X[8] = {
                1.0f,            -1.0f,             0.0f, 
                0.0f,  ONE_BY_ROOT_TWO, -ONE_BY_ROOT_TWO, 
     ONE_BY_ROOT_TWO, -ONE_BY_ROOT_TWO, 
};

static const f32 GRAD_Y[8] = {
                0.0f,             0.0f,             1.0f, 
               -1.0f,  ONE_BY_ROOT_TWO,  ONE_BY_ROOT_TWO, 
    -ONE_BY_ROOT_TWO, -ONE_BY_ROOT_TWO, 
};

static inline f32 grad2(i32 hash, f32 x, f32 y) {
    i32 h = hash & 7;
    return GRAD_X[h] * x + GRAD_Y[h] * y;
}

f32 perlin2d(f32 x, f32 y) {
    init_perlin();
    
    i32 idx_x = (i32)floorf(x) & 255;
    i32 idx_y = (i32)floorf(y) & 255;
    
    x -= floorf(x);
    y -= floorf(y);
    
    f32 u = fade(x);
    f32 v = fade(y);
    
    i32 a = permutation[idx_x] + idx_y;
    i32 aa = permutation[a];
    i32 ab = permutation[a + 1];
    
    i32 b = permutation[idx_x + 1] + idx_y;
    i32 ba = permutation[b];
    i32 bb = permutation[b + 1];
    
    f32 n00 = grad2(permutation[aa],        x,        y);
    f32 n10 = grad2(permutation[ba], x - 1.0f,        y);
    f32 n01 = grad2(permutation[ab],        x, y - 1.0f);
    f32 n11 = grad2(permutation[bb], x - 1.0f, y - 1.0f);
    
    f32 nx0 = lerp(u, n00, n10);
    f32 nx1 = lerp(u, n01, n11);
    
    f32 nxy = lerp(v, nx0, nx1);
    
    return nxy;
}