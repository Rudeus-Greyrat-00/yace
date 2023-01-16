#include "maskm.h"

int compute_deltax(Guiw_mask* mask, int doc_cursor_x){ //how much the mask has to move horizontally so that the document cursor is inside it
    int startx, endx;
    startx = mask->posx;
    endx = mask->posx + mask->cols - 1;
    if(doc_cursor_x > endx){
        return doc_cursor_x - endx;
    }
    else if(doc_cursor_x < startx){
        return doc_cursor_x - startx;
    }
    return 0;
}

int compute_deltay(Guiw_mask* mask, int doc_cursor_y){ //how much the mask has to move vertically that the document cursor is inside it
    int starty, endy;
    starty = mask->posy;
    endy = mask->posy + mask->rows - 1;
    if(doc_cursor_y >= endy){
        return doc_cursor_y - endy;
    }
    else if(doc_cursor_y < starty){
        return doc_cursor_y - starty;
    }
    return 0;
}

void move_guiw_mask(Guiw_mask* mask, int deltax, int deltay){
    if(mask->posx + deltax > 0) mask->posx += deltax;
    else mask->posx = 0;

    if(mask->posy + deltay > 0) mask->posy += deltay;
    else mask->posy = 0;
}

void update_mask(Guiw_mask* mask, Document* doc){ //make sure that the document cursor is inside the mask (the mask is the part of the text that is show to the screen) and then fill the mask matrix with the characters that will be printed on screen
    int doc_cursor_x, doc_cursor_y;
    get_cursor_pos_units(doc, &doc_cursor_x, &doc_cursor_y);
    int deltax = compute_deltax(mask, doc_cursor_x);
    int deltay = compute_deltay(mask, doc_cursor_y);

    mask->posx += deltax;
    if(deltax < 0) mask->cursor_x = 0;
    else if(deltax > 0) mask->cursor_x = mask->cols - 1;
    else mask->cursor_x = doc_cursor_x - mask->posx;

    mask->posy += deltay;
    if(deltay < 0) mask->cursor_y = 0;
    else if(deltay > 0) mask->cursor_y = mask->rows - 1;
    else mask->cursor_y = doc_cursor_y - mask->posy;

    populate_guiw_mask(mask, doc);
}

Guiw_mask* alloc_guiw_mask(int rows, int cols){
    Guiw_mask* new_mask = (Guiw_mask*)malloc(sizeof(Guiw_mask));
    if(new_mask == NULL) return NULL;
    wchar_t** matrix = (wchar_t**) malloc(sizeof(wchar_t*) * rows);
    if(matrix == NULL){
        free(new_mask);
        return NULL;
    }
    for(int i = 0; i < rows; i++){
        matrix[i] = (wchar_t*) malloc(sizeof(wchar_t) * (cols + 1));
        if(matrix[i] == NULL){ //if some wchar fail to alloc
            for(int j = i; j >= 0; j--){
                free(matrix[j]);
            }
            free(new_mask);
            return NULL;
        }
        matrix[i][cols] = '\0';
    }
    new_mask->matrix = matrix;
    new_mask->posx = 0;
    new_mask->posy = 0;
    new_mask->rows = rows;
    new_mask->cols = cols;
    new_mask->cursor_x = 0;
    new_mask->cursor_y = 0;
    return new_mask;
}

void dealloc_guiw_mask(Guiw_mask* mask){
    if(mask->matrix != NULL){
        for(int i = 0; i < mask->rows; i++){
            if(mask->matrix[i] != NULL) free(mask->matrix[i]);
        }
        free(mask->matrix);
    }
    free(mask);
}

int populate_guiw_mask(Guiw_mask* mask, Document* doc){
    if(mask->posy >= doc->size) return -1;
    int i;
    for(i = 0; i < mask->rows && (i + mask->posy) < doc->size; i++){ //fill with the rows that are within the mask
        Document_string* curr_str = convert_docstrings_units(doc->lines[i + mask->posy]); //get a string where tabs are replaced by spaces
        if(curr_str == NULL) return -1;
        int j;
        for(j = 0; j < mask->cols && (j + mask->posx) < curr_str->_used; j++){ //fill with the part of the string that is within the mask
            mask->matrix[i][j] = curr_str->string[j + mask->posx];
        }
        for(; j < mask->cols; j++){ //fill with space the remaining space
            mask->matrix[i][j] = ' ';
        }
        dealloc_string(curr_str);
    }
    for(; i < mask->rows; i++){//fill with space the remaining rows
        for(int j = 0; j < mask->cols; j++){
            mask->matrix[i][j] = ' ';
        }
    }
    return 0;
}
