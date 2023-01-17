#ifndef MASKM
#define MASKM
#include "../main.h"
#include "../datam/docm.h"

typedef struct{
    wchar_t** matrix;
    int posx;
    int posy;
    int rows;
    int cols;
    int cursor_x;
    int cursor_y;
} Guiw_mask;

Guiw_mask* alloc_guiw_mask(int rows, int cols);
void dealloc_guiw_mask(Guiw_mask* mask);
int populate_guiw_mask(Guiw_mask* mask, Document* doc);
void move_guiw_mask(Guiw_mask* mask, int deltax, int deltay);
void update_mask(Guiw_mask* mask, Document* doc);

#endif //MASKM
