#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "include/zfetch.h"
#include "include/zfetch-config.h"
#include "include/zfetch-vars.h"

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

#define PBEGIN "/proc/"
#define PEND "/comm"

char* get_pname(pid_t pid) {
  char* _spid = calloc(12, sizeof(char));
  sprintf(_spid, "%u", pid);
  char* path = malloc(strlen(PBEGIN PEND) + 
      strlen(_spid));
  sprintf(path, PBEGIN "%u" PEND, pid);
  free(_spid);

  FILE* fp = fopen(path, "r");
  if (!fp) return("/sbin/init");
  char* ans;
  size_t n = 0;
  getline(&ans, &n, fp);
  fclose(fp);
  free(path);

  return(ans);
}

#define _GETPPIDOF_C_PBEGIN "/proc/" // <what><pid>/stat
#define _GETPPIDOF_C_PEND "/stat"    // /proc/<pid><what>

// https://gist.github.com/Amchik/64e1c40bdd531a9fb09df6a2931cfda9
pid_t getppidof(pid_t pid) {
  char* spid = calloc(12, sizeof(char));
  sprintf(spid, "%u", pid);
  char* path = malloc(strlen(_GETPPIDOF_C_PBEGIN _GETPPIDOF_C_PEND) + 
      strlen(spid) + 1);
  sprintf(path, _GETPPIDOF_C_PBEGIN "%s" _GETPPIDOF_C_PEND, spid);
  free(spid);

  FILE* fp = fopen(path, "r");
  if (!fp) return(0); // process $pid doesnt exists
  char* sppid;
  size_t n = 0;
  getline(&sppid, &n, fp);
  sppid = strtok(sppid, ")"); // see proc(5)
  sppid = strtok(0, ")");     // for more info...
  sppid = strtok(sppid, " ");
  sppid = strtok(0, " ");     // getting ppid
  fclose(fp);

  pid_t ppid = atoi(sppid);
  free(path);
  return(ppid);
}

bool strends(const char* str, const char* suffix) {
  // audit: move functionS into file
  size_t sn = strlen(suffix);
  size_t strn = strlen(str);
  if (sn > strn || strn == 0 || sn == 0) return(false);
  for (int i = sn - 1; i >= strn - 1; i--) {
    if (str[i] != suffix[i]) return(false);
  }
  return(true);
}

