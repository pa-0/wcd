/*
   filename: WCD.C

   WCD - Chdir for Dos and Unix.


Author: Erwin Waterlander

E-mail  : waterlan@xs4all.nl
WWW     : http://waterlan.home.xs4all.nl/

======================================================================
= Copyright                                                          =
======================================================================
Copyright (C) 1997-2011 Erwin Waterlander

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

=======================================================================

Jason Mathews' file filelist.c was a starting point for this file.

TAB = 3 spaces

*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifndef __TURBOC__
# include <unistd.h>
#endif
#ifdef __TURBOC__
#define __FLAT__
#endif
#include <sys/stat.h>
#ifdef DJGPP
# include <dir.h>
#endif
#include "wcd.h"
#if !defined(MSDOS) || defined(WIN32) || defined(OS2) /* Win32, OS/2, Unix, Cygwin */
#include <locale.h>
#endif

#if !defined(MSDOS)  /* Unix, Cygwin */
# include <langinfo.h>
#endif

#include "querycp.h"
#include "dosdir.h"
#include "match.h"
#ifdef WCD_UNICODE
#  include "matchw.h"
#else
#  include "matchl.h"
#endif
#include "std_macr.h"
#include "structur.h"
#include "Error.h"
#include "Text.h"
#include "nameset.h"
#include "WcdStack.h"
#include "dirnode.h"
#include "stack.h"
#include "display.h"
#include "wfixpath.h"
#include "graphics.h"
#include "wcddir.h"
#include "config.h"
#ifdef WCD_UTF16
#  include <wchar.h>
#endif
#if defined (WIN32) && !defined(__CYGWIN__)
#  include <windows.h>
#endif

#ifdef OS2
#undef UNIX
#endif

#ifdef __CYGWIN__
#  undef WIN32
#endif

/* Global variables */

const wcd_char *default_mask = ALL_FILES_MASK;

FILE *wcd_fopen(const char *filename, const char *m, int quiet)
{
  struct stat buf;
  FILE *f;
  char *errstr;

  //printf("wcd_fopen %s %d\n",filename, quiet);

  if (m[0] == 'r') /* we try to read an existing file */
  {
    if (stat(filename, &buf) != 0) /* check if file exists */
    {
      if ( !quiet )
      {
        errstr = strerror(errno);
        fprintf(stderr,_("Wcd: error: Unable to read file %s: %s\n"), filename, errstr);
      }
      return(NULL);
    }

    if (!S_ISREG(buf.st_mode)) /* check if it is a file */
    {
      if ( !quiet )
      {
        fprintf(stderr,_("Wcd: error: Unable to read file %s: Not a regular file.\n"), filename);
      }
      return(NULL);
    }
  }

  f = fopen(filename, m); /* open the file */
  if ( !quiet && (f == NULL))
  {
    errstr = strerror(errno);
    if (m[0] == 'r')
      fprintf(stderr,_("Wcd: error: Unable to read file %s: %s\n"), filename, errstr);
    else
      fprintf(stderr,_("Wcd: error: Unable to write file %s: %s\n"), filename, errstr);
  }
  return(f);
}

/********************************************************************
 * void cleanPath(char path[], int len, minlength)
 *
 * Clean path string from ending '/' character.
 *
 * Borland's chdir() function cannot change to paths ending with '/'
 * or '\' except root itself (/ or \).
 *
 ********************************************************************/

void cleanPath(char path[], int len, int minlength)
{
   /* strip ending separator (if present) */
   if (len > minlength && path[len-1]==DIR_SEPARATOR && path[len-2]!=':') path[len-1] = '\0';
}
void rmDriveLetter(char path[], int *use_HOME)
{
   char *ptr;

   if (path == NULL) return;

   /* remove drive letter */
   if (*use_HOME == 0 )
      if ( (ptr=strstr(path,"/")) != NULL)
         strcpy(path,ptr);
}

void rmDirFromList(char *string, nameset n)
{
   char dir[DD_MAXPATH];
   char subdir[DD_MAXPATH];
   size_t i;

   strcpy(dir,string);

   wcd_fixpath(dir,sizeof(dir));

   strcpy(subdir,dir);
   strcat(subdir,"/*");

   i = 0;
   while (i < n->size )
   {
#ifdef MSDOS
      if( dd_match(n->array[i],dir,1) || dd_match(n->array[i],subdir,1))
#else
      if( dd_match(n->array[i],dir,0) || dd_match(n->array[i],subdir,0))
#endif
       removeElementAtNamesetArray(i,n,true);
      else
      ++i;
   }
}

/********************************/
void writeList(char * filename, nameset n)
{
   size_t i;
   FILE *outfile;

   if ( (outfile = wcd_fopen(filename,"w",0)) != NULL)
   {
      for(i=0;(i<n->size);i++)
      {
         fprintf(outfile,"%s\n",n->array[i]);
      }
      fclose(outfile);
   }
}
#ifdef UNIX
/*************************************************************
 * void stripTmpMnt(char* path)
 *
 * This function stips /tmp_mnt from path if it begins with it.
 * /tmp_mnt is used by the automounter.
 *
 *************************************************************/
void stripTmpMnt(char* path)
{
  char* ptr;

  if (path != NULL)
  {
     if (strncmp(path,TMP_MNT_STR,strlen(TMP_MNT_STR)) == 0)
    {
       ptr = path + strlen(TMP_MNT_STR) - 1 ;
      strcpy(path,ptr);
    }
  }

}
#endif

#ifdef WCD_UNIXSHELL

/*************************************************************
 *
 * void quoteString(char *string)
 *
 * -----
 * quote all metacharacters (; & $ ( ) | < > space), quotes ("),
 * apostrophes ('), grave accents (`), wildcard characters
 * (* ? [ ]), and backslashes (\) with a backslash.
 *
 * Quoting every character with a backslash seems to be
 * the most portable solution. Using apostrophes ('...') to quote the whole
 * string makes it impossible to quote an apostrophe in the path. Using quotes
 * ("...") does not work well in csh, because you can't escape a $ character
 * inside quotes to stop variable substitution, and you can't escape a grave
 * accent.
 *
 * Functions is_slash() and is_term() in file wfixpath.c have also
 * been modified so that a backslash is not replaced by slash anymore
 * on UNIX systems.
 *
 * Erwin Waterlander Oct 17 2001
 *************************************************************/

void quoteString(char *string)
{
 size_t i, j;
 int k,kmax;
 char help1_str[DD_MAXPATH];

 j = strlen(string);
 k = 0;
 kmax = DD_MAXPATH -3;

 for (i=0; (i < j)&&(k < kmax) ; i++)
 {
   if ( (string[i] == '"') ||
        (string[i] == '$') ||
        (string[i] == '&') ||
        (string[i] == '\'') ||
        (string[i] == '(') ||
        (string[i] == ')') ||
        (string[i] == '*') ||
        (string[i] == ';') ||
        (string[i] == '<') ||
        (string[i] == '>') ||
        (string[i] == '?') ||
        (string[i] == '[') ||
        (string[i] == '\\') ||
        (string[i] == ']') ||
        (string[i] == ' ') ||
        (string[i] == '`') ||
        (string[i] == '|')
      )
   {
      help1_str[k] = '\\';
     k++;
   }
   help1_str[k] = string[i];
   k++ ;
 }
 help1_str[k] = '\0' ;

 strcpy(string,help1_str);
}
#endif

/*************************************************************
 *
 * void quoteString(char *string)
 *
 * we need backslashes instead of forward slashes.
 *
 * -----
 * Quoting ("...") is needed for paths that contain '&' characters
 * Quote '%' characters with '%', needed when cd command is executed
 * via batch file.
 * Oct 16 2001
 *
 *************************************************************/
#if (defined(WIN32) && !defined(WCD_WINZSH)) || (defined(OS2) && !defined(WCD_OS2BASH))
void quoteString(char *string)
{
 size_t i, j;
 int k,kmax;
 char help1_str[DD_MAXPATH];

 j = strlen(string);
 help1_str[0] = '"';
 k = 1;
 kmax = DD_MAXPATH -3;

 for (i=0; (i < j)&&(k < kmax) ; i++)
 {
#ifndef WCD_WINPWRSH
   if (string[i] == '%')
   {
      help1_str[k] = '%';
     k++;
   }
#endif

   if (string[i] == '/')
      help1_str[k] = '\\';
   else
      help1_str[k] = string[i];
   k++ ;
 }
 help1_str[k] = '"' ;
 k++;
 help1_str[k] = '\0' ;

 strcpy(string,help1_str);

}
#endif

/*************************************************************
 *
 *  changeDisk
 *
 *  Returns:
 *  On succes : the new drive number >=0, or the current
 *              disk number >=0 if there was no drive to go to.
 *  Fail      : -1
 *
 *************************************************************/

#ifdef MSDOS
int changeDisk(char *path, int *changed, char *newdrive, int *use_HOME)
{
   int i = -1, destDisk;
   char *ptr;
   char drive[DD_MAXDRIVE];


   i = getdisk();  /* current disk */

   if (strlen(path)>1)
   {

      strncpy(drive,path,2);
      drive[DD_MAXDRIVE-1] = '\0';

      if (dd_match(drive,"[a-z]:",1))
      {
         destDisk = islower(*drive) ? *drive-'a' : *drive-'A';

         if (destDisk >= 0)
         {
            setdisk(destDisk);
            i = getdisk();    /* new disk */
         }

         if ((i==destDisk) && (i>=0))  /* succes ? */
         {
            *changed = 1 ;
            strcpy(newdrive,drive) ;

           if((use_HOME == NULL)||(*use_HOME == 0))
           {
            ptr = path + 2;
            if (strcmp(ptr,"") == 0)
            {
               strcpy(path,"/");
            }
            else
            {
               strcpy(path,ptr);
            }
           }
         }
         else
           i = -1;
      }
   }
   return(i);
}
#endif
/*****************************************************************/
void getCurPath(char *buffer, size_t size, int *use_HOME)
{
 size_t len;

 if(wcd_getcwd(buffer, size) != NULL)
 {
   len = strlen(buffer);
   if (len==0)
      buffer[len] = '\0';
   wcd_fixpath(buffer,size);
   rmDriveLetter(buffer,use_HOME);
 }
}
/*****************************************************************
 *  add current path to file.
 *
 *****************************************************************/
void addCurPathToFile(char *filename,int *use_HOME, int parents)
{

 char tmp[DD_MAXPATH];      /* tmp string buffer */
 FILE *outfile;

 getCurPath(tmp,(size_t)DD_MAXPATH,use_HOME);

 if(tmp != NULL)
 {
   /* open the treedata file */
   if  ((outfile = wcd_fopen(filename,"a",0)) != NULL)
   {
     fprintf(outfile,"%s\n",tmp);
     wcd_printf(_("Wcd: %s added to file %s\n"),tmp,filename);

     if (parents)
     {
     char *ptr ;

      while ( (ptr = strrchr(tmp,DIR_SEPARATOR)) != NULL)
      {
         *ptr = '\0' ;
         /* keep one last separator in the path */
         if (strrchr(tmp,DIR_SEPARATOR) != NULL)
         {
            fprintf(outfile,"%s\n",tmp);
            wcd_printf(_("Wcd: %s added to file %s\n"),tmp,filename);
         }
      }
     }
     fclose(outfile);
   }
 }

}
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

/********************************************************************
 *
 *                    rmTree(dir)
 *
 * Recursively delete directory: *dir
 *
 ********************************************************************/
#ifdef DJGPP
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
*/
void rmTree(char *dir)
{
   static struct ffblk fb;       /* file block structure */
   char tmp[DD_MAXPATH];                /* tmp string */
   int rc;                       /* error code */
   TDirList list;                /* directory queue */
   char *errstr;

   if (dir)
   {
      if (wcd_chdir(dir,0)) return; /* Go to the dir, else return */
   }
   else
       return ;  /* dir == NULL */

   rc = findfirst( default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL);
   list.head = list.tail = 0;

   while (rc==0)   /* go through all the files in the current dir */
   {
      if (DD_ISDIREC(fb.ff_attrib))
      {

#ifndef VMS
         /*  Ignore directory entries starting with '.'
      *  which includes the current and parent directories.
      */
         if (!SpecialDir(fb.ff_name))
#endif /* ?!VMS */
            q_insert(&list, fb.ff_name);   /* add all directories in current dir to list */
      }
      else
      if ( unlink(fb.ff_name) != 0)  /* not a directory */
      {
         errstr = strerror(errno);
         fprintf(stderr,_("Wcd: error: Unable to remove file %s: %s\n"), fb.ff_name, errstr);
      }

      rc = findnext(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) */
   while (q_remove(&list, tmp))
      {
        rmTree(tmp);
        wcd_rmdir(tmp,0);
      }

   wcd_chdir(DIR_PARENT,0); /* go to parent directory */
}

#else /* not DJGPP */

void rmTree(char *dir)
{
   static dd_ffblk fb;       /* file block structure */
   char tmp[DD_MAXPATH];                /* tmp string */
   int rc;                       /* error code */
   char *errstr;
   TDirList list;                /* directory queue */

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
   rc = dd_findfirst( default_mask, &fb, DD_DIREC|DD_RDONLY|DD_HIDDEN|DD_SYSTEM|DD_ARCH|DD_DEVICE );
   /* Unix: dd_findfirst is a wrapper around 'opendir'. The directory opened in dd_findfirst
    * has to be closed by 'closedir' in dd_findnext. So do not exit this function until
    * the dd_findnext loop is complete. Otherwise directories will be left open and you
    * will have a memory leak. When too many directories are left open getcwd() may return
    * an incorrect pathname. */

   list.head = list.tail = 0;

   while (rc==0)   /* go through all the files in the current dir */
   {
      if (DD_ISDIREC(fb.dd_mode))
      {

#ifndef VMS
         /*  Ignore directory entries starting with '.'
      *  which includes the current and parent directories.
      */
         if (!SpecialDir(fb.dd_name))
#endif /* ?!VMS */
            q_insert(&list, fb.dd_name);   /* add all directories in current dir to list */
      }
      else
      if ( unlink(fb.dd_name) != 0)  /* not a directory */
      {
         errstr = strerror(errno);
         fprintf(stderr,_("Wcd: error: Unable to remove file %s: %s\n"), fb.dd_name, errstr);
      }

      rc = dd_findnext(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) */
   while (q_remove(&list, tmp))
      {
        rmTree(tmp);
        wcd_rmdir(tmp,0);
      }

   wcd_chdir(DIR_PARENT,0); /* go to parent directory */
}
#endif

/********************************************************************
 *
 *  pathInNameset(char *path, nameset set)
 *
 *  Test if *path is a directory or a subdirectory of one
 *  of the paths in set.
 *
 *  set is a list of paths.
 *  wildcards are supported.
 *
 *
 *  Returns (size_t)-1 if directory is not in nameset.
 *  Returns index number (>=0) of first match found if directory is in nameset.
 *
 ********************************************************************/
size_t pathInNameset (text path, nameset set)
{
   char tmp[DD_MAXPATH+2];
   size_t size, index = 0;

   if ((path == NULL)||(set == NULL))
      return((size_t)-1);

   size = getSizeOfNamesetArray(set);

   while (index < size)
   {
      strcpy(tmp,set->array[index]);
      strcat(tmp,"/*");

#ifdef MSDOS
      if ((dd_match(path,set->array[index],1)) || (dd_match(path, tmp,1)))
#else
      if ((dd_match(path,set->array[index],0)) || (dd_match(path, tmp,0)))
#endif
      {
         return(index);
      }
      ++index;
   }
   return((size_t)-1);
}

/********************************************************************
 *
 *  finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, int quiet)
 *
 ********************************************************************/
#ifdef DJGPP
/*
 * See comment above function rmTree().
 */
void finddirs(char* dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet)
{
   static struct ffblk fb;       /* file block structure */
   static char tmp[DD_MAXPATH];      /* tmp string buffer */
   int rc;                       /* error code */
   size_t len ;
   TDirList list;                /* directory queue */
   char *tmp_ptr ;


   if (dir)
   {
      if (wcd_chdir(dir,quiet)) return; /* ?err */
   }
   else
     return ;  /* dir == NULL */

   if (wcd_getcwd(tmp, sizeof(tmp)) == NULL)
   {
      fprintf(stdout,_("Wcd: error: finddirs(): can't determine path in directory %s\nWcd: path probably too long.\n"),dir);
      wcd_chdir(DIR_PARENT,quiet); /* go to parent directory */
      return;
   };

   wcd_fixpath(tmp,sizeof(tmp));
   rmDriveLetter(tmp,use_HOME);

   if (pathInNameset(tmp,exclude) != (size_t)-1)
   {
      wcd_chdir(DIR_PARENT,quiet); /* go to parent directory */
      return;
   }

   len = strlen(tmp);

   if(*offset < len)
     tmp_ptr = tmp + *offset ;
   else
     tmp_ptr = tmp + len;   /* tmp_ptr points to ending '\0' of tmp */

   fprintf(outfile,"%s\n", tmp_ptr);

   rc = findfirst( default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL );
   list.head = list.tail = 0;

   while (rc==0)   /* go through all the files in the current dir */
   {
      if (DD_ISDIREC(fb.ff_attrib))
      {
         /*  Ignore directory entries starting with '.'
            *  which includes the current and parent directories.
            */
         if (!SpecialDir(fb.ff_name))
            q_insert(&list, fb.ff_name);
      }
      rc = findnext(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) (quiet) */
   while (q_remove(&list, tmp))
      finddirs(tmp,offset, outfile, use_HOME, exclude, 1);

   if (dir) wcd_chdir(DIR_PARENT,quiet); /* go to parent directory */
}

#else /* not DJGPP */

void finddirs(char *dir, size_t *offset, FILE *outfile, int *use_HOME, nameset exclude, int quiet)
{
   static dd_ffblk fb;       /* file block structure */
   static char tmp[DD_MAXPATH];      /* tmp string buffer */
   int rc;                       /* error code */
   size_t len ;
   TDirList list;                /* directory queue */
   char *tmp_ptr ;

   if (dir)
   {
      if (wcd_chdir(dir,quiet)) return; /* Go to the dir, else return */
   }
   else
     return ;  /* dir == NULL */


   if (wcd_getcwd(tmp, sizeof(tmp)) == NULL)
   {
      fprintf(stdout,_("Wcd: error: finddirs(): can't determine path in directory %s\nWcd: path probably too long.\n"),dir);
      wcd_chdir(DIR_PARENT,quiet); /* go to parent directory */
      return;
   };

#ifdef MSDOS
   wcd_fixpath(tmp,sizeof(tmp));
   rmDriveLetter(tmp,use_HOME);
#endif

   if (pathInNameset(tmp,exclude) != (size_t)-1)
   {
      wcd_chdir(DIR_PARENT,quiet); /* go to parent directory */
      return;
   }

   len = strlen(tmp);

   if(*offset < len)
     tmp_ptr = tmp + *offset ;
   else
     tmp_ptr = tmp + len;   /* tmp_ptr points to ending '\0' of tmp */

   fprintf(outfile,"%s\n", tmp_ptr);

/*
   Don't set DD_LABEL. Otherwise wcd compiled with
   Borland CPP 5.x bcc32 finds no files or directories at all.
   This was seen with Borland CPP 5.02 and 5.5.1.
   Apr 29 2005   Erwin Waterlander
*/
   rc = dd_findfirst( default_mask, &fb, DD_DIREC|DD_RDONLY|DD_HIDDEN|DD_SYSTEM|DD_ARCH|DD_DEVICE );
   list.head = list.tail = 0;

   /* Unix: dd_findfirst is a wrapper around 'opendir'. The directory opened in dd_findfirst
    * has to be closed by 'closedir' in dd_findnext. So do not exit this function until
    * the dd_findnext loop is complete. Otherwise directories will be left open and you
    * will have a memory leak. When too many directories are left open getcwd() may return
    * an incorrect pathname. */

   while (rc==0)   /* go through all the files in the current dir */
   {
      if (DD_ISDIREC(fb.dd_mode))
      {

#ifndef VMS
         /*  Ignore directory entries starting with '.'
      *  which includes the current and parent directories.
      */
         if (!SpecialDir(fb.dd_name))
#endif /* ?!VMS */
            q_insert(&list, fb.dd_name);   /* add all directories in current dir to list */
      }

#ifdef UNIX
      if ( S_ISLNK(fb.dd_mode))  /* is it a link ? */
      {
        static struct stat buf ;

        if ((stat(fb.dd_name, &buf) == 0) && S_ISDIR(buf.st_mode)) /* does the link point to a dir */
           fprintf(outfile,"%s/%s\n", tmp_ptr, fb.dd_name);
      }
#endif
      rc = dd_findnext(&fb);
   } /* while !rc */

   /* recursively parse subdirectories (if any) (quiet) */
   while (q_remove(&list, tmp))
      finddirs(tmp,offset, outfile, use_HOME, exclude, 1);

   wcd_chdir(DIR_PARENT,quiet); /* go to parent directory */
}
#endif

/********************************************************************
 *
 *     scanDisk(char *path, char *treefile, int scanreldir, size_t append, int *use_HOME)
 *
 *     append == 1        : append to treefile
 *     scanreldir == 1    : write relative paths in rtdata.wcd
 *
 ********************************************************************/
void scanDisk(char *path, char *treefile, int scanreldir, size_t append, int *use_HOME, nameset exclude)
{
   size_t  offset = 0 ;     /* offset to remove scanned from path */
   char tmp[DD_MAXPATH];    /* tmp string buffer */
   char tmp2[DD_MAXPATH];   /* tmp string buffer */
   FILE *outfile;
#ifdef MSDOS
   char drive[DD_MAXDRIVE];
   int  changedrive = 0;
#endif

   wcd_fixpath(path,(size_t)DD_MAXPATH);
   wcd_fixpath(treefile,(size_t)DD_MAXPATH);
   wcd_getcwd(tmp2, sizeof(tmp2)); /* remember current dir */

   if(wcd_isdir(path,0) != 0)
   {
      wcd_printf(_("Wcd: %s is not a directory.\n"),path);
      return ;
   }

   wcd_printf(_("Wcd: Please wait. Scanning disk. Building treedata-file %s from %s\n"),treefile, path);

#ifdef MSDOS
      changeDisk(path,&changedrive,drive,use_HOME);
#endif
   if (scanreldir)
   {
     if ( !wcd_chdir(path,0) )
     {
      wcd_getcwd(tmp, sizeof(tmp)); /* get full path */
#ifdef MSDOS
          wcd_fixpath(tmp,sizeof(tmp));
          rmDriveLetter(tmp,use_HOME);
#endif
      offset = strlen(tmp);
      /* do not count ending separator (if present) */
      if (offset > 0 && tmp[offset-1]==DIR_SEPARATOR)
        offset--;
      offset++;
     }
     wcd_chdir(tmp2,0);          /* go back */
    }

#ifdef MSDOS

   /* open the output file */
   if (append)
     outfile = wcd_fopen(treefile,"a",0);  /* append to database */
   else
     outfile = wcd_fopen(treefile,"w",0);  /* create new database */

   if (!outfile) /* Try to open in a temp dir */
   {
      char *ptr;

      if  ( (ptr = getenv("TEMP")) != NULL )
      {
      strcpy(treefile,ptr);
      strcat(treefile,TREEFILE);
      outfile = wcd_fopen(treefile,"w",1);
      }

      if (outfile == NULL)
      {
         if  ( (ptr = getenv("TMP")) != NULL )
            {
            strcpy(treefile,ptr);
            strcat(treefile,TREEFILE);
            outfile = wcd_fopen(treefile,"w",1);
            }

         if (outfile == NULL)
         {
            if  ( (ptr = getenv("TMPDIR")) != NULL )
               {
               strcpy(treefile,ptr);
               strcat(treefile,TREEFILE);
               outfile = wcd_fopen(treefile,"w",1);
               }
         }
      }
   }

   if (outfile == NULL) /* Did we succeed? */
   {
      fprintf(stderr, "%s", _("Wcd: error: Write access to tree-file denied.\nWcd: Set TEMP environment variable if this is a read-only disk.\n"));
      return ;
   }
#else
   /* open the output file */
   if (append)
     outfile = wcd_fopen(treefile,"a",0);  /* append to database */
   else
     outfile = wcd_fopen(treefile,"w",0);  /* create new database */

   if  (outfile == NULL)
      return ;
#endif
   finddirs( path, &offset, outfile, use_HOME, exclude, 0); /* Build treedata-file */
   fclose(outfile);
   wcd_chdir(tmp2,0);          /* go back */
}

#if (defined(WIN32) || defined(__CYGWIN__))
/***********************************************************************
 * scanServer()
 * scan all the shared directories on a server
 ***********************************************************************/
void scanServer(char *path, char *treefile, size_t append, int *use_HOME, nameset exclude)
{
   size_t i;
   nameset shares;

   shares = namesetNew();
   wcd_getshares(path, shares);

   i = 0;
   while (i<getSizeOfNamesetArray(shares))
   {
      scanDisk(elementAtNamesetArray(i,shares),treefile, 0, i + append, use_HOME, exclude);
      ++i;
   }
   freeNameset(shares, 1);
}
#endif

/********************************************************************
 *
 *     makeDir(char *path, char *treefile, int *use_HOME)
 *
 ********************************************************************/
void makeDir(char *path, char *treefile, int *use_HOME)
{
   char tmp2[DD_MAXPATH];
#if (defined(UNIX) || defined(DJGPP) || defined(__OS2__))
   mode_t m;
#endif
#ifdef MSDOS
   char drive[DD_MAXDRIVE];
   int  changedrive = 0;
#endif

   wcd_fixpath(path,(size_t)DD_MAXPATH);

#if (defined(DJGPP) || defined(__OS2__))
   /* is there a drive to go to ? */
   changeDisk(path,&changedrive,drive,use_HOME);
   m = S_IRWXU | S_IRWXG | S_IRWXO;
   if (wcd_mkdir(path,m,0)==0)
#elif defined(UNIX)
   m = S_IRWXU | S_IRWXG | S_IRWXO;
   if (wcd_mkdir(path,m,0)==0)
#else
   /* is there a drive to go to ? */
   changeDisk(path,&changedrive,drive,use_HOME);
   if (wcd_mkdir(path,0)==0)
#endif
   {
      wcd_getcwd(tmp2, (size_t)DD_MAXPATH);  /* remember current dir */
      if(wcd_chdir(path,0)==0)        /* goto dir and add */
       addCurPathToFile(treefile,use_HOME,0);
      wcd_chdir(tmp2,0) ;                /* go back */
   }
}

/********************************************************************
 *
 *     deleteLink(char *path, char *treefile)
 *
 ********************************************************************/

#ifdef UNIX
void deleteLink(char *path, char *treefile)
{
   static struct stat buf ;
   char tmp2[DD_MAXPATH];
   char *errstr;

 if (stat(path, &buf) == 0)
 {
   if (S_ISDIR(buf.st_mode)) /* does the link point to a dir */
   {
        char *line_end ;

        /* get the parent path of the link */

        if( (line_end = strrchr(path,DIR_SEPARATOR)) != NULL)
          {
            *line_end = '\0' ;
            line_end++;
            wcd_chdir(path,0);  /* change to parent dir of link */
          }
        else
          line_end = path;  /* we were are already there */

        strcpy(tmp2,line_end);
        wcd_getcwd(path, (size_t)DD_MAXPATH);  /* get the full path of parent dir*/
        strcat(path,"/");
        strcat(path,tmp2);
        wcd_fixpath(path,(size_t)DD_MAXPATH);

        if (unlink(tmp2)==0)    /* delete the link */
          {
            wcd_printf(_("Wcd: Removed symbolic link %s\n"),path);
            cleanTreeFile(treefile,path);
          }
        else
        {
          errstr = strerror(errno);
          fprintf(stderr,_("Wcd: error: Unable to remove symbolic link %s: %s\n"),path, errstr);
        }
   }
   else
      wcd_printf(_("Wcd: %s is a symbolic link to a file.\n"),path);
 }
 else
 {
   errstr = strerror(errno);
   fprintf(stderr,"Wcd: error: %s: %s\n",path,errstr);
 }

}
#endif

/********************************************************************
 *
 *     deleteDir(char *path, char *treefile, int recursive, int *use_HOME)
 *
 ********************************************************************/
void deleteDir(char *path, char *treefile, int recursive, int *use_HOME)
{
#ifdef UNIX
   static struct stat buf ;
   char *errstr;
#endif
#ifdef MSDOS
   char drive[DD_MAXDRIVE];
   int  changedrive = 0;
#endif
   char tmp2[DD_MAXPATH];

   wcd_fixpath(path,(size_t)DD_MAXPATH);

#ifdef UNIX
   if (lstat(path, &buf) != 0)
   {
     errstr = strerror(errno);
     fprintf(stderr,"Wcd: error: %s: %s\n",path,errstr);
     return;
   }

   if (S_ISLNK(buf.st_mode))  /* is it a link? */
   {
      deleteLink(path,treefile);
   }
   else
   {
#else
   /* is there a drive to go to ? */
   changeDisk(path,&changedrive,drive,use_HOME);
#endif

   if (wcd_isdir(path,0) == 0) /* is it a dir */
   {
      wcd_getcwd(tmp2, (size_t)DD_MAXPATH);  /* remember current path */

      if(wcd_chdir(path,0)==0)
      {
         wcd_getcwd(path, (size_t)DD_MAXPATH);   /* path to remove */

#ifdef MSDOS
         wcd_fixpath(path,DD_MAXPATH);
         rmDriveLetter(path,use_HOME);
#endif
         wcd_chdir(tmp2,0);
      }

      if(recursive)
      {
         int c ;

         c = 'x' ;

         while ( (c != 'y') && (c != 'Y') && (c != 'n') && (c != 'N'))
         {
            printf(_("Wcd: Recursively remove %s  Are you sure? y/n :"),path);

                /* Note that getchar reads from stdin and
                   is line buffered; this means it will
                   not return until you press ENTER. */

            c = getchar(); /* get first char */
            if (c != '\n')
               while ((getchar()) != '\n') ; /* skip the rest */
         }
            if ( (c == 'y') ||  (c == 'Y') )
            {

            wcd_chdir(tmp2,0);       /* go back */
            rmTree(path);       /* delete the stuff */
            wcd_chdir(tmp2,0);       /* go back */

            wcd_rmdir(path,0);

            cleanTreeFile(treefile,path);

         } /* ( (c != 'y') ||  (c != 'Y') ) */
      }
      else
        if (wcd_rmdir(path,0)==0)
        {
          wcd_printf(_("Wcd: Removed directory %s\n"),path);
          cleanTreeFile(treefile,path);
        }
   }
   else
     wcd_printf(_("Wcd: %s is not a directory.\n"),path);

#ifdef UNIX
    }
#endif
}
/********************************************************************
 *
 * Read a line from an already opened file.
 * Returns: length of the string , not including the ter-
 *          minating `\0' character.
 ********************************************************************/

int wcd_getline(char s[], int lim, FILE* infile)
{
   int c, i ;

   for (i=0; i<lim-1 && ((c=getc(infile)) != '\n') && (!feof(infile)) ; ++i)
      {
      s[i] = (char)c ;
      if (c == '\r') i--;
   }

   s[i] = '\0' ;

   if (i == lim-1)
      fprintf(stderr,_("Wcd: error: line too long in wcd_getline() ( > %d). Fix: increase DD_MAXPATH.\n"),(lim-1));

   return i ;
}

#ifdef WIN32
int wcd_wgetline(wchar_t s[], int lim, FILE* infile)
{
   int i ;
   wint_t c;

   for (i=0; i<lim-1 && ((c=fgetwc(infile)) != L'\n') && (!feof(infile)) ; ++i)
      {
      s[i] = (wchar_t)c ;
      if (c == L'\r') i--;
   }

   s[i] = L'\0' ;

   if (i == lim-1)
      fprintf(stderr,_("Wcd: error: line too long in wcd_getline() ( > %d). Fix: increase DD_MAXPATH.\n"),(lim-1));

   return i ;
}

int wcd_wgetline_be(wchar_t s[], int lim, FILE* infile)
{
   int i ;
   wint_t c;

   for (i=0; i<lim-1 && ((c=fgetwc(infile)) != 0xa00) && (!feof(infile)) ; ++i)
      {
      s[i] = (wchar_t)c ;
      printf ("i=%d c=%x\n",i,s[i]);
      s[i] = MAKEWORD( HIBYTE(s[i]), LOBYTE(s[i]));
      printf ("i=%d c=%d (%c)\n",i,s[i],(char)s[i]);
      if (c == 0xd00) i--;
   }

   s[i] = L'\0' ;

   if (i == lim-1)
      fprintf(stderr,_("Wcd: error: line too long in wcd_getline() ( > %d). Fix: increase DD_MAXPATH.\n"),(lim-1));

   return i ;
}
#endif

/********************************************************************
 *
 * Read treefile in a structure.
 *
 ********************************************************************/
void read_treefileA(FILE *f, nameset bd)
{
    int len;
    char path[DD_MAXPATH];

    while (!feof(f) )
    {
       /* read a line */
       len = wcd_getline(path,DD_MAXPATH,f);

       if (len > 0 )
       {
          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) ) */
}
#ifdef WIN32
void read_treefileUTF16LE(FILE *f, nameset bd)
{
    int len;
    char path[DD_MAXPATH];
    wchar_t pathw[DD_MAXPATH];

    while (!feof(f) )
    {
       /* read a line */
       len = wcd_wgetline(pathw,DD_MAXPATH,f);
       printf("len:%d\n",len);
       wcstoutf8(path,pathw,sizeof(path));
       printf("path:%s\n",path);

       if (len > 0 )
       {
          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) ) */
}
void read_treefileUTF16BE(FILE *f, nameset bd)
{
    int len;
    char path[DD_MAXPATH];
    wchar_t pathw[DD_MAXPATH];

    while (!feof(f) )
    {
       /* read a line */
       len = wcd_wgetline_be(pathw,DD_MAXPATH,f);
       printf("len:%d\n",len);
       wcstoutf8(path,pathw,sizeof(path));
       printf("path:%s\n",path);

       if (len > 0 )
       {
          wcd_fixpath(path,sizeof(path));
          addToNamesetArray(textNew(path),bd);
       }
    } /* while (!feof(f) ) */
}
#endif
void read_treefile(char* filename, nameset bd, int quiet)
{
   FILE *infile;
   int bom[3];
   /* BOMs
    * UTF16-LE  ff fe
    * UTF16-BE  fe ff
    * UTF-8     ef bb bf
    */

   printf ("r %x\n", '\r');
   /* open treedata-file */
   if  ((infile = wcd_fopen(filename,"rb", quiet)) != NULL)
   {
      if ((bom[0] = fgetc(infile)) == EOF)
      {
         fclose(infile);
         return;
      }
      if ((bom[0] != 0xff) && (bom[0] != 0xfe) && (bom[0] != 0xef))
      {
         ungetc(bom[0], infile);
         read_treefileA(infile, bd);
         fclose(infile);
         return;
      }
      if ((bom[1] = fgetc(infile)) == EOF)
      {
         fclose(infile);
         return;
      }
#ifdef WIN32
      if ((bom[0] == 0xff) && (bom[1] == 0xfe)) /* UTF16-LE */
      {
         printf("UTF16-LE\n");
         read_treefileUTF16LE(infile, bd);
         fclose(infile);
         return;
      }
      if ((bom[0] == 0xfe) && (bom[1] == 0xff)) /* UTF16-BE */
      {
         printf("UTF16-BE\n");
         read_treefileUTF16BE(infile, bd);
         fclose(infile);
         return;
      }
#endif
      if (((bom[2] = fgetc(infile))== 0xbf) && (bom[0] == 0xef) && (bom[1] == 0xbb)) /* UTF-8 */
      {
         printf("UTF-8\n");
         read_treefileA(infile, bd);
         fclose(infile);
         return;
      }
      printf("OTHER\n");
      ungetc(bom[2], infile);
      ungetc(bom[1], infile);
      ungetc(bom[0], infile);
      read_treefileA(infile, bd);
      fclose(infile);
      return;
   }
}
/********************************************************************
 *
 *    void cleanTreeFile(char *filename, char *dir)
 *
 *  remove path from treefile
 *
 ********************************************************************/

void cleanTreeFile(char *filename, char *dir)
{
   nameset dirs;

   dirs = namesetNew();
   read_treefile(filename,dirs,0);
   rmDirFromList(dir,dirs);
   writeList(filename, dirs);
   freeNameset(dirs, 1);
}


/********************************************************************
 *
 *                    check_double_match(char *dir, nameset set)
 *
 *  Returns 0 if directory had no match before.
 *  Returns 1 if directory is double matched.
 *
 ********************************************************************/

int check_double_match(char *dir, nameset set)
{
   size_t i = 0;

   if ((dir == NULL) || (set == NULL))
      return(0);

   while(i < set->size)
   {
#ifdef MSDOS
      if( stricmp(set->array[i],dir) == 0) return(1);
#else
      if( strcmp(set->array[i],dir) == 0) return(1);
#endif
      ++i;
   }

   return(0);
}

/********************************************************************
 *
 *                    check_filter(char *dir, nameset filter)
 *
 *  Returns 0 if directory is in filter list or filter list is empty.
 *  Returns 1 if directory is in filter list.
 *
 ********************************************************************/
int check_filter(char *dir, nameset filter)
{
   size_t index = 0;

   if (filter->size == 0) return (0);

   while (index < filter->size)
   {
#ifdef MSDOS
      if(dd_match(dir,filter->array[index],1)) return(0);
#else
      if(dd_match(dir,filter->array[index],0)) return(0);
#endif
         ++index;
   }
   return(1);
}
/********************************************************************
 *
 *    scanfile(char *org_dir, char *filename, int
 *             ignore_case, nameset pm, nameset wm, nameset bd, int nfs)
 *
 *
 ********************************************************************/

void scanfile(char *org_dir, char *filename, int ignore_case,
              nameset pm, nameset wm, nameset bd, nameset filter, int relative, int wildOnly, int ignore_diacritics)
{
   FILE *infile;
   char line[DD_MAXPATH];            /* database path */
   char *line_end;                  /* database directory */
   char path_str[DD_MAXPATH];        /* path name to match */
   char dirwild_str[DD_MAXPATH];     /* directory name to wild match */
   char *dir_str ;                  /* directory name to perfect match */
   char relative_prefix[DD_MAXPATH];      /* relative prefix */
   char tmp[DD_MAXPATH];
   int wild = 0;

   /* open treedata-file */
   if  ((infile = wcd_fopen(filename,"r",0)) == NULL)
      return;

   if( (dir_str = strrchr(org_dir,DIR_SEPARATOR)) != NULL)
      dir_str++;
   else dir_str = org_dir;

   strcpy(dirwild_str,dir_str);

#ifdef MSDOS
   if ((strlen(org_dir)>1) && (dd_match(org_dir,"[a-z]:*",1)))
   {
     /* If user searches "c:bin" (a directory "bin" on drive c:) set path_str to "c:*bin" */
     strncpy(path_str,org_dir,2);
     path_str[DD_MAXDRIVE-1] = '\0';
     line_end = org_dir + DD_MAXDRIVE ;
     strcat(path_str,"*");
     strcat(path_str,line_end);
   }
   else
#endif
   {
     strcpy(path_str,"*");
     strcat(path_str,org_dir);
   }

   if (!dd_iswild(dir_str))
   {
      strcat(dirwild_str,"*");
      strcat(path_str,"*");
      wild = 0;
   }
   else
      wild = 1;

   if (wildOnly)
      wild = 1;

   if (relative)
   {
      strcpy(relative_prefix,filename);
      if( (line_end = strrchr(relative_prefix,DIR_SEPARATOR)) != NULL)
      {
         line_end++ ;
         *line_end = '\0';
      }
      else
        relative_prefix[0] = '\0';
   }

   while (!feof(infile) )  /* parse the file */
   {
      int len;

      /* read a line */
      len = wcd_getline(line,DD_MAXPATH,infile);

      cleanPath(line,len,1) ;

      if( (line_end = strrchr(line,DIR_SEPARATOR)) != NULL)
         line_end++;
      else
         line_end = line;

      /* test for a perfect match */

#ifdef WCD_UNICODE
      if ( (wild == 0) && (dd_matchmbs(line_end,dir_str,ignore_case,ignore_diacritics))  &&
           (dd_matchmbs(line,path_str,ignore_case,ignore_diacritics)) )
#else
      if ( (wild == 0) && (dd_matchl(line_end,dir_str,ignore_case,ignore_diacritics))  &&
           (dd_matchl(line,path_str,ignore_case,ignore_diacritics)) )
#endif
            {

               if (relative)
               {
                  strcpy(tmp,relative_prefix);
                  strcat(tmp,line);
                  strcpy(line,tmp);
               }

               if ((pathInNameset(line,bd) == (size_t)-1) &&
                  (check_double_match(line,pm)==0)&&
                  (check_filter(line,filter)==0))
               {
                  addToNamesetArray(textNew(line),pm);
               }
            }
         else
         {

            /* test for a wild match if no perfect match */

#ifdef WCD_UNICODE
            if ( (dd_matchmbs(line_end,dirwild_str,ignore_case,ignore_diacritics)) &&
                 (dd_matchmbs(line,path_str,ignore_case,ignore_diacritics)) && (pm->size == 0))
#else
            if ( (dd_matchl(line_end,dirwild_str,ignore_case,ignore_diacritics)) &&
                 (dd_matchl(line,path_str,ignore_case,ignore_diacritics)) && (pm->size == 0))
#endif
               {

                   if (relative)
                   {
                      strcpy(tmp,relative_prefix);
                      strcat(tmp,line);
                      strcpy(line,tmp);
                   }

                  if((pathInNameset(line,bd) == (size_t)-1) &&
                     (check_double_match(line,wm)==0) &&
                     (check_filter(line,filter)==0))
                  {
                  addToNamesetArray(textNew(line),wm);
                  }
               }
         }
   }   /* while (!feof(infile) ) */
   fclose(infile);
}
/********************************************************************
 *
 *     scanaliasfile(char *org_dir, char *filename,
 *               nameset *pm, nameset *wm, int wildOnly)
 *
 *
 ********************************************************************/

void scanaliasfile(char *org_dir, char *filename,
              nameset pm, nameset wm, int wildOnly)
{
   FILE *infile;
   char *dir;
   char line[DD_MAXPATH];
   char alias[256];

   dir = org_dir;

#ifdef MSDOS
   /* wcd_fixpath() could have added a '/' before the alias
      e.g.  wcd d:alias   =>  /alias */
   if (*dir == '/')
    dir++;
#endif

   /* open treedata-file */
   if  ((infile = wcd_fopen(filename,"r",1)) != NULL)
   {

      while (!feof(infile) )
      {
         int len;

         if(fscanf(infile,"%s",alias)==1)
         {

         /* skip spaces between alias and path */
         while ((line[0]=(char)getc(infile)) == ' '){};

         /* read a line */
         len = wcd_getline(line+1,DD_MAXPATH,infile);
         len++;

         if (len > 0 )
         /* Only a perfect match counts, case sensitive */
            if  ((strcmp(alias,dir)==0) &&
                 (check_double_match(line,pm)==0) /* &&
                 (check_filter(line,filter)==0) */ )
               {
                  if(wildOnly)
                     addToNamesetArray(textNew(line),wm);
                  else
                     addToNamesetArray(textNew(line),pm);
               }
         }
      }   /* while (!feof(infile) ) */
   fclose(infile);
   }
}
/********************************************************************
 *
 *                 Get int
 *
 ********************************************************************/

int wcd_get_int()
{
   int i;
   char string[32];

   fgets(string,(int)sizeof(string),stdin);
   fflush(stdin); /* flush the input stream in case of bad input */

   /* fgets retains the newline character (LF) at the end of string
      and appends a null byte to string to mark the end of the
      string. */
   string[strlen(string)-1] = '\0'; /* remove LF */

   i=atoi(string);

   return(i);
}

/********************************************************************
 *
 *                 exit
 *
 ********************************************************************/

int wcd_exit(nameset pm, nameset wm, nameset ef, nameset bd, nameset nfs, WcdStack ws, nameset excl)
{

   /* free datastructures */
   freeNameset(pm, 1); /* perfect list */
   freeNameset(wm, 1); /* wild list */
   freeNameset(ef, 1); /* extra files */
   freeNameset(bd, 1); /* banned dirs */
   freeNameset(nfs, 1); /* relative files */
   freeWcdStack(ws, 1); /* directory stack */
   freeNameset(excl, 1); /* excluded paths */

   return(0);
}

/********************************************************************
 *
 *                 Print help
 *
 ********************************************************************/
void print_help()
{
        printf(_("\
wcd  %s  (%s) - Wherever Change Directory\n\
Usage: wcd [options] [directory]\n\
\n\
directory:  Name of directory to change to.\n\
            Wildcards *, ? and [SET] are supported.\n\n"),VERSION,VERSION_DATE);

  printf(_("options:\n\
  -a             Add current path to treefile\n\
  -aa            Add current and all parent paths to treefile\n\
  -A PATH        Add tree from PATH\n\
  -b             Ban current path\n\
  -c             direct CD mode\n\
  -d DRIVE       set DRIVE for stack & go files (DOS)\n\
  -e             add current path to Extra treefile\n\
  -ee            add current and all parent paths to Extra treefile\n\
  -E PATH        add tree from PATH to Extra treefile\n\
  -f FILE        use extra treeFile\n\
  +f FILE        add extra treeFile\n\
  -g             Graphics\n\
  -ga            Graphics, alternative navigation\n\
  -gc            Graphics, centered\n\
  -gd            Graphics, dump tree\n\
  -G PATH        set PATH Go-script\n\
  -GN            No Go-script\n\
  -h, --help     show this Help\n\
  -i             Ignore case\n\
  +i             Regard case\n\
  -I             Ignore diacritics\n\
  +I             Regard diacritics (default)\n\
  -j             Just go mode\n\
  -k             Keep paths\n\
  -K             Colors\n\
  -l ALIAS       aLias current directory\n\
  -L             print software License\n\
  -m DIR         Make DIR, add to treefile\n\
  -M DIR         Make DIR, add to extra treefile\n\
  -n PATH        use relative treefile in PATH\n\
  +n PATH        add relative treefile in PATH\n\
  -N             use Numbers\n\
  -o             use stdOut\n\
  -od            dump matches\n\
  -q             Quieter operation\n\
  -r DIR         Remove DIR\n\
  -rmtree DIR    Remove DIR recursive\n\
  -s             Scan disk from $HOME\n\
  -S PATH        Scan disk from PATH\n\
  +S PATH        Scan disk from PATH, create relative treefile\n\
  -t             Don't strip /tmp_mnt from paths\n\
  -T             Draw tree with ASCII characters\n\
  -u USER        use USER's treefile\n\
  +u USER        add USER's treefile\n\
  -v, --verbose  Verbose operation\n\
  -V, --version  print Version info\n\
  -w             Wild matching only\n\
  -x PATH        eXclude PATH during disk scan\n\
  -xf FILE       eXclude paths from FILE\n\
  -z NUMBER      set max stack siZe\n\
  -[NUMBER]      Push dir (NUMBER times)\n\
  +[NUMBER]      Pop dir (NUMBER times)\n\
  =              Show stack\n"));
}

#ifdef ENABLE_NLS
void print_version(char *localedir)
#else
void print_version()
#endif
{
   printf(_("wcd %s (%s) - Wherever Change Directory\n"),VERSION,VERSION_DATE);
   printf(_("Chdir for Dos and Unix.\n\n"));
#ifdef WIN32
# ifdef WCD_WINZSH
   printf(_("This version is for MSYS and WinZsh.\n"));
# elif defined(WCD_WINPWRSH)
   printf(_("This version is for Windows PowerShell.\n"));
# else
   printf(_("This version is for Windows Command Prompt (cmd.exe).\n"));
# endif
# ifdef __WIN64__
   printf(_("Win64 version.\n"));
# else
   printf(_("Win32 version.\n"));
# endif
#elif defined(MSDOS) && defined(GO32)
   printf(_("DOS 32 bit version.\n"));
#endif
#ifdef __MSYS__
   printf(_("This version is for native MSYS.\n"));
#endif
#ifdef WCD_DOSBASH
   printf(_("This version is for DJGPP DOS bash.\n"));
#endif
#ifdef WCD_OS2BASH
   printf(_("This version is for OS/2 bash.\n"));
#endif
   printf(_("Interface: "));
#ifdef WCD_USECONIO
   printf(_("conio\n"));
#else
# ifdef WCD_USECURSES
#  ifdef NCURSES_VERSION
   printf(_("ncurses version %s\n"),NCURSES_VERSION);
#  else
#   ifdef __PDCURSES__
   printf(_("PDCurses build %d\n"),PDC_BUILD);
#   else
   printf(_("curses\n"));
#   endif
#  endif
#  ifdef ASCII_TREE
   printf("ASCII_TREE=1\n");
#  else
   printf("ASCII_TREE=0\n");
#  endif
# else
   printf(_("stdout\n"));
# endif
#endif
#ifdef ENABLE_NLS
   printf(_("Native language support included.\n"));
   printf(_("LOCALEDIR=%s\n"),localedir);
#else
   printf(_("No native language support included.\n"));
#endif
#if defined(MSDOS)
   printf(_("Current locale uses CP%u encoding.\n"),query_con_codepage());
#else
   printf(_("Current locale uses %s encoding.\n"),nl_langinfo(CODESET));
#endif
#ifdef WCD_UNICODE
   printf(_("With Unicode support.\n"));
#ifndef WCD_UTF16
   if (strcmp(nl_langinfo(CODESET), "UTF-8") == 0)
#endif
   {
      printf(_("  Euro symbol: "));
      wcd_printf ("\u20AC\n");
      printf(_("  Chinese characters: "));
      wcd_printf ("\u4e2d\u6587\n");
   }
#else
   printf(_("Without Unicode support.\n"));
#endif
#ifdef WCD_UNINORM
   printf(_("With Unicode normalization.\n"));
#else
   printf(_("Without Unicode normalization.\n"));
#endif
   printf("\n");
   printf(_("Download the latest executables and sources from:\n"));
   printf(_("http://waterlan.home.xs4all.nl/\n"));
}

/* Recurively create directory path, to enable writing
 * the file */
void create_dir_for_file(char *f)
{
   char path[DD_MAXPATH];
   char *ptr ;
#if (defined(UNIX) || defined(DJGPP) || defined(OS2))
   mode_t m;
#endif

   strncpy(path, f, sizeof(path));
   if ( (ptr = strrchr(path,DIR_SEPARATOR)) != NULL)
   {
      *ptr = '\0' ;
       if (
           (path[0] != '\0') && /* not an empty string */
#ifdef MSDOS
           (!dd_match(path,"[a-z]:",1)) && /* not a drive letter */
#endif
           (wcd_isdir(path,1) != 0) /* dir does not exist */
          )
       {
          create_dir_for_file(path);
#if (defined(UNIX) || defined(DJGPP) || defined(OS2))
          m = S_IRWXU | S_IRWXG | S_IRWXO;
          if (wcd_mkdir(path,m,0)==0)
#else
          if (wcd_mkdir(path,0)==0)
#endif
             fprintf(stderr,_("Wcd: creating directory %s\n"), path);
       }
   }
}

/********************************************************************
 *
 *             empty wcd.go file
 *
 ********************************************************************/

#if defined(UNIX) || defined(WIN32) || defined(OS2)
void empty_wcdgo(char *go_file, int use_GoScript)
{
   FILE *outfile;

   if (use_GoScript == 0)
      return;

   /* try to create directory for go-script if it doesn't exist */
   create_dir_for_file(go_file);

   if  ((outfile = wcd_fopen(go_file,"w",0)) == NULL)
      exit(0);

   fprintf(outfile, "%s", "\n");
   fclose(outfile);

}
#endif

#ifdef WCD_DOSBASH
void empty_wcdgo(char *go_file, int changedrive, char *drive, int use_GoScript)
{
   FILE *outfile;

   if (use_GoScript == 0)
      return;

   /* try to create directory for go-script if it doesn't exist */
   create_dir_for_file(go_file);

   if  ((outfile = wcd_fopen(go_file,"w",0)) == NULL)
      exit(0);

   if(changedrive == 1)
      fprintf(outfile,"cd %s\n",drive);
   else
      fprintf(outfile, "%s", "\n");
   fclose(outfile);

}
#endif


/********************************************************************
 *
 * pickDir()
 *
 * gets current dirname.
 * picks dirname from a nameset. The next one after the current,
 * otherwise the first dirname.
 *
 * Returns 0, if no dir found. Otherwise the indexnumber of the
 *         list + 1.
 *
 * ******************************************************************/


size_t pickDir(nameset list, int *use_HOME)
{
   char curDir[DD_MAXPATH];
   size_t i;

   if (list == NULL) /* there is no list */
      return(0);

   sort_list(list);

   getCurPath(curDir,(size_t)DD_MAXPATH,use_HOME); /* get previous dirname from file */

   if (curDir == NULL)  /* no dirname found */
      return(1);            /* return first of list */

   if ((i = inNameset(curDir,list)) == (size_t)-1)  /* not in list */
      return(1);                            /* return first of list */

   i++;  /* next dirname */
   if (i >= getSizeOfNamesetArray(list))
      i = 0;             /* wrap to beginning */

   return(i+1);
}

/********************************************************************
 *
 *
 ********************************************************************/

#ifdef WCD_SHELL
void writeGoFile(char *go_file, int *changedrive, char *drive, char *best_match, int use_GoScript)
{
   FILE *outfile;
#ifdef WCD_UNIXSHELL
   char *ptr ;
#endif
#if defined (WIN32) && !defined(__CYGWIN__)
   unsigned int codepage_ansi, codepage_dos;

   codepage_ansi = GetACP();
   codepage_dos  = GetConsoleOutputCP();
#endif

   if (use_GoScript == 0)
      return;

   /* try to create directory for go-script if it doesn't exist */
   create_dir_for_file(go_file);

   /* open go-script */
   if  ((outfile = wcd_fopen(go_file,"w",0)) == NULL)
      return;

# ifdef WCD_UNIXSHELL
   /* unix shell */
#  ifdef WCD_DOSBASH
   /* In DOS Bash $SHELL points to the windows command shell.
      So we use $BASH instead. */
   if ((ptr = getenv("BASH")) != NULL)
      fprintf(outfile,"#!%s\n",ptr);
   if (*changedrive)
      fprintf(outfile,"cd %s ; ",drive);
#  else
   /* Printing of #!$SHELL is needed for 8 bit characters.
      Some shells otherwise think that the go-script is a binary file
      and will not source it. */
   if ((ptr = getenv("SHELL")) != NULL)
      fprintf(outfile,"#!%s\n",ptr);
#  endif
   fprintf(outfile,"cd %s\n", best_match);
# else
   /* Go-script required, but not unix shell type. */
#  ifdef WCD_WINPWRSH
   /* Windows powershell */
#    ifdef WCD_UTF16
   /* PowerShell can run UTF-8 encoded scripts when the UTF-8 BOM is in. */
   fprintf(outfile, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
#    endif
   if (codepage_ansi != codepage_dos)
      fprintf(outfile,"chcp %d > null\n", codepage_ansi);
   fprintf(outfile,"set-location %s\n", best_match);
   if (codepage_ansi != codepage_dos)
      fprintf(outfile,"chcp %d > null\n", codepage_dos);
#  else
   /* Windows Command Prompt, os/2 */
   fprintf(outfile, "%s", "@echo off\n");
   if (*changedrive)
      fprintf(outfile,"%s\n",drive);
#   ifdef WIN32
   if (codepage_ansi != codepage_dos)
      fprintf(outfile,"chcp %d > nul\n", codepage_ansi);
#   endif
   if (strncmp(best_match,"\"\\\\",3) == 0)
      fprintf(outfile,"pushd %s\n", best_match); /* UNC path */
   else
      fprintf(outfile,"cd %s\n", best_match);
#   ifdef WIN32
   if (codepage_ansi != codepage_dos)
      fprintf(outfile,"chcp %d > nul\n", codepage_dos);
#   endif
#  endif
# endif
   fclose(outfile);
}
#endif


void addListToNameset(nameset set, char *list)
{
   char tmp[DD_MAXPATH];      /* tmp string buffer */

   if (list != NULL)
   {
      list = strtok(list, LIST_SEPARATOR);
      while (list != NULL)
      {
         if (strlen(list) < (DD_MAXPATH-2)) /* prevent buffer overflow */
         {
            strcpy(tmp,list);
            wcd_fixpath(tmp,sizeof(tmp));
            addToNamesetArray(textNew(tmp),set);
         }
         list = strtok(NULL, LIST_SEPARATOR);
      }
   }
}

void addListToNamesetFilter(nameset set, char *list)
{
   char tmp[DD_MAXPATH];      /* tmp string buffer */

   if (list != NULL)
   {
      list = strtok(list, LIST_SEPARATOR);
      while (list != NULL)
      {
         if (strlen(list) < (DD_MAXPATH-2)) /* prevent buffer overflow */
         {
            strcpy(tmp,"*");
            strcat(tmp,list);
            strcat(tmp,"*");
            wcd_fixpath(tmp,sizeof(tmp));
            addToNamesetArray(textNew(tmp),set);
         }
         list = strtok(NULL, LIST_SEPARATOR);
      }
   }
}

/********************************************************************
 *
 *                             MAIN
 *
 ********************************************************************/
#if defined(XCURSES)
   char *XCursesProgramName = "wcd";
#endif

int main(int argc,char** argv)
{
   FILE *outfile;
   char best_match[DD_MAXPATH];
   char verbose = 0;
   int i;
   size_t ii, j;
   int exit_wcd = 0;
   int use_default_treedata = 1;
   int use_numbers = 0; /* use numbers instead of letters in conio or curses interface */
   int use_stdout = WCD_STDOUT_NO; /* use stdout interface instead of curses */
#ifdef UNIX
   int strip_mount_string = 1 ;     /* remove mount string prefix( e.g. /tmp_mnt ) from path before /home */
#endif
   int  stack_hit = 0, stack_index;
   char rootdir[DD_MAXPATH],treefile[DD_MAXPATH],banfile[DD_MAXPATH],aliasfile[DD_MAXPATH];
   char stackfile[DD_MAXPATH];
   char scandir[DD_MAXPATH];
   char rootscandir[DD_MAXPATH];
   char extratreefile[DD_MAXPATH];
   char homedir[DD_MAXPATH];
   char dir[DD_MAXPATH];  /* directory to go to, or dir to scan, make or remove */
   FILE *infile;
   char scan_mk_rm = 0; /* scan disk, or make dir, or remove dir */
   char *ptr, *stackptr;
   int  quieter = 0, cd = 0 ;
   size_t len;
   char tmp[DD_MAXPATH];      /* tmp string buffer */
   char tmp2[DD_MAXPATH];      /* tmp string buffer */
   int  stack_is_read = 0;
   WcdStack DirStack;
   nameset extra_files, banned_dirs, scan_dirs;
   nameset perfect_list, wild_list ;  /* match lists */
   nameset relative_files;
   nameset exclude; /* list of paths to exclude from scanning */
   nameset filter;
   int use_HOME = 0 ;
   int wildOnly = 0 ;
   int justGo = 0;
   int graphics = WCD_GRAPH_NO ;
   int keep_paths =0; /* Keep paths in treedata.wcd when wcd can't change to them */
#ifdef WCD_USECURSES
   dirnode rootNode ;
#endif
   int  changedrive = 0;
   char drive[DD_MAXDRIVE];
   int ignore_diacritics = 0;
#ifdef MSDOS
   int ignore_case = 1;
# if (defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2))
   char go_file[DD_MAXPATH];
   int use_GoScript = 1;
# endif
#else
   int ignore_case = 0;
   char go_file[DD_MAXPATH];
   int use_GoScript = 1;
#endif
#ifdef WCD_UTF16
    wchar_t *cmdstr;
    wchar_t **wargv;
#endif

#if !defined(MSDOS) || defined(WIN32) || defined(OS2) /* Win32, OS/2, Unix, Cygwin */
   /* setlocale is required for correct working of nl_langinfo()
      DOS versions of wcd will use query_con_codepage(). */
   setlocale (LC_ALL, "");
#endif
#ifdef ENABLE_NLS
   char localedir[DD_MAXPATH];
   ptr = getenv("WCDLOCALEDIR");
   if (ptr == NULL)
      strcpy(localedir,LOCALEDIR);
   else
   {
      if (strlen(ptr) < sizeof(localedir))
      {
         strcpy(localedir,ptr);
         wcd_fixpath(localedir,sizeof(localedir));
      }
      else
      {
         fprintf(stderr, "%s", _("Wcd: error: Value of environment variable WCDLOCALEDIR is too long.\n"));
         strcpy(localedir,LOCALEDIR);
      }
   }
   bindtextdomain (PACKAGE, localedir);
   textdomain (PACKAGE);
#endif

#ifdef __PDCURSES__
  /* if ( getenv("PDC_RESTORE_SCREEN") == NULL )
      putenv("PDC_RESTORE_SCREEN=1");  */ /* restore screen after exit */
   if ( getenv("PDC_ORIGINAL_COLORS") == NULL )
      putenv("PDC_ORIGINAL_COLORS=1");  /* use command prompt original colours */

   /* When PDC_RESTORE_SCREEN is set, wcd scrolls away if command window buffer
      is too large. This has been fixed in PDCurses 2.7. Bug 1144353.
      Don't assume user has PDCurses 2.7, so don't set PDC_RESTORE_SCREEN by default.
      Erwin */
#endif

    if ((ptr = getenv("HOME")) == NULL)
    {
       strcpy(rootscandir,ROOTDIR);
    } else {
       if (strlen(ptr) > (DD_MAXPATH -20))
       {
         fprintf(stderr, "%s", _("Wcd: error: Value of environment variable HOME is too long.\n"));
         return(1);
       }
       strncpy(rootscandir,ptr,sizeof(rootscandir));
    }

#ifdef MSDOS

   if ((ptr = getenv("WCDHOME")) == NULL)
       ptr = getenv("HOME");

   if (ptr != NULL)
   {
      if (strlen(ptr) > (DD_MAXPATH -20))
      {
         fprintf(stderr, "%s", _("Wcd: error: Value of environment variable HOME or WCDHOME is too long.\n"));
         return(1);
      }

      use_HOME = 1 ;
      strcpy(rootdir,ptr);
      wcd_fixpath(rootdir,sizeof(rootdir));
      strcpy(treefile,rootdir);
      strcat(treefile,TREEFILE);
      strcpy(extratreefile,rootdir);
      strcat(extratreefile,EXTRA_TREEFILE);
# if (defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2))
      strcpy(go_file,rootdir);
      strcat(go_file,GO_FILE);
# endif
      strcpy(banfile,rootdir);
      strcat(banfile,BANFILE);
      strcpy(aliasfile,rootdir);
      strcat(aliasfile,ALIASFILE);
      if ((ptr = getenv("WCDSTACKFILE")) == NULL)
      {
         strcpy(stackfile,rootdir);
         strcat(stackfile,STACKFILE);
      }
      else
      {
         strncpy(stackfile,ptr,sizeof(stackfile));
      }
   }
   else
   {
# if (defined(WCD_WINZSH) || defined(WCD_WINPWRSH))
     fprintf(stderr, "%s", _("Wcd: error: Environment variable HOME or WCDHOME is not set.\n"));
      return(1);
# endif
# if (defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2))
      strcpy(go_file,STACK_GO_DRIVE);
      strcat(go_file,GO_FILE);
# endif
      strcpy(rootdir,ROOTDIR);
      strcpy(treefile,TREEFILE);
      strcpy(extratreefile,EXTRA_TREEFILE);
      strcpy(banfile,BANFILE);
      strcpy(aliasfile,ALIASFILE);
      if ((ptr = getenv("WCDSTACKFILE")) == NULL)
      {
         strcpy(stackfile,STACK_GO_DRIVE);
         strcat(stackfile,STACKFILE);
      }
      else
      {
         strncpy(stackfile,ptr,sizeof(stackfile));
      }
   }
#else

   if ((ptr = getenv("WCDHOME")) == NULL)
      ptr = getenv("HOME");

   if (ptr == NULL)
   {
      fprintf(stderr, "%s", _("Wcd: error: Environment variable HOME or WCDHOME is not set.\n"));
      return(1);
   }
   else
   {
      if (strlen(ptr) > (DD_MAXPATH -20))
      {
         fprintf(stderr, "%s", _("Wcd: error: Value of environment variable HOME or WCDHOME is too long.\n"));
         return(1);
      }
      strcpy(rootdir,ptr);
   }
   wcd_fixpath(rootdir,sizeof(rootdir));
   strcpy(treefile,rootdir);
   strcat(treefile,TREEFILE);
   strcpy(extratreefile,rootdir);
   strcat(extratreefile,EXTRA_TREEFILE);
   strcpy(go_file,rootdir);
   strcat(go_file,GO_FILE);
   strcpy(banfile,rootdir);
   strcat(banfile,BANFILE);
   strcpy(aliasfile,rootdir);
   strcat(aliasfile,ALIASFILE);
   if ((ptr = getenv("WCDSTACKFILE")) == NULL)
   {
      strcpy(stackfile,rootdir);
      strcat(stackfile,STACKFILE);
   }
   else
   {
      strncpy(stackfile,ptr,sizeof(stackfile));
   }
#endif

   create_dir_for_file(treefile);
   strncpy(scandir,rootdir,(size_t)DD_MAXPATH);

   strcpy(dir,"");

   perfect_list = namesetNew();
   wild_list = namesetNew();
   extra_files = namesetNew();
   banned_dirs = namesetNew();
   scan_dirs = namesetNew();
   relative_files = namesetNew();
   exclude = namesetNew();
   filter = namesetNew();
   DirStack = WcdStackNew(WCD_STACK_SIZE);

   read_treefile(banfile,banned_dirs,1);

   ptr = getenv("WCDSCAN");

   addListToNameset(scan_dirs, ptr);

   ptr = getenv("WCDEXCLUDE");

   addListToNameset(exclude, ptr);

   ptr = getenv("WCDBAN");

   addListToNameset(banned_dirs, ptr);

   ptr = getenv("WCDFILTER");

   addListToNamesetFilter(filter, ptr);

   stackptr = NULL;

   ptr = getenv("WCDUSERSHOME");
   if (ptr == NULL)
      strcpy(homedir,HOMESTRING);
   else
   {
      if (strlen(ptr) > DD_MAXPATH)
      {
         fprintf(stderr, "%s", _("Wcd: error: Value of environment variable WCDUSERSHOME is too long.\n"));
         return(1);
      }
      strcpy(homedir,ptr);
      wcd_fixpath(homedir,sizeof(homedir));
   }

#ifdef WCD_UTF16
    /* Get Unicode parameters. */
    cmdstr = GetCommandLineW();
    wargv = CommandLineToArgvW(cmdstr, &argc);
#endif
   /* ---------------------- parse the commandline ------------*/

   for (i=1; i < argc; i++)
   {
      if (*argv[i]=='-') /* is it a switch? */
         switch (argv[i][1]) {
         case '\0':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }

            stackptr = stack_push(DirStack,1);
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }
            ptr = argv[i];
            ptr++;
            stackptr = stack_push(DirStack,atoi(ptr));
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case 's':
            if (getSizeOfNamesetArray(scan_dirs) == 0)
               scanDisk(rootscandir,treefile,0,(size_t)0,&use_HOME,exclude);
            else
            {
               j = 0;
               while (j<getSizeOfNamesetArray(scan_dirs))
               {
#if (defined(WIN32) || defined(__CYGWIN__))
                  if (wcd_isServerPath(elementAtNamesetArray(j,scan_dirs)))
                  {
                     scanServer(elementAtNamesetArray(j,scan_dirs),treefile,j,&use_HOME,exclude);
                  }
                  else
#endif
                     scanDisk(elementAtNamesetArray(j,scan_dirs),treefile, 0, j, &use_HOME, exclude);
                  ++j;
               }
            }
            scan_mk_rm = 1;
            break ;
         case 'S':
         case 'A':
         case 'E':
         case 'm':
         case 'M':
         case 'r':
            scan_mk_rm = 1;
            break;
         case 'a':
            if (argv[i][2] == 'a')
               addCurPathToFile(treefile,&use_HOME,1);
            else
               addCurPathToFile(treefile,&use_HOME,0);
            scan_mk_rm = 1;
            break;
         case 'e':
            if (argv[i][2] == 'e')
               addCurPathToFile(extratreefile,&use_HOME,1);
            else
               addCurPathToFile(extratreefile,&use_HOME,0);
            scan_mk_rm = 1;
            break;
         case 'B':
         case 'b':
            addCurPathToFile(banfile,&use_HOME,0);
            scan_mk_rm = 1;
            break;
         case 'l':
            break;
         case 'v':
            verbose = 1;
            break;
         case 'q':
            quieter = 1;
            break;
         case 'V':
#ifdef ENABLE_NLS
            print_version(localedir);
#else
            print_version();
#endif

#if defined(UNIX) || defined(WIN32) || defined(OS2)       /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         case 'g':
#ifdef WCD_USECURSES
            graphics |= WCD_GRAPH_NORMAL ;
            if (argv[i][2] == 'd') /* dump tree to stdout */
               graphics |= WCD_GRAPH_DUMP ;
            if (argv[i][2] == 'a') /* alternative navigation */
               graphics |= WCD_GRAPH_ALT ;
            if (argv[i][2] == 'c') /* alternative navigation */
               graphics |= WCD_GRAPH_CENTER ;
#else
            fprintf(stderr, "%s", _("Wcd: Graphics mode only supported in wcd with curses based interface.\n"));
#endif
            break;
         case 'L':
        printf(_("wcd %s (%s) - Wherever Change Directory\n"),VERSION,VERSION_DATE);
        printf(_("\
Chdir for Dos and Unix.\n\
Copyright (C) 1997-2011 Erwin Waterlander\n\
Copyright (C) 1994-2002 Ondrej Popp on C3PO\n\
Copyright (C) 1995-1996 DJ Delorie on _fixpath()\n\
Copyright (C) 1995-1996 Borja Etxebarria & Olivier Sirol on Ninux Czo Directory\n\
Copyright (C) 1994-1996 Jason Mathews on DOSDIR\n\
Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,\n\
Kai Uwe Rommel and Igor Mandrichenko on recmatch()\n\
Source code to scan Windows LAN was originally written and placed\n\
in the public domain by Felix Kasza.\n\
Markus Kuhn's free wcwidth() implementation is used\n\
in Wcd for Windows with Unicode support.\n\
Rugxulo is the original author of query_con_codepage() (public domain).\n\n\
\
This program is free software; you can redistribute it and/or\n\
modify it under the terms of the GNU General Public License\n\
as published by the Free Software Foundation; either version 2\n\
of the License, or (at your option) any later version.\n\n\
\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\n\
\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n"));

#if defined(UNIX) || defined(WIN32) || defined(OS2)       /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         case 'c':
         case 'C':
            cd = 1;
            break;
         case 'x':
         case 'f':
         case 'z':
         case 'n':
            break;
         case 'G':
#if (defined(UNIX) || defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2))
            if (argv[i][2] == 'N') /* No Go-script */
               use_GoScript = 0;
#endif
            break;
         case 'N':
            use_numbers = 1;
            break;
         case 'o':
            use_stdout |= WCD_STDOUT_NORMAL ;
            if (argv[i][2] == 'd') /* dump matches to stdout */
               use_stdout |= WCD_STDOUT_DUMP ;
            break;
         case 'i':
            ignore_case = 1;
            break;
         case 'I':
            ignore_diacritics = 1;
            break;
         case 'k':
            keep_paths = 1;
            break;
         case 'K':
            graphics |= WCD_GRAPH_COLOR ;
            break;
         case 'w':
            wildOnly = 1;
            break;
         case 'j':
            justGo = 1;
            break;
         case 'u':
             break;
#ifdef UNIX
         case 't':
                strip_mount_string = 0;
            break;
#endif
         case 'T':
               graphics |= WCD_GRAPH_ASCII ;
            break;
#ifdef MSDOS
         case 'd':
            break;
#endif
         case '-':  /* long option */
            if (strcmp(argv[i]+2,"version") == 0) {
#ifdef ENABLE_NLS
               print_version(localedir);
#else
               print_version();
#endif
#if defined(UNIX) || defined(WIN32) || defined(OS2)     /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
            } else if (strcmp(argv[i]+2,"verbose") == 0) {
               verbose = 1;
            } else {
               print_help();
#if defined(UNIX) || defined(WIN32) || defined(OS2)     /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
            }
            break;
         default:               /* any switch except the above */
            print_help();

#if defined(UNIX) || defined(WIN32) || defined(OS2)     /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         }
      else
      if (*argv[i]=='+') /* Pop dir */
         switch (argv[i][1]) {
         case '\0':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }
            stackptr = stack_pop(DirStack,1);
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
               if (stack_is_read == 0)
               {
                stack_read(DirStack,stackfile);
                stack_is_read = 1;
               }
            ptr = argv[i];
            ptr++;
            stackptr = stack_pop(DirStack,atoi(ptr));
            if (stackptr != NULL)
            {
               stack_hit = 1;
               stack_write(DirStack,stackfile);
            }
            break;
         case 'f':
            break;
         case 'S':
            scan_mk_rm = 1;
            break;
         case 'n':
            break;
         case 'i':
            ignore_case = 0;
            break;
         case 'I':
            ignore_diacritics = 0;
            break;
         case 'u':
             break;
         default:
            break;
         }
      else
      if (*argv[i]=='=') /* Print stack */
         {
           if (stack_is_read == 0)
           {
            stack_read(DirStack,stackfile);
            stack_is_read = 1;
           }
            stack_index = stack_print(DirStack,use_numbers,use_stdout);
         if (stack_index >= 0)
            {
               stackptr = DirStack->dir[stack_index] ;
               if (stackptr != NULL)
               {
                  stack_hit = 1;
                  stack_write(DirStack,stackfile);
               }
            }
            else
            {
#if defined(UNIX) || defined(WIN32) || defined(OS2)     /* empty wcd.go file */
               empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
               empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
               return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
            }
         }
      else /* Not a switch. Must be a dir or filename. */
      {
         if (strcmp(argv[i-1],"-l") == 0 )
         {
            if (wcd_getcwd(tmp, sizeof(tmp)) != NULL)
            {
               len = strlen(tmp);
               if (len==0)
                  tmp[len] = '\0';

               /* open the treedata file */
               if  ((outfile = wcd_fopen(aliasfile,"a",0)) != NULL)
               {
                  wcd_fixpath(tmp,sizeof(tmp)) ;
                  rmDriveLetter(tmp,&use_HOME);
                  fprintf(outfile,"%s %s\n",argv[i],tmp);
                  printf(_("Wcd: %s added to aliasfile %s\n"),tmp,aliasfile);
                  fclose(outfile);
               }
            }
#if defined(UNIX) || defined(WIN32) || defined(OS2)     /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH     /* empty wcd.go file */
            empty_wcdgo(go_file,0,drive,use_GoScript);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         }
         else
         if ((strcmp(argv[i-1],"-f") == 0 ) || (strcmp(argv[i-1],"+f") == 0 ))
         {
            if (argv[i-1][0] == '-')
               use_default_treedata = 0;
            addToNamesetArray(textNew(argv[i]),extra_files);
         }
         else
         if (strncmp(argv[i-1],"-x",(size_t)2) == 0 )  /* exclude paths from scanning */
         {
            strncpy(tmp,argv[i],sizeof(tmp));
            wcd_fixpath(tmp,sizeof(tmp));

            if (argv[i-1][2] == 'f')
               read_treefile(tmp,exclude,0);   /* read exclude paths from file */
            else
               addToNamesetArray(textNew(tmp),exclude); /* get path from argument */

             /* printNameset("exclude ==>",exclude,stderr,true);  */
         }
            else
            if ((strcmp(argv[i-1],"-n") == 0 ) || (strcmp(argv[i-1],"+n") == 0 ))
            {
               if (argv[i-1][0] == '-')
                  use_default_treedata = 0;

               strncpy(tmp,argv[i],sizeof(tmp));
               wcd_fixpath(tmp,sizeof(tmp));
#ifdef MSDOS
               /* is there a drive to go to ? */
               changeDisk(tmp,&changedrive,drive,&use_HOME);
#endif
               if (wcd_isdir(tmp,1) == 0) /* is it a dir */
               {
                  strcat(tmp,RELTREEFILE);
                  wcd_fixpath(tmp,sizeof(tmp));
                  addToNamesetArray(textNew(tmp),relative_files);
               }
               else  /* it is a file */
               {
                  addToNamesetArray(textNew(tmp),relative_files);
               }
            }
            else
            if ((strcmp(argv[i-1],"-u") == 0 ) || (strcmp(argv[i-1],"+u") == 0 ))
            {
               if (argv[i-1][0] == '-')
                  use_default_treedata = 0;

               if (strcmp(argv[i],"root") == 0)
                  strcpy(tmp,"/");
               else
                  strcpy(tmp,homedir);
               if ((strlen(tmp)+strlen(argv[i])+strlen(TREEFILE)+1) > DD_MAXPATH )
               {
                  fprintf(stderr, "%s", _("Wcd: error: Value of environment variable WCDUSERSHOME is too long.\n"));
                  return(1);
               }
               strcat(tmp,"/");
               strcat(tmp,argv[i]);
               strcat(tmp,TREEFILE);
               if ((infile = wcd_fopen(tmp,"r",1)) != NULL)
               {
                  fclose(infile);
                  addToNamesetArray(textNew(tmp),extra_files);
               }
               else
               {
                  if (strcmp(argv[i],"root") == 0)
                     strcpy(tmp2,"/");
                  else
                     strcpy(tmp2,homedir);
                  if ((strlen(tmp2)+strlen(argv[i])+strlen(TREEFILE)+1+5) > DD_MAXPATH )
                  {
                     fprintf(stderr, "%s", _("Wcd: error: Value of environment variable WCDUSERSHOME is too long.\n"));
                     return(1);
                  }
                  strcat(tmp2,"/");
                  strcat(tmp2,argv[i]);
                  strcat(tmp2,"/.wcd");
                  strcat(tmp2,TREEFILE);
                  if ((infile = wcd_fopen(tmp2,"r",1)) != NULL)
                  {
                     fclose(infile);
                     addToNamesetArray(textNew(tmp2),extra_files);
                  }
                  else
                     fprintf(stderr, _("Wcd: error: Unable to read file %s or %s\n"), tmp, tmp2);
               }
            }
            else
#ifdef MSDOS
            if (strcmp(argv[i-1],"-d") == 0 )
            {
              if (stack_is_read == 0)
              {
                  stackfile[0] = argv[i][0];
#ifdef WCD_DOSBASH
                  go_file[0] = argv[i][0];
#endif
               }
            }
            else
#endif

            if (strcmp(argv[i-1],"-S") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               strncpy(scandir,argv[i],sizeof(scandir));
#endif
#if (defined(WIN32) || defined(__CYGWIN__))
               if (wcd_isServerPath(scandir))
               {
                  scanServer(scandir,treefile,0,&use_HOME,exclude);
               }
               else
#endif
               scanDisk(scandir,treefile,0,(size_t)0,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"+S") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               strncpy(scandir,argv[i],sizeof(scandir));
#endif
               strncpy(treefile,scandir,sizeof(treefile) - strlen(RELTREEFILE));
               strcat(treefile,RELTREEFILE);

               scanDisk(scandir,treefile,1,(size_t)0,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"-z") == 0 )
            {
             if ((stack_is_read == 0) && (atoi(argv[i]) >= 0))
               DirStack->maxsize = atoi(argv[i]);
            }
            else
            if (strcmp(argv[i-1],"-A") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               strncpy(scandir,argv[i],sizeof(scandir));
#endif
#if (defined(WIN32) || defined(__CYGWIN__))
               if (wcd_isServerPath(scandir))
               {
                  scanServer(scandir,treefile,1,&use_HOME,exclude);
               }
               else
#endif
               scanDisk(scandir,treefile,0,(size_t)1,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"-E") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(scandir,wargv[i],sizeof(scandir));
#else
               strncpy(scandir,argv[i],sizeof(scandir));
#endif
#if (defined(WIN32) || defined(__CYGWIN__))
               if (wcd_isServerPath(scandir))
               {
                  scanServer(scandir,extratreefile,1,&use_HOME,exclude);
               }
               else
#endif
               scanDisk(scandir,extratreefile,0,(size_t)1,&use_HOME,exclude);
            }
            else
            if (strcmp(argv[i-1],"-m") == 0 )
            {
#ifdef WCD_UTF16
               wcstoutf8(tmp,wargv[i],sizeof(tmp));
#else
               strncpy(tmp,argv[i],sizeof(tmp));
#endif
               makeDir(tmp,treefile,&use_HOME) ;
            }
            else
            if (strcmp(argv[i-1],"-M") == 0 )
            {
               strncpy(tmp,argv[i],sizeof(tmp));
               makeDir(tmp,extratreefile,&use_HOME) ;
            }
            else
            if (strcmp(argv[i-1],"-r") == 0 )  /* remove one dir, or link to dir */
            {
#ifdef WCD_UTF16
               wcstoutf8(tmp,wargv[i],sizeof(tmp));
#else
               strncpy(tmp,argv[i],sizeof(tmp));
#endif
               deleteDir(tmp,treefile,0,&use_HOME) ;
            }
            else
            if (strcmp(argv[i-1],"-rmtree") == 0 )  /* remove dir recursive */
            {
               strncpy(tmp,argv[i],sizeof(tmp));
               deleteDir(tmp,treefile,1,&use_HOME) ;
            }
#if (defined(UNIX) || defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2))
            else
            if (strcmp(argv[i-1],"-G") == 0)
            {
               strncpy(go_file,argv[i],sizeof(go_file));
               wcd_fixpath(go_file,sizeof(go_file)) ;
# ifdef UNIX
               strcat(go_file,GO_FILE2);
# else
               strcat(go_file,GO_FILE);
# endif
            }
#endif
            else
               {
#ifdef WCD_UTF16
                  wcstoutf8(dir,wargv[i],sizeof(dir));
#else
                  strncpy(dir,argv[i],sizeof(dir));
#endif
                  wcd_fixpath(dir,sizeof(dir));
               }
      }
   } /* for */


   /*--- end parsing commandline -----------*/

   if (verbose > 0)
   {
      for (ii=0; ii<scan_dirs->size; ++ii)
         printf(_("Wcd: WCDSCAN directory {%s}\n"),elementAtNamesetArray(ii, scan_dirs));
      for (ii=0; ii<banned_dirs->size; ++ii)
         printf(_("Wcd: banning {%s}\n"),elementAtNamesetArray(ii, banned_dirs));
      for (ii=0; ii<exclude->size; ++ii)
         printf(_("Wcd: excluding {%s}\n"),elementAtNamesetArray(ii, exclude));
      for (ii=0; ii<filter->size; ++ii)
         printf(_("Wcd: filtering {%s}\n"),elementAtNamesetArray(ii, filter));
   }



   if (stack_is_read == 0)
   {
      stack_read(DirStack,stackfile);
      stack_is_read = 1;
   }

   if ((strcmp(dir,"") == 0 )&& !(graphics & WCD_GRAPH_NORMAL)) /* no directory given, no graphics, so we go HOME */
      addToNamesetArray(textNew(rootscandir),perfect_list);


   /*--- stack hit ? ------------------------*/

   if (stack_hit==1)
   {
      strcpy(best_match,stackptr);
#ifdef UNIX
        /* strip the /tmp_mnt string */
      if (strip_mount_string)
         stripTmpMnt(best_match);
#endif
      if ((!quieter)&&(!justGo))
         wcd_printf("-> %s\n",best_match);
#ifdef MSDOS
       /* is there a drive to go to ? */
       changeDisk(best_match,&changedrive,drive,&use_HOME);
#endif

#if defined(UNIX) || defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2)
         quoteString(best_match);
         if (justGo)
            wcd_printf("%s\n",best_match);
         writeGoFile(go_file,&changedrive,drive,best_match,use_GoScript);
#else
         wcd_chdir(best_match,0); /* change to directory */
#endif

         stack_write(DirStack,stackfile);
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
      }

   /*--- end stack hit ? ------------------------*/

   /*--- Direct CD mode -------------------------*/

   if ((cd==1)&&(strcmp(dir,"")!=0)) /* Try open dir direct. */
   {
      if(wcd_isdir(dir,1) == 0) /* GJM */
      {
         if ( use_stdout & WCD_STDOUT_DUMP ) /* just dump the match and exit */
         {
            wcd_printf("%s\n", dir);
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         }
         strcpy(best_match,dir);
         if ((!quieter)&&(!justGo))
            wcd_printf("-> %s\n",best_match);

#ifdef MSDOS
         /* is there a drive to go to ? */
         changeDisk(best_match,&changedrive,drive,&use_HOME);
#endif
         wcd_chdir(dir,0);
         if(wcd_getcwd(tmp, sizeof(tmp)) != NULL)
         {
            len = strlen(tmp);
            if (len==0)
                  tmp[len] = '\0';

            wcd_fixpath(tmp,sizeof(tmp));

            if ( (ptr=strstr(tmp,"/")) != NULL)
            {
                 strcpy(best_match,tmp);
#ifdef UNIX
                   /* strip the /tmp_mnt string */
                  if (strip_mount_string)
                  stripTmpMnt(best_match);
#endif
               stack_add(DirStack,tmp);      /* stack include /tmp_mnt string */
               stack_write(DirStack,stackfile);
            }

         }

#if defined(UNIX) || defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2)
         quoteString(best_match);
         if (justGo)
            wcd_printf("%s\n",best_match);
         writeGoFile(go_file,&changedrive,drive,best_match,use_GoScript);
#else
         wcd_chdir(best_match,0); /* change to directory */
#endif
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
      } /* ? (wcd_isdir(dir) == 0) */



   } /* ? direct cd mode  ((cd==1)&&(strcmp(dir,"")!=0)) */

   /*--- end Direct CD mode -------------------------*/

#ifdef MSDOS
   /* is there a drive to go to ? */
   changeDisk(dir,&changedrive,drive,&use_HOME);
#endif

   /* does default treedata-file exist? */

   if  (use_default_treedata)
   {
     if ((infile = wcd_fopen(treefile,"r",1)) == NULL)
     {
        /* create treefile */
        if (getSizeOfNamesetArray(scan_dirs) == 0)
           scanDisk(rootscandir,treefile,0,(size_t)0,&use_HOME,exclude);
        else
        {
           j = 0;
           while (j<getSizeOfNamesetArray(scan_dirs))
           {
              if (j == 0)
                 scanDisk(elementAtNamesetArray(j,scan_dirs),treefile, 0, (size_t)0, &use_HOME, exclude);
              else
                 scanDisk(elementAtNamesetArray(j,scan_dirs),treefile, 0, (size_t)1, &use_HOME, exclude);
              ++j;
           }
        }
     }
     else
       fclose(infile);
   }



   if((strcmp(dir,"")==0) && (scan_mk_rm))
   {
   /* No directory given. Don't go HOME. Exit and stay in current dir,
      because we only wanted to scan the disk or make or remove a directory */
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
      empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
      empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
      return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
   }


   /*********** scan treedata files ***************/

   if (strcmp(dir,"")!=0) /* Is there a directory to go to? */
   {
      if (use_default_treedata)
         scanfile(dir, treefile,ignore_case,perfect_list,wild_list,banned_dirs,filter,0,wildOnly,ignore_diacritics); /* scan the treedata file */


      if  ((outfile = wcd_fopen(extratreefile,"r",1)) != NULL)
      {
         fclose(outfile);
         scanfile(dir, extratreefile,ignore_case,perfect_list,wild_list,banned_dirs,filter,0,wildOnly,ignore_diacritics); /* scan the extra treedata file */
      }


      /* search extra files */

      for (ii=0;ii<extra_files->size;ii++)
      {
         scanfile(dir, extra_files->array[ii],ignore_case,perfect_list,wild_list,banned_dirs,filter,0,wildOnly,ignore_diacritics); /* scan the extra treedata file */
      }
      /* search relative files */

      for (ii=0;ii<relative_files->size;ii++)
      {
         scanfile(dir, relative_files->array[ii],ignore_case,perfect_list,wild_list,banned_dirs,filter,1,wildOnly,ignore_diacritics); /* scan the nfs treedata file */
      }

      /* search alias file */

      scanaliasfile(dir, aliasfile, perfect_list, wild_list,wildOnly);

      freeNameset(filter, 1); /* free filter list */
    }
#ifdef WCD_USECURSES
   else
   {
      /* graphics? */
      if (graphics & WCD_GRAPH_NORMAL)
      {
         nameset dirs;

         dirs = namesetNew();
         rootNode = createRootNode();
         if(dirs != NULL)
         {
            if (use_default_treedata)
               read_treefile(treefile,dirs,0);
            read_treefile(extratreefile,dirs,1);
            for (ii=0;ii<extra_files->size;ii++)
            {
               read_treefile(extra_files->array[ii],dirs,0);
            }
            for (ii=0;ii<relative_files->size;ii++)
            {
               read_treefile(relative_files->array[ii],dirs,0);
            }

            buildTreeFromNameset(dirs, rootNode);
            setXYTree(rootNode);
         }

         freeNameset(dirs, 1);
         if (graphics & WCD_GRAPH_DUMP)
         {
            dumpTree(rootNode);
            ptr = NULL;
         }
         else
            ptr = selectANode(rootNode,&use_HOME,ignore_case,graphics,ignore_diacritics);

         if (ptr != NULL)
            addToNamesetArray(textNew(ptr),perfect_list);
         else
         {
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         }

      }
   }
#endif


   /*********** search is done ***************/


   if ((perfect_list->size==0)&&(wild_list->size == 0)&&(cd==0)) /* No match at all & no direct CD mode */
   {
      if(wcd_isdir(dir,1) == 0) /* GJM */
      {
         /* typed directory exists */
         addToNamesetArray(textNew(dir),perfect_list);
      }
   }


   /*******************************/


   if ((perfect_list->size==0)&&(wild_list->size == 0))  /* No match at all */
   {
      if ( !(use_stdout & WCD_STDOUT_DUMP) ) /* don't print message if option -od is used */
         wcd_printf(_("Wcd: No directory found matching %s\nWcd: Perhaps you need to rescan the disk or path is banned.\n"),dir);
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
      empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
      empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
      return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
   }
   else if (perfect_list->size > 1)     /* choose from perfect list */
   {
#ifdef WCD_USECURSES
      if(graphics & WCD_GRAPH_NORMAL)
      {
         rootNode = createRootNode();
         buildTreeFromNameset(perfect_list, rootNode);
         setXYTree(rootNode);
         if (graphics & WCD_GRAPH_DUMP)
         {
            dumpTree(rootNode);
            ptr = NULL;
         }
         else
            ptr = selectANode(rootNode,&use_HOME,ignore_case,graphics,ignore_diacritics);
         if (ptr != NULL)
            strcpy(best_match,ptr);
         else
            exit_wcd = 1;
      }
      else
#endif
      {
         if (justGo)
            i = (int)pickDir(perfect_list,&use_HOME);
         else
            i = display_list(perfect_list,1,use_numbers,use_stdout);

         if ( (i>0) && (i <= (int)perfect_list->size))
         {
            i--;
            strcpy(best_match,perfect_list->array[i]);
         }
         else
            exit_wcd = 1;
      }
      if (exit_wcd)
      {
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
         empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
         empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
      }
   }
   else if (perfect_list->size==1)   /* one perfect match */
   {
      strcpy(best_match,perfect_list->array[0]);
   }
   else if ((perfect_list->size==0)&&(wild_list->size > 1))  /* more than one wild match, zero perfect matches */
   {                                   /* choose from wild list */
#ifdef WCD_USECURSES
      if(graphics & WCD_GRAPH_NORMAL)
      {
         rootNode = createRootNode();
         buildTreeFromNameset(wild_list, rootNode);
         setXYTree(rootNode);
         if (graphics & WCD_GRAPH_DUMP)
         {
            dumpTree(rootNode);
            ptr = NULL;
         }
         else
            ptr = selectANode(rootNode,&use_HOME,ignore_case,graphics,ignore_diacritics);
         if (ptr != NULL)
            strcpy(best_match,ptr);
         else
            exit_wcd = 1;
      }
      else
#endif
      {
         if (justGo)
            i = (int)pickDir(wild_list,&use_HOME);
         else
            i = display_list(wild_list,0,use_numbers,use_stdout);

         if ( (i>0) && (i <= (int)wild_list->size))
         {
            i--;
            strcpy(best_match,wild_list->array[i]);
         }
         else
            exit_wcd = 1;
      }
      if (exit_wcd)
      {
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
         empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
         empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
      }
   }
   else  /* (perfect_list->size==0) && (wild_list->size==1) */   /* one wild match, zero perfect matches */
   {
      strcpy(best_match,wild_list->array[0]);
   }


   /*******************************/

      /* Yes, a match (best_match) */
      if ( use_stdout & WCD_STDOUT_DUMP ) /* just dump the match and exit */
      {
         wcd_printf("%s\n", best_match);
#if defined(UNIX) || defined(WIN32) || defined(OS2)    /* empty wcd.go file */
         empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
         empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
         return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
      }

          strcpy(tmp,best_match); /* remember path (with /tmp_mnt) */
#ifdef UNIX
           /* strip the /tmp_mnt string */
          if (strip_mount_string)
            stripTmpMnt(best_match);
#else
         changeDisk(tmp,&changedrive,drive,&use_HOME);
#endif

         if ( wcd_chdir(best_match,0) == 0)  /* change to dir to get full path */
         {
            if(wcd_getcwd(tmp, sizeof(tmp)) != NULL)
            {
               if ((!quieter)&&(!justGo))
                  wcd_printf("-> %s\n",best_match); /* print match without /tmp_mnt string */

               len = strlen(tmp);
               if (len==0)
                  tmp[len] = '\0';

               wcd_fixpath(tmp,sizeof(tmp));

               if ( (ptr=strstr(tmp,"/")) != NULL)
               {
                       strcpy(best_match,tmp);
#ifdef UNIX
                  /* strip the /tmp_mnt string */
                      if (strip_mount_string)
                     stripTmpMnt(best_match);
#endif
                  stack_add(DirStack,tmp);     /* stack includes /tmp_mnt string */
                  stack_write(DirStack,stackfile);
               }

            }
         }
         else
         {
            wcd_printf(_("Wcd: Cannot change to %s\n"),best_match);
#if defined(UNIX) || defined(WIN32) || defined(OS2)     /* empty wcd.go file */
            empty_wcdgo(go_file,use_GoScript);
#endif
#ifdef WCD_DOSBASH       /* empty wcd.go file */
            empty_wcdgo(go_file,changedrive,drive,use_GoScript);
#endif
            if (keep_paths == 0)
               cleanTreeFile(treefile,tmp);
            return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
         }

#if defined(UNIX) || defined(WIN32) || defined(WCD_DOSBASH) || defined(OS2)
         quoteString(best_match);
         if (justGo)
            wcd_printf("%s\n",best_match);
         writeGoFile(go_file,&changedrive,drive,best_match,use_GoScript);
#else
         wcd_chdir(best_match,0); /* change to directory */
#endif


   return wcd_exit(perfect_list,wild_list,extra_files,banned_dirs,relative_files,DirStack,exclude);
}

