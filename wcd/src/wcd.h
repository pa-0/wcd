/*
Copyright (C) 1997-2009 Erwin Waterlander

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _WCD_H
#define _WCD_H

#include "tailor.h"

#ifdef VMS
#  define EXIT_OK 1
#  define ALT_SW || *argv[i]=='/'
#  define CHDIR(s) vms_chdir(s)
#else
#  define EXIT_OK 0
#  define CHDIR(s) chdir(s)
#  ifdef MSDOS
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
#    elif defined(WIN32)
#      define GO_FILE "/wcdgo.bat"
#    elif defined(OS2)
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
#  endif /* ?MSDOS */
#endif /* ?VMS */

#ifdef MSDOS
# define LIST_SEPARATOR ";"
#else
# define LIST_SEPARATOR ":"
#endif

#if defined(MSDOS) || defined(VMS)
#  define OP_DIR ""
#else /* ?unix */
#  define OP_DIR "."
#endif /* ?MSDOS|VMS */

#define VERSION      "5.0.2-beta1"
#define VERSION_DATE "May 22 2009"


/* Function prototypes */

#if defined(UNIX) || defined(WCD_DOSBASH) || defined(WIN32) || defined(OS2)
void quoteString(char *string);
#endif

typedef unsigned char wcd_char;

void finddirs(char *dir, int *offset, FILE *outfile, int *use_HOME, nameset exclude);
void read_treefile(char *filename, nameset bd, int silent);
void rmDirFromList(char *string, nameset n);
void writeList(char *filename, nameset n);
void cleanTreeFile(char *filename, char *dir);

char *removeBackSlash(char *string);
void addCurPathToFile(char *filename, int *use_HOME, int parents);
int check_double_match(char *dir, nameset set);
void scanfile(char *org_dir, char *filename, int ignore_case, nameset pm, nameset wm, nameset bd, nameset filter, int relative, int wildOnly);
void scanaliasfile(char *org_dir, char *filename, nameset pm, nameset wm, int wildOnly);
int  strcompare(const char*, const char*, int);
void print_help(void);
#ifdef ENABLE_NLS
void print_version(char *localedir);
#else
void print_version(void);
#endif
int  wcd_get_int(void);
int wcd_getline(char s[], int lim, FILE* infile);
int wcd_exit(nameset pm, nameset wm, nameset ef, nameset bd, nameset nfs, WcdStack ws, nameset excl);
void getCurPath(char *buffer, int size, int *use_HOME);

#ifdef WCD_DOSBASH
	void empty_wcdgo(char *go_file, int changedrive, char *drive, int use_GoScript);
#else
	void empty_wcdgo(char *go_file, int use_GoScript);
#endif
int SpecialDir(const char *path);

#endif