int main(int argc, char* argv[]) {
  if (argc > 1) {
    // todo: 3.3
    for (int i = 1; i < argc; ++i) {
      if (strcmp("--os-release", argv[i]) == 0 && ++i < argc) {
        // parse /etc/os-release w/ info. um...
        zfconfig* osr = parse_config("/etc/os-release"); // ...
        char* val = zfconfig_get_key(osr, argv[i]);
        if (!val) return(1);
        val[strlen(val) - 1] = 0;
        val++;
        printf("%s\n", val);
        return(0);
      } else if (strcmp("--shell", argv[i]) == 0) {
        char* ans = get_pname(getppid());
        puts(ans);
        return(0);
      } else if (strcmp("--shell-short", argv[i]) == 0) {
        char* ans = get_pname(getppid());
        if (strends(ans, "zfetch")) {
          ans = get_pname( getppidof( getppid() ) );
        }
        char* res = malloc(strlen(ans) + 1);
        size_t res0 = 0;
        for (int i = strlen(ans) - 1; i >= 0; i--) {
          if (ans[i] == '/') {
            res0 = i + 1;
            break;
          } else if (ans[i] == '\n') {
            res[i] = ' ';
          } else {
            res[i] = ans[i];
          }
        }
        puts(res + res0);
        return(0);
      } else if (strcmp("--term", argv[i]) == 0) {
        char* ans = get_pname( getppidof( getppid() ) );
        puts(ans);
        return(0);
      } else if (strcmp("--help-base-files", argv[i]) == 0) {
        puts(" === ZFETCH ===\n"
            "  \e[1m--regenerate-\e[3msomething [out]\e[0m - regenerates file(s)\n"
            "   - \e[1mall\e[0m - All files (\e[1mout\e[0m ignores)\n"
            "   - \e[1mzfconfig\e[0m - ~/.zfetch/zfconfig\n"
            "   - \e[1minfo\e[0m - ~/.zfetch/info\n"
            "   - \e[1mlogo\e[0m - ~/.zfetch/logo\n"
            "  After this argument program regenerate file(s) and exit.\n"
            "  Pass \e[1mout\e[0m for specify output file. Examples:\n"
            " #\e[3mGenerate logo file and put it to logo.txt\e[0m\n"
            " $ zfetch --regenerate-logo logo.txt\n"
            " #\e[3mRegenerate and \e[31moverwrite\e[0;3m all files\e[0m\n"
            " $ zfetch --regenerate-all");
        return(0);
      } else if (strcmp("--regenerate-all", argv[i]) == 0) {
        puts(" \e[0;34m[i]\e[0m Regenerating all files...");
        unsigned char res = init_base_dirs();
        if (!res) {
          puts(" \e[0;31m[e]\e[0m Failed to regenerate \e[1mall\e[0m files. Try to remove ~/.zfetch directory");
          return(1);
        } else if ((res & 0b01110) != 0b01110) {
          puts(" \e[0;33m[w]\e[0m Failed to regenerate \e[1msome\e[0m files. Re-check permissions.");
          printf(" \e[0;34m[i]\e[0m Result of init_base_dirs(): 0x%x. Try to backup and remove ~/.zfetch directory.\n", res);
          return(2);
        }
        puts(" \e[0;34m[i]\e[0m Done. Now you can run \e[1m$ zfetch\e[0m.");
        return(0);
      } else if (strcmp("--regenerate-zfconfig", argv[i]) == 0) {
        char* out = 0;
        if (++i < argc) {
          out = argv[i];
        }
        puts(" \e[0;34m[i]\e[0m Generating zfconfig...");
        bool res = init_zfconfig(out);
        if (!res) {
          puts(" \e[0;31m[e]\e[0m Failed to regenerate file.");
          return(1);
        }
        puts(" \e[0;34m[i]\e[0m Done.");
        return(0);
        //audit: move it into function
      } else if (strcmp("--regenerate-info", argv[i]) == 0) {
        char* out = 0;
        if (++i < argc) {
          out = argv[i];
        }
        puts(" \e[0;34m[i]\e[0m Generating info...");
        bool res = init_info(out);
        if (!res) {
          puts(" \e[0;31m[e]\e[0m Failed to regenerate file.");
          return(1);
        }
        puts(" \e[0;34m[i]\e[0m Done.");
        return(0);
      } else if (strcmp("--regenerate-logo", argv[i]) == 0) {
        char* out = 0;
        if (++i < argc) {
          out = argv[i];
        }
        puts(" \e[0;34m[i]\e[0m Generating logo...");
        bool res = init_logo(out);
        if (!res) {
          puts(" \e[0;31m[e]\e[0m Failed to regenerate file.");
          return(1);
        }
        puts(" \e[0;34m[i]\e[0m Done.");
        return(0);
      }
    }
  }

  char* userhome = get_user_home();
  char* logofile = malloc(strlen(userhome) + strlen(config_dir) + strlen(logo_file_name));
  char* confdir = malloc(strlen(userhome) + strlen(config_dir) + strlen(main_file_name));
  sprintf(confdir, "%s%s%s", userhome, config_dir, main_file_name);
  sprintf(logofile, "%s%s%s", userhome, config_dir, logo_file_name);

  //signal(SIGSEGV, _segv);
  //signal(SIGABRT, _segv);
  //signal(SIGIOT, _segv);

  if (argc > 1 && strcmp(argv[1], "--init-base-dirs") == 0) {
    printf(" \e[0;34m[i]\e[0m Creating base directories...\n");
    init_base_dirs();
    return(0);
  }

  if (!has_base_dirs()) {
    printf(" \e[0;33m[w]\e[0m Installation corrupted: failed to locate %s%s... files\n",
        get_user_home(), config_dir);
    printf(" \e[0;34m[i]\e[0m You can get help about it using \e[1m--help-base-files\e[0m.\n");
    return(1);
  }
  
  zfconfig* cfg = parse_config(confdir);
  char* title;
  if (strcmp(zfconfig_get_key(cfg, "info.autotitle"), "yes") == 0) {
    char* username = get_user_name();
    char* hostname = get_host_name();
    title = malloc(strlen(username) + strlen(hostname) + 2);
    title[0] = '\0';
    sprintf(title, "%s@%s", username, hostname);
  } else {
    title = zfconfig_get_key(cfg, "info.title");
    if (!title) title = "<failed to parse>";
  }

  int width = 0, height = 0;
  width = atoi(zfconfig_get_key(cfg, "logo.geometry.width"));
  height = atoi(zfconfig_get_key(cfg, "logo.geometry.height"));

  logo* lgo = mk_logo(height, width, 0);
  lgo->content = read_file(logofile);

  char* _ifl = malloc(strlen(get_user_home()) + strlen(config_dir) + strlen(info_file_name));
  sprintf(_ifl, "%s%s%s", get_user_home(), config_dir, info_file_name);
  info_file* _inf = parse_info_file(_ifl);
  info* inf = mk_info(_inf->lines, 0, zfconfig_get_key(cfg, "info.separator"));
  inf->content = _inf->content; 

  card* crd = mk_card(inf, lgo, title, 1);

  if (inf->separator == 0) inf->separator = ": ";
  if (strcmp(zfconfig_get_key(cfg, "info.bold"), "yes") == 0) inf->bold = 1;

  clrscm clrs;
  // todo: add value checks
  clrs.primary = atoi(zfconfig_get_key(cfg, "clrscm.primary"));
  clrs.secondary = atoi(zfconfig_get_key(cfg, "clrscm.secondary"));
  clrs.header = atoi(zfconfig_get_key(cfg, "clrscm.header"));
  clrs.border = atoi(zfconfig_get_key(cfg, "clrscm.border"));
  clrs.separator = atoi(zfconfig_get_key(cfg, "clrscm.separator"));

  prin_card(crd, &clrs);
  prin_clrs(2);

  destroy_logo(lgo);
  destroy_info(inf);
  destroy_card(crd);
  return(0);
}
