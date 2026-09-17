#include "file_writer/file_writer.h"
#include "map/map.h"

int main() {
    MapSeeds map_seeds;
    map_seeds.height = 12101u;
    map_seeds.moisture = 11701u;
    
    MapInfo map_info;
    map_info.width = 1024;
    map_info.height = 1024;
    map_info.octaves = 5;
    map_info.scale = 140.0f;
    map_info.persistence = 0.5f;
    map_info.lacunarity = 2.0f;
    map_info.offset_x = 0.0f;
    map_info.offset_y = 0.0f;
    
    Viewport viewport;
    viewport.width = 1024;
    viewport.height = 1024;
    viewport.offset_x = 0;
    viewport.offset_y = 0;
    
    u8 *map = build_map(&map_info, &map_seeds);
    u8 *image = (u8 *)calloc((size_t)(viewport.width * 
        viewport.height * 3), sizeof(u8));
    render_viewport(&map_info, &viewport, map, image);
    
    bmp_writer("output/bmp/map.bmp", viewport.width, 
        viewport.height, image);
    
    free(image);
    image = NULL;
    free(map);
    map = NULL;
    
    return 0;
}