#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"

#include "Error.h"
#include "Textw.h"
#include "namesetw.h"

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
expfun namesetw namesetwNew(void)
{
   namesetw n = (namesetw) malloc (sizeof(namesetw_struct));
   if (n != NULL)
   {
      n->array = NULL;
      n->size = 0;
   }
   else
      malloc_error("namesetwNew()");

   return n;
}
expfun namesetw copyNamesetw(namesetw src,
                           c3po_bool Deep)
{
   if (src == NULL)
      return NULL;
   else
   {
      namesetw n;

      n = (namesetw) malloc (sizeof(namesetw_struct));
      if (n != NULL)
      {
         n->array = NULL;
         n->size = 0;

         if (Deep == true)
            deepExtendNamesetwArray(src, n);
         else
            extendNamesetwArray(src, n);
      }
      else
         malloc_error("copyNamesetw()");

      return n;
   }
}
expfun textw* namesetwGetArray(namesetw n)
{
   if (n != NULL)
      return n->array;
   else
      return NULL;
}
expfun c3po_bool namesetwHasArray(namesetw n)
{
   if (isEmptyNamesetwArray(n) == false)
      return true;
   else
      return false;
}
expfun size_t getSizeOfNamesetwArray(namesetw n)
{
   if (n != NULL)
      return n->size;
   else
      return 0;
}
expfun void setSizeOfNamesetwArray(namesetw n,
                                  size_t size)
{
   if (n != NULL)
   {
      if (size == 0)
      {
         if (isEmptyNamesetwArray(n) eq false)
         {
            free((void *) n->array);
            n->array = NULL;
            n->size = 0;
         }
      }
      else if (size ne n->size)
      {
         if (isEmptyNamesetwArray(n) eq true)
            n->array = (textw*) malloc(sizeof(textw) * size);
         else
            n->array = (textw*) realloc((void *) n->array, sizeof(textw) * size);

         if (n->array ne NULL)
         {
            size_t index = n->size;
            while(index < size)
            {
               n->array[index] = NULL;
               index = index + 1;
            }
            n->size = size;
         }
         else
         {
            n->size = 0;
            malloc_error("setSizeOfNamesetwArray(n, size)");
         }
      }
   }
}
expfun c3po_bool isEmptyNamesetwArray(namesetw n)
{
   if (getSizeOfNamesetwArray(n) > 0)
      return false;
   else
      return true;
}
expfun void addToNamesetwArray(textw t,
                              namesetw set)
{
   if (set != NULL)
   {
      setSizeOfNamesetwArray(set, set->size + 1);
      if (set->array != NULL)
         set->array[set->size - 1] = t;
      else
         malloc_error("addToNamesetwArray()");
   }
}
expfun void putElementAtNamesetwArray(textw t,
                                     size_t position,
                                     namesetw set)
{
   if (set != NULL)
   {
      if (position >= set->size)
         setSizeOfNamesetwArray(set, position + 1);
      if (set->array != NULL)
         set->array[position] = t;
      else
         malloc_error("putElementAtNamesetwArray(t, position, set)");
   }
}
expfun void insertElementAtNamesetwArray(textw t,
                                        size_t position,
                                        namesetw set)
{
   if (set != NULL)
   {
      size_t index = set->size;
      while(index > position)
      {
         putElementAtNamesetwArray(set->array[index-1], index, set);
         index = index - 1;
      }
      putElementAtNamesetwArray(t, position, set);
   }
}
expfun void removeElementAtNamesetwArray(size_t position,
                                        namesetw set,
                                        c3po_bool FreeAtPos)
{
   if (set != NULL)
   {
      if (position < set->size)
      {
         if (FreeAtPos eq true)
         {
            if (set->array[position] ne NULL)
               free((void *) set->array[position]);
         }

         size_t index = position + 1;
         while(index < set->size)
         {
            putElementAtNamesetwArray(set->array[index], index - 1, set);
            index = index + 1;
         }
         setSizeOfNamesetwArray(set, set->size - 1);
      }
   }
}
expfun textw elementAtNamesetwArray(size_t position,
                                  namesetw set)
{
   if (set != NULL)
   {
      if (position < set->size)
         return set->array[position];
   }

   return NULL;
}
expfun void extendNamesetwArray(namesetw src,
                               namesetw dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToNamesetwArray(elementAtNamesetwArray(index, src), dest);
         index = index + 1;
      }
   }
}
expfun void deepExtendNamesetwArray(namesetw src,
                                   namesetw dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToNamesetwArray(textwNew(elementAtNamesetwArray(index, src)), dest);
         index = index + 1;
      }
   }
}
expfun void printNamesetw(textw Offset,
                         namesetw n,
                         FILE* fp,
                         c3po_bool showEmpty)
{
   textw increment = L" ";
   textw new_Offset = (textw) malloc(sizeof(char) * (wcslen(Offset) + wcslen(increment) + 1));
   swprintf(new_Offset, L"%s%s", Offset, increment);

   if (n == NULL)
   {
      if (showEmpty == true)
      {
         fwprintf(fp, L"%s{\n", Offset);
         fwprintf(fp, L"%sNULL\n", new_Offset);
         fwprintf(fp, L"%s}\n", Offset);
      }
   }
   else
   {
      fwprintf(fp, L"%s{\n", Offset);

      if (n->array == NULL)
      {
         if (showEmpty == true)
            fwprintf(fp, L"%stextw array : NULL\n", new_Offset);
      }
      else
      {
         if ((isEmptyNamesetwArray(n) == false) || (showEmpty == true))
         {
            fwprintf(fp, L"%sint size : %zu\n", new_Offset, n->size);

            size_t index = 0;
            while(index < n->size)
            {
               if (n->array[index] ne NULL)
               fwprintf(fp, L"%stextw array[%zu] : %s\n", new_Offset, index, n->array[index]);
               else if (showEmpty == true)
                  fwprintf(fp, L"%stextw array[%zu] : NULL\n", new_Offset, index);
               index = index + 1;
            }
         }
      }

      fwprintf(fp, L"%s}\n", Offset);
   }
   free(new_Offset);
}
expfun void freeNamesetw(namesetw n,
                        c3po_bool Deep)
{
   if (n ne NULL)
   {
      if (Deep eq true)
      {

         size_t index = 0;
         while(index < n->size)
         {
            if (n->array[index] ne NULL)
               free((void *) n->array[index]);

            index = index + 1;
         }
      }

      if (n->array ne NULL)
      {
         free((void *) n->array);
         n->array = NULL;
         n->size = 0;
      }

      free((void *) n);
   }
}
