#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "include/zfetch-config.h"

char* _get_config_location() {
  char* userdir = get_user_home();
  char* confdir = malloc(strlen(userdir) + strlen(config_dir) + strlen(main_file_name));
  sprintf(confdir, "%s%s%s", userdir, config_dir, main_file_name);
  return confdir;

}

zfconfig* parse_config(const char* confdir) {
  FILE* fp = fopen(confdir, "r");
  if (fp == 0) return(0);
 
  zfconfig* cfg = (zfconfig*)malloc(sizeof(zfconfig));
  cfg->keys = 0;
  cfg->values = 0;

  size_t objsize = 2 * sizeof(char*);

  char* ln;
  size_t ln_n = 0;
  while (getline(&ln, &ln_n, fp) >= 0) {
    char* key;
    char* val;

    key = strtok(ln, "=");
    char* tmp = strtok(0, "=");
    if (!key || !tmp) continue;
    val = malloc(strlen(tmp));
    strcpy(val, tmp);
    tmp = strtok(0, "=");
    while (tmp) {
      char* tv = malloc(strlen(val) + strlen(tmp) + 2);
      tv[0] = '\0';
      sprintf(tv, "%s=%s", val, tmp);
      free(val);
      val = tv;
      tmp = strtok(0, "=");
    }

    // audit: is it good idea?
    val[strlen(val) - 1] = '\0';

    if (cfg->values == 0) {
      cfg->values = malloc(objsize);
    } else {
      cfg->values = realloc(cfg->values, cfg->keys * 2 * sizeof(char*) + objsize);
    }

    cfg->keys++;

    cfg->values[(cfg->keys - 1) * 2] = malloc(strlen(key));
    cfg->values[(cfg->keys - 1) * 2 + 1] = malloc(strlen(val));

    strcpy(cfg->values[(cfg->keys - 1) * 2], key);
    strcpy(cfg->values[(cfg->keys - 1) * 2 + 1], val);
  }
  fclose(fp);
  return(cfg);
}
//char* strappend(char* original, char* new) {
//  original = realloc(original, strlen(original) + strlen(new));
//  strcpy(original, new);
//  return(original);
//}
char* strcappend(char* original, char new) {
  size_t size;
  if (original) size = strlen(original) + 2;
  else size = 2;
  char* tmp = malloc(size);
  tmp[0] = '\0';
  if (!original) original = "";
  sprintf(tmp, "%s%c", original, new);
  free(original);
  return(tmp);
}
info_file* parse_info_file(const char* _ifl) {
  info_file* fl = malloc(sizeof(info_file));
  fl->lines = 0;
  fl->content = 0;

  FILE* ifl = fopen(_ifl, "r");
  
  char* ln;
  size_t n = 0;
  while (getline(&ln, &n, ifl) >= 0) {
    char* key = malloc(1);
    char* val = malloc(1);
    key[0] = 0;
    val[0] = 0;

    size_t pointer = 0;

    bool empty = false;
    bool invalid = false;
    bool done = false;

    bool in_str = false;
    bool escape = false;

    // getting key
    for(; pointer < strlen(ln); pointer++) {
      char e = *(ln + pointer);
      if (!in_str) {
        switch(e) {
          case '"':
            in_str = true;
            break;
          case '#':
            empty = true;
            break;
          case ' ':
          case '\t':
            break;
          default:
            invalid = true;
            break;
        }
        if (empty || invalid) break;
        continue;
      }
      if (escape) {
        escape = false;
        if (e == 'e') e = '\e';
        key = strcappend(key, e);
        continue;
      }
      switch(e) {
        case '\\':
          escape = true;
          continue;
        case '"':
          in_str = false;
          done = true;
          break;
        default:
          key = strcappend(key, e);
          break;
      }
      if (!in_str) break;
    }
    if (empty || !done) continue;
    if (invalid || in_str) {
      fl->content = realloc(fl->content, fl->lines++ * 2 * sizeof(char*));
      fl->content[(fl->lines - 1) * 2] = "<invalid line>";
      fl->content[(fl->lines - 1) * 2 + 1] = "";
      continue;
    }
    done = false;
    pointer++;

    bool command = false;
    // getting value
    for(; pointer < strlen(ln); pointer++) {
      char e = *(ln + pointer);
      if (!in_str) {
        switch(e) {
          case '{':
            command = true;
            in_str = true;
            break;
          case '"':
            in_str = true;
            break;
          case ' ':
          case '\t':
            break;
          default:
            invalid = true;
            break;
        }
        if (invalid) break;
        continue;
      }
      if (escape) {
        escape = false;
        if (e == 'e') e = '\e';
        val = strcappend(val, e);
        continue;
      }
      switch(e) {
        case '\\':
          escape = true;
          continue;
        default:
          if (
              (!command && (e == '"')) ||
              ( command && (e == '}'))) {
            in_str = false;
            done = true;
            break;
          }
          val = strcappend(val, e);
          break;
      }
      if (!in_str) break;
    }
    if (invalid || in_str || !done) {
      fl->content = realloc(fl->content, fl->lines++ * 2 * sizeof(char*));
      fl->content[(fl->lines - 1) * 2] = "";
      fl->content[(fl->lines - 1) * 2 + 1] = "<invalid>";
      continue;
    }
    if (command) {
      FILE* out = popen(val, "r");
      if (!out) {
        val = "<failed to execute command>";
      } else {
        char* vln = 0;
        size_t vn = 0;
        getline(&vln, &vn, out);
        free(val);
        val = vln;
        size_t laste = strlen(val) - 1;
        if (val[laste] == '\n')
          val[laste] = '\0';
        // Uncomment this line for...
        // free(): invalid pointer
        // [1]    9331 IOT instruction (core dumped)  ./zfetch
        // (???)
        //pclose(out);
        // just ignore popen(3)
      }
    }
    fl->content = realloc(fl->content, (++fl->lines + 1) * 2 * sizeof(char*));
    fl->content[(fl->lines - 1) * 2] = malloc(strlen(key));
    fl->content[(fl->lines - 1) * 2 + 1] = malloc(strlen(val));
    strcpy(fl->content[(fl->lines - 1) * 2], key);
    strcpy(fl->content[(fl->lines - 1) * 2 + 1], val);
    free(key);
    free(val);
  }

  return(fl);
}

