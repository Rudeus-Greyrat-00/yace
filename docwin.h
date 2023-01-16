#ifndef DOCWIN
#define DOCWIN
#include "main.h"
#include "maskm.h"
#include "guim.h"

//void initialize_gui_docw(WINDOW** docwin, Guiw_mask** mask);
//void deinitialize_gui_docw(WINDOW** docwin, Guiw_mask** mask);
//void re_initialize_gui_docw(WINDOW** docwin, Guiw_mask** mask);
//void draw_docw(WINDOW* document_window, Document* doc, Guiw_mask* mask);

int docw_init(UserControl uc);
int docw_deinit(UserControl uc);
int docw_reinit(UserControl uc);
int docw_draw(UserControl uc);

int docw_handle_input(UserControl uc, wchar_t input, int crm);


#endif // DOCWIN
