/*
Copyright (C) 1996-2017 Erwin Waterlander

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

#ifdef _WIN32
#  ifdef WCD_UNICODE
#    define UNICODE
#    define _UNICODE
#  endif
#  include <windows.h>
#endif
#if defined(UNIX) || defined(__CYGWIN__) || defined(__EMX__)
#  define _GNU_SOURCE /* Required for DT_DIR and DT_LNK */
#  include <dirent.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif
#include <string.h>
#include "dosdir.h"
#include "wcddir.h"
#include "wcd.h"
#include "wfixpath.h"
#include "nameset.h"
#include "config.h"
#include "display.h"
#include "tailor.h"

const wcd_char *default_mask = ALL_FILES_MASK;

/****************************************************************/
typedef struct TDirTag {
   char* dirname;
   struct TDirTag *next;
} TDirEntry;

typedef struct {
   TDirEntry *head, *tail;
} TDirList;

/* Function: SpecialDir
 *
 * Purpose:  Test for special directories
 *
 * Returns: 1 if path = "." or ".."
 *          0 otherwise.
 */
int SpecialDir(const char *path)
{
   if (*path != '.') return 0;
   if (*(++path) == '.') path++;
   return (*path=='/' || *path=='\0');
}

/******************************************************************
 *
 *          q_insert - insert directory name to queue
 *
 ******************************************************************/

void q_insert(TDirList *list,const char *s)
{
   TDirEntry *ptr;
   size_t len = strlen(s);
   if (!len) return;
   if ((ptr = (TDirEntry*) malloc(sizeof(TDirEntry))) == NULL )
   {
      perror("malloc");
      return;
   }
   if ((ptr->dirname = (char*) malloc(len+1)) == NULL )
   {
      perror("malloc");
      free(ptr);
      return;
   }
   strcpy(ptr->dirname, s);
   ptr->next = NULL;
   if (!list->head) list->head = ptr;
   else list->tail->next = ptr;
   list->tail = ptr;
}

/*******************************************************************
 *
 *         q_remove - remove directory name from queue
 *
 *******************************************************************/

int q_remove(TDirList *list,char *s)
{
   TDirEntry *ptr = list->head;
   if (!ptr) return 0;     /* queue empty? */
   strcpy(s, ptr->dirname);
   list->head = ptr->next;
   free(ptr->dirname);
   free(ptr);
   return 1;         /* okay */
}

/*
The 32 bit dos versions are compiled with DJGPP and do not use DOSDIR. DJGPP is
a mix of DOS/Unix (both 'MSDOS' and 'unix' are defined).  DOSDIR's
dd_findfirst/dd_findnext implementation for Unix is build with opendir/readdir.
Using DJGPP's implementation of opendir/readdir to scan a disk is about a
factor 100 slower than using findfirst/findnext (with DJGPP 2.01, gcc 2.7.2).
Also using DOSDIR's dd_findfirst/dd_findnext for DOS is very slow when it is
compiled with DJGPP.  It is about a factor 35 slower than using DJGPP's
findfirst/findnext.  Probably due to a slow stat() function in dd_initstruct().
Using DOSDIR in combination with DJGPP would make scanning the disk very slow.

The Windows version of wcd doesn't use DOSDIR anymore. Using the Windows API functions results
in about 4 times faster disc scanning. EW Jan 27 2017.

The Unix version of wcd doesn't use DOSDIR anymore. Using opendir/readdir directly results
in about 4 times faster disc scanning. EW Jan 29 2017.

Only native DOS and OS/2 compilers still use dosdir. E.g. Borland C and Watcom C.

*/

#ifdef __DJGPP__
#  define WCD_FB_NAME  fb.ff_name
#  define WCD_FB_MODE  fb.ff_attrib
#  define WCD_FINDNEXT findnext
#else
#  define WCD_FB_NAME  fb.dd_name
#  define WCD_FB_MODE  fb.dd_mode
#  define WCD_FINDNEXT dd_findnext
#endif


#if defined(_WIN32) && !defined(__CYGWIN__)
#  ifdef WCD_UNICODE
#    define WCD_FINDFIRSTFILE FindFirstFileW
#    define WCD_FINDNEXTFILE  FindNextFileW
#  else
#    define WCD_FINDFIRSTFILE FindFirstFileA
#    define WCD_FINDNEXTFILE  FindNextFileA
#  endif
#endif

/********************************************************************
 *
 *                    rmTree(dir)
 *
 * Recursively delete directory: *dir
 *
 ********************************************************************/
void rmTree(char *dir)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;
#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))
#  ifdef __DJGPP__
   static struct ffblk fb;       /* file block structure */
   int rc;                       /* error code */
#  else
   static dd_ffblk fb;           /* file block structure */
   wcd_intptr_t rc;              /* error code */
#  endif
   char tmp[DD_MAXPATH];         /* tmp string */
   TDirList list;                /* directory queue */
#else /* Unix, Cygwin, MSYS, EMX */
   DIR *dirp;
   struct dirent *dp;
#endif

   if (dir)
   {
      if (wcd_chdir(dir,0)) return; /* Go to the dir, else return */
   }
   else
     return ;  /* dir == NULL */


#if defined(_WIN32) && !defined(__CYGWIN__)

   hFind = WCD_FINDFIRSTFILE(ALL_FILES_MASK, &FindFileData);
   while (hFind != INVALID_HANDLE_VALUE)
   {
      char directory[DD_MAXPATH];
#  ifdef WCD_UNICODE
      wcstoutf8(directory, FindFileData.cFileName, sizeof(directory));
#  else
      wcd_strncpy(directory,FindFileData.cFileName,sizeof(directory));
#  endif
      if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         if (!SpecialDir(directory)) {
            if(wcd_islink(directory,0)) {
               if (unlink(directory) != 0)
                  print_error(_("Unable to remove file %s: %s\n"), directory, strerror(errno));
            }
            else {
               rmTree(directory);
               wcd_rmdir(directory,0);
            }
         }

      } else {  /* not a directory */
         if (unlink(directory) != 0)
            print_error(_("Unable to remove file %s: %s\n"), directory, strerror(errno));

      }
      if (!WCD_FINDNEXTFILE(hFind, &FindFileData)) {
          FindClose(hFind);
          hFind = INVALID_HANDLE_VALUE;
      }

   }

#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))

/* Don't set DD_LABEL. Otherwise wcd compiled with
   Borland CPP 5.x bcc32 finds no files or directories at all.
   This was seen with Borland CPP 5.02 and 5.5.1.
   Apr 29 2005   Erwin Waterlander */
#  ifdef __DJGPP__
   rc = findfirst(default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL);
#  else
   rc = dd_findfirst(default_mask, &fb, DD_DIREC|DD_RDONLY|DD_HIDDEN|DD_SYSTEM|DD_ARCH|DD_DEVICE);
#  endif
   list.head = list.tail = 0;

   while (rc==0) {  /* go through all the files in the current dir */
      if (DD_ISDIREC(WCD_FB_MODE)) {
         if (!SpecialDir(WCD_FB_NAME))
            q_insert(&list, WCD_FB_NAME);   /* add all directories in current dir to list */
      } else { /* not a directory */
         if (unlink(WCD_FB_NAME) != 0) {
            print_error(_("Unable to remove file %s: %s\n"), WCD_FB_NAME, strerror(errno));
         }
      }
      rc = WCD_FINDNEXT(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) (quiet) */
   while (q_remove(&list, tmp)) {
      rmTree(tmp);
      wcd_rmdir(tmp,0);
   }

#else /* Unix, Cygwin, MSYS, EMX */

   dirp = opendir(".");
   if (dirp == NULL) {
      wcd_chdir(DIR_PARENT,1); /* go to parent directory */
      return;
   }
   dp = readdir(dirp);
   while (dp) {
#ifdef _DIRENT_HAVE_D_TYPE
      if (dp->d_type == DT_DIR) {
         if (!SpecialDir(dp->d_name)) {
            rmTree(dp->d_name);
            wcd_rmdir(dp->d_name,0);
         }
      
      } else { /* not a directory */
         if (unlink(dp->d_name) != 0) {
            print_error(_("Unable to remove file %s: %s\n"), dp->d_name, strerror(errno));
         }
      }
#else
      /* Not all systems have d_type. */
      static struct stat buf ;
      if (lstat(dp->d_name, &buf) == 0) {
         if (S_ISDIR(buf.st_mode)) {
            if (!SpecialDir(dp->d_name)) {
               rmTree(dp->d_name);
               wcd_rmdir(dp->d_name,0);
            }
         } else { /* not a directory */
            if (unlink(dp->d_name) != 0) {
               print_error(_("Unable to remove file %s: %s\n"), dp->d_name, strerror(errno));
            }
         }
      }
#endif
      dp = readdir(dirp);
   }
   if (closedir(dirp))
      print_error(_("Unable to close directory %s: %s\n"), dir, strerror(errno));
#endif

   wcd_chdir(DIR_PARENT,1); /* go to parent directory */
}



/********************************************************************
 *
 *  finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, int quiet)
 *
 ********************************************************************/

void finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet)
{
   char curdir[DD_MAXPATH];
   char *tmp_ptr ;
   size_t len ;
#if defined(_WIN32) && !defined(__CYGWIN__)
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;
#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))
#  ifdef __DJGPP__
   static struct ffblk fb;       /* file block structure */
   int rc;                       /* error code */
#  else
   static dd_ffblk fb;           /* file block structure */
   wcd_intptr_t rc;              /* error code */
#  endif
   TDirList list;                /* directory queue */
#else /* Unix, Cygwin, MSYS, EMX */
   DIR *dirp;
   struct dirent *dp;
#endif

   if (dir)
   {
      if (wcd_chdir(dir,quiet)) return; /* Go to the dir, else return */
   }
   else
     return ;  /* dir == NULL */


   if (wcd_getcwd(curdir, sizeof(curdir)) == NULL)
   {
      print_error(_("finddirs(): can't determine path in directory %s\n"),dir);
      print_error(_("path probably too long.\n"));
      wcd_chdir(DIR_PARENT,1); /* go to parent directory */
      return;
   };

#ifdef _WCD_DOSFS
   wcd_fixpath(curdir,sizeof(curdir));
   rmDriveLetter(curdir,use_HOME);
#endif

   if (pathInNameset(curdir,exclude) != (size_t)-1)
   {
      wcd_chdir(DIR_PARENT,1); /* go to parent directory */
      return;
   }

   len = strlen(curdir);

   if(*offset < len)
     tmp_ptr = curdir + *offset ;
   else
     tmp_ptr = curdir + len;   /* tmp_ptr points to ending '\0' of curdir */

   if (wcd_fprintf(outfile,"%s\n", tmp_ptr) < 0) {
      wcd_chdir(DIR_PARENT,1); /* go to parent directory */
      return;  /* Quit when we can't write path to disk */
   }

#if defined(_WIN32) && !defined(__CYGWIN__)

   hFind = WCD_FINDFIRSTFILE(ALL_FILES_MASK, &FindFileData);
   while (hFind != INVALID_HANDLE_VALUE)
   {
      if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         char directory[DD_MAXPATH];
#  ifdef WCD_UNICODE
         wcstoutf8(directory, FindFileData.cFileName, sizeof(directory));
#  else
         wcd_strncpy(directory,FindFileData.cFileName,sizeof(directory));
#  endif
         if (!SpecialDir(directory)) {
            if(wcd_islink(directory,quiet))
               wcd_fprintf(outfile,"%s/%s\n", tmp_ptr, directory);
            else
               finddirs(directory,offset, outfile, use_HOME, exclude, 1);
         }

      }
      if (!WCD_FINDNEXTFILE(hFind, &FindFileData)) {
          FindClose(hFind);
          hFind = INVALID_HANDLE_VALUE;
      }

   }

#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))

/*
   Don't set DD_LABEL. Otherwise wcd compiled with
   Borland CPP 5.x bcc32 finds no files or directories at all.
   This was seen with Borland CPP 5.02 and 5.5.1.
   Apr 29 2005   Erwin Waterlander
*/
#  ifdef __DJGPP__
   rc = findfirst(default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL);
#  else
   rc = dd_findfirst(default_mask, &fb, DD_DIREC|DD_RDONLY|DD_HIDDEN|DD_SYSTEM|DD_ARCH|DD_DEVICE);
#  endif
   list.head = list.tail = 0;

   while (rc==0) {  /* go through all the files in the current dir */
      if (DD_ISDIREC(WCD_FB_MODE)) {
         if (!SpecialDir(WCD_FB_NAME))
            q_insert(&list, WCD_FB_NAME);   /* add all directories in current dir to list */
      }
      rc = WCD_FINDNEXT(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) (quiet) */
   while (q_remove(&list, curdir))
      finddirs(curdir,offset, outfile, use_HOME, exclude, 1);

#else /* Unix, Cygwin, MSYS, EMX */

   dirp = opendir(curdir);
   if (dirp == NULL) {
      wcd_chdir(DIR_PARENT,1); /* go to parent directory */
      return;
   }
   dp = readdir(dirp);
   while (dp) {
#ifdef _DIRENT_HAVE_D_TYPE
      if (dp->d_type == DT_DIR) {
         if (!SpecialDir(dp->d_name))
            finddirs(dp->d_name,offset, outfile, use_HOME, exclude, 1);
      } else if (dp->d_type == DT_LNK) { /* Is it a symbolic link? */
         static struct stat buf ;
         if ((stat(dp->d_name, &buf) == 0) && S_ISDIR(buf.st_mode)) /* does the link point to a dir */
            wcd_fprintf(outfile,"%s/%s\n", tmp_ptr, dp->d_name);
      } else if (dp->d_type == DT_UNKNOWN) {
         /* File type is not supported by all file systems.
            On Cygwin DT_LNK only works for Windows native links. Cygwin links return DT_UNKNOWN. */
         static struct stat buf ;
         if (lstat(dp->d_name, &buf) == 0) {
            if (S_ISDIR(buf.st_mode)) {
               if (!SpecialDir(dp->d_name))
                  finddirs(dp->d_name,offset, outfile, use_HOME, exclude, 1);
            } else if (S_ISLNK(buf.st_mode)) { /* Is it a symbolic link? */
               if ((stat(dp->d_name, &buf) == 0) && S_ISDIR(buf.st_mode)) /* does the link point to a dir */
                  wcd_fprintf(outfile,"%s/%s\n", tmp_ptr, dp->d_name);
            }
         }
      }
#else
      /* Not all systems have d_type. */
      static struct stat buf ;
      if (lstat(dp->d_name, &buf) == 0) {
         if (S_ISDIR(buf.st_mode)) {
            if (!SpecialDir(dp->d_name))
               finddirs(dp->d_name,offset, outfile, use_HOME, exclude, 1);
         } else if (S_ISLNK(buf.st_mode)) { /* Is it a symbolic link? */
            if ((stat(dp->d_name, &buf) == 0) && S_ISDIR(buf.st_mode)) /* does the link point to a dir */
               wcd_fprintf(outfile,"%s/%s\n", tmp_ptr, dp->d_name);
         }
      }
#endif
      dp = readdir(dirp);
   }
   if (closedir(dirp))
      print_error(_("Unable to close directory %s: %s\n"), curdir, strerror(errno));
#endif

   wcd_chdir(DIR_PARENT,1); /* go to parent directory */
}
