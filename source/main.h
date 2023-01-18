#include <ncurses.h>
#include <stdio.h>
#include <locale.h>
#include <stddef.h>
#include <wchar.h>
#include <wctype.h>
#include <sys/time.h>
#include <stdlib.h>
#include <limits.h>

#define YACE_VER "0.2"

#define true 1
#define false 0

#define TAB_NSPACES 4

#define CNTRL_SIGN_BUFFER_SIZE 10
#define MAX_FNAME_SIZE 255
#define MAX_ROW_GOTOL_INPUT 10

#define MIN_DOCW_WIDTH 30
#define MIN_DOCW_HEIGH 10
#define DEF_DOCW_OFF_X 1
#define DEF_DOCW_OFF_Y 1
#define DEF_CURSOR_OFF_X 6
#define DEF_CURSOR_OFF_Y 0

#define DELTA 8000
#define STR_REALLOC_DEF_INTERVAL 1023

#define MAX_LINES_N 99999

#define QUIT 17 //CTRL Q
#define COPY 3 //CTRL C
#define PASTE 22 //CTRL V
#define SELECT_ALL 1 //CTRL A
#define ESCAPE_KEY 27
#define BACKSPACE_KEY 127
#define ENTER_KEY 13
#define REFRESH 18 //CTRL R
#define FIND 6 //CTRL F
#define SAVE 19 //CTRL S
#define SAVE_WITH_NAME 4 //CTRL D
#define CUT 24 //CTRL X
#define UNDO 26 //CTRL Z
#define GOTOL 7 //CTRL G
#define OPEN 15 //CTRL O

#define MODE_DETECT_LINUX 1
#define MODE_DETECT_WINDOWS 2
#define MODE_DETECT_MACOS 3
#define MODE_DETECT_MIXED 4

#define UC_STACK_SIZE 10

char g_current_directory[PATH_MAX];


