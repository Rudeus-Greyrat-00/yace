#include "docm.h"

void validate_cursor_position(Document* doc){
    //todo y
}

int cursor_is_in_valid_position(Document* doc){
    return true; //todo
}

void get_cursor_pos_units(Document* doc, int* x, int* y){ //return a new position of the cursor in which tab spacing are converted to spaces
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    *y = doc->cursor_y;
    int cursor_x = 0;
    Document_string* curr_str = doc->lines[doc->cursor_y];
    for(int i = 0; i < doc->cursor_x; i++){
        if(curr_str->string[i] == '\t'){
            cursor_x = (cursor_x + TAB_NSPACES) / TAB_NSPACES * TAB_NSPACES; //refer to this stackoverflow discussion https://stackoverflow.com/questions/13094690/how-many-spaces-for-tab-characters-t
        }
        else cursor_x++;
    }
    *x = cursor_x;
}

Document_string* convert_docstrings_units(Document_string* str){ //return a new docstring where tabs are replace with spaces
    Document_string* converted = alloc_string(str->size);
    if(converted == NULL) return NULL;
    for(int i = 0; i < str->_used; i++){
        if(str->string[i] != '\t') add_character(converted, -1, str->string[i]);
        else{
            int position = (converted->_used + TAB_NSPACES) / TAB_NSPACES * TAB_NSPACES; //refer to this stackoverflow discussion https://stackoverflow.com/questions/13094690/how-many-spaces-for-tab-characters-t
            add_character(converted, position - 1, ' ');
        }
    }
    return converted;
}

Document* alloc_document(int size, int line_size){
    if(size < 1) size = 1;
    Document* doc = (Document*) malloc(sizeof(Document));
    if(doc == NULL) return NULL;
    doc->lines = (Document_string**) malloc(sizeof(Document_string*) * size);
    if(doc->lines == NULL){
        free(doc);
        return NULL;
    }
    for(int i = 0; i < size; i++){
        doc->lines[i] = alloc_string(line_size);
        if(doc->lines[i] == NULL){ //some docstring fail to alloc
            for(int j = i; j >= 0; j--){
                dealloc_string(doc->lines[j]);
            }
            free(doc);
            return NULL;
        }
    }
    doc->cursor_x = 0;
    doc->cursor_y = 0;
    doc->selection_start_x = -1;
    doc->selection_start_y = -1;
    doc->selection_end_x = -1;
    doc->selection_end_y = -1;
    doc->size = size;
    doc->mode = 0;
    sprintf(doc->docname, "UNNAMED");
    doc->unsaved = true;
    return doc;
}

void dealloc_document(Document* doc){
    for(int i = 0; i < doc->size; i++){
        if(doc->lines[i] != NULL) dealloc_string(doc->lines[i]);
    }
    free(doc->lines);
    free(doc);
}

int realloc_document(Document* doc, int new_size){
    if(doc->size == new_size) return 0;
    if(new_size < 1) return -1;
    Document_string** old = doc->lines;
    doc->lines = (Document_string**) malloc(sizeof(Document_string*) * new_size);
    if(doc->lines == NULL){
        doc->lines = old;
        return -1;
    }
    if(doc->size < new_size){ //increasing size
        for(int i = 0; i < doc->size; i++){ //copy old pointers
            doc->lines[i] = old[i];
        }
        for(int i = doc->size; i < new_size; i++){ //add new lines
            doc->lines[i] = alloc_string(STR_REALLOC_DEF_INTERVAL);
        }
    }
    else{ //decreasing size
        for(int i = 0; i < new_size; i++){ //copy old pointers
            doc->lines[i] = old[i];
        }
        for(int i = new_size; i < doc->size; i++){ //dealloc pointers that doesn't fit anymore
            dealloc_string(old[i]);
        }
    }
    free(old);
    doc->size = new_size;
    validate_cursor_position(doc);
    return 0;
}

