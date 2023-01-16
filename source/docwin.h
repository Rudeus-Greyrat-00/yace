#ifndef DOCWIN
#define DOCWIN
#include "main.h"
#include "maskm.h"
#include "guim.h"

int docw_init(UserControl uc);
int docw_deinit(UserControl uc);
int docw_reinit(UserControl uc);
int docw_draw(UserControl uc);

int docw_handle_input(UserControl uc, wchar_t input, int crm);


#endif // DOCWIN
