/*
Copyright (C) 1997-2012 Erwin Waterlander

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef _WCD_H
#define _WCD_H

#include <stdio.h>
#include <stdlib.h>
#include "tailor.h"
#include "std_macr.h"
#include "structur.h"

#if defined(__MSDOS__) || defined(__OS2__) || (defined(_WIN32) && !defined(__CYGWIN__))
/* A DOS like file system:
    *  with drive letters
    *  case insensitive
    *  backslash separator. */
#define _WCD_DOSFS 1
#endif

#ifdef VMS
#  define EXIT_OK 1
#  define ALT_SW || *argv[i]=='/'
#  define CHDIR(s) vms_chdir(s)
#else
#  define EXIT_OK 0
#  define CHDIR(s) chdir(s)
#  ifdef _WCD_DOSFS
#    define ALT_SW || *argv[i]=='/'
#    define TREEFILE "/treedata.wcd"
#    define RELTREEFILE "/rtdata.wcd"   /* relative treedata */
#    define EXTRA_TREEFILE "/extra.wcd"
#    define BANFILE "/ban.wcd"
#    define ALIASFILE "/alias.wcd"
#	  define STACK_GO_DRIVE "c:"
#    define STACKFILE "/stack.wcd"
#    ifdef WCD_WINZSH
#      define GO_FILE "/wcd.go"
#    elif defined(WCD_WINPWRSH)
#      define GO_FILE "/wcdgo.ps1"
#    elif defined(_WIN32)
#      define GO_FILE "/wcdgo.bat"
#    elif defined(__OS2__)
#      define GO_FILE "/wcdgo.cmd"
#    else
#      define GO_FILE "/wcd.go"
#    endif
#    define ROOTDIR "/"
#    define HOMESTRING "//users"
#    define DIR_SEPARATOR '/'
#  else /* ?UNIX */
#    define ALT_SW
#    define TREEFILE "/.treedata.wcd"
#    define RELTREEFILE "/.rtdata.wcd"   /* relative treedata */
#    define EXTRA_TREEFILE "/.extra.wcd"
#    define BANFILE "/.ban.wcd"
#    define ALIASFILE "/.alias.wcd"
#    define STACKFILE "/.stack.wcd"
#    define ROOTDIR "/"
#    define HOMESTRING "/home"
#    define TMP_MNT_STR "/tmp_mnt/" /* temp mount dir for automounter, including ending / */
#    define GO_FILE "/bin/wcd.go"
#    define GO_FILE2 "/wcd.go"
#    define DIR_SEPARATOR '/'
#  endif /* ?__MSDOS__ */
#endif /* ?VMS */

#if defined(UNIX) || defined(_WIN32) || defined(WCD_DOSBASH) || defined(__OS2__)
/* A go-script is required */
#define WCD_SHELL
#endif

#if defined(UNIX) || defined(WCD_WINZSH) || defined(WCD_DOSBASH) || defined(WCD_OS2BASH)
#define WCD_UNIXSHELL
#endif

/* Unix shells DJGPP-bash and WinZsh use Windows style paths, e.g. "c:/Program Files".
 * So in these shells we have to use a semicolon ';' as list separator. */
#ifdef _WCD_DOSFS
# define LIST_SEPARATOR ";"
#else
# define LIST_SEPARATOR ":"
#endif

#if defined(__MSDOS__) || defined(_WIN32) || defined(__OS2__) || defined(VMS)
#  define OP_DIR ""
#else /* ?unix */
#  define OP_DIR "."
#endif /* ?__MSDOS__|VMS */

/* Function prototypes */

#if defined(UNIX) || defined(WCD_DOSBASH) || defined(_WIN32) || defined(__OS2__)
void quoteString(char *string);
#endif


#if defined(_WIN32) && !defined(__CYGWIN__) && defined(WCD_UNICODE)
#  define WCD_UTF16
#  define WCSTOMBS wcstoutf8
#  define MBSTOWCS utf8towcs
#elif defined(_WIN32) && !defined(__CYGWIN__) && !defined(WCD_UTF16)
#  define WCD_ANSI
#  define WCSTOMBS wcstoansi
#  define MBSTOWCS ansitowcs
#else
#  define WCSTOMBS wcstombs
#  define MBSTOWCS mbstowcs
#endif

#ifdef WCD_UTF16
typedef unsigned char wcd_uchar;
typedef wchar_t wcd_char;
#else
typedef unsigned char wcd_uchar;
typedef char wcd_char;
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#  define WCD_WINDOWS
#endif

#define FILE_MBS     0  /* Multi-byte string or 8-bit char */
#define FILE_UTF16LE 1  /* UTF-16 Little Endian */
#define FILE_UTF16BE 2  /* UTF-16 Big Endian */
#define FILE_UTF8    3  /* UTF-8 */

FILE *wcd_fopen(const char *filename, const char *m, int quiet);
FILE *wcd_fopen_bom(const char *filename, const char *m, int quiet, int *bomtype);
void finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet);
int read_treefile(char *filename, nameset bd, int silent);
void rmDirFromList(char *string, nameset n);
void writeList(char *filename, nameset n, int bomtype);
void cleanTreeFile(char *filename, char *dir);
void create_dir_for_file(char *f);

char *removeBackSlash(char *string);
void addCurPathToFile(char *filename, int *use_HOME, int parents);
int check_double_match(char *dir, nameset set);
void scanfile(char *org_dir, char *filename, int ignore_case, nameset pm, nameset wm, nameset bd, nameset filter, int relative, int wildOnly, int ignore_diacritics);
void scanaliasfile(char *org_dir, char *filename, nameset pm, nameset wm, int wildOnly);
int  strcompare(const char*, const char*, int);
void print_help(void);
#ifdef ENABLE_NLS
void print_version(char *localedir);
#else
void print_version(void);
#endif
int  wcd_get_int(void);
int wcd_getline(char s[], int lim, FILE* infile, const char* file_name, const int* line_nr);
int wcd_exit(nameset pm, nameset wm, nameset ef, nameset bd, nameset nfs, WcdStack ws, nameset excl);
void getCurPath(char *buffer, size_t size, int *use_HOME);

#ifdef WCD_DOSBASH
	void empty_wcdgo(char *go_file, int changedrive, char *drive, int use_GoScript, int verbose);
#else
	void empty_wcdgo(char *go_file, int use_GoScript, int verbose);
#endif
int SpecialDir(const char *path);

#endif
