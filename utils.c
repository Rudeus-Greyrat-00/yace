#include "utils.h"
#include "main.h"
#include <string.h>

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
