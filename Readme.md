# mapgen

Procedural biome map generator. Perlin noise drives two fields, height and
moisture, which get classified into biomes and rendered to RGB.

Each field is a sum of octaves with its own random offsets, then normalized to
0..1. A radial falloff is subtracted from height so the edges sink into water.
Biomes come from a small elevation-by-moisture table, with water, sand, and
snow handled before the table gets involved. Output goes straight to a BMP.