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


#if defined(_WIN32) && !defined(__CYGWIN__)
#  ifdef WCD_UNICODE
#    define UNICODE
#    define _UNICODE
#    define WCD_FINDFIRSTFILE FindFirstFileW
#    define WCD_FINDNEXTFILE  FindNextFileW
#    include "display.h"  /* for wcstoutf8() */
#  else
#    define WCD_FINDFIRSTFILE FindFirstFileA
#    define WCD_FINDNEXTFILE  FindNextFileA
#  endif
#  include <windows.h>
#  include <direct.h>
#  ifdef __WATCOMC__
#    include <dos.h> /* Watcom C does not have _getdrives(). We use the dos functions. */
#  endif
#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))
#  ifdef __DJGPP__
#    include <dir.h>
#    define WCD_FB_NAME    fb.ff_name
#    define WCD_FB_MODE    fb.ff_attrib
#    define WCD_FINDNEXT   findnext
#    define WCD_ISDIREC(m) ((m) & FA_DIREC)
#  else  /* Watcom C */
#    include <dos.h>
#    define WCD_FB_NAME    fb.name
#    define WCD_FB_MODE    fb.attrib
#    define WCD_FINDNEXT   _dos_findnext
#    define WCD_ISDIREC(m) ((m) & _A_SUBDIR)
#  endif
#  if (defined(__OS2__) && !defined(__EMX__))
#    include <os2.h>
#  endif
#else
#  define _GNU_SOURCE /* Required for DT_DIR and DT_LNK */
#  include <dirent.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <errno.h>
#endif

#include <string.h>
#include "wcddir.h"
#include "wcd.h"
#include "wfixpath.h"
#include "nameset.h"
#include "config.h"
#include "tailor.h"
#include "finddirs.h"



const wcd_char *default_mask = ALL_FILES_MASK;

/* Various implementations of setdisk/getdisk */

#if defined(__MSDOS__) || (defined(__WATCOMC__) && defined(__NT__))
#  ifndef __TURBOC__
/*
 * getdisk
 *
 * Returns: -1 if error, otherwise: 0=drive A, 1=drive B, and so on.
 */
int getdisk()
{
   unsigned d;
   _dos_getdrive(&d);
   return ((int)d - 1);
}

/*
 * setdisk: 0=drive A, 1=drive B, and so on.
 *
 * Returns: total number of drive available
 *
 */
int setdisk( int drive )
{
   unsigned numdrives;
   _dos_setdrive((unsigned) (drive + 1), &numdrives);
   return numdrives;
}

#  endif /* ?!__TURBOC__ */

#elif defined(__OS2__)

/* OS/2 implementation of getdisk and setdisk for EMX */

/*
 * getdisk
 *
 * Returns: -1 if error, otherwise: 0=drive A, 1=drive B, and so on.
 */
int getdisk()
{
   ULONG ulDrive;
   ULONG ulLogical;
   /* APIRET rc; */
   int d;

   /* rc= */ DosQueryCurrentDisk(&ulDrive, &ulLogical); /* Get current drive */
   d = (int)ulDrive;
   return(d-1);
}

/*
 * setdisk: 0=drive A, 1=drive B, and so on.
 *
 * Returns: total number of drive available
 *
 */
int setdisk( int drive )
{
   int d;
   ULONG ulDrive;
   ulDrive = (ULONG)(drive+1);
   if(DosSetDefaultDisk(ulDrive)) /* Set default drive, 1=A, 2=B, 3=C, 4=D */
   {
      /* printf("Wcd: error: Can not change default drive to %d\n",drive); */
      return(-1);
   }

   d = getdisk();
   return(d);
}
#elif defined(_WIN32)

int getdisk(void)
{
   int d;
   d = _getdrive();
   return(d-1);
}
int setdisk (int drive)
{
   unsigned long numdrives;
   _chdrive(drive+1);
   numdrives = _getdrives();
   return ((int)numdrives);
}
#else
/* stub functions for get/set disk
 * fake MS-DOS functions that do not apply to unix or vms:
 */

int getdisk()
{
   return 0;
}

int setdisk( int drive )
{
   return 0;
}
#endif

/****************************************************************/

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
   struct ffblk fb;       /* file block structure */
   int rc;                /* error code */
#  else
   struct _find_t fb;     /* file block structure */
   unsigned rc;           /* error code */
#  endif
#else /* Unix, Cygwin, MSYS, EMX */
   DIR *dirp;
   struct dirent *dp;
#endif

   if (dir) {
      if (wcd_chdir(dir,0)) return; /* Go to the dir, else return */
   } else
      return ;  /* dir == NULL */


#if defined(_WIN32) && !defined(__CYGWIN__)

   hFind = WCD_FINDFIRSTFILE(ALL_FILES_MASK, &FindFileData);
   while (hFind != INVALID_HANDLE_VALUE) {
      char directory[WCD_MAXPATH];
#  ifdef WCD_UNICODE
      wcstoutf8(directory, FindFileData.cFileName, sizeof(directory));
#  else
      wcd_strncpy(directory,FindFileData.cFileName,sizeof(directory));
#  endif
      if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         if (!SpecialDir(directory)) {
            if(wcd_islink(directory,0)) {
               if (wcd_unlink(directory) != 0)
                  print_error(_("Unable to remove file %s: %s\n"), directory, strerror(errno));
            } else {
               rmTree(directory);
               wcd_rmdir(directory,0);
            }
         }
      } else {  /* not a directory */
         if (wcd_unlink(directory) != 0)
            print_error(_("Unable to remove file %s: %s\n"), directory, strerror(errno));
      }
      if (!WCD_FINDNEXTFILE(hFind, &FindFileData)) {
          FindClose(hFind);
          hFind = INVALID_HANDLE_VALUE;
      }
   }

#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))

#  ifdef __DJGPP__
   rc = findfirst(default_mask, &fb, FA_DIREC|FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH|FA_LABEL);
#  else
   rc = _dos_findfirst(default_mask, _A_SUBDIR|_A_RDONLY|_A_HIDDEN|_A_SYSTEM|_A_ARCH, &fb);
#  endif

   while (rc==0) {  /* go through all the files in the current dir */
      if (WCD_ISDIREC(WCD_FB_MODE)) {
         if (!SpecialDir(WCD_FB_NAME)) {
            rmTree(WCD_FB_NAME);
            wcd_rmdir(WCD_FB_NAME,0);
         }
      } else { /* not a directory */
         if (wcd_unlink(WCD_FB_NAME) != 0) {
            print_error(_("Unable to remove file %s: %s\n"), WCD_FB_NAME, strerror(errno));
         }
      }
      rc = WCD_FINDNEXT(&fb);
   } /* while !rc */
#  if defined(__OS2__)
   _dos_findclose( &fb );
#  endif


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
         if (wcd_unlink(dp->d_name) != 0) {
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
            if (wcd_unlink(dp->d_name) != 0) {
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
   char curdir[WCD_MAXPATH];
   char *tmp_ptr ;
   size_t len ;
#if defined(_WIN32) && !defined(__CYGWIN__)
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;
#elif defined(__MSDOS__) || (defined(__OS2__) && !defined(__EMX__))
#  ifdef __DJGPP__
   struct ffblk fb;       /* file block structure */
   int rc;                /* error code */
#  else
   struct _find_t fb;     /* file block structure */
   unsigned rc;           /* handle and error code */
#  endif
#else /* Unix, Cygwin, MSYS, EMX */
   DIR *dirp;
   struct dirent *dp;
#endif

   if (dir) {
      if (wcd_chdir(dir,quiet)) return; /* Go to the dir, else return */
   }
   else
     return ;  /* dir == NULL */


   if (wcd_getcwd(curdir, sizeof(curdir)) == NULL) {
      print_error(_("finddirs(): can't determine path in directory %s\n"),dir);
      print_error(_("path probably too long.\n"));
      wcd_chdir(DIR_PARENT,1); /* go to parent directory */
      return;
   };

#ifdef _WCD_DOSFS
   wcd_fixpath(curdir,sizeof(curdir));
   rmDriveLetter(curdir,use_HOME);
#endif

   if (pathInNameset(curdir,exclude) != (size_t)-1) {
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
   while (hFind != INVALID_HANDLE_VALUE) {
      if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         char directory[WCD_MAXPATH];
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

/* Only DJGPP and Watcom C are supported.  */

#  ifdef __DJGPP__
   rc = findfirst(default_mask, &fb, FA_DIREC);
#  else
   rc = _dos_findfirst(default_mask, _A_SUBDIR, &fb);
#  endif

   while (rc==0) {  /* go through all the files in the current dir */
      if (WCD_ISDIREC(WCD_FB_MODE)) {
         if (!SpecialDir(WCD_FB_NAME))
            finddirs(WCD_FB_NAME,offset, outfile, use_HOME, exclude, 1);
      }
      rc = WCD_FINDNEXT(&fb);
   } /* while !rc */
#  if defined(__OS2__)
   _dos_findclose( &fb );
#  endif


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
