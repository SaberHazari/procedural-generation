#include <stdio.h>
#include "noise/noise.h"

int main() {
    f32 x = perlin2d(13.7f, 21.34f);
    printf("perlin noise of (13.7, 21.34) is %f\n", x);
    
    return 0;
}