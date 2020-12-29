#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void prin_logo(logo *instance) {
  //printf("%s\n", instance->content);
  //return;
  char* point = instance->content - 1;
  for (int i = 0; i < instance->height; i++) {
    // wait_for_escape_end
    char wfee = 0;
    for (int j = 0; j < instance->width;) {
      char* c = ++point;
      //printf("%c=>", *c);
      //printf("%d\n", (c - instance->content));
      //j++;
      //continue;
      if (*c == '\n') {
        if (j == 0) continue;
        if (wfee) {
          wfee = 0;
          printf("m");
        }
        if (!instance->transparent &&
            j - 1 != instance->width) {
          for (; j < instance->width; j++) {
            printf(" ");
          } // todo: optimize
        }
        break;
      }
      if (*c == 0) continue;
      printf("%c", *c);
      //printf("%d\n", c - instance->content);
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
    //printf("--\\n--");
    printf("\n");
  }
}

int _max(int a, int b) {
  if (a > b) return(a);
  return(b);
}

void _prin_crd_l(card* crd) {
  logo* lgo = crd->lgo;
  info* inf = crd->inf;

  char* bld;
  if (inf->bold) bld = "\e[1m";
  else bld = "";

  int width = lgo->width;
  int height = _max(lgo->height, inf->lines + 2);

  char* lgoc = lgo->content - 1;
  for (int i = 0; i < height; i++) {
    if (i >= lgo->height)
      for(int j = 0; j < width; j++)
        printf(" ");
    else {
      char wfee = 0;
      for (int j = 0; j < width;) {
        char* c = ++lgoc;
        //printf("%c=>", *c);
        //printf("%d\n", (c - instance->content));
        //j++;
        //continue;
        if (*c == '\n') {
          if (j == 0) continue;
          if (wfee) {
            wfee = 0;
            printf("m");
          }
          if (!lgo->transparent &&
              j - 1 != width) {
            for (; j < width; j++) {
              printf(" ");
            } // todo: optimize
          }
          break;
        }
        if (*c == 0) continue;
        printf("%c", *c);
        //printf("%d\n", c - instance->content);
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
    printf("\e[0m ");
    if (i == 0) printf("| \e[1m%s\e[0m", crd->head);
    else if (i == 1) {
      printf("|");
      for (int p = 1; p < strlen(crd->head) + 2; p++)
        printf("-");
    }
    else if (i > 1 && (i - 2) < inf->lines) {
      char** key = inf->content + (i - 2) * 2;
      printf("| %s%s\e[0m%s%s", bld, *key, inf->separator, *(key + 1));
    }
    printf("\n");
  }
}
void prin_card(card* crd) {
  if (crd->lgo_pos) _prin_crd_l(crd);
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
