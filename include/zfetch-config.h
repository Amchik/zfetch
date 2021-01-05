#ifndef _ZF_H_ZFETCH_CONFIG
#define _ZF_H_ZFETCH_CONFIG

#include <stdbool.h>

static char* config_dir = "/.zfetch/"; // ~/.zfetch/
static char* default_logo = "(B[m[30m      ___(B[m\n(B[m[30m     ((B[m[37m.. (B[m[30m\\(B[m\n(B[m[30m     ((B[m[33m<> (B[m[30m|(B[m\n(B[m[30m    /(B[m[37m/  \\ (B[m[30m\\(B[m\n(B[m[30m   ( (B[m[37m|  | (B[m[30m/|(B[m\n(B[m[33m  _(B[m[30m/\\ (B[m[37m__)(B[m[30m/(B[m[33m_(B[m[30m)(B[m\n(B[m[33m  \\/(B[m[30m-____(B[m[33m\\/(B[m\n\nThanks ufetch for this art. (ufetch-linux)\n";
static int default_logo_geometry[2] = {7, 13};

static char* main_file_name = "zfconfig";
static char* logo_file_name = "logo";
static char* info_file_name = "info";

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

void init_base_dirs();
bool has_base_dirs();

#endif
