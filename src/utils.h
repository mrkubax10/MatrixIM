#ifndef SRC_UTILS_H
#define SRC_UTILS_H
#include <stdbool.h>
char** split(char* str,char ch,int* len);
void array_free(void** data,int len);
char* getConfigDir();
char* intToString(int num);
void createFolder(char* path);
bool folderExists(char* path);
#endif