#ifndef __ZF_H_ZFETCH
#define __ZF_H_ZFETCH

#include <stdbool.h>

// structures

struct _logo {
  int height;
  int width;
  
  bool transparent;

  char* content;
};
typedef struct _logo logo;

struct _info {
  int lines;
  bool bold;
  
  char* separator;

  char** content;
};
typedef struct _info info;

struct _clrscm {
  char primary;
  char secondary;
  char separator;
  char header;
  char border;
};
typedef struct _clrscm clrscm;

struct _card {
  info* inf;
  logo* lgo;

  // is right
  bool lgo_pos;

  char* head;
};
typedef struct _card card;

// constructors and deconstructors

logo* mk_logo(int height, int width, bool transparent);
info* mk_info(int lines, bool bold, char* separator);
card* mk_card(info* inf, logo* lgo, char* head, bool lgo_pos);

void destroy_logo(logo* instance);
void destroy_info(info* instance);
void destroy_card(card* instance);

// struct _logo

// struct _info
void info_append(info* instance, const char* key, const char* value);

// struct _card

// printing
void prin_info(const info* instance);
void prin_logo(const logo* instance);

void prin_card(const card* crd, const clrscm* clrs);

// utils
void prin_clrs(unsigned char rows);

#endif
