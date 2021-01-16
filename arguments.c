#include <stdlib.h>
#include <string.h>

#include "include/arguments.h"

struct args* mk_args() {
  struct args* args = malloc(sizeof(struct args));
  args->n = 0;
  args->elements = 0;
  return(args);
}

void append_argument(struct args *args, char *arg, void (*handler)(int, char **, size_t *)) {
  args->n++;
  args->elements = realloc(args->elements, sizeof(struct arg_t) * (args->n + 1));
  
  struct arg_t argr = {.arg = arg, .handler = handler };
  args->elements[args->n - 1] = argr;
}

bool execute_argument(struct args* args, int argc, char *argv[], size_t* n) {
  for (int i = 0; i < args->n; i++) {
    if (strcmp(args->elements[i].arg, argv[*n]) == 0) {
      args->elements[i].handler(argc, argv, n);
      return(true);
    }
  }
  return(false);
}
