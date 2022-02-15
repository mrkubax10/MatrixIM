#ifndef SRC_UTILS_STR_H
#define SRC_UTILS_STR_H
char** split(char* str,char ch,int* len);
char** splitByString(char* str,char* str2,int* len);
char* intToString(int num);
#endif