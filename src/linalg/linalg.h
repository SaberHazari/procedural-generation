#ifndef LINALG_H
#define LINALG_H

#include "utils/utils.h"

typedef struct Vec3 {
				f32 x, y, z;
} Vec3;

// This currently does nothing
static inline Vec3 vec3_new(f32 x, f32 y, f32 z) {
    Vec3 v3 = { x, y, z };
    return v3;
}

#endif // LINALG_H