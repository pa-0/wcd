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
 */

#include <stdio.h>		/* For FILENAME_MAX */
#include <stdlib.h>
#include <string.h>
#include "tailor.h"
#include "config.h"

#ifdef OS2
#undef UNIX
#endif

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

void wcd_fixpath(char *in, int lim)
{
  int	i=0,drive_number= -1;
  const char	*ip = in;
  char  *out;
  char	*op;

  if (in == NULL)
   	return ;

  if ((out = (char *) malloc(lim)) == NULL)
  {
  	fprintf(stderr,_("Wcd: malloc error in wcd_fixpath()\n"));
	return;
  }

  op = out;

#ifdef MSDOS
  /* Add drive specification to output string (if present) */
  if (((*ip >= 'a' && *ip <= 'z') ||
       (*ip >= 'A' && *ip <= 'Z'))
      && (*(ip + 1) == ':'))
  {
    if (*ip >= 'a' && *ip <= 'z')
    {
      drive_number = *ip - 'a';
      *op++ = *ip++;
    }
    else
    {
      drive_number = *ip - 'A';
      if (*ip <= 'Z')
	*op++ = drive_number + 'a';
      else
	*op++ = *ip;
      ++ip;
    }
    *op++ = *ip++;
	i+=2;
  }
#endif

#if (defined(WIN32) || defined(__CYGWIN__))
   /* Keep first "//" if present (UNC path) */
	 if (wcd_is_slash(*ip) && wcd_is_slash(*(ip + 1)))
	 {
	   ip+=2 ;
	   *op++ = '/';
	   *op++ = '/';
	   i+=2 ;
	 }
	 else
#endif
    /* Keep first slash if present */
    if (wcd_is_slash(*ip))
    {
      ip++;
	  *op++ = '/';
	  i++ ;
    }
	else  /* return if ip == "." */
	if ((*ip == '.') && (*(ip + 1) == '\0'))
	  {
		free(out);
	    return ;
	  }
	else  /* Keep first "./" if present */
	if (*ip == '.' && wcd_is_slash(*(ip + 1)))
	{
	  ip+=2 ;
	  *op++ = '.';
	  *op++ = '/';
	  i+=2 ;
	}


  /* Step through the input path */
  while ((*ip) && (i<(lim-1)))
  {
    /* Skip input slashes */
    if (wcd_is_slash(*ip))
    {
      ip++;
      continue;
    }

    /* Skip "." and output nothing */
    if (*ip == '.' && is_term(*(ip + 1)))
    {
      ip++;
      continue;
    }

    /* Copy path component from in to out */
	if ((op > out) && (*(op-1) != '/'))
	{
		*op++ = '/';
		i++ ;
	}
    while (!is_term(*ip))
	{
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
  char fixed[FILENAME_MAX];
  if (argc > 1)
    {
	  strcpy(fixed,argv[1]);
	  wcd_fixpath (fixed,FILENAME_MAX);
      printf ("You mean %s?\n", fixed);
    }
  return 0;
}

#endif
