#include "openwin.h"
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#define OPENW_DEF_POSX 8
#define OPENW_DEF_POSY 1

#define OPENW_WIDTH 80
#define OPENW_HEIGH 21

#define MASK_POSX 5
#define MASK_POSY 3

#define MASK_H 13
#define MASK_W 74

#define MAX_PATHLENGHT_STRING 55

//in struct of this module, the doc field should always contains an updated version of the list of item in the current directory 

int openw_on_press_enter(UserControl uc){
    if(uc->doc->size == 1 && uc->doc->lines[0]->_used == 0) return 0; //current folder is empty
    char dir[PATH_MAX];
    int news = wchar_to_char(uc->doc->lines[uc->doc->cursor_y]->string, dir, PATH_MAX);
    if(news <= 0) return -1;
    int result = open_file_or_directory(dir, false);
    if(result < 0) return -1;
    if(result == WAS_A_FILE) return WAS_A_FILE;
    else if(result == WAS_A_DIR){
        result = compile_file_list(uc->doc, false, 0);
        if(result < 0){
            int sec_chance = move_current_directory_up();
            if(sec_chance < 0) log_unfixable_error(true, "Multiple errors [OPENWIN.C]");
            return result;
        }
        else return WAS_A_DIR;
    }
    log_unfixable_error(true, "Function open_file_or_directory retunred an unexpected value [OPENWIN.C]");
    return -1; //never reached, put to stop the compiler from complain
}

int openw_on_press_ctrlup(UserControl uc){
    return move_current_directory_up();
}

int openw_init(UserControl uc){
    WINDOW *savewin = create_window(OPENW_HEIGH, OPENW_WIDTH, OPENW_DEF_POSY, OPENW_DEF_POSX);
    Guiw_mask *mask = alloc_guiw_mask(MASK_H, MASK_W);
    int result = compile_file_list(uc->doc, false, 0);
    if(savewin == NULL || mask == NULL || result < 0) {
        if(mask != NULL) dealloc_guiw_mask(mask);
        return -1;
    }
    uc->window = savewin;
    uc->mask = mask;
    return 0;
}
int openw_deinit(UserControl uc){
    dealloc_guiw_mask(uc->mask);
    dealloc_document(uc->doc);
    return 0;
}
int openw_reinit(UserControl uc){
    openw_deinit(uc);
    return openw_init(uc);
}
int openw_draw(UserControl uc){
    update_mask(uc->mask, uc->doc);
    werase(uc->window);
    box(uc->window, 0, 0);
    int empty = false;
    wmove(uc->window, MASK_POSY + MASK_H + 1, 1);
    wprintw(uc->window, "Press [CTRL + ???] or [Page up] to go one folder up");
    wmove(uc->window, MASK_POSY + MASK_H + 2, 1);
    wprintw(uc->window, "Press [ENTER] to open the selected file, or go into the selected folder");
    wmove(uc->window, MASK_POSY + MASK_H + 3, 1);
    wprintw(uc->window, "Press [CTRL + Q] to cancel");
    wmove(uc->window, 1, 1);
    wprintw(uc->window, "Open file:");
    wmove(uc->window, 2, 1);
    wprintw(uc->window, "Current directory: ");
    char display_path[MAX_PATHLENGHT_STRING];
    generate_path_str(display_path, g_current_directory, MAX_PATHLENGHT_STRING);
    wprintw(uc->window, "%s", display_path);
    for(int i = 0; i < uc->mask->rows; i++){ //print document text from mask matrix
        wmove(uc->window, MASK_POSY + i, MASK_POSX);
        wprintw(uc->window, "%ls", uc->mask->matrix[i]);
    }
    if(uc->doc->size == 1 && uc->doc->lines[0]->_used == 0) {
        wmove(uc->window, MASK_POSY, 1);
        wprintw(uc->window, "There is nothing inside this folder (nor files nor directories)");
        empty = true;
    }
    if(!empty){
        wmove(uc->window, MASK_POSY + MASK_H, 0);
        wprintw(uc->window, "???????????????????????????????????????????????????????????????????????????????????????[Item %d/%d]", uc->doc->cursor_y + 1, uc->doc->size);
        wmove(uc->window, MASK_POSY + uc->mask->cursor_y, 2); //move cursor to right position
        wprintw(uc->window, ">>");
    }
    else{
        wmove(uc->window, MASK_POSY + MASK_H, 0);
        wprintw(uc->window, "???????????????????????????????????????????????????????????????????????????????????????[Item 0/0]");
        wmove(uc->window, MASK_POSY + 1, 1);
    }
    wrefresh(uc->window);
    return 0;
}

int input_buffer[CNTRL_SIGN_BUFFER_SIZE];

int openw_handle_input(UserControl uc, wchar_t input, int crm){
    wchar_t ch = input;
    Document* doc = uc->doc;
    //Guiw_mask* mask = uc->mask;

    if(ch == ESCAPE_KEY) reset_cnt_buffer(input_buffer);
    if(crm && ch != ESCAPE_KEY){
            cnt_buffer_addchar(input_buffer, ch);
            int action = check_action(input_buffer);
            if(action != -1){ //if an action is recognised
                switch(action){
                    case A_KEY_LEFT:
                        break;
                    case A_KEY_RIGHT:
                        break;
                    case A_KEY_UP:
                        doc_cursor_move_up(doc, C_MODE_STD, false);
                        break;
                    case A_KEY_DOWN:
                        doc_cursor_move_down(doc, C_MODE_CTRL, false);
                        break;
                    case CTRL_A_KEY_UP:
                    case KEY_PGUP:
                        openw_on_press_ctrlup(uc);
                        int result = compile_file_list(uc->doc, false, 0);
                        if(result < 0) return result;
                        break;
                }
                reset_cnt_buffer(input_buffer);
            }
    }
    else{
            switch(ch){
                case QUIT:
                    GUI_REMOVE_WINDOW();
                    break;
                case ENTER_KEY:{
                    int result = openw_on_press_enter(uc);
                    if(result < 0) break; ///TODO add error code and error names
                    if(result == WAS_A_FILE) GUI_REMOVE_WINDOW();
                    break;
                    }
                default:
                    break;
        }
    }
    return R_OK;
}