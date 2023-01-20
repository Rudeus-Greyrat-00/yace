#include "utils.h"
#include <string.h>
#include <dirent.h>
#include <errno.h>

int left_in[] = KEY_LEFT_INPUT, right_in[] = KEY_RIGHT_INPUT, up_in[] = KEY_UP_INPUT, down_in[] = KEY_DOWN_INPUT, c_left_in[] = CTRL_KEY_LEFT_INPUT, c_right_in[] = CTRL_KEY_RIGHT_INPUT, c_up_in[] = CTRL_KEY_UP_INPUT, c_down_in[] = CTRL_KEY_DOWN_INPUT, pgd[] = PGDOWN_INPUT, pgup[] = PGUP_INPUT;

int is_valid_character(wint_t character){
    return !iswcntrl(character);
}

int usec_diff(struct timeval start, struct timeval end){
    return abs((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
}

int check_control_mode(struct timeval start, struct timeval end){
    return usec_diff(start, end) < DELTA;
}
void reset_cnt_buffer(int* buffer){
    for(int i = 0; i < CNTRL_SIGN_BUFFER_SIZE; i++){
        buffer[i] = -1;
    }
}

int cnt_buffer_addchar(int* buffer, int character){
    int i = 0;
    for(i = 0; i < CNTRL_SIGN_BUFFER_SIZE; i++){
        if(buffer[i] == -1) break;
    }
    buffer[i] = character;
    return 0;
}

int compare_input_v(int* first, int* second){
    for(int i = 0; i < CNTRL_SIGN_BUFFER_SIZE; i++){
        if(first[i] != second[i]) return false;
        if(first[i] == -1 && second[i] == -1) return true;
    }
    return true;
}

int check_action(int* buffer){
    if(compare_input_v(buffer, left_in)) return A_KEY_LEFT;
    if(compare_input_v(buffer, right_in)) return A_KEY_RIGHT;
    if(compare_input_v(buffer, up_in)) return A_KEY_UP;
    if(compare_input_v(buffer, down_in)) return A_KEY_DOWN;
    if(compare_input_v(buffer, c_left_in)) return CTRL_A_KEY_LEFT;
    if(compare_input_v(buffer, c_right_in)) return CTRL_A_KEY_RIGHT;
    if(compare_input_v(buffer, c_up_in)) return CTRL_A_KEY_UP;
    if(compare_input_v(buffer, c_down_in)) return CTRL_A_KEY_DOWN;
    if(compare_input_v(buffer, pgd)) return KEY_PGDOWN;
    if(compare_input_v(buffer, pgup)) return KEY_PGUP;
    return -1;
}

int wchar_to_char(wchar_t* source, char* destination, int size){
    return (int) wcstombs(destination, source, (size_t)size);
}

void log_unfixable_error(int deinit_gui, char* error){
    Document* doc = g_usercontrol_stack[0]->doc;
    if(deinit_gui) GUI_DEINIT(NULL);
    dealloc_document(doc);
    printf("An error occurred and yace had to be closed. Yace version: ");
    printf(YACE_VER);
    printf("\nError (it would be like, really kind of you if you reported this to the developer): %s\n", error);
    exit(-1);
}

void generate_path_str(char* dest, char* source, int limit){
    if(strlen(source) > limit){
        sprintf(dest + 2, "%s", source + strlen(source) - limit + 2);
        dest[0] = '.';
        dest[1] = '.';
    }
    else sprintf(dest, "%s", source);
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

int compile_file_list(Document* doc, int directory_only, int starting_line){ //take a document, remove everything from starting_line and then write from starting_line the list of item in current g_current_directory
    if(doc == NULL || starting_line < 0) return -1; 
    if(doc->size > starting_line){ //remove everything over starting_line
        for(int i = doc->size - 1; i > starting_line; i--){
            int result = doc_remove_line(doc, i);
            if(result < 0) return result;
        }
        empty_string(doc->lines[doc->size - 1]); //last string
    }
    else if(doc->size <= starting_line){
        for(int i = doc->size; i <= starting_line; i++){
            int result = doc_add_line(doc, doc->size);
            if(result < 0) return result;
        }
    }
    doc->cursor_x = 0;
    doc->cursor_y = starting_line;
    DIR *d;
    struct dirent *dir;
    d = opendir(g_current_directory);
    if(d == NULL) return -1;
    errno = 0;
    while ((dir = readdir(d)) != NULL){
        if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
        if(dir->d_type != DT_REG && dir->d_type != DT_DIR) continue;
        if(directory_only && dir->d_type != DT_DIR) continue;
        int sz = strlen(dir->d_name);
        if(dir->d_type == DT_DIR){
            int result = doc_add_character(doc, '/');
            if(result < 0) {
                closedir(d);
                return result;
            }
        }
        for(int i = 0; i < sz; i++){
            int result = doc_add_character(doc, (wchar_t)dir->d_name[i]);
            if(result < 0) {
                closedir(d);
                return result;
            }
        }
        int result = doc_cursor_enter(doc);
        if(result < 0) {
            closedir(d);
            return result;
        }
    }
    if(errno != 0) return -1;
    closedir(d);
    int result = doc_remove_character(doc); //remove the last enter, if any
    if(result < 0 && doc->size > 1) return result; 
    doc->cursor_x = 0;
    doc->cursor_y = 0;
    return 0;
}

int get_dir_type(char* str){ //in current working directory
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

int move_current_directory_up(){
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
        return 0;
    }
    return -1;
}

int open_file_or_directory(char* dir, int directory_only){ //if the parameter string is the name of a file in current directory, it open it. Otherwise it goes into the directory and update g_current_directory
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
            return WAS_A_DIR;
        }
        else return -1; //something went wrong
    }
    else if(dirtype == DT_REG && !directory_only){
        dealloc_document(g_usercontrol_stack[0]->doc);
        g_usercontrol_stack[0]->doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
        if(g_usercontrol_stack[0]->doc == NULL) log_unfixable_error(true, "Memory allocation for main document failed [FROM openwin.c]");
        int result = load_file_name(dir, g_usercontrol_stack[0]->doc);
        if(result < 0) return result;
        return WAS_A_FILE;
    }
    return -1; //if function compile_file_list was used, this should never occur
}

void update_docdir(Document* doc){
    strcpy(doc->docdir, g_current_directory);
}
