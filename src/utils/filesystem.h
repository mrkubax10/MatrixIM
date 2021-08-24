#ifndef SRC_UTILS_FILESYSTEM_H
#define SRC_UTILS_FILESYSTEM_H
#include <stdbool.h>
#include <stdio.h>
void createFolder(char* path);
bool folderExists(char* path);
char* getConfigDir();
char* loadFullFile(char* path);
#endif