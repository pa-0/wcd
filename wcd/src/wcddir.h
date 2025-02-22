/*
   filename: wcddir.h

   WCD - Chdir for Dos and Unix.

Author: Erwin Waterlander

======================================================================
= Copyright                                                          =
======================================================================
Copyright (C) 2002-2017 Erwin Waterlander

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
*/

#ifndef _WCD_WCDDIR_H
#define _WCD_WCDDIR_H

#if (defined(_WIN32) || defined(__CYGWIN__))
#include <stdio.h>
#include "std_macr.h"
#include "structur.h"
#include "nameset.h"
int wcd_isServerPath (char* path);
void wcd_getshares(char* path, nameset n);
#endif

#if (defined(_WIN32) && !defined(__CYGWIN__))

int wcd_islink(const char *dir, int quiet);
int wcd_mkdir(const char *path, int quiet);

#else  /* not WIN32 API */


#  if defined(UNIX) || defined(__DJGPP__) || defined(__EMX__)
#  include <sys/types.h>
   int wcd_mkdir(const char *path, mode_t m, int quiet);
#  else
   int wcd_mkdir(const char *path, int quiet);
#  endif

#endif

char *wcd_getcwd(char *buf, size_t size);
int wcd_chdir(const char *path, int quiet);
int wcd_rmdir(const char *path, int quiet);
int wcd_unlink(const char *path);
int wcd_isdir(char *dir, int quiet);

#endif
