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
#include "Error.h"
#include "nameset.h"
#include "intset.h"

expfun text textNewSize(size_t size)
{
   text t = NULL;

   if (size > 0)
   {
      t = (text) malloc (sizeof(char) * size);
      if (t eq NULL)
         malloc_error("textNewSize(size)");
   }

   return t;
}
expfun text textNew(text string)
{
   text t = NULL;

   if (string ne NULL)
   {
      t = textNewSize(strlen(string) + 1);
      if (t ne NULL)
         strcpy(t, string);
      else
         malloc_error("textNew(text)");
   }

   return t;
}
expfun c3po_bool eqText(text a,
                   text b)
{
   if ((a eq NULL) or (b eq NULL))
      return false;
   else if (strcmp(a, b) eq 0)
      return true;
   else
      return false;
}
expfun size_t inNameset(text name,
                     nameset set)
{
   if (isEmptyNamesetArray(set) eq false)
   {
      size_t index = 0;
      while(index < getSizeOfNamesetArray(set))
      {
         if (eqText(name, elementAtNamesetArray(index, set)) eq true)
            return index;

         index = index + 1;
      }
   }
   return (size_t)-1;
}
expfun intset matchNameset(text name,
                           nameset set)
{
   static intset i_set = NULL;

   if (i_set eq NULL)
      i_set = intsetNew();
   else
      setSizeOfIntset(i_set, (size_t)0);

   if (isEmptyNamesetArray(set) eq false)
   {
      size_t index = 0;
      while(index < getSizeOfNamesetArray(set))
      {
         if (eqText(name, elementAtNamesetArray(index, set)) eq true)
            addToIntset(index, i_set);

         index = index + 1;
      }
   }
   return i_set;
}
expfun size_t matchCountNameset(text name,
                             nameset set)
{
   size_t count = 0;
   if (isEmptyNamesetArray(set) eq false)
   {
      size_t index = 0;
      while(index < getSizeOfNamesetArray(set))
      {
         if (eqText(name, elementAtNamesetArray(index, set)) eq true)
            count = count + 1;

         index = index + 1;
      }
   }
   return count;
}
expfun text concat(text a,
                   text b)
{
   text res = (text) malloc(sizeof(char) * (strlen(a) + strlen(b) + 1));
   if (res ne NULL)
   {
      strcpy(res, a);
      strcpy(res + strlen(res), b);
   }
   else
      malloc_error("concat()");


   return res;
}
expfun text concat3(text a,
                    text b,
                    text c)
{
   text res = (text) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + 1));
   if (res ne NULL)
   {
      strcpy(res, a);
      strcpy(res + strlen(res), b);
      strcpy(res + strlen(res), c);
   }
   else
      malloc_error("concat3()");


   return res;
}
expfun text concat4(text a,
                    text b,
                    text c,
                    text d)
{
   text res = (text) malloc(sizeof(char) * (strlen(a) + strlen(b) + strlen(c) + strlen(d) + 1));
   if (res ne NULL)
   {
      strcpy(res, a);
      strcpy(res + strlen(res), b);
      strcpy(res + strlen(res), c);
      strcpy(res + strlen(res), d);
   }
   else
      malloc_error("concat4()");


   return res;
}
expfun text repeatOnBuffer(text pattern,
                           size_t amount,
                           size_t bufferNr)
{
   size_t count = 0;
   static nameset buffers = NULL;
   text buffer;

   if (buffers eq NULL)
      buffers = namesetNew();

   buffer = elementAtNamesetArray(bufferNr, buffers);

   if (buffer eq NULL)
      buffer = textNewSize(strlen(pattern) * amount + 1);
   else {
      text new_buffer = (text) realloc((void *) buffer, sizeof(char) * (strlen(pattern) * amount + 1));
      if (new_buffer != NULL)
         buffer = new_buffer;
      else {
         malloc_error("repeatOnBuffer()");
         buffer = NULL;
      }
   }
   if (buffer == NULL)
      return NULL;

   putElementAtNamesetArray(buffer, bufferNr, buffers);

   buffer[0] = '\0';
   while(count < amount)
   {
      strcpy(buffer + strlen(buffer), pattern);
      count = count + 1;
   }

   return buffer;
}
expfun text repeat(text pattern,
                   size_t amount)
{
   return repeatOnBuffer(pattern, amount, (size_t)0);
}
expfun text spacesOnBuffer(size_t amount,
                           size_t bufferNr)
{
   return repeatOnBuffer(" ", amount, bufferNr);
}
expfun text spaces(size_t amount)
{
   return spacesOnBuffer(amount, (size_t)0);
}
expfun text TabOnBuffer(size_t amount,
                        size_t bufferNr)
{
   size_t tabSize = 3;
   return spacesOnBuffer(tabSize*amount, bufferNr);
}
expfun text Tab(size_t amount)
{
   return TabOnBuffer(amount, (size_t)0);
}
expfun text intToString(int i)
{
   char buffer[100];
   sprintf(buffer, "%d", i);
   return textNew(buffer);
}
expfun text doubleToString(double d)
{
   char buffer[100];
   sprintf(buffer, "%g", d);
   return textNew(buffer);
}
