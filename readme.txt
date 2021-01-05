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
+----------------+  | $ zfetch --init-base-dirs         |
 ...or:             |  [i] Creating base directories... |
 +--------------+   +-----------------------------------+
 | $ make clean |   Run:
 +--------------+   +--------------------------------------------+
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
I recommend to hide all  +----------------+
warnings because...      | $ make clean   |
                         | $ make | wc -l |
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

### CONFIGURATION ###
Init base files with --init-base-dirs argument:
+-----------------------------------+
| $ zfetch --init-base-dirs         |
|  [i] Creating base directories... |
+-----------------------------------+
...and this action not creates info file.

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
+---------------------------------------------------------------------------------+
| "os"     "bedian"                                                               |
| "pkgs"   {apt list --installed | wc -l}                                         |
| "kernel" {uname -r}                                                             |
| "wm/de"  {echo $DESKTOP_SESSION}                                                |
| "term"   {ps -o comm= -p "$(($(ps -o ppid= -p "$(($(ps -o sid= -p "$$")))")))"} |
+---------------------------------------------------------------------------------+

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

Example autogenerates by --init-base-dirs.

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
Sun Jan  3 11:06:54 PM UTC 2021 [date --utc]
