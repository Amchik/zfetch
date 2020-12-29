#include <stdio.h>
#include <stdlib.h>

#include "include/zfetch.h"

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

int main(void) {
  puts("ITS RUNS ONLY IN SHOWCASE MODE.");
  logo* lgo = mk_logo(7, 11, 0);
  info* inf = mk_info(0, 1, ": ");
  card* crd = mk_card(inf, lgo, "zfetch@opensuse", 1);

  lgo->content = read_file("logo.txt");

  info_append(inf, "os",    "suse");
  info_append(inf, "pkgs",  "2157");
  info_append(inf, "uname", "5.9");
  info_append(inf, "sh",    "zsh");
  info_append(inf, "wm",    "xfwm4");
  info_append(inf, "term",  "xterm-based");

  prin_card(crd);

  prin_clrs(2);

  destroy_logo(lgo);
  destroy_info(inf);
  destroy_card(crd);
  return(0);
}
