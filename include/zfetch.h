#ifndef __ZF_H_ZFETCH
#define __ZF_H_ZFETCH

// structures

struct _logo {
  int height;
  int width;
  
  int transparent: 1;

  char* content;
};
typedef struct _logo logo;

struct _info {
  int lines;
  int bold: 1;
  
  char* separator;

  char** content;
};
typedef struct _info info;

struct _card {
  info* inf;
  logo* lgo;

  // 0 - left, 1 - right
  int lgo_pos: 1;

  char* head;
};
typedef struct _card card;

// constructors and deconstructors

logo* mk_logo(int height, int width, unsigned char transparent);
info* mk_info(int lines, unsigned char bold, char* separator);
card* mk_card(info* inf, logo* lgo, char* head, unsigned char lgo_pos);

void destroy_logo(logo* instance);
void destroy_info(info* instance);
void destroy_card(card* instance);

// struct _logo

// struct _info
void info_append(info* instance, char* key, char* value);

// struct _card

// printing
void prin_info(info* instance);
void prin_logo(logo* instance);

void prin_card(card* crd);

// utils
void prin_clrs(unsigned char rows);

#endif
