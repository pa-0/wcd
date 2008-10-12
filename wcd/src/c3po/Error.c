/*
Copyright (C) 1997-2000 Ondrej Popp
This code is generated with Ondrej Popp's C3PO.

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"
#include "../config.h"

expfun void c3po_beep(int number_times)
{
   int counter = 0;
   while (counter < number_times)
   {
      fprintf(stderr, "\007");
      counter = counter + 1;
   }
}
expfun void malloc_error(text function_name)
{
   c3po_beep(1);
   fprintf (stderr, _("Wcd: error in '%s', no space left for allocation\nWcd: Insufficient memory\n"), function_name);
 
	exit(1);
}
