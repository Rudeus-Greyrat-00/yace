#include "openwin.h"
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#define CHWDW_DEF_POSX 8
#define CHWDW_DEF_POSY 1

#define CHWDW_WIDTH 80
#define CHWDW_HEIGH 21

#define MASK_POSX 5
#define MASK_POSY 3

#define MASK_H 13
#define MASK_W 74

#define MAX_PATHLENGHT_STRING 55

//in struct of this module, the doc field should always contains an updated version of the list of item in the current directory 

int chwd_on_press_enter(UserControl uc){
    if(uc->doc->size == 1 && uc->doc->lines[0]->_used == 0) return 0; //current folder is empty
    char dir[PATH_MAX];
    int news = wchar_to_char(uc->doc->lines[uc->doc->cursor_y]->string, dir, PATH_MAX);
    if(news <= 0) return -1;
    int result = open_file_or_directory(dir, true);
    if(result < 0) return -1;
    else if(result == WAS_A_DIR){
        result = compile_file_list(uc->doc, true, 0);
        if(result < 0){
            int sec_chance = move_current_directory_up();
            if(sec_chance < 0) log_unfixable_error(true, "Multiple errors [OPENWIN.C]");
            return result;
        }
        else return 0; //all ok!
    }
    log_unfixable_error(true, "Function open_file_or_directory retunred an unexpected value [OPENWIN.C]");
    return -1; //never reached, put to stop the compiler from complain
}

int chwd_on_press_ctrlup(UserControl uc){
    return move_current_directory_up();
}

int chwdw_init(UserControl uc){
    WINDOW *savewin = create_window(CHWDW_HEIGH, CHWDW_WIDTH, CHWDW_DEF_POSY, CHWDW_DEF_POSX);
    Guiw_mask *mask = alloc_guiw_mask(MASK_H, MASK_W);
    int result = compile_file_list(uc->doc, true, 0);
    if(savewin == NULL || mask == NULL || result < 0) {
        if(mask != NULL) dealloc_guiw_mask(mask);
        return -1;
    }
    uc->window = savewin;
    uc->mask = mask;
    return 0;
}
int chwdw_deinit(UserControl uc){
    dealloc_guiw_mask(uc->mask);
    dealloc_document(uc->doc);
    return 0;
}
int chwdw_reinit(UserControl uc){
    chwdw_deinit(uc);
    return openw_init(uc);
}
int chwdw_draw(UserControl uc){
    update_mask(uc->mask, uc->doc);
    werase(uc->window);
    box(uc->window, 0, 0);
    int empty = false;
    wmove(uc->window, MASK_POSY + MASK_H + 1, 1);
    wprintw(uc->window, "Press [CTRL + ↑] or [Page up] to go one folder up");
    wmove(uc->window, MASK_POSY + MASK_H + 2, 1);
    wprintw(uc->window, "Press [ENTER] to go into the selected folder");
    wmove(uc->window, MASK_POSY + MASK_H + 3, 1);
    wprintw(uc->window, "Press [CTRL + Q] to confirm and exit from this menu");
    wmove(uc->window, 1, 1);
    wprintw(uc->window, "Change working directory:");
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
        wprintw(uc->window, "This directory does not contains any other directories"); 
        empty = true;
    }
    wmove(uc->window, MASK_POSY + MASK_H, 0);
    if(!empty) {
        wprintw(uc->window, "├────────────────────────────[Item %d/%d]", uc->doc->cursor_y + 1, uc->doc->size);
        wmove(uc->window, MASK_POSY + uc->mask->cursor_y, 2); //move cursor to right position
        wprintw(uc->window, ">>");
    }
    else {
        wprintw(uc->window, "├────────────────────────────[Item 0/0]");
        wmove(uc->window, MASK_POSY + 1, 1);
    }
    wrefresh(uc->window);
    return 0;
}

int input_buffer[CNTRL_SIGN_BUFFER_SIZE];

int chwdw_handle_input(UserControl uc, wchar_t input, int crm){
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
                        chwd_on_press_ctrlup(uc);
                        int result = compile_file_list(uc->doc, true, 0);
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
                    int result = chwd_on_press_enter(uc);
                    if(result < 0) break; ///TODO add error code and error names
                    break;
                    }
                default:
                    break;
        }
    }
    return R_OK;
}