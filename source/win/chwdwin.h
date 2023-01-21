#ifndef CHWDWIN
#define CHWDWIN
#include "../main.h"
#include "../visual/guim.h"
#include "../utils/utils.h"
#include "../utils/filem.h"
#include "../visual/guim.h"

int chwdw_init(UserControl uc);
int chwdw_deinit(UserControl uc);
int chwdw_reinit(UserControl uc);
int chwdw_draw(UserControl uc);

int chwdw_handle_input(UserControl uc, wchar_t input, int crm);



#endif // CHWDWIN