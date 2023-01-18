#include "../visual/guim.h"
#include "../win/docwin.h"
#include "../win/savewin.h"
#include "../win/gotolwin.h"
#include "../win/openwin.h"
#include <string.h>

//GUI UTILS

WINDOW *create_window(int heigh, int width, int starty, int startx){
    WINDOW *new_win = newwin(heigh, width, starty, startx);
    box(new_win, 0, 0);
    wrefresh(new_win);
    return(new_win);
}

//GUI MEMORY MANAG

UserControl alloc_user_control(){
    UserControl control = (UserControl) malloc(sizeof(struct UControl));
    control->name = UC_NAME_UNNAMED;
    return control;
}

void dealloc_user_control(UserControl uc){
    if(uc != NULL) free(uc);
}

//CREATING USER CONTROLS

UserControl create_docw_usercontrol(Document* doc){  //create the document window user control
    UserControl uc = alloc_user_control();
    if(uc == NULL) return NULL;
    uc->doc = doc;
    uc->initialize = docw_init;
    uc->deinitialize = docw_deinit;
    uc->reinitialize = docw_reinit;
    uc->draw = docw_draw;
    uc->handle_input = docw_handle_input;
    uc->name = UC_NAME_MAINW;
    return uc;
}

UserControl create_savew_usercontrol(Document* doc){ //create the save with name window user control
    UserControl uc = alloc_user_control();
    if(uc == NULL) return NULL;
    if(doc == NULL) uc->doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
    else uc->doc = doc;
    if(uc->doc == NULL) {
        dealloc_user_control(uc);
        return NULL;
    }
    uc->initialize = savew_init;
    uc->deinitialize = savew_deinit;
    uc->reinitialize = savew_reinit;
    uc->draw = savew_draw;
    uc->handle_input = savew_handle_input;
    uc->name = UC_NAME_SWN;
    return uc;
}

UserControl create_gotow_usercontrol(Document* doc){ //create the goto line user control
    UserControl uc = alloc_user_control();
    if(uc == NULL) return NULL;
    if(doc == NULL) uc->doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
    else uc->doc = doc;
    if(uc->doc == NULL) {
        dealloc_user_control(uc);
        return NULL;
    }
    uc->initialize = gotow_init;
    uc->deinitialize = gotow_deinit;
    uc->reinitialize = gotow_reinit;
    uc->draw = gotow_draw;
    uc->handle_input = gotow_handle_input;
    uc->name = UC_NAME_GOTOL;
    return uc;
}

UserControl create_openw_usercontrol(Document* doc){ //create the goto line user control
    UserControl uc = alloc_user_control();
    if(uc == NULL) return NULL;
    if(doc == NULL) uc->doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
    else uc->doc = doc;
    if(uc->doc == NULL) {
        dealloc_user_control(uc);
        return NULL;
    }
    uc->initialize = openw_init;
    uc->deinitialize = openw_deinit;
    uc->reinitialize = openw_reinit;
    uc->draw = openw_draw;
    uc->handle_input = openw_handle_input;
    uc->name = UC_NAME_OPEN;
    return uc;
}

//ACTUAL GUI MANAGEMENT

void GUI_INIT(Document* doc){
    for(int i = 0; i < UC_STACK_SIZE; i++) g_usercontrol_stack[i] = NULL; 
    initscr();
    raw();				// Line buffering disabled
	keypad(stdscr, TRUE);		// We get F1, F2 etc..
	noecho();			// Don't echo() while we do getch
    //creating controls:
    UserControl docwindow = create_docw_usercontrol(doc); //the user control that manage the text editor
    docwindow->initialize(docwindow); //the user control that manage the text editor is always initialized (this is a text editor after all)
    //adding controls to the stack
    g_usercontrol_stack[0] = docwindow; //the user control that manage the text editor is always in this position
}

void GUI_DEINIT(Document* doc){
    int i = 0;
    while(g_usercontrol_stack[i] != NULL){
        g_usercontrol_stack[i]->deinitialize(g_usercontrol_stack[i]);
        dealloc_user_control(g_usercontrol_stack[i]);
        i++;
    }
    endwin();
}

void GUI_REINIT(Document* doc){
    Document* curr_doc;
    if(doc == NULL){
        curr_doc = g_usercontrol_stack[0]->doc;
    }
    else curr_doc = doc;
    GUI_DEINIT(curr_doc);
    GUI_INIT(curr_doc);
}

void GUI_DRAW(){
    int i = 0;
    while(g_usercontrol_stack[i] != NULL){
        g_usercontrol_stack[i]->draw(g_usercontrol_stack[i]);
        i++;
    }
}

int GUI_HANDLE_INPUT(wchar_t input, int crm){
    int i = 0;
    while(g_usercontrol_stack[i] != NULL){
        i++;
    }
    i--;
    if(i >= 0){
        return g_usercontrol_stack[i]->handle_input(g_usercontrol_stack[i], input, crm);
    }
    return -1;
}

int GUI_GET_CURRENT_NAME(){
    int i = 0;
    while(g_usercontrol_stack[i] != NULL){
        i++;
    }
    i--;
    if(i >= 0) return g_usercontrol_stack[i]->name;
    return -1;
}

void unfixable_error_handler(){
    GUI_DEINIT(NULL);
    dealloc_document(g_usercontrol_stack[0]->doc);
    perror("Error: usercontrol not found or allocation failed. Please report this error to the developer.");
    exit(-1);
}

int GUI_ADD_WINDOW(int name){ //add the selected window to the stack
    UserControl new_win;
    switch(name){
        case UC_NAME_SWN:
            new_win = create_savew_usercontrol(NULL);
            if(new_win == NULL) unfixable_error_handler();
            new_win->initialize(new_win);
            break;
        case UC_NAME_GOTOL:
            new_win = create_gotow_usercontrol(NULL);
            if(new_win == NULL) unfixable_error_handler();
            new_win->initialize(new_win);
            break;
        case UC_NAME_OPEN:
            new_win = create_openw_usercontrol(NULL);
            if(new_win == NULL) unfixable_error_handler();
            new_win->initialize(new_win);
            break;
        default:
            unfixable_error_handler();
    }
    if(new_win == NULL) return -1;
    int i = 1; //position 0 is always reserved for the default docw
    while(g_usercontrol_stack[i] != NULL){
        i++;
    }
    g_usercontrol_stack[i] = new_win;
    return 0;
}

int GUI_REMOVE_WINDOW(){ //remove the last window of the stack
    int i = 1;
    while(g_usercontrol_stack[i] != NULL){
        i++;
    }
    i--;
    if(i >= 1){
        g_usercontrol_stack[i]->deinitialize(g_usercontrol_stack[i]);
        dealloc_user_control(g_usercontrol_stack[i]);
        g_usercontrol_stack[i] = NULL;
        return i;
    }
    return -1;
}