int add_line(Document* doc, int position){
    if(position < 0 || position > doc->size) return -1;
    doc->unsaved = true;
    int result = realloc_document(doc, doc->size + 1); //a new line is added to the end of the array
    if(result < 0 || position == doc->size) return result;
    Document_string* new_line = doc->lines[doc->size - 1];
    for(int i = doc->size - 1; i > position; i--){ //shift
        doc->lines[i] = doc->lines[i - 1];
    }
    doc->lines[position] = new_line;
    return result;
}

int remove_line(Document* doc, int position){
    if(position < 0 || position >= doc->size) return -1;
    doc->unsaved = true;
    if(position == doc->size - 1) return realloc_document(doc, doc->size - 1);
    else{
        Document_string* to_remove = doc->lines[position];
        for(int i = position; i < doc->size - 1; i++){
            doc->lines[i] = doc->lines[i+1];
        }
        doc->lines[doc->size - 1] = to_remove;
        return realloc_document(doc, doc->size - 1);
    }
}

int doc_add_character(Document* doc, wchar_t character){
    if(!cursor_is_in_valid_position(doc)) return -1;
    Document_string* curr_str = doc->lines[doc->cursor_y];
    int result = add_character(curr_str, doc->cursor_x, character);
    if(result == 0) {
        doc_cursor_move_right(doc, C_MODE_STD, false);
        doc->unsaved = true;
    }
    return result;
}

int doc_remove_character(Document* doc){
    if(!cursor_is_in_valid_position(doc) || (doc->cursor_x == 0 && doc->cursor_y == 0)) return -1;
    if(doc->cursor_x == 0)
    {
        int next_x_position = doc->lines[doc->cursor_y-1]->_used;
        merge_string(doc->lines[doc->cursor_y - 1], doc->lines[doc->cursor_y], doc->lines[doc->cursor_y - 1]->_used);
        int result = remove_line(doc, doc->cursor_y);
        if(result == 0)
        {
            doc->cursor_y--;
            doc->cursor_x = next_x_position;
            if(doc->cursor_y == doc->size - 1) return FEEDB_DELLAST_LINE;
            else return FEEDB_DELLINE;
        }
        else return result;
    }
    else{
        Document_string* curr_str = doc->lines[doc->cursor_y];
        int last = doc->cursor_x == curr_str->_used;
        int result = remove_character(curr_str, doc->cursor_x - 1); //I have not reasoned about the reason for this "-1" to exists, the program didn't work like intended, I tried adding some +1 and -1 around and then it worked. Classical C troubleshooting
        if(result == 0) {
            doc_cursor_move_left(doc, C_MODE_STD, false);
            doc->unsaved = true;
        }
        if(last) return FEEDB_DELLAST_CHAR;
        return 0;
    }
}

int doc_set_character(Document* doc, wchar_t character){
    if(!cursor_is_in_valid_position(doc)) return -1;
    Document_string* curr_str = doc->lines[doc->cursor_y];
    int result = set_character(curr_str, doc->cursor_x, character);
    if(result == 0) doc->unsaved = true;
    return result;
}

int compute_control_right_newpos(Document_string* str, int starting_from){ //[case 1: |word --> word|  ] [case2: word1|      word2 --> word1     |word2]
    if(starting_from >= str->_used) return starting_from;
    int index = starting_from;
    if(str->string[starting_from + 1] != ' '){ //we are in the case 1
        while(index < str->_used){
            index++;
            if(str->string[index] == ' ')break;
        }
        return index;
    }
    if(str->string[starting_from + 1] == ' '){//we are in the case 2
        while(index < str->_used){
            index++;
            if(str->string[index] != ' ') break;
        }
        return index;
    }
    return index + 1;
}

