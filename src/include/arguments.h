#ifndef _ZF_H_ARGUMENTS
#define _ZF_H_ARGUMENTS

#include <stdio.h>
#include <stdbool.h>

struct arg_t {
  void (*handler) (int, char*[], size_t*);
  char* arg;
};

struct args {
  size_t n;
  struct arg_t* elements;
};

struct args* mk_args();

void append_argument(struct args* args, char* arg, void (*handler) (int, char*[], size_t*));

bool execute_argument(struct args* args, int argc, char *argv[], size_t* n);

#endif
