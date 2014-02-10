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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"
#include "../config.h"
#include "../wcd.h"

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
   print_error(_("in '%s', insufficient memory for allocation\n"), function_name);
   exit(1);
}
