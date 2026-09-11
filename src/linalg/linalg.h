#ifndef LINALG_H
#define LINALG_H

#include "utils.h"

typedef struct Vec3 {
				f32 x, y, z;
} Vec3;

inline Vec3 vec3_new(f32 x, f32 y, f32 z);

#endif // LINALG_H