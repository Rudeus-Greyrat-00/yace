#include "openwin.h"
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#define OPENW_DEF_POSX 8
#define OPENW_DEF_POSY 1

#define OPENW_WIDTH 80
#define OPENW_HEIGH 18

#define MASK_POSX 5
#define MASK_POSY 3

#define MASK_H 13
#define MASK_W 75

#define MAX_PATHLENGHT_STRING 55

#define WAS_A_FILE 1
#define WAS_A_DIR 2

void generate_path_str(char* dest){
    if(strlen(g_current_directory) > MAX_PATHLENGHT_STRING){
        sprintf(dest + 2, "%s", g_current_directory + strlen(g_current_directory) - MAX_PATHLENGHT_STRING + 2);
        dest[0] = '.';
        dest[1] = '.';
    }
    else sprintf(dest, "%s", g_current_directory);
}

int compile_file_list(UserControl uc){
    if(uc->doc != NULL){
        dealloc_document(uc->doc);
    }
    uc->doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
    if(uc->doc == NULL) return -1; 
    DIR *d;
    struct dirent *dir;
    d = opendir(g_current_directory);
    if(d == NULL) return -1;
    errno = 0;
    while ((dir = readdir(d)) != NULL){
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
        if(dir->d_type != DT_REG && dir->d_type != DT_DIR) continue;
        int sz = strlen(dir->d_name);
        if(dir->d_type == DT_DIR){
            int result = doc_add_character(uc->doc, '/');
            if(result < 0) {
                closedir(d);
                return result;
            }
        }
        for(int i = 0; i < sz; i++){
            int result = doc_add_character(uc->doc, (wchar_t)dir->d_name[i]);
            if(result < 0) {
                closedir(d);
                return result;
            }
        }
        int result = doc_cursor_enter(uc->doc);
        if(result < 0) {
            closedir(d);
            return result;
        }
    }
    if(errno != 0) return -1;
    closedir(d);
    int result = doc_remove_character(uc->doc);
    if(result < 0) return result;
    uc->doc->cursor_x = 0;
    uc->doc->cursor_y = 0;
    return 0;
}

int get_dir_type(char* str){
    DIR* d;
    struct dirent *dir;
    d = opendir(g_current_directory);
    if(d == NULL) return -1;
    while ((dir = readdir(d)) != NULL){
        if(str[0] == '/'){
            if(strcmp(dir->d_name, str + 1) == 0) { //str is always something like /namefolder, the / is added by the function compile_file_list in the document of this window. Doing +1 ignore that first character
                closedir(d);
                return dir->d_type;
            }
        }
        else{
            if(strcmp(dir->d_name, str) == 0) {
                closedir(d);
                return dir->d_type;
            }
        }
    }
    closedir(d);
    return -1;
}

int path_exists(char* str){
    DIR* d;
    d = opendir(str);
    if(d) {
        closedir(d);
        return true;
    }
    return false;
}

int on_press_enter(UserControl uc){
    if(uc->doc->size == 1 && uc->doc->lines[0]->_used == 0) return 0; //current folder is empty
    char dir[PATH_MAX];
    int news = wchar_to_char(uc->doc->lines[uc->doc->cursor_y]->string, dir, PATH_MAX);
    if(news <= 0) return -1;
    int dirtype = get_dir_type(dir);
    if(dirtype < 0) return -1;
    if(dirtype == DT_DIR){
        char new_path[PATH_MAX];
        strcpy(new_path, g_current_directory);
        if(strlen(new_path) > 1)strcat(new_path, dir);
        else strcat(new_path, dir + 1); //dir contains always "/" as first character. If we are in a folder, current directory will be something like /bla/bla/bla/folder, and then I want to add /newfolder, to get /bla/bla/bla/folder/newfolder
        //but in I'm in the root folder /, then I don't want to get //newfolder, so this is the reason of this if/else statements
        if(path_exists(new_path)){
            strcpy(g_current_directory, new_path);
            int result = compile_file_list(uc);
            if(result < 0) return result;
            return WAS_A_DIR;
        }
        else return -1; //something went wrong
    }
    else if(dirtype == DT_REG){
        dealloc_document(g_usercontrol_stack[0]->doc);
        g_usercontrol_stack[0]->doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
        if(g_usercontrol_stack[0]->doc == NULL) log_unfixable_error(true, "Memory allocation for main document failed [FROM openwin.c]");
        int result = load_file_name(dir, g_usercontrol_stack[0]->doc);
        if(result < 0) return result;
        return WAS_A_FILE;
    }
    return -1;
}

int on_press_ctrlup(UserControl uc){
    char new_dir[PATH_MAX];
    strcpy(new_dir, g_current_directory);
    if(strcmp(new_dir, "/") == 0) return 0; //can't go upper, but not an error
    int i = strlen(new_dir);
    while(new_dir[i] != '/'){
        new_dir[i] = '\0';
        i--;
    }
    if(i > 0) new_dir[i] = '\0';
    if(path_exists(new_dir)){
        strcpy(g_current_directory, new_dir);
        int result = compile_file_list(uc);
        if(result <= 0) return result;
        return 0;
    }
    return -1;
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
    wmove(uc->window, 2, 1);
    wprintw(uc->window, "Current directory: ");
    char display_path[MAX_PATHLENGHT_STRING];
    generate_path_str(display_path);
    wprintw(uc->window, "%s", display_path);
    for(int i = 0; i < uc->mask->rows; i++){ //print document text from mask matrix
        wmove(uc->window, MASK_POSY + i, MASK_POSX);
        wprintw(uc->window, "%ls", uc->mask->matrix[i]);
    }
    wmove(uc->window, MASK_POSY + uc->mask->cursor_y, 2); //move cursor to right position
    wprintw(uc->window, ">>");
    if(uc->doc->size == 1 && uc->doc->lines[0]->_used == 0) {
        wmove(uc->window, MASK_POSY, 1);
        wprintw(uc->window, "There is nothing inside this folder");
    }
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
                    case CTRL_A_KEY_UP:
                    case KEY_PGUP:
                        on_press_ctrlup(uc);
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
                    int result = on_press_enter(uc);
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