#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "include/zfetch-config.h"
#include "include/zfetch-vars.h"

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
    val = calloc(strlen(tmp) + 1, sizeof(char));
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

char* _getconfdir() {
  char* userdir = get_user_home();
  char* confdir = malloc(strlen(userdir) + strlen(config_dir) + 1);
  *confdir = '\0';
  strcat(confdir, userdir);
  strcat(confdir, config_dir);
  return(confdir);
}

bool init_zfconfig(const char* _out) {
  bool need_clean = false;
  char* out = (char*)_out;
  if (!out) {
    char* confdir = _getconfdir();
    out = malloc(strlen(confdir) + strlen(main_file_name) + 1);
    *out = '\0';
    strcat(out, confdir);
    strcat(out, main_file_name);

    free(confdir);
    need_clean = true;
  }
  FILE* f_mainfile = fopen(out, "w");
  if (!f_mainfile) return(false);
  fputs(
      " If string doesn't contains equals symbol it ignores\n",
      f_mainfile);
  fprintf(f_mainfile,
      "logo.geometry.height=%d\n"
      "logo.geometry.width=%d\n",
      default_logo_height,
      default_logo_width);
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
  fclose(f_mainfile);
  if (need_clean) free(out);

  return(true);
}

bool init_info(const char* _out) {
  char* out = (char*)_out;
  bool need_clean = false;
  if (!out) {
    char* confdir = _getconfdir();
    out = malloc(strlen(confdir) + strlen(info_file_name) + 1);
    *out = '\0';
    strcat(out, confdir);
    strcat(out, info_file_name);

    free(confdir);
    need_clean = true;
  }

  FILE* f_info = fopen(out, "w");
  if (!f_info) return(false);
  fputs(
      "\"os\" {zfetch --os-release NAME}\n"
      "# You can use ID for pretty\n"
      "\"pkgs\" \"unknown\"\n"
      "\"kernel\" {uname -r}\n"
      "\"wm/de\" {echo $DESKTOP_SESSION}\n"
      "\"term\" {ps -o comm= -p \"$(($(ps -o ppid= -p \"$(($(ps -o sid= -p \"$$\")))\")))\"}"
      , f_info);
  fclose(f_info);
  if (need_clean) free(out);
  
  return(true);
}

bool init_logo(const char* _out) {
  char* out = (char*)_out;
  bool need_clean = false;
  if (!out) {
    char* confdir = _getconfdir();
    out = malloc(strlen(confdir) + strlen(logo_file_name) + 1);
    *out = '\0';
    strcat(out, confdir);
    strcat(out, logo_file_name);

    free(confdir);
    need_clean = true;
  }
  FILE* f_logofile = fopen(out, "w");
  if (!f_logofile) return(false);
  fputs(default_logo, f_logofile);
  fclose(f_logofile);
  if (need_clean) free(out);
  
  return(true);
}

unsigned char init_base_dirs() {
  char* confdir = _getconfdir();
  mkdir(confdir, 0755);
  free(confdir);

  unsigned char result = 0;
  if (init_zfconfig(0)) result += 1 << 1;
  if (init_info(0))     result += 1 << 2;
  if (init_logo(0))     result += 1 << 3;

  return(result);
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
  char* infofile = malloc(strlen(confdir) + strlen(info_file_name));
  strcat(infofile, confdir);
  strcat(infofile, info_file_name);

  bool ex1 = access(mainfile, F_OK) != -1;
  bool ex2 = access(logofile, F_OK) != -1;
  bool ex3 = access(infofile, F_OK) != -1;

  free(logofile);
  free(mainfile);
  free(infofile);
  free(confdir);
  return(ex1 && ex2 && ex3);
}
