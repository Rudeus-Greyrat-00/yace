#ifndef UTILS
#define UTILS
#include "../main.h"
#include "../visual/guim.h"

#define A_KEY_LEFT 1
#define A_KEY_RIGHT 2
#define A_KEY_UP 3
#define A_KEY_DOWN 4
#define CTRL_A_KEY_LEFT 5
#define CTRL_A_KEY_RIGHT 6
#define CTRL_A_KEY_UP 7
#define CTRL_A_KEY_DOWN 8
#define KEY_PGDOWN 9
#define KEY_PGUP 10

#define KEY_LEFT_INPUT {79, 68, -1, -1, -1, -1, -1, -1, -1}
#define KEY_RIGHT_INPUT {79, 67, -1, -1, -1, -1, -1, -1, -1}
#define KEY_UP_INPUT {79, 65, -1, -1, -1, -1, -1, -1, -1}
#define KEY_DOWN_INPUT {79, 66, -1, -1, -1, -1, -1, -1, -1}
#define CTRL_KEY_DOWN_INPUT {91, 49, 59, 53, 66, -1, -1, -1, -1}
#define CTRL_KEY_UP_INPUT {91, 49, 59, 53, 65, -1, -1, -1, -1}
#define CTRL_KEY_LEFT_INPUT {91, 49, 59, 53, 68, -1, -1, -1, -1}
#define CTRL_KEY_RIGHT_INPUT {91, 49, 59, 53, 67, -1, -1, -1, -1}
#define PGUP_INPUT {91, 53, 126, -1, -1, -1, -1, -1, -1}
#define PGDOWN_INPUT {91, 54, 126, -1, -1, -1, -1, -1, -1}

#define WAS_A_FILE 1
#define WAS_A_DIR 2

int is_valid_character(wint_t character);
int check_control_mode(struct timeval start, struct timeval end);

void reset_cnt_buffer(int* buffer);
int cnt_buffer_addchar(int* buffer, int character);
int check_action(int* buffer);
int wchar_to_char(wchar_t* source, char* destination, int size);

void log_unfixable_error(int deinit_gui, char* str);
void generate_path_str(char* dest, int limit);
int path_exists(char* str);
int get_dir_type(char* str);
int compile_file_list(Document* doc, int directory_only);
int open_file_or_directory(char* dir, int directory_only);
int move_current_directory_up();

#endif // UTILS
