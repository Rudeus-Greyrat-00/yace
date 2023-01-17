#ifndef GOTOWIN
#define GOTOWIN
#include "../main.h"
#include "../visual/guim.h"
#include "../utils/utils.h"
#include "../utils/filem.h"
#include "../visual/guim.h"

int gotow_init(UserControl uc);
int gotow_deinit(UserControl uc);
int gotow_reinit(UserControl uc);
int gotow_draw(UserControl uc);

int gotow_handle_input(UserControl uc, wchar_t input, int crm);

#endif // SAVEWIN
