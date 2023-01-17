#ifndef DOCWIN
#define DOCWIN
#include "../main.h"
#include "../visual/maskm.h"
#include "../visual/guim.h"
#include "../utils/utils.h"
#include "../utils/filem.h"

int docw_init(UserControl uc);
int docw_deinit(UserControl uc);
int docw_reinit(UserControl uc);
int docw_draw(UserControl uc);

int docw_handle_input(UserControl uc, wchar_t input, int crm);


#endif // DOCWIN
