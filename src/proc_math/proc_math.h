#ifndef PROC_MATH_H
#define PROC_MATH_H

#include "utils/utils.h"

static inline f32 fade(f32 t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline f32 lerp(f32 t, f32 a, f32 b) {
    return a + t * (b - a);
}

#endif // PROC_MATH_H