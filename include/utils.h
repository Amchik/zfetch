#ifndef _ZF_H_UTILS
#define _ZF_H_UTILS

#include <stdbool.h>
#include <sys/types.h>

char* read_file(const char* filename);

char* get_pname(pid_t pid);
pid_t getppidof(pid_t pid);

bool strends(const char* str, const char* suffix);
int strdlen(const char* string);
char* strapnd(const char* s1, const char* s2);
char* strcapnd(char* original, char new);

#endif
