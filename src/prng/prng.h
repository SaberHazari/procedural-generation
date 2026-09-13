#ifndef PRNG_H
#define PRNG_H

#include "proc_math/proc_math.h"

typedef struct Prng {
    u32 state;
} Prng;

static inline void prng_seed(Prng *r, u32 seed) {
    r->state = (seed != 0u) ? seed : 1u;
}

static inline u32 prng_next(Prng *r) {
    u32 x = r->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    r->state = x;
    return x;
}

static inline f32 prng_range(Prng *r, f32 lo, f32 hi) {
    f32 t = (f32)(prng_next(r) >> 8) / (f32)(1u << 24);
    return lerp(t, lo, hi);
}

#endif // PRNG_H