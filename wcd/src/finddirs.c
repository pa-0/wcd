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

#ifdef WCD_UNICODE
#  define UNICODE
#  define _UNICODE
#endif
#ifdef _WIN32
#  include <windows.h>
#endif
#include <string.h>
#include "dosdir.h"
#include "wcddir.h"
#include "wcd.h"
#include "wfixpath.h"
#include "nameset.h"
#include "config.h"
#include "display.h"

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

The Windows version of wcd don't use DOSDIR anymore. Using the Windows API functions results
in about 4 times faster disc scanning. EW Jan 2017.

*/

/********************************************************************
 *
 *                    rmTree(dir)
 *
 * Recursively delete directory: *dir
 *
 ********************************************************************/
#ifdef __DJGPP__
#  define WCD_FB_NAME  fb.ff_name
#  define WCD_FB_MODE  fb.ff_attrib
#  define WCD_FINDNEXT findnext
#else
#  define WCD_FB_NAME  fb.dd_name
#  define WCD_FB_MODE  fb.dd_mode
#  define WCD_FINDNEXT dd_findnext
#endif
void rmTree(char *dir)
{
#ifdef __DJGPP__
   static struct ffblk fb;   /* file block structure */
   int rc;                   /* error code */
#else
   static dd_ffblk fb;       /* file block structure */
   wcd_intptr_t rc;          /* error code */
#endif
   char tmp[DD_MAXPATH];     /* tmp string */
   char *errstr;
   TDirList list;            /* directory queue */

   if (dir)
   {
      if (wcd_chdir(dir,0)) return; /* Go to the dir, else return */
   }
   else
       return ;  /* dir == NULL */

/*
   Don't set DD_LABEL. Otherwise wcd compiled with
   Borland CPP 5.x bcc32 finds no files or directories at all.
   This was seen with Borland CPP 5.02 and 5.5.1.
   Apr 29 2005   Erwin Waterlander
*/
#ifdef __DJGPP__
   rc = findfirst( default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL);
#else
   rc = dd_findfirst( default_mask, &fb, DD_DIREC|DD_RDONLY|DD_HIDDEN|DD_SYSTEM|DD_ARCH|DD_DEVICE );
#endif
   /* Unix: dd_findfirst is a wrapper around 'opendir'. The directory opened in dd_findfirst
    * has to be closed by 'closedir' in dd_findnext. So do not exit this function until
    * the dd_findnext loop is complete. Otherwise directories will be left open and you
    * will have a memory leak. When too many directories are left open getcwd() may return
    * an incorrect pathname. */

   list.head = list.tail = 0;

   while (rc==0)   /* go through all the files in the current dir */
   {
      if (DD_ISDIREC(WCD_FB_MODE))
      {

#ifndef VMS
     /*  Ignore directory entries starting with '.'
      *  which includes the current and parent directories.
      */
         if (!SpecialDir(WCD_FB_NAME))
#endif /* ?!VMS */
            q_insert(&list, WCD_FB_NAME);   /* add all directories in current dir to list */
      }
      else
      if ( unlink(WCD_FB_NAME) != 0)  /* not a directory */
      {
         errstr = strerror(errno);
         print_error(_("Unable to remove file %s: %s\n"), WCD_FB_NAME, errstr);
      }

      rc = WCD_FINDNEXT(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) */
   while (q_remove(&list, tmp))
      {
        rmTree(tmp);
        wcd_rmdir(tmp,0);
      }

   wcd_chdir(DIR_PARENT,0); /* go to parent directory */
}

/********************************************************************
 *
 *  finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, int quiet)
 *
 ********************************************************************/
#if defined(_WIN32) && !defined(__CYGWIN__)

#ifdef WCD_UNICODE
#  define WCD_FINDFIRSTFILE FindFirstFileW
#  define WCD_FINDNEXTFILE  FindNextFileW
#else
#  define WCD_FINDFIRSTFILE FindFirstFileA
#  define WCD_FINDNEXTFILE  FindNextFileA
#endif
void finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet)
{
   char curdir[DD_MAXPATH];
   char nextdir[DD_MAXPATH];
   char *tmp_ptr ;
   size_t len ;
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;

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

   if (wcd_fprintf(outfile,"%s\n", tmp_ptr) < 0)
      return;  /* Quit when we can't write path to disk */

   hFind = WCD_FINDFIRSTFILE(ALL_FILES_MASK, &FindFileData);
   while (hFind != INVALID_HANDLE_VALUE)
   {
      if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         char directory[DD_MAXPATH];
#ifdef WCD_UNICODE
         wcstoutf8(directory, FindFileData.cFileName, sizeof(directory));
#else
         wcd_strncpy(directory,FindFileData.cFileName,sizeof(directory));
#endif
         if (!SpecialDir(directory)) {
            wcd_strncpy(nextdir,curdir,sizeof(nextdir));
            wcd_strncat(nextdir,"/",sizeof(nextdir));
            wcd_strncat(nextdir,directory,sizeof(nextdir));
            if(wcd_islink(nextdir,quiet))
               wcd_fprintf(outfile,"%s\n", nextdir);
            else
               finddirs(nextdir,offset, outfile, use_HOME, exclude, 1);
         }

      }
      if (!WCD_FINDNEXTFILE(hFind, &FindFileData)) {
          FindClose(hFind);
          hFind = INVALID_HANDLE_VALUE;
      }

   }
}
#else
void finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet)
{
#ifdef __DJGPP__
   static struct ffblk fb;       /* file block structure */
   int rc;                       /* error code */
#else
   static dd_ffblk fb;           /* file block structure */
   wcd_intptr_t rc;              /* error code */
#endif
   static char curdir[DD_MAXPATH];  /* curdir string buffer */
   size_t len ;
   TDirList list;                /* directory queue */
   char *tmp_ptr ;

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

   if (wcd_fprintf(outfile,"%s\n", tmp_ptr) < 0)
      return;  /* Quit when we can't write path to disk */

/*
   Don't set DD_LABEL. Otherwise wcd compiled with
   Borland CPP 5.x bcc32 finds no files or directories at all.
   This was seen with Borland CPP 5.02 and 5.5.1.
   Apr 29 2005   Erwin Waterlander
*/
#ifdef __DJGPP__
   rc = findfirst( default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL );
#else
   rc = dd_findfirst( default_mask, &fb, DD_DIREC|DD_RDONLY|DD_HIDDEN|DD_SYSTEM|DD_ARCH|DD_DEVICE );
#endif
   list.head = list.tail = 0;

   /* Unix: dd_findfirst is a wrapper around 'opendir'. The directory opened in dd_findfirst
    * has to be closed by 'closedir' in dd_findnext. So do not exit this function until
    * the dd_findnext loop is complete. Otherwise directories will be left open and you
    * will have a memory leak. When too many directories are left open getcwd() may return
    * an incorrect pathname. */

   while (rc==0)   /* go through all the files in the current dir */
   {
      if (DD_ISDIREC(WCD_FB_MODE))
      {
#ifndef VMS
         /*  Ignore directory entries starting with '.'
          *  which includes the current and parent directories. */
         if (!SpecialDir(WCD_FB_NAME))
#endif /* ?!VMS */
#ifdef _WIN32
         {
            /* On Windows a symbolic directory link is seen as a normal directory
             * by DD_ISDIREC().
             */
            if(wcd_islink(fb.dd_name,quiet))
               wcd_fprintf(outfile,"%s/%s\n", tmp_ptr, fb.dd_name);
            else
               q_insert(&list, fb.dd_name);   /* add all directories in current dir to list */
         }
#else
            q_insert(&list, WCD_FB_NAME);   /* add all directories in current dir to list */
#endif
      }

#if defined(UNIX) || ((defined(__OS2__) && defined(__EMX__)))
      if ( S_ISLNK(fb.dd_mode))  /* is it a link ? */
      {
        static struct stat buf ;

        if ((stat(fb.dd_name, &buf) == 0) && S_ISDIR(buf.st_mode)) /* does the link point to a dir */
           wcd_fprintf(outfile,"%s/%s\n", tmp_ptr, fb.dd_name);
      }
#endif
      rc = WCD_FINDNEXT(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) (quiet) */
   while (q_remove(&list, curdir))
      finddirs(curdir,offset, outfile, use_HOME, exclude, 1);

   /* Quiet, because on OS/2 changing to .. from a disk root dir gives an error. */
   wcd_chdir(DIR_PARENT,1); /* go to parent directory */
}
#endif
