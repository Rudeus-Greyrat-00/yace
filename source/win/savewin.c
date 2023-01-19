#include "savewin.h"
#include <string.h>

#define SAVEW_DEF_POSX 8
#define SAVEW_DEF_POSY 1

#define SAVEW_WIDTH 80
#define SAVEW_HEIGH 9

#define MASK_H 13
#define MASK_W 74

#define MAX_PATHLENGHT_STRING 55

int savew_init(UserControl uc){
    WINDOW *savewin = create_window(SAVEW_HEIGH, SAVEW_WIDTH, SAVEW_DEF_POSY, SAVEW_DEF_POSX);
    Guiw_mask *mask = alloc_guiw_mask(MASK_H, MASK_W);
    if(savewin == NULL || mask == NULL) {
        if(mask != NULL) dealloc_guiw_mask(mask);
        return -1;
    }
    uc->window = savewin;
    uc->mask = mask;
    return 0;
}
int savew_deinit(UserControl uc){
    dealloc_guiw_mask(uc->mask);
    dealloc_document(uc->doc);
    return 0;
}
int savew_reinit(UserControl uc){
    savew_deinit(uc);
    return savew_init(uc);
}
int savew_draw(UserControl uc){
    update_mask(uc->mask, uc->doc);
    werase(uc->window);
    wmove(uc->window, 1, 1);
    wprintw(uc->window, "Save with name:");
    wmove(uc->window, 2, 1);
    wprintw(uc->window, "Current directory: ");
    char display_path[MAX_PATHLENGHT_STRING];
    generate_path_str(display_path, MAX_PATHLENGHT_STRING);
    wprintw(uc->window, "%s", display_path);
    wmove(uc->window, 3, 1);
    wprintw(uc->window, "%ls", uc->mask->matrix[0]);
    wmove(uc->window, 5, 1);
    int size = uc->doc->lines[0]->_used;
    if(size > 0 || size < MAX_FNAME_SIZE){
        char filename[MAX_FNAME_SIZE];
        int news = wchar_to_char(uc->doc->lines[0]->string, filename, MAX_FNAME_SIZE);
        if(news > 0){
            if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) wprintw(uc->window, "The names .. and . are reserved");
            else if(file_exist_with_same_name(filename)) wprintw(uc->window, "There exist a file with the same name in current directory\n By pressing [ENTER], the old file will be overwritten");
        }
            else if(size >= MAX_FNAME_SIZE){
            wprintw(uc->window, "Please write a shorter name");
        }
    }
    wmove(uc->window, 7, 1);
    wprintw(uc->window, "Press [ENTER] to confirm, [CTRL+Q] to cancel");

    wmove(uc->window, 3, 1 + uc->mask->cursor_x);
    box(uc->window, 0, 0);
    wrefresh(uc->window);
    return 0;
}

int input_buffer[CNTRL_SIGN_BUFFER_SIZE];

int savew_handle_input(UserControl uc, wchar_t input, int crm){
    wchar_t ch = input;
    Document* doc = uc->doc;
    Guiw_mask* mask = uc->mask;

    int feedback = 0;
    if(ch == ESCAPE_KEY) reset_cnt_buffer(input_buffer);
    if(crm && ch != ESCAPE_KEY){
            cnt_buffer_addchar(input_buffer, ch);
            int action = check_action(input_buffer);
            if(action != -1){ //if an action is recognised
                switch(action){
                    case A_KEY_LEFT:
                        doc_cursor_move_left(doc, C_MODE_STD, false);
                        break;
                    case A_KEY_RIGHT:
                        doc_cursor_move_right(doc, C_MODE_STD, false);
                        break;
                    case CTRL_A_KEY_LEFT:
                        doc_cursor_move_left(doc, C_MODE_CTRL, false);
                        break;
                    case CTRL_A_KEY_RIGHT:
                        doc_cursor_move_right(doc, C_MODE_CTRL, false);
                        break;
                    case CTRL_A_KEY_UP:
                    case KEY_PGUP:
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
                case BACKSPACE_KEY:
                    feedback = doc_remove_character(doc);
                    if(feedback == FEEDB_DELLAST_LINE) move_guiw_mask(mask, 0, -1);
                    else if(feedback == FEEDB_DELLAST_CHAR) move_guiw_mask(mask, -1, 0);
                    break;
                case ENTER_KEY:{
                    int size = uc->doc->lines[0]->_used;
                    if(size == 0 || size >= MAX_FNAME_SIZE) break;
                    char filename[MAX_FNAME_SIZE];
                    int news = wchar_to_char(uc->doc->lines[0]->string, filename, MAX_FNAME_SIZE);
                    if(news <= 0) break;
                    if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) break;
                    int result = save_file_name(filename, g_usercontrol_stack[0]->doc);
                    if(result >= 0) GUI_REMOVE_WINDOW();
                    break;
                    }
                default:
                    if(is_valid_character(ch) && ch != '/') doc_add_character(doc, ch);
        }
    }
    return R_OK;
}
