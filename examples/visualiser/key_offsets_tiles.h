/*
out = "";
key_x_offsets = [0, 3, 4, 7, 8, 12, 15, 16, 19, 20, 23, 24];
key_tiles = [0, 6, 2, 6, 4, 0, 6, 2, 6, 2, 6, 4];
for (i = 0; i < 128; i++) {
    out += 
        "{" + 
        ((Math.floor(i / 12) * 28) + key_x_offsets[i % 12]) + ", " +
        key_tiles[i % 12] + 
        "},";
}
*/

typedef struct offset_tile_s {
    unsigned char x;
    unsigned char tile;
} offset_tile_t;

const offset_tile_t key_offsets_tiles[128] = {
    {0, 0},{3, 6},{4, 2},{7, 6},{8, 4},{12, 0},
    {15, 6},{16, 2},{19, 6},{20, 2},{23, 6},{24, 4},
    {28, 0},{31, 6},{32, 2},{35, 6},{36, 4},{40, 0},
    {43, 6},{44, 2},{47, 6},{48, 2},{51, 6},{52, 4},
    {56, 0},{59, 6},{60, 2},{63, 6},{64, 4},{68, 0},
    {71, 6},{72, 2},{75, 6},{76, 2},{79, 6},{80, 4},
    {84, 0},{87, 6},{88, 2},{91, 6},{92, 4},{96, 0},
    {99, 6},{100, 2},{103, 6},{104, 2},{107, 6},
    {108, 4},{112, 0},{115, 6},{116, 2},{119, 6},
    {120, 4},{124, 0},{127, 6},{128, 2},{131, 6},
    {132, 2},{135, 6},{136, 4},{140, 0},{143, 6},
    {144, 2},{147, 6},{148, 4},{152, 0},{155, 6},
    {156, 2},{159, 6},{160, 2},{163, 6},{164, 4},
    {168, 0},{171, 6},{172, 2},{175, 6},{176, 4},
    {180, 0},{183, 6},{184, 2},{187, 6},{188, 2},
    {191, 6},{192, 4},{196, 0},{199, 6},{200, 2},
    {203, 6},{204, 4},{208, 0},{211, 6},{212, 2},
    {215, 6},{216, 2},{219, 6},{220, 4},{224, 0},
    {227, 6},{228, 2},{231, 6},{232, 4},{236, 0},
    {239, 6},{240, 2},{243, 6},{244, 2},{247, 6},
    {248, 4},{252, 0},{255, 6},{256, 2},{259, 6},
    {260, 4},{264, 0},{267, 6},{268, 2},{271, 6},
    {272, 2},{275, 6},{276, 4},{280, 0},{283, 6},
    {284, 2},{287, 6},{288, 4},{292, 0},{295, 6},{296, 2}
};