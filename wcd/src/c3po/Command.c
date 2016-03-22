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
#include <ctype.h>

#include "std_macr.h"
#include "structur.h"
#include "Error.h"
#include "nameset.h"

expfun text getBasename(text input_file)
{
   static char base[200];
   int i;
   strcpy(base, input_file);
   i = (int)strlen(base) - 1;
   while(i >= 0)
   {
      if (base[i] eq '/')
         return base + i + 1;
      i = i - 1;
   }
   return base;
}
expfun text getCore(text input_file)
{
   static char core[200];
   int i;
   strcpy(core, getBasename(input_file));
   i = (int)strlen(core) - 1;
   while(i >= 0)
   {
      if (core[i] eq '.')
         core[i] = '\0';
      i = i - 1;
   }
   return core;
}
expfun c3po_bool inArgList(text name,
                      int argc,
                      char** argv)
{
   c3po_bool found = false;
   int index = 1;
   while(index < argc)
   {
      text arg;
      arg = *(argv + index);
      if (strcmp(name, arg) eq 0)
      {
         found = true;
         break;
      }
      index = index + 1;
   }
   return found;
}
expfun text getArg(text name,
                   int argc,
                   char** argv)
{
   int index = 1;
   text arg = NULL;
   text current_arg;
   c3po_bool found = false;
   while(index < argc)
   {
      current_arg = *(argv + index);
      if (strcmp(name, current_arg) eq 0)
      {
         found = true;
         break;
      }

      index = index + 1;
   }

   if (found eq true)
   {
      index = index + 1;
      if (index < argc)
      {
         current_arg = *(argv + index);
         if ( *current_arg ne '-')
            arg = current_arg;
      }
   }

   return arg;
}
expfun nameset getArgs(text name,
                       int argc,
                       char** argv)
{
   int index = 0;
   nameset args = NULL;
   c3po_bool found = false;
   while(index < argc)
   {
      text arg;
      arg = *(argv + index);
      if (strcmp(name, arg) eq 0)
      {
         index = index + 1;
         if (index < argc)
         {
            args = namesetNew();
            found = true;
         }
         else
            return NULL;
      }

      if (found eq true)
      {
         arg = *(argv + index);
         if ( *arg eq '-')
         {
            if (args->size eq 0)
            {
               free((void *) args);
               args = NULL;
            }
            break;
         }
         else
            addToNamesetArray(arg, args);
      }

      index = index + 1;
   }

   return args;
}

expfun c3po_bool isInt(text string)
{
   unsigned int index = 0;
   c3po_bool is_int = true;
   while(index < strlen(string))
   {
      if (isdigit(string[index]) ne 0)
         index = index + 1;
      else if ((index eq 0) and (string[0] eq '-'))
         index = index + 1;
      else
      {
         is_int = false;
         break;
      }
   }
   return is_int;
}

expfun c3po_bool isFloat(text string)
{
   unsigned int index = 0;
   c3po_bool digits = true;
   c3po_bool found_point = false;

   while(index < strlen(string))
   {
      if (isdigit(string[index]) ne 0)
         index = index + 1;
      else if (string[index] eq '.')
      {
         index = index + 1;
         found_point = true;
      }
      else
      {
         digits = false;
         break;
      }
   }

   if (found_point eq true)
   {
      if (digits eq true)
         return true;
      else
         return false;
   }
   else
      return false;
}
expfun void dumpCommandline(text file,
                            int argc,
                            char** argv)
{
   FILE *fp;

   fp = fopen(file, "a");
   if (fp ne NULL)
   {
      fprintf(stderr, "\n- appending commandline to '%s'\n", file);
      int index = 0;
      while(index < argc)
      {
         fprintf(fp, "%s ", argv[index]);
         index = index + 1;
      }
      fprintf(fp, "\n");
      fclose(fp);
   }
   else
   {
      c3po_beep(1);
      fprintf(stderr, "- couldn't append commandline to '%s'\n", file);
   }
}
