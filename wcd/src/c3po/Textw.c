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
#include <stdlib.h>
#include <wchar.h>

#include "std_macr.h"
#include "structur.h"
#include "Error.h"
#include "namesetw.h"
#include "intset.h"

expfun textw textwNewSize(size_t size)
{
   textw t = NULL;

   if (size > 0)
   {
      t = (textw) malloc (sizeof(wchar_t) * size);
      if (t eq NULL)
         malloc_error("textwNewSize(size)");
   }

   return t;
}
expfun textw textwNew(textw string)
{
   textw t = NULL;

   if (string ne NULL)
   {
      t = textwNewSize(wcslen(string) + 1);
      if (t ne NULL)
         wcscpy(t, string);
      else
         malloc_error("textwNew(textw)");
   }

   return t;
}
expfun c3po_bool eqTextw(textw a,
                   textw b)
{
   if ((a eq NULL) or (b eq NULL))
      return false;
   else if (wcscmp(a, b) eq 0)
      return true;
   else
      return false;
}
expfun size_t inNamesetw(textw name,
                     namesetw set)
{
   if (isEmptyNamesetwArray(set) eq false)
   {
      size_t index = 0;
      while(index < getSizeOfNamesetwArray(set))
      {
         if (eqTextw(name, elementAtNamesetwArray(index, set)) eq true)
            return index;

         index = index + 1;
      }
   }
   return (size_t)-1;
}
expfun intset matchNamesetw(textw name,
                           namesetw set)
{
   static intset i_set = NULL;

   if (i_set eq NULL)
      i_set = intsetNew();
   else
      setSizeOfIntset(i_set, (size_t)0);

   if (isEmptyNamesetwArray(set) eq false)
   {
      size_t index = 0;
      while(index < getSizeOfNamesetwArray(set))
      {
         if (eqTextw(name, elementAtNamesetwArray(index, set)) eq true)
            addToIntset(index, i_set);

         index = index + 1;
      }
   }
   return i_set;
}
expfun size_t matchCountNamesetw(textw name,
                             namesetw set)
{
   size_t count = 0;
   if (isEmptyNamesetwArray(set) eq false)
   {
      size_t index = 0;
      while(index < getSizeOfNamesetwArray(set))
      {
         if (eqTextw(name, elementAtNamesetwArray(index, set)) eq true)
            count = count + 1;

         index = index + 1;
      }
   }
   return count;
}
expfun textw concatw(textw a,
                   textw b)
{
   textw res = (textw) malloc(sizeof(wchar_t) * (wcslen(a) + wcslen(b) + 1));
   if (res ne NULL)
   {
      wcscpy(res, a);
      wcscpy(res + wcslen(res), b);
   }
   else
      malloc_error("concatw()");


   return res;
}
expfun textw concatw3(textw a,
                    textw b,
                    textw c)
{
   textw res = (textw) malloc(sizeof(wchar_t) * (wcslen(a) + wcslen(b) + wcslen(c) + 1));
   if (res ne NULL)
   {
      wcscpy(res, a);
      wcscpy(res + wcslen(res), b);
      wcscpy(res + wcslen(res), c);
   }
   else
      malloc_error("concatw3()");


   return res;
}
expfun textw concatw4(textw a,
                    textw b,
                    textw c,
                    textw d)
{
   textw res = (textw) malloc(sizeof(wchar_t) * (wcslen(a) + wcslen(b) + wcslen(c) + wcslen(d) + 1));
   if (res ne NULL)
   {
      wcscpy(res, a);
      wcscpy(res + wcslen(res), b);
      wcscpy(res + wcslen(res), c);
      wcscpy(res + wcslen(res), d);
   }
   else
      malloc_error("concatw4()");


   return res;
}
expfun textw repeatOnBufferw(textw pattern,
                           size_t amount,
                           size_t bufferNr)
{
   size_t count = 0;
   static namesetw buffers = NULL;
   textw buffer;

   if (buffers eq NULL)
      buffers = namesetwNew();

   buffer = elementAtNamesetwArray(bufferNr, buffers);

   if (buffer eq NULL)
      buffer = textwNewSize(wcslen(pattern) * amount + 1);
   else {
      textw new_buffer = (textw) realloc((void *) buffer, sizeof(wchar_t) * (wcslen(pattern) * amount + 1));
      if (new_buffer != NULL)
         buffer = new_buffer;
      else {
         malloc_error("repeatOnBufferw()");
         buffer = NULL;
      }
   }
   if (buffer == NULL)
      return NULL;

   putElementAtNamesetwArray(buffer, bufferNr, buffers);

   buffer[0] = '\0';
   while(count < amount)
   {
      wcscpy(buffer + wcslen(buffer), pattern);
      count = count + 1;
   }

   return buffer;
}
expfun textw repeatw(textw pattern,
                   size_t amount)
{
   return repeatOnBufferw(pattern, amount, (size_t)0);
}
expfun textw spacesOnBufferw(size_t amount,
                           size_t bufferNr)
{
   return repeatOnBufferw(L" ", amount, bufferNr);
}
expfun textw spacesw(size_t amount)
{
   return spacesOnBufferw(amount, (size_t)0);
}
expfun textw TabOnBufferw(size_t amount,
                        size_t bufferNr)
{
   size_t tabSize = 3;
   return spacesOnBufferw(tabSize*amount, bufferNr);
}
expfun textw Tabw(size_t amount)
{
   return TabOnBufferw(amount, (size_t)0);
}
expfun textw intToStringw(int i)
{
   wchar_t buffer[100];
   swprintf(buffer, L"%d", i);
   return textwNew(buffer);
}
expfun textw doubleToStringw(double d)
{
   wchar_t buffer[100];
   swprintf(buffer, L"%g", d);
   return textwNew(buffer);
}
