#include "docwin.h"
#include <string.h>


int generate_info_string(Document* doc, char* str){
    sprintf(str, "[Line: %d, Col: %d]─", doc->cursor_y, doc->cursor_x);
    strcat(str, "[UTF-8]─");
    switch(doc->mode){
        case MODE_DETECT_LINUX:
            strcat(str, "[Unix/Linux (LF)]");
            break;
        case MODE_DETECT_WINDOWS:
            strcat(str, "[Windows (CRLF)]");
            break;
        case MODE_DETECT_MACOS:
            strcat(str, "[MacOS (CR)]");
            break;
        case MODE_DETECT_MIXED:
            strcat(str, "[Undetermined]");
            break;
    }
    return strlen(str);
}

int generate_title_string(char* docname, char* str, int unsaved){
    sprintf(str, "[%s", docname);
    if(unsaved) strcat(str, "*");
    strcat(str, "]─[yace v.");
    strcat(str, YACE_VER);
    strcat(str, "]");
    return strlen(str);
}

void draw_docw(WINDOW* document_window, Document* doc, Guiw_mask* mask){
    update_mask(mask, doc);
    box(document_window, 0, 0);
    int n_line_gt_max = false;
    int mask_posx_gt_zero = mask->posx > 0;
    for(int j = mask->posy, i = 0; j < mask->posy + mask->rows; i++, j++){ //print line numbers
        n_line_gt_max = j > MAX_LINES_N;
        wmove(document_window, DEF_DOCW_OFF_Y + i, DEF_DOCW_OFF_X);
        if(n_line_gt_max) wprintw(document_window, "%d     ", MAX_LINES_N);
        else wprintw(document_window, "%d     ", j);
        wmove(document_window, DEF_DOCW_OFF_Y + i, DEF_DOCW_OFF_X + DEF_CURSOR_OFF_X - 1);
        if(n_line_gt_max) wprintw(document_window, "+");
        else if(mask_posx_gt_zero){
            if(i % 2) wprintw(document_window, "<");
            else wprintw(document_window, " ");
        }
        else wprintw(document_window, ":");
    }
    for(int i = 0; i < mask->rows; i++){ //print document text from mask matrix
        wmove(document_window, DEF_DOCW_OFF_Y + DEF_CURSOR_OFF_Y + i, DEF_DOCW_OFF_X + DEF_CURSOR_OFF_X);
        wprintw(document_window, "%ls", mask->matrix[i]);
    }
    int x, y; //start printing last line (info string)
    getmaxyx(document_window, y, x);
    char info_string[256];
    int strsize = generate_info_string(doc, info_string);
    wmove(document_window, y - 1, x - strsize + 3); //this is definetely not right
    wprintw(document_window, "%s", info_string);
    char title_string[256]; //start printing title string
    strsize = generate_title_string(doc->docname, title_string, doc->unsaved);
    wmove(document_window, 0, x - strsize + 1); //this is definitely not right, like the other line. IDK why they are working this way.
    wprintw(document_window, "%s", title_string);
    wmove(document_window, DEF_DOCW_OFF_Y + DEF_CURSOR_OFF_Y + mask->cursor_y, DEF_DOCW_OFF_X + DEF_CURSOR_OFF_X + mask->cursor_x); //move cursor to right position
    wrefresh(document_window);
}

void initialize_gui_docw(WINDOW** docwin, Guiw_mask** mask){
    //int initial_row = DEF_DOCW_OFF_Y + DEF_CURSOR_OFF_Y, initial_col = DEF_DOCW_OFF_X + DEF_CURSOR_OFF_X;
    int yaceditor_x = DEF_DOCW_OFF_X - 1, yaceditor_y = DEF_DOCW_OFF_Y - 1;
    int docw, doch;
    getmaxyx(stdscr, doch, docw);

    if(doch < MIN_DOCW_HEIGH) doch = MIN_DOCW_HEIGH;
    if(docw < MIN_DOCW_WIDTH) docw = MIN_DOCW_WIDTH;

    docw -= DEF_DOCW_OFF_X;
    doch -= DEF_DOCW_OFF_Y;

    *docwin = create_window(doch, docw, yaceditor_y, yaceditor_x);                              ///TODO check if allocation succeded for both
    *mask = alloc_guiw_mask(doch - 2 - DEF_CURSOR_OFF_Y, docw - 2 - DEF_CURSOR_OFF_X);
}

