              _/                            _/       
   _/_/_/_/  _/_/_/    _/  _/_/  _/    _/  _/_/_/    
      _/    _/    _/  _/_/      _/    _/  _/    _/   
   _/      _/    _/  _/        _/    _/  _/    _/    
_/_/_/_/  _/_/_/    _/          _/_/_/  _/    _/     
 is a fetch script, written in .
// todo: fix line upper

Usage:             Expert mode:
+----------------+  Init configs:
| $ rm zfetch    |  +-----------------------------------+
+----------------+  | $ zfetch --regenerate-all         |
 ...or:             |  [i] Creating base directories... |
 +--------------+   |  [i] Done.                        |
 | $ make clean |   +-----------------------------------+
 +--------------+   Run:
                    +--------------------------------------------+
                    | $ zfetch                                   |
                    | [1] 24223 segmentation fault (core dumped) |
                    +--------------------------------------------+

### BUILDING ###
You need a gcc (or another cc) and make:
+--NORMAL-----------------+ +--DEGENERATE-------+
| $ make > /dev/null 2>&1 | | $ export CC=uwucc |
| $ echo $?               | | $ uwu             |
| 0                       | +-------------------+
+-------------------------+
                         +----------------+
 I recommend to hide all | $ make clean   |
 warnings because...     | $ make | wc -l |
                         | 77             |
                         +----------------+

Put ./zfetch to $PATH (/bin, /usr/bin or /usr/local/bin)

Another way:
Go to directory with sources and write this commands:
+------------------------+
| $ cd ..                | I recommend use this way,
| $ rm -rf zfetch/       | because my fetch script fucking
| # $PM install neofetch | sucks. Use neofetch
+------------------------+ 

### TOOLS ###
For some configs you can use built-in arguments for zfetch:

#### PARSING /ETC/OS-RELEASE ####
If you pass --os-release argument program parse /etc/os-release and
exits.
Error codes:
  0 - ok
  1 - not found

Usage:
  +-----------------------------+
  | $ zfetch --os-release [KEY] |
  | VALUE                       |
  +-----------------------------+

Example in info:
+-------------------------------+
| "os" {zfetch --os-release ID} |
+-------------------------------+

TIP: use
  $ less /etc/os-release
  for view all keys.

#### GETTING TERMINAL NAME, SHELL ####
Usage:
  +-----------------+
  | $ zfetch --term |
  | xfce4-terminal  |
  +-----------------+

All options:
  --term: return terminal name (ex.: xfce4-terminal)
  --shell: return shell path/name (ex.: /usr/bin/zsh)
  --shell-short: return only shell name (ex.: zsh)

After returning name program exits.

### CONFIGURATION ###
For generate all files use --regenerate-all.

Syntax:
  --regenerate-<zfconfig|info|logo> [output]

Error codes:
  0 - ok
  1 - fail ~wow~

output is optional, if not provided uses default file
location.

Syntax:
  --regenerate-all

Error codes:
  0 - ok
  1 - failed to regenerate all files
  2 - failed to regenerate some files

Output with error code 2:
  With error message zfetch provides tech code in hex.
  First byte (from start) always zero.
  * 0b1110 - all files generated.
  * 0b1000 - generated only logo.
  * 0b0100 - generated only info.
  * 0b0010 - generated only zfconfig.
  If generated 2/3 files zfetch returns AND, eg. 0b0110 - 
  - generated info and zfconfig, failed to generate logo.
  TIP: re-read book named "Информатика. 6 класс"

#### INFO ####
Info file located in ~/.zfetch/info
Syntax:
+-----------------+
| "key" "value"   | If value not present it
| "key" {command} | will be sets to "<invalid>"
+-----------------+
It supports VERY BASE escape char:
"\e[0;33m"  --> "^[[0;33m"
"\b"        --> "b"
"\\\"it\"/" --> \"it"/
  It works in commands too:
   For echo '\Hello world!' you need:
   {echo '\\Hello world!'}

ZFetch written in C, but speed equals neofetch.
Example info:
+-----------------------------------------+
| "os"     "bedian"                       |
| "pkgs"   {apt list --installed | wc -l} |
| "kernel" {uname -r}                     |
| "wm/de"  {echo $DESKTOP_SESSION}        |
| "term"   {zfetch --term}                |
+-----------------------------------------+

#### ZFCONFIG ####
zfconfig - main configuration file. Located in
~/.zfetch/zfconfig
Syntax:
+----------------------+
| key=value            | If line doesnt contains '='
| Yep, this is comment | it comment.
+----------------------+
Fields:
  logo:
    logo.geometry:
      logo.geometry.height: integer, logo height
      logo.geometry.width:  integer, logo width
  info:
    info.autotitle: boolean aka yes/no. Sets info.title
      to {username}@{hostname}
    info.title: string, title...
    info.bold: boolean (yes/no). Allow bold in info
    info.separator: string, separator
    NOTE: if info.separator is ":" info is: key:value
          if info.separator is ": " info is: key: value
  clrscm:
    NOTE: Ranges for colors is 30-39 and 90-99 (tech max: 127)
    clrscm.primary: integer, primary color
    clrscm.secondary: integer, secondary color
    clrscm.header: integer, bla bla bla
    clrscm.separator:
    clrscm.border:

Example autogenerates by [see section CONFIGURATION] or --init-base-dirs.

#### LOGO ####
ASCII art. Located in ~/.zfetch/logo
Its raw ascii art, one fact:
If logo geometry has    +-------------------------+
3x3 (example) logo file | AAABBB                  |
can contains more that  | CCC   blablablabla...   |
3 chars and 3 lines.    | This text not displayed |
                        +-------------------------+
                        Out is:
                        +-----+
                        | AAA |
                        | BBB |
                        | CCC |
                        +-----+

### PS ###
More information you can read in todo.txt.

---
Amchik <am4ik1337@gmail.com>, chk[rg]
Sat Jan  9 01:54:57 AM UTC 2021 [date --utc]
