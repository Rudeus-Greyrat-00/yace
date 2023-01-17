#ifndef MEMM
#define MEMM
#include "../main.h"

typedef struct{
    wchar_t* string;
    int size; //current allocated space (do not change manually)
    int _used; //acqual lenght of the string (do not change manually)
}Document_string;

Document_string* alloc_string(int size);
int realloc_string(Document_string* str, int new_size);
void dealloc_string(Document_string* old);
int add_character(Document_string* str, int position, wchar_t character);
int set_character(Document_string* str, int position, wchar_t character);
int remove_character(Document_string* str, int position);

void empty_string(Document_string* str);
int merge_string(Document_string* dest, Document_string* add, int position);
Document_string* split_string(Document_string* source, int position);

#endif // MEMM
