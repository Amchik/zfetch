#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/zfetch.h"

logo* mk_logo(int height, int width, unsigned char transparent) {
  if (transparent != 0 && transparent != 1) {
    return(0);
  }
  logo* i = (logo*)malloc(sizeof(logo));
  i->height = height;
  i->width = width;

  i->transparent = transparent;

  return i;
}
info* mk_info(int lines, unsigned char bold, char* separator) {
  info* i = (info*)malloc(sizeof(info));
  i->lines = lines;
  i->bold = bold;
  
  i->separator = separator;

  return i;
}
card* mk_card(info* inf, logo* lgo, char* head, unsigned char lgo_pos) {
  if (lgo_pos != 0 && lgo_pos != 1) return(0);
  card* crd = (card*)malloc(sizeof(card));

  crd->inf = inf;
  crd->lgo = lgo;

  crd->lgo_pos = lgo_pos;

  crd->head = head;

  return(crd);
}

void destroy_logo(logo* instance) {
  free(instance);
}
void destroy_info(info* instance) {
  free(instance);
}
void destroy_card(card* instance) {
  free(instance);
}

void info_append(info* instance, char* key, char* value) {
  // my code fucking sucks:
  //size_t objsize = strlen(key) + strlen(value);
  size_t objsize = 2 * sizeof(char*);
  if (instance->content == 0) {
    instance->content = (char**) malloc(objsize);
  } else {
    instance->content = realloc(instance->content, (instance->lines * 2 * sizeof(char*)) + objsize);
  }
  instance->lines++;

  instance->content[(instance->lines - 1) * 2] = key;
  instance->content[(instance->lines - 1) * 2 + 1] = value;
}

void prin_info(info* instance) {
  char* separator = instance->separator;
  char* bld;
  if (instance->bold == 0) bld = "";
  else bld = "\e[1m";

  for(int i = 0; i < ((instance->lines - 1) * 2 + 1); i += 2) {
    char** key = instance->content + i;
    printf("%s%s\e[0m%s%s\n", bld, *key, separator, *(key + 1));
  }
}
void _prin_lgo_ln(logo* lgo, size_t *pos) {
  char wfee = 0;
  for (int j = 0; j < lgo->width;) {
    char* c = (char*)((size_t)lgo->content + (*pos)++ - 1);
    if (*c == '\n') {
      if (j == 0) continue;
      if (wfee) {
        wfee = 0;
        printf("m");
      }
      if (!lgo->transparent &&
          j - 1 != lgo->width) {
        for (; j < lgo->width; j++) {
          printf(" ");
        } // todo: optimize
      }
      break;
    }
    if (*c == 0) continue;
    printf("%c", *c);
    if (*c == '\e') wfee = 1;
    if ( *c == 0             ||
        (*c == 'm' && wfee ) ||
         *c == '\e'          ||
         wfee                ){
      if (wfee && *c == 'm') wfee = 0;
      continue;
    }
    j++;
  }
}
void prin_logo(logo *instance) {
  size_t pos = 0;
  for (int i = 0; i < instance->height; i++) {
    _prin_lgo_ln(instance, &pos);
    printf("\n");
  }
}

int _max(int a, int b) {
  if (a > b) return(a);
  return(b);
}

int strdlen(char* string) {
  int dlen = 0;
  int rlen = strlen(string);
  bool escape = false;
  for (int i = 0; i < rlen; i++) {
    if (string[i] == '\e') {
      escape = true;
    } else if (escape && string[i] == 'm') {
      escape = false;
    } else if (!escape) {
      dlen++;
    }
  }
  return(dlen);
}
void _prin_crd_l(card* crd, clrscm* clrs) {
  logo* lgo = crd->lgo;
  info* inf = crd->inf;

  char* bld;
  if (inf->bold) bld = "\e[1m";
  else bld = "";

  int width = lgo->width;
  int height = _max(lgo->height, inf->lines + 2);

  size_t lgo_pos = 0;
  for (int i = 0; i < height; i++) {
    if (i >= lgo->height)
      for(int j = 0; j < width; j++)
        printf(" ");
    else {
      _prin_lgo_ln(lgo, &lgo_pos);
    }
    printf("\e[0;%dm ", clrs->border);
    if (i == 0) printf("| \e[1;%dm%s\e[0m", clrs->header, crd->head);
    else if (i == 1) {
      printf("\e[0;%dm|", clrs->border);
      int dlen = strdlen(crd->head);
      for (int p = 1; p < dlen + 2; p++)
        printf("-");
    }
    else if (i > 1 && (i - 2) < inf->lines) {
      char** key = inf->content + (i - 2) * 2;
      printf("\e[0;%dm| \e[0;%dm%s%s\e[0;%dm%s\e[0;%dm%s\e[0m", 
          clrs->border, clrs->primary, bld, *key, clrs->separator, inf->separator, clrs->secondary, *(key + 1));
    }
    printf("\n");
  }
}
void prin_card(card* crd, clrscm* clrs) {
  if (crd->lgo_pos) _prin_crd_l(crd, clrs);
  else puts("NI: sorry");
}


void prin_clrs(unsigned char rows) {
  for (int i = 0; i <= rows; i++) {
    for (int j = 0; j < 10; j++) {
      printf("\e[0;%dm   \e[0m", (40 + 60 * i) + j);
    }
    printf("\n");
  }
}
