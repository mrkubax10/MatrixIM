#ifndef SRC_UTILS_FILESYSTEM_H
#define SRC_UTILS_FILESYSTEM_H
#include <stdbool.h>
void createFolder(char* path);
bool folderExists(char* path);
char* getConfigDir();
#endif