#include "openwin.h"
#include <string.h>
#include <dirent.h>

#define OPENW_DEF_POSX 8
#define OPENW_DEF_POSY 1

#define OPENW_WIDTH 80
#define OPENW_HEIGH 18

#define MASK_POSX 1
#define MASK_POSY 4

#define MASK_H 14
#define MASK_W 78

#define MAX_PATHLENGHT_STRING 60

void generate_path_str(char* dest){
    if(strlen(g_current_directory) > MAX_PATHLENGHT_STRING){
        sprintf(dest + 2, "%s", g_current_directory + strlen(g_current_directory) - MAX_PATHLENGHT_STRING + 2);
        dest[0] = '.';
        dest[1] = '.';
    }
    else sprintf(dest, "%s", g_current_directory);
}

int compile_file_list(UserControl uc){
    DIR *d;
    struct dirent *dir;
    d = opendir(g_current_directory);
    if(d){
        while ((dir = readdir(d)) != NULL){
            int sz = strlen(dir->d_name);
            for(int i = 0; i < 4; i++){
                int result = doc_add_character(uc->doc, ' ');
                if(result < 0) return result;
            }
            for(int i = 0; i < sz; i++){
                int result = doc_add_character(uc->doc, (wchar_t)dir->d_name[i]);
                if(result < 0) return result;
            }
            int result = doc_cursor_enter(uc->doc);
            if(result < 0) return result;
        }
    }
    uc->doc->cursor_x = 0;
    uc->doc->cursor_y = 0;
    return 0;
}

int openw_init(UserControl uc){
    WINDOW *savewin = create_window(OPENW_HEIGH, OPENW_WIDTH, OPENW_DEF_POSY, OPENW_DEF_POSX);
    Guiw_mask *mask = alloc_guiw_mask(MASK_H, MASK_W);
    int result = compile_file_list(uc);
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
    wmove(uc->window, 1, 1);
    wprintw(uc->window, "Open file:");
    wmove(uc->window, 3, 1);
    wprintw(uc->window, "Current directory: ");
    char display_path[MAX_PATHLENGHT_STRING];
    generate_path_str(display_path);
    wprintw(uc->window, "%s", display_path);
    for(int i = 0; i < uc->mask->rows; i++){ //print document text from mask matrix
        wmove(uc->window, MASK_POSY + i, MASK_POSX);
        wprintw(uc->window, "%ls", uc->mask->matrix[i]);
    }
    wmove(uc->window, MASK_POSY + uc->mask->cursor_y, MASK_POSX + uc->mask->cursor_x); //move cursor to right position
    box(uc->window, 0, 0);
    wrefresh(uc->window);
    return 0;
}

int input_buffer[CNTRL_SIGN_BUFFER_SIZE];

int openw_handle_input(UserControl uc, wchar_t input, int crm){
    wchar_t ch = input;
    Document* doc = uc->doc;
    //Guiw_mask* mask = uc->mask;

    int feedback = 0;
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
                    break;
                    }
                default:
                    break;
        }
    }
    return R_OK;
}