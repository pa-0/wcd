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
#include <stdlib.h>
#include <wchar.h>

#include "std_macr.h"
#include "structur.h"
#include "Error.h"
#include "namesetw.h"
#include "intset.h"

expfun textw textwNewSize(int size)
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
expfun int inNamesetw(textw name,
                     namesetw set)
{
   int index;
   if (isEmptyNamesetwArray(set) eq false)
   {
      index = 0;
      while(index < getSizeOfNamesetwArray(set))
      {
         if (eqTextw(name, elementAtNamesetwArray(index, set)) eq true)
            return index;

         index = index + 1;
      }
   }
   return -1;
}
expfun intset matchNamesetw(textw name,
                           namesetw set)
{
   int index;

   static intset i_set = NULL;

   if (i_set eq NULL)
      i_set = intsetNew();
   else
      setSizeOfIntset(i_set, 0);

   if (isEmptyNamesetwArray(set) eq false)
   {
      index = 0;
      while(index < getSizeOfNamesetwArray(set))
      {
         if (eqTextw(name, elementAtNamesetwArray(index, set)) eq true)
            addToIntset(index, i_set);

         index = index + 1;
      }
   }
   return i_set;
}
expfun int matchCountNamesetw(textw name,
                             namesetw set)
{
   int count = 0;
   int index;
   if (isEmptyNamesetwArray(set) eq false)
   {
      index = 0;
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
                           int amount,
                           int bufferNr)
{
   int count = 0;
   static namesetw buffers = NULL;
   textw buffer;

   if (buffers eq NULL)
      buffers = namesetwNew();

   if (bufferNr < 0)
      bufferNr = 0;

   if (amount < 0)
      amount = 0;

   buffer = elementAtNamesetwArray(bufferNr, buffers);

   if (buffer eq NULL)
      buffer = textwNewSize(wcslen(pattern) * amount + 1);
   else
      buffer = (textw) realloc((void *) buffer, sizeof(wchar_t) * (wcslen(pattern) * amount + 1));

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
                   int amount)
{
   return repeatOnBufferw(pattern, amount, 0);
}
expfun textw spacesOnBufferw(int amount,
                           int bufferNr)
{
   return repeatOnBufferw(L" ", amount, bufferNr);
}
expfun textw spacesw(int amount)
{
   return spacesOnBufferw(amount, 0);
}
expfun textw TabOnBufferw(int amount,
                        int bufferNr)
{
   int tabSize = 3;
   return spacesOnBufferw(tabSize*amount, bufferNr);
}
expfun textw Tabw(int amount)
{
   return TabOnBufferw(amount, 0);
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
