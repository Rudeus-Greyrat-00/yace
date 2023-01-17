#ifndef SAVEWIN
#define SAVEWIN
#include "../main.h"
#include "../visual/guim.h"
#include "../utils/utils.h"
#include "../utils/filem.h"
#include "../visual/guim.h"

int savew_init(UserControl uc);
int savew_deinit(UserControl uc);
int savew_reinit(UserControl uc);
int savew_draw(UserControl uc);

int savew_handle_input(UserControl uc, wchar_t input, int crm);



#endif // SAVEWIN
