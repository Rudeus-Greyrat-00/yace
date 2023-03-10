#ifndef FILEM
#define FILEM
#include "../main.h"
#include "../datam/docm.h"

#define ERR_NOT_FOUND -1

//int load_file_path(char* path, Document *doc);
//int save_file_path(char* path, Document *doc);
int save_file_name(char* filename, Document *doc);
int load_file_name(char* fname, Document* doc);

int save_file(Document* doc);

int file_exist_with_same_name(char* str);

#endif // FILEM
