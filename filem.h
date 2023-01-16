#ifndef FILEM
#define FILEM
#include "main.h"
#include "docm.h"

#define ERR_NOT_FOUND -1

int load_file(char* path, Document *doc);
int save_file(char* path, Document *doc);
int save_file_name(char* filename, Document *doc);
int load_file_name(char* fname, Document* doc);

int file_exist_with_same_name(char* str);

#endif // FILEM
