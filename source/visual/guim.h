#ifndef GUIM
#define GUIM
#include "../main.h"
#include "maskm.h"

struct UControl{
    WINDOW* window;
    Guiw_mask* mask;
    Document* doc;
    int (*handle_input)(struct UControl*, wchar_t input, int crm);
    int (*initialize)(struct UControl* uc);
    int (*deinitialize)(struct UControl* uc);
    int (*reinitialize)(struct UControl* uc);
    int (*draw)(struct UControl* uc);
    int name; //to easely identify different user controls
};

typedef struct UControl* UserControl;

UserControl g_usercontrol_stack[UC_STACK_SIZE];

WINDOW *create_window(int heigh, int width, int starty, int startx);

void GUI_INIT(); //init the GUI of the entire program
void GUI_DEINIT(); //deinit the GUI of the entire program
void GUI_REINIT();
void GUI_DRAW(); //draw the GUI of the entire program
int GUI_HANDLE_INPUT(wchar_t input, int crm); //take the last user control in the stack and call his input handler
int GUI_GET_CURRENT_NAME(); //return the name of the last user control in the stack

int GUI_ADD_WINDOW(int name); //create and add a window to the top of the stack
int GUI_REMOVE_WINDOW(); //return the index of the removed window on the stack. If didn remove anything, return -1

#define R_OK 0 //these values are returned by GUI_HANDLE_INPUT, they are evaluated in the main loop in yace.c
#define R_QUIT 1
#define R_REFRESH 2
#define R_SAVEFILE 3
#define R_CONFIRM 4
#define R_CANCEL 5

#define UC_NAME_UNNAMED 0
#define UC_NAME_MAINW 1 //the main window editor
#define UC_NAME_SWN 2 //save with name window
#define UC_NAME_GOTOL 3 //go to line window
#define UC_NAME_OPEN 4 //open file window
#define UC_NAME_CHANGEWD 5 //change working directory window

#endif // GUIM