int doc_cursor_move_right(Document* doc, int mode, int selection){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    Document_string* curr_str = doc->lines[doc->cursor_y];
    if(doc->cursor_x == curr_str->_used) return -1;
    switch(mode){
        case C_MODE_STD:
            if(curr_str->_used > doc->cursor_x) doc->cursor_x++;
            break;
        case C_MODE_CTRL:
            doc->cursor_x = compute_control_right_newpos(curr_str, doc->cursor_x);
            break;
        }
    return 0;
}

int compute_control_left_newpos(Document_string* str, int starting_from){ //[case 1: word| --> |word  ] [case2: word1      |word2 --> word1|     word2]
    if(starting_from == 0) return 0;
    int index = starting_from;
    if(str->string[starting_from - 1] == ' '){ //we are in the case 2
        while(index > 0){
            index--;
            if(str->string[index - 1] != ' ') break;
        }
        return index;
    }
    if(str->string[starting_from - 1] != ' '){ //we are in the case 1
        while(index >= 0){
            index--;
            if(str->string[index] == ' ') break;
        }
        return index + 1;
    }
    return index - 1;
}

int doc_cursor_move_left(Document* doc, int mode, int selection){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    Document_string* curr_str = doc->lines[doc->cursor_y];
    if(doc->cursor_x == 0) return -1;
    switch(mode){
        case C_MODE_STD:
            if(doc->cursor_x > 0) doc->cursor_x--;
            break;
        case C_MODE_CTRL:
            doc->cursor_x = compute_control_left_newpos(curr_str, doc->cursor_x);
            break;
        }
    return 0;
}

int doc_cursor_move_down(Document* doc, int mode, int selection){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    bool move_to_end = (doc->cursor_x == doc->lines[doc->cursor_y]->_used); //TODO DO NOT MOVE TO END BUT RATHER MOVE TO THE LAST NON WHITE CHARACTER
    if(doc->cursor_y < doc->size - 1){
        doc->cursor_y++;
        if(move_to_end || doc->cursor_x > doc->lines[doc->cursor_y]->_used) doc->cursor_x = doc->lines[doc->cursor_y]->_used;
        return 0;
    }
    return -1;
}

int doc_cursor_move_up(Document* doc, int mode, int selection){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    bool move_to_end = (doc->cursor_x == doc->lines[doc->cursor_y]->_used);
    if(doc->cursor_y > 0){
        doc->cursor_y--;
        if(move_to_end || doc->cursor_x > doc->lines[doc->cursor_y]->_used) doc->cursor_x = doc->lines[doc->cursor_y]->_used;
        return 0;
    }
    return -1;
}

int doc_cursor_enter(Document* doc){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    Document_string* new_str = split_string(doc->lines[doc->cursor_y], doc->cursor_x);
    if(new_str == NULL) return -1;
    doc->unsaved = true;
    int result = realloc_document(doc, doc->size + 1);
    if(result < 0) return result;
    for(int i = doc->size - 1; i > doc->cursor_y + 1; i--){ //shift
        doc->lines[i] = doc->lines[i - 1];
    }
    doc->lines[doc->cursor_y + 1] = new_str;
    doc->cursor_y++;
    doc->cursor_x = 0;
    return FEEDB_NEWLINE;
}

int doc_pagedown(Document* doc, int pageh){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    if(doc->size > doc->cursor_y + pageh){
        doc->cursor_y += pageh;
        doc->cursor_x = 0;
        return 1; //have to move the mask (check docwin.c)
    }
    else doc->cursor_y = doc->size - 1;
    doc->cursor_x = 0;
    return 0;
}

int doc_pageup(Document* doc, int pageh){
    if(!cursor_is_in_valid_position(doc)) validate_cursor_position(doc);
    if(doc->cursor_y - pageh >= 0){
        doc->cursor_y -= pageh;
        doc->cursor_x = 0;
        return 1; //have to move the mask (check docwin.c)
    }
    else doc->cursor_y = 0;
    doc->cursor_x = 0;
    return 0;
}

