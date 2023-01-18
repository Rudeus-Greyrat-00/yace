#include "filem.h"
#include "utils.h"
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <wchar.h>

#define CARRIAGE_RETURN '\r'
#define LINE_FEED '\n'

int load_file(char* path, Document* doc){
    if(access(path, F_OK) != 0){
        return ERR_NOT_FOUND;
    }
    FILE* file = fopen(path, "r");
    wint_t wc;
    int mode_detected = false;
    int detected_mode = -1;
    while((wc = fgetwc(file)) != WEOF){
        if(wc != CARRIAGE_RETURN && wc != LINE_FEED) doc_add_character(doc, wc);
        else if(wc == CARRIAGE_RETURN){ //CR
            doc_cursor_enter(doc);
            wint_t next = fgetwc(file);
            if(next == WEOF) break;
            if(next == LINE_FEED){ //CRLF (windows)
                if(mode_detected && detected_mode != MODE_DETECT_WINDOWS) {
                    detected_mode = MODE_DETECT_MIXED;
                }
                else{
                    detected_mode = MODE_DETECT_WINDOWS;
                    mode_detected = true;
                }
            }
            else if(next == CARRIAGE_RETURN){ //CR CR
                if(mode_detected && detected_mode != MODE_DETECT_MACOS){
                    detected_mode = MODE_DETECT_MIXED;
                }
                else{
                    detected_mode = MODE_DETECT_MACOS;
                    mode_detected = true;
                }
                doc_cursor_enter(doc);
            }
            else if(next != LINE_FEED && next != CARRIAGE_RETURN){
                if(mode_detected && detected_mode != MODE_DETECT_MACOS){
                    detected_mode = MODE_DETECT_MIXED;
                }
                else{
                    detected_mode = MODE_DETECT_MACOS;
                    mode_detected = true;
                }
                doc_add_character(doc, wc);
            }
        }
        else if(wc == LINE_FEED){
            if(mode_detected && detected_mode != MODE_DETECT_LINUX){
                detected_mode = MODE_DETECT_MIXED;
            }
            else{
                detected_mode = MODE_DETECT_LINUX;
                mode_detected = true;
            }
            doc_cursor_enter(doc);
        }
    }
    fclose(file);
    doc->cursor_x = doc->lines[0]->_used;
    doc->cursor_y = 0;
    if(detected_mode == -1) detected_mode = MODE_DETECT_LINUX;
    doc->mode = detected_mode;
    doc->unsaved = false;
    return 0;
}

int save_file(char* path, Document* doc){
    FILE* file = fopen(path, "w");
    for(int i = 0; i < doc->size; i++){
        Document_string* curr_str = doc->lines[i];
        for(int j = 0; j < curr_str->_used; j++){
            fputwc(curr_str->string[j], file);
        }
        if(i + 1 == doc->size) break;
        switch(doc->mode){
            case MODE_DETECT_LINUX:
            case MODE_DETECT_MIXED:
                fputwc(LINE_FEED, file);
                break;
            case MODE_DETECT_WINDOWS:
                fputwc(CARRIAGE_RETURN, file);
                fputwc(LINE_FEED, file);
                break;
            case MODE_DETECT_MACOS:
                fputwc(CARRIAGE_RETURN, file);
                break;
        }
    }
    fclose(file);
    doc->unsaved = false;
    return 0;
}

int save_file_name(char* fname, Document* doc){
    char cwd[PATH_MAX + 255];
    strcpy(cwd, g_current_directory);
    strcat(cwd, "/");
    strcat(cwd, fname);
    if(save_file(cwd, doc) == 0){
        sprintf(doc->docname, "%s", fname);
        return 0;
    }
    else return -1;
}

int load_file_name(char* fname, Document* doc){
    char cwd[PATH_MAX + 255];
    strcpy(cwd, g_current_directory);
    strcat(cwd, "/");
    strcat(cwd, fname);
    int result;
    if((result = load_file(cwd, doc)) == 0){
        sprintf(doc->docname, "%s", fname);
        return 0;
    }
    else return result;
}

int file_exist_with_same_name(char* str){
    char cwd[PATH_MAX + 255];
    strcpy(cwd, g_current_directory);
    strcat(cwd, "/");
    strcat(cwd, str);
    if(access(cwd, F_OK) == 0){
        return 1;
    }
    return 0;
}