char* zfconfig_get_key(const zfconfig* config, const char* key) {
  for (int i = 0; i < config->keys * 2; i += 2) {
    if (strcmp(config->values[i], key) == 0) {
      return config->values[i + 1];
    }
  }
  return(0);
}

char* get_user_name() {
  return getenv("USER");
}
char* get_host_name() {
  return getenv("HOSTNAME");
}

char* get_user_home() {
  return getenv("HOME");
}

void init_base_dirs() {
  // oh, i read this code...
  // audit: fix it, fix it all
  char* userdir = get_user_home();
  char* confdir = malloc(strlen(userdir) + strlen(config_dir));
  strcat(confdir, userdir);
  strcat(confdir, config_dir);
  mkdir(confdir, 0755);

  char* logofile = malloc(strlen(confdir) + strlen(logo_file_name));
  strcat(logofile, confdir);
  strcat(logofile, logo_file_name);
  
  FILE* f_logofile = fopen(logofile, "w");
  fputs(default_logo, f_logofile);
  fclose(f_logofile);
  
  char* mainfile;
  sprintf(mainfile, "%s%s", confdir, main_file_name);

  FILE* f_mainfile = fopen(mainfile, "w");
  fputs(
      " If string doesn't contains equals symbol it ignores\n",
      f_mainfile);
  fprintf(f_mainfile,
      "logo.geometry.height=%d\n"
      "logo.geometry.width=%d\n",
      default_logo_geometry[0],
      default_logo_geometry[1]);
  fputs(
      "\n"
      " info.autotitle sets info.title to {username}@{hostname}\n"
      " info.title will be ignored\n"
      "info.autotitle=yes\n"
      "info.title=zfetch are awesome\n"
      "info.bold=yes\n"
      "info.separator=: \n"
      "\n"
      " Color scheme: color.primary - key,\n"
      " color.secondary - value. ranges: 30-39, 90-99\n"
      "clrscm.primary=34\n"
      "clrscm.secondary=34\n"
      "clrscm.header=32\n"
      "clrscm.separator=35\n"
      "clrscm.border=31\n"
      , f_mainfile);
  // yes, todo: serialization
  fclose(f_mainfile);

  char* info_file = malloc(strlen(confdir) + strlen(info_file_name));
  sprintf(info_file, "%s%s", confdir, info_file_name);
  FILE* f_info = fopen(info_file, "w");
  fputs(
      "\"os\" {zfetch --os-release NAME}\n"
      "# You can use ID for pretty\n"
      "\"pkgs\" \"unknown\"\n"
      "\"kernel\" {uname -r}\n"
      "\"wm/de\" {echo $DESKTOP_SESSION}\n"
      "\"term\" {ps -o comm= -p \"$(($(ps -o ppid= -p \"$(($(ps -o sid= -p \"$$\")))\")))\"}"
      , f_info);
  
  free(confdir);
}
bool has_base_dirs() {
  char* userdir = get_user_home();
  char* confdir = malloc(strlen(userdir) + strlen(config_dir));
  strcat(confdir, userdir);
  strcat(confdir, config_dir);
  char* mainfile = malloc(strlen(confdir) + strlen(main_file_name));
  strcat(mainfile, confdir);
  strcat(mainfile, main_file_name);
  char* logofile = malloc(strlen(confdir) + strlen(logo_file_name));
  strcat(logofile, confdir);
  strcat(logofile, logo_file_name);
  
  bool ex1 = access(mainfile, F_OK) != -1;
  bool ex2 = access(logofile, F_OK) != -1;
  // yes, todo: dynamic info, fixme:

  free(logofile);
  free(mainfile);
  free(confdir);
  return(ex1 && ex2);
}