void deinitialize_gui_docw(WINDOW** docwin, Guiw_mask** mask){
    dealloc_guiw_mask(*mask);
}

void re_initialize_gui_docw(WINDOW** docwin, Guiw_mask** mask){
    deinitialize_gui_docw(docwin, mask);
    initialize_gui_docw(docwin, mask);
}

//INTERFACE

int input_buffer[CNTRL_SIGN_BUFFER_SIZE];

int docw_init(UserControl uc){
    WINDOW *window;
    Guiw_mask *mask;
    initialize_gui_docw(&window, &mask);
    if(window == NULL || mask == NULL) {
        if(mask != NULL) dealloc_guiw_mask(mask);
        return -1;
    }
    uc->window = window;
    uc->mask = mask;
    reset_cnt_buffer(input_buffer);
    return 0;
}

int docw_draw(UserControl uc){
    draw_docw(uc->window, uc->doc, uc->mask);
    return 0;
}

int docw_deinit(UserControl uc){
    deinitialize_gui_docw(&(uc->window), &(uc->mask));
    return 0;
}

int docw_reinit(UserControl uc){
    docw_deinit(uc);
    return docw_init(uc);
}

int docw_handle_input(UserControl uc, wchar_t input, int crm){
    wchar_t ch = input;
    int control_receiving_mode = crm;
    Document* doc = uc->doc;
    Guiw_mask* mask = uc->mask;

    //temp debug (comment GUI_DRAW on yace.c)
    //wprintw(uc->window, "%d ", ch);
    //wrefresh(uc->window);
    //return 0;

    if(ch == ESCAPE_KEY) reset_cnt_buffer(input_buffer);
    if(control_receiving_mode && ch != ESCAPE_KEY){
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
                    case A_KEY_UP:
                        doc_cursor_move_up(doc, C_MODE_STD, false);
                        break;
                    case A_KEY_DOWN:
                        doc_cursor_move_down(doc, C_MODE_STD, false);
                        break;
                    case CTRL_A_KEY_LEFT:
                        doc_cursor_move_left(doc, C_MODE_CTRL, false);
                        break;
                    case CTRL_A_KEY_RIGHT:
                        doc_cursor_move_right(doc, C_MODE_CTRL, false);
                        break;
                    case CTRL_A_KEY_UP:
                    case KEY_PGUP:{
                        int movemask = doc_pageup(doc, mask->rows);
                        if(movemask) move_guiw_mask(mask, 0, -mask->rows);
                        break;
                    }
                    case CTRL_A_KEY_DOWN:
                    case KEY_PGDOWN:{
                        int movemask = doc_pagedown(doc, mask->rows);
                        if(movemask) move_guiw_mask(mask, 0, +mask->rows);
                        break;
                    }
                }
                reset_cnt_buffer(input_buffer);
            }
        }
        else{
            int feedback = 0;
            switch(ch){
                case QUIT:
                    return R_QUIT;
                    break;
                case SELECT_ALL:
                    break;
                case COPY:
                    break;
                case PASTE:
                    break;
                case BACKSPACE_KEY:
                    feedback = doc_remove_character(doc);
                    if(feedback == FEEDB_DELLAST_LINE) move_guiw_mask(mask, 0, -1);
                    else if(feedback == FEEDB_DELLAST_CHAR) move_guiw_mask(mask, -1, 0);
                    break;
                case ENTER_KEY:
                    doc_cursor_enter(doc);
                    break;
                case REFRESH:
                    return R_REFRESH;
                case SAVE:
                    feedback = save_file_name(doc->docname, doc);
                    if(feedback < 0) return feedback;
                case SAVE_WITH_NAME:
                    GUI_ADD_WINDOW(UC_NAME_SWN);
                    break;
                case GOTOL:
                    GUI_ADD_WINDOW(UC_NAME_GOTOL);
                    break;
                case OPEN:
                    GUI_ADD_WINDOW(UC_NAME_OPEN);
                    break;
                default:
                    if(is_valid_character(ch) || ch == '\t') doc_add_character(doc, ch);
            }
        }
    return R_OK;
}
