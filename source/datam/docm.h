#ifndef DOCM
#define DOCM
#include "../main.h"
#include "memm.h"
#include <limits.h>

#define C_MODE_STD 0
#define C_MODE_CTRL 1

#define FEEDB_NEWLINE 1 //returned when a line is added
#define FEEDB_DELLINE 2 //returned when a line is deleted
#define FEEDB_DELLAST_LINE 3 //returned when the last LINE of the doc is deleted
#define FEEDB_DELLAST_CHAR 10 //returned then the last character of a line is deleted

typedef struct{
    Document_string** lines;
    int size;
    int cursor_x;
    int cursor_y;
    int selection_start_x;
    int selection_start_y;
    int selection_end_x;
    int selection_end_y;
    int mode;
    char docdir[PATH_MAX];
    char docname[MAX_FNAME_SIZE];
    int unsaved;
} Document;

Document* alloc_document(int size, int line_size);
void dealloc_document(Document*);

int doc_add_character(Document* doc, wchar_t character);
int doc_set_character(Document* doc, wchar_t character);
int doc_remove_character(Document* doc);

void get_cursor_pos_units(Document* doc, int* x, int* y);
Document_string* convert_docstrings_units(Document_string* str);

int doc_cursor_move_right(Document* doc, int mode, int selection);
int doc_cursor_move_left(Document* doc, int mode, int selection);
int doc_cursor_move_up(Document* doc, int mode, int selection);
int doc_cursor_move_down(Document* doc, int mode, int selection);
int doc_cursor_enter(Document* doc);
int doc_pagedown(Document* doc, int pageh);
int doc_pageup(Document* doc, int pageh);

int doc_remove_line(Document* doc, int position);
int doc_add_line(Document* doc, int position);

#endif // DOCM
