#include "memm.h"

Document_string* alloc_string(int size){
    if(size < 1) size = 1;
    Document_string* new_str = (Document_string*) malloc(sizeof(Document_string));
    if(new_str == NULL) return NULL;
    wchar_t* string = (wchar_t*) malloc(sizeof(wchar_t)*(size + 1));
    if(string == NULL) return NULL;
    new_str->string = string;
    for(int i = 0; i < size + 1; i++)
        new_str->string[i] = '\0';
    new_str->size = size;
    new_str->_used = 0;
    return new_str;
}

int realloc_string(Document_string* str, int new_size){
    if(new_size < 1) new_size = 1;
    wchar_t *new_str = (wchar_t*) malloc(sizeof(wchar_t)*(new_size + 1));
    if(new_str == NULL) return -1;
    if(str->_used > new_size) str->_used = new_size;
    for(int i = 0; i < str->_used; i++){
        new_str[i] = str->string[i];
    }
    free(str->string);
    str->string = new_str;
    str->size = new_size;
    for(int i = str->_used; i < str->size; i++){
        str->string[i] = '\0';
    }
    return 0;
}

void dealloc_string(Document_string* old){
    if(old->string != NULL) free(old->string);
    free(old);
}

int add_character(Document_string* str, int position, wchar_t character){
    if(str->_used == str->size){
        int result = realloc_string(str, str->size + STR_REALLOC_DEF_INTERVAL);
        if(result < 0) return result;
    }
    if(position > str->size){
        int result = realloc_string(str, str->size + position);
        if(result < 0) return result;
    }
    if(position < 0){
        if(abs(position) - 1 <= str->_used){
            position = str->_used - (abs(position) -1); //this way with position == -1 we put ourself AFTER the last letter
        }
        else return -1;
    }
    for(int i = str->_used; i > position; i--){
        str->string[i] = str->string[i-1];
    }
    str->string[position] = character;
    if(position > str->_used){ //if the new position is after the old length of the string
        for(int i = str->_used; i < position; i++){
            str->string[i] = ' ';
        }
        str->_used = position + 1;
    }
    else
        str->_used++;
    return 0;
}

int remove_character(Document_string* str, int position){
    if(str->_used == 0){
        return -1;
    }
    if(position < 0){
        if(abs(position) <= str->_used){
            position = str->_used - abs(position); //this way with position == -1 we put ourself ON the last letter
        }
        else return -1;
    }
    for(int i = position; i < str->_used; i++){
        str->string[i] = str->string[i+1]; //this work because the string is greater than size of one unit (see alloc string and realloc string functions)
    }
    str->_used--;
    if(str->_used + STR_REALLOC_DEF_INTERVAL < str->size) realloc_string(str, str->_used + STR_REALLOC_DEF_INTERVAL); //realloching with a smaller number doesn't need to be checked
    return 0;
}

int set_character(Document_string* str, int position, wchar_t character){
    if(position >= str->size) return -1;
    if(position > str->_used) return add_character(str, position, character);
    str->string[position] = character;
    return 0;
}

void empty_string(Document_string* str){
    for(int i = 0; i < str->_used; i++){
        str->string[i] = '\0';
    }
}

int merge_string(Document_string* dest, Document_string* add, int position){
    if(position < 0 || position >= dest->size) return -1;
    if((dest->_used + add->_used) > dest->size) {
        int result = realloc_string(dest, dest->size + add->_used);
        if(result < 0) return result;
    }
    for(int i = dest->_used - 1; i >= position; i--){
        dest->string[i+add->_used] = dest->string[i];
    }
    for(int i = position; i < position + add->_used; i++){
        dest->string[i] = add->string[i - position];
    }
    dest->_used += add->_used;
    return 0;
}

Document_string* split_string(Document_string* source, int position){
    if(position < 0 || position > source->size) return NULL;
    if(position == source->size) return alloc_string(STR_REALLOC_DEF_INTERVAL);
    Document_string* new_str = alloc_string(source->_used);
    if(new_str == NULL) return NULL;
    for(int i = position; i < source->_used; i++){
        add_character(new_str, -1, source->string[i]);
    }
    for(int i = position; i < source->_used; i++){
        source->string[i] = '\0';
    }
    source->_used = position;
    return new_str;
}


