#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#include "include/zfetch.h"
#include "include/zfetch-config.h"

void _segv(int code) {
  printf(" \e[1;31m[e]\e[0m An unkown error occured. Please report this bug to \e[1mhttps://github.com/Amchik/zfetch/issues\e[0m [%d]\n",
      code);
  exit(2);
}

char* read_file(const char* filename) {
  FILE* f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  //same as rewind(f);

  char* string = (char*)malloc(fsize + 1);
  fread(string, fsize, 1, f);
  fclose(f);

  string[fsize] = 0;
  return(string);
}

int main(int argc, char* argv[]) {
  char* userhome = get_user_home();
  char* logofile = malloc(strlen(userhome) + strlen(config_dir) + strlen(logo_file_name));
  sprintf(logofile, "%s%s%s", userhome, config_dir, logo_file_name);

//  signal(SIGSEGV, _segv);
//  signal(SIGABRT, _segv);
//  signal(SIGIOT, _segv);

  if (argc > 1 && strcmp(argv[1], "--init-base-dirs") == 0) {
    printf(" \e[0;34m[i]\e[0m Creating base directories...\n");
    init_base_dirs();
    return(0);
  }

  if (!has_base_dirs()) {
    printf(" \e[0;33m[w]\e[0m Installation corrupted: failed to locate %s%s... files\n",
        get_user_home(), config_dir);
    printf(" \e[0;34m[i]\e[0m You can regenerate it using \e[1m--init-base-dirs\e[0m. This action will be overwrites exists files\n");
    return(1);
  }
  
  zfconfig* cfg = parse_config();
  char* title;
  if (strcmp(zfconfig_get_key(cfg, "info.autotitle"), "yes") == 0) {
    title = "zfetch 1";
  } else {
    title = zfconfig_get_key(cfg, "info.title");
    if (!title) title = "<failed to parse>";
  }

  logo* lgo = mk_logo(7, 13, 0);
  lgo->content = read_file(logofile);

  info_file* _inf = parse_info_file();
  info* inf = mk_info(_inf->lines, 0, zfconfig_get_key(cfg, "info.separator"));
  inf->content = _inf->content; 

  card* crd = mk_card(inf, lgo, title, 1);

  if (inf->separator == 0) inf->separator = ": ";
  if (strcmp(zfconfig_get_key(cfg, "info.bold"), "yes") == 0) inf->bold = 1;

  prin_card(crd);
  prin_clrs(2);

  destroy_logo(lgo);
  destroy_info(inf);
  destroy_card(crd);
  return(0);
}
