/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */

/* Modified by Erwin Waterlander. Mar 30 1999 */
/* Oct 16 2001: On Unix directories can have a backslash (\)
 *              in the name, so they must not be replaced a slash (/)
 *              Erwin Waterlander
 * Oct 26 2002: Keep first two slashes if WIN32 (UNC path)
 *              Erwin Waterlander
 * Sep 8 2005:  Keep first two slashes also for Cygwin (UNC path)
 *              Erwin Waterlander
 * Sep 15 2011: changed parameter 'lim' to size_t type.
 *              Erwin Waterlander
 */

#include <stdio.h>    /* For FILENAME_MAX */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tailor.h"
#include "config.h"
#include "wcd.h"

int wcd_is_slash(int c)
{
#if (defined(UNIX) && !defined(__CYGWIN__))  /* Oct 16 2001 */
  return c == '/';
#else
  return c == '/' || c == '\\';
#endif
}

static int is_term(int c)
{
#if (defined(UNIX) && !defined(__CYGWIN__))  /* Oct 16 2001 */
  return c == '/' || c == '\0';
#else
  return c == '/' || c == '\\' || c == '\0';
#endif
}

/* Takes as input an arbitrary path.  Fixes up the path by:
   1. Removing consecutive slashes
   2. Removing trailing slashes
   4. Removing "." in the path
   7. Converting all slashes to '/'

   8. Keep beginning "/"
   9. Keep beginning "./"
  10. Don't fix path "."
*/

void wcd_fixpath(char *in, size_t lim)
{
  size_t i=0;
  int   drive_number= -1;
  const char *ip = in;
  char  *out;
  char  *op;

  if ((in == NULL) || (lim == 0))
    return ;

  if ((out = (char *) malloc(lim)) == NULL) {
    print_error(_("Memory allocation error in %s: %s\n"),"wcd_fixpath()",strerror(errno));
    return;
  }

  op = out;

#if defined(__MSDOS__) || defined(_WIN32) || defined(__OS2__)
  /* Add drive specification to output string (if present) */
  if (((*ip >= 'a' && *ip <= 'z') ||
       (*ip >= 'A' && *ip <= 'Z'))
      && (*(ip + 1) == ':')) {
    if (*ip >= 'a' && *ip <= 'z') {
      drive_number = *ip - 'a';
      *op++ = *ip++;
    } else {
      drive_number = *ip - 'A';
      if (*ip <= 'Z')
        *op++ = (char)(drive_number + 'a');
      else
        *op++ = *ip;
      ++ip;
    }
    *op++ = *ip++;
    i+=2;
  }
#endif

#if (defined(_WIN32) || defined(__CYGWIN__))
   /* Keep first "//" if present (UNC path) */
  if (wcd_is_slash(*ip) && wcd_is_slash(*(ip + 1))) {
    ip+=2 ;
    *op++ = '/';
    *op++ = '/';
    i+=2 ;
  } else {
#endif
    /* Keep first slash if present */
    if (wcd_is_slash(*ip)) {
      ip++;
      *op++ = '/';
      i++ ;
    } else { /* return if ip == "." */
      if ((*ip == '.') && (*(ip + 1) == '\0')) {
        free(out);
        return ;
      } else { /* Keep first "./" if present */
        if (*ip == '.' && wcd_is_slash(*(ip + 1))) {
          ip+=2 ;
          *op++ = '.';
          *op++ = '/';
          i+=2 ;
        }
      }
    }
#if (defined(_WIN32) || defined(__CYGWIN__))
  }
#endif


  /* Step through the input path */
  while ((*ip) && (i<(lim-1))) {
    /* Skip input slashes */
    if (wcd_is_slash(*ip)) {
      ip++;
      continue;
    }

    /* Skip "." and output nothing */
    if (*ip == '.' && is_term(*(ip + 1))) {
      ip++;
      continue;
    }

    /* Copy path component from in to out */
    if ((op > out) && (*(op-1) != '/')) {
      *op++ = '/';
      i++ ;
    }
    while (!is_term(*ip)) {
      *op++ = *ip++;
       i++;
    }
  }

  /* If root directory, insert trailing slash */
  if (((drive_number >= 0) && (op == out + 2)) || (op == out)) *op++ = '/';

  /* Null terminate the output */
  *op = '\0';

  strcpy(in,out);

  free(out);

}

#ifdef TEST

int main (int argc, char *argv[])
{
  if (argc > 1)
    {
      char fixed[FILENAME_MAX];
      strncpy(fixed,argv[1],FILENAME_MAX -1);
      fixed[FILENAME_MAX -1] = '\0';
      wcd_fixpath (fixed,FILENAME_MAX);
      printf ("You mean %s?\n", fixed);
    }
  return 0;
}

#endif
