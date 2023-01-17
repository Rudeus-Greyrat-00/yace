#include "gotolwin.h"

#define GOTOW_DEF_POSX 8
#define GOTOW_DEF_POSY 2

#define GOTOW_WIDTH 40
#define GOTOW_HEIGH 10

int gotow_init(UserControl uc){
    WINDOW *gotowin = create_window(GOTOW_HEIGH, GOTOW_WIDTH, GOTOW_DEF_POSY, GOTOW_DEF_POSX);
    Guiw_mask *mask = alloc_guiw_mask(1, MAX_ROW_GOTOL_INPUT);
    if(gotowin == NULL || mask == NULL) {
        if(mask != NULL) dealloc_guiw_mask(mask);
        return -1;
    }
    uc->window = gotowin;
    uc->mask = mask;
    return 0;
}
int gotow_deinit(UserControl uc){
    dealloc_guiw_mask(uc->mask);
    dealloc_document(uc->doc);
    return 0;
}
int gotow_reinit(UserControl uc){
    gotow_deinit(uc);
    return gotow_reinit(uc);
}

int check_input(UserControl uc){
    char val[2 * MAX_ROW_GOTOL_INPUT];
    int max_value = g_usercontrol_stack[0]->doc->size; //size of the document
    int news = wchar_to_char(uc->doc->lines[0]->string, val, 2 * MAX_ROW_GOTOL_INPUT);
    if(news > 0 && news <= MAX_ROW_GOTOL_INPUT && atoi(val) < max_value){
        return atoi(val);
    }
    return -1;
}

int gotow_draw(UserControl uc){
    int max_value = g_usercontrol_stack[0]->doc->size; //size of the document
    update_mask(uc->mask, uc->doc);
    werase(uc->window);
    wmove(uc->window, 1, 1);
    wprintw(uc->window, "Go to line:");
    wmove(uc->window, 3, 1);
    wprintw(uc->window, "0 >= ");
    wprintw(uc->window, "%ls", uc->mask->matrix[0]);
    wprintw(uc->window, " >= %d", max_value - 1);
    char val[2 * MAX_ROW_GOTOL_INPUT];
    int news = wchar_to_char(uc->doc->lines[0]->string, val, 2 * MAX_ROW_GOTOL_INPUT);
    if(news > 0 && news <= MAX_ROW_GOTOL_INPUT && atoi(val) < max_value); //ok
    else if(news > 0){
        wmove(uc->window, 5, 1);
        wprintw(uc->window, "Index out of range");
    }
    wmove(uc->window, 7, 1);
    wprintw(uc->window, "ENTER to confirm, CTRL+Q to cancel");

    wmove(uc->window, 3, 6 + uc->mask->cursor_x);
    box(uc->window, 0, 0);
    wrefresh(uc->window);
    return 0;
}

int input_buffer[CNTRL_SIGN_BUFFER_SIZE];

int gotow_handle_input(UserControl uc, wchar_t input, int crm){
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
                    int y;
                    if((y = check_input(uc)) != -1){
                        Document* curr_doc = g_usercontrol_stack[0]->doc;
                        curr_doc->cursor_y = y;
                        curr_doc->cursor_x = 0;
                        GUI_REMOVE_WINDOW();
                    }
                    break;
                    }
                default:
                    if(iswdigit(ch)) doc_add_character(doc, ch);
        }
    }
    return R_OK;
}
