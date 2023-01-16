#ifndef SAVEWIN
#define SAVEWIN
#include "main.h"
#include "guim.h"
#include "utils.h"

int savew_init(UserControl uc);
int savew_deinit(UserControl uc);
int savew_reinit(UserControl uc);
int savew_draw(UserControl uc);

int savew_handle_input(UserControl uc, wchar_t input, int crm);



#endif // SAVEWIN

