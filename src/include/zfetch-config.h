#ifndef _ZF_H_ZFETCH_CONFIG
#define _ZF_H_ZFETCH_CONFIG

#include <stdbool.h>

struct _zfconfig {
  int keys;

  char** values;
};
typedef struct _zfconfig zfconfig;

struct _info_file {
  int lines;

  char** content;
};
typedef struct _info_file info_file;

zfconfig* parse_config(const char* confdir);
info_file* parse_info_file(const char* _ifl);

char* zfconfig_get_key(const zfconfig* config, const char* key);

char* get_user_name();
char* get_host_name();

char* get_user_home();

bool init_zfconfig(const char* _out);
bool init_info(const char* _out);
bool init_logo(const char* _out);

unsigned char init_base_dirs();
bool has_base_dirs();

#endif
