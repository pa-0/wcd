/*

 A rudimentary implementation of the nl_langinfo function,
 written by Keith Marshall.
 
 This file was taken from package mingw-catgets.
 http://sourceforge.net/projects/mingw/files/
 http://en.sourceforge.jp/projects/mingw/releases/35416
 http://mingw.cvs.sourceforge.net/mingw/catgets/

   
Copyright (C) 2007, 2008, Keith Marshall

Written by Keith Marshall  <keithmarshall@users.sourceforge.net>

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

#include <stdlib.h>
#include <locale.h>
#include "langinfo.h"  /* EW */
#include "tailor.h"    /* EW */
#include <string.h>

static char *replace( char *prev, char *value )
{
  if( value == NULL )
    return prev;

  if( prev )
    free( prev );
  return strdup( value );
}

char *nl_langinfo( nl_item index )
{
  // static char result[128];
  static char *result = NULL;
  static char *nothing = "US-ASCII";  /* EW */

  switch( index )
  {
    case CODESET:
      {
        char *p;
        result = replace( result, setlocale( LC_CTYPE, NULL ));
        if( (p = strrchr( result, '.' )) == NULL )
	  return nothing;

#ifdef WIN32  /* EW */
	if( (++p - result) > 2 )
	  strcpy( result, "cp" );
	else
#endif
	  *result = '\0';
	strcat( result, p );
        return result;
      }
  }
  return nothing;
}

#ifdef DEBUGGING
#include <stdio.h>
#include <stdlib.h>

int main()
{
  setlocale( LC_ALL, "" );
  printf( "codeset = %s\n", nl_langinfo( CODESET ));
  return EXIT_SUCCESS;
}
#endif
