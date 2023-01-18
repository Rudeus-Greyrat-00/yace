#ifndef OPENWIN
#define OPENWIN
#include "../main.h"
#include "../visual/guim.h"
#include "../utils/utils.h"
#include "../utils/filem.h"
#include "../visual/guim.h"

int openw_init(UserControl uc);
int openw_deinit(UserControl uc);
int openw_reinit(UserControl uc);
int openw_draw(UserControl uc);

int openw_handle_input(UserControl uc, wchar_t input, int crm);



#endif // SAVEWIN