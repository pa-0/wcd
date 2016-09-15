#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"

#include "Error.h"
#include "Text.h"
#include "nameset.h"

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
expfun nameset namesetNew(void)
{
   nameset n = (nameset) malloc (sizeof(nameset_struct));
   if (n != NULL)
   {
      n->array = NULL;
      n->size = 0;
   }
   else
      malloc_error("namesetNew()");

   return n;
}
expfun nameset copyNameset(nameset src,
                           c3po_bool Deep)
{
   if (src == NULL)
      return NULL;
   else
   {
      nameset n;

      n = (nameset) malloc (sizeof(nameset_struct));
      if (n != NULL)
      {
         n->array = NULL;
         n->size = 0;

         if (Deep == true)
            deepExtendNamesetArray(src, n);
         else
            extendNamesetArray(src, n);
      }
      else
         malloc_error("copyNameset()");

      return n;
   }
}
expfun text* namesetGetArray(nameset n)
{
   if (n != NULL)
      return n->array;
   else
      return NULL;
}
expfun c3po_bool namesetHasArray(nameset n)
{
   if (isEmptyNamesetArray(n) == false)
      return true;
   else
      return false;
}
expfun size_t getSizeOfNamesetArray(nameset n)
{
   if (n != NULL)
      return n->size;
   else
      return 0;
}
expfun void setSizeOfNamesetArray(nameset n,
                                  size_t size)
{
   if (n != NULL)
   {
      if (size == 0)
      {
         if (isEmptyNamesetArray(n) eq false)
         {
            free((void *) n->array);
            n->array = NULL;
            n->size = 0;
         }
      }
      else if (size ne n->size)
      {
         if (isEmptyNamesetArray(n) eq true)
            n->array = (text*) malloc(sizeof(text) * size);
         else
            n->array = (text*) realloc((void *) n->array, sizeof(text) * size);

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
            malloc_error("setSizeOfNamesetArray(n, size)");
         }
      }
   }
}
expfun c3po_bool isEmptyNamesetArray(nameset n)
{
   if (getSizeOfNamesetArray(n) > 0)
      return false;
   else
      return true;
}
expfun void addToNamesetArray(text t,
                              nameset set)
{
   if (set != NULL)
   {
      setSizeOfNamesetArray(set, set->size + 1);
      if (set->array != NULL)
         set->array[set->size - 1] = t;
      else
         malloc_error("addToNamesetArray()");
   }
}
expfun void putElementAtNamesetArray(text t,
                                     size_t position,
                                     nameset set)
{
   if (set != NULL)
   {
      if (position >= set->size)
         setSizeOfNamesetArray(set, position + 1);
      if (set->array != NULL)
         set->array[position] = t;
      else
         malloc_error("putElementAtNamesetArray(t, position, set)");
   }
}
expfun void insertElementAtNamesetArray(text t,
                                        size_t position,
                                        nameset set)
{
   if (set != NULL)
   {
      size_t index = set->size;
      while(index > position)
      {
         putElementAtNamesetArray(set->array[index-1], index, set);
         index = index - 1;
      }
      putElementAtNamesetArray(t, position, set);
   }
}
expfun void removeElementAtNamesetArray(size_t position,
                                        nameset set,
                                        c3po_bool FreeAtPos)
{
   if (set != NULL)
   {
      if (position < set->size)
      {
         size_t index;
         if (FreeAtPos eq true)
         {
            if (set->array[position] ne NULL)
               free((void *) set->array[position]);
         }

         index = position + 1;
         while(index < set->size)
         {
            putElementAtNamesetArray(set->array[index], index - 1, set);
            index = index + 1;
         }
         setSizeOfNamesetArray(set, set->size - 1);
      }
   }
}
expfun text elementAtNamesetArray(size_t position,
                                  nameset set)
{
   if (set != NULL)
   {
      if (position < set->size)
         return set->array[position];
   }

   return NULL;
}
expfun void extendNamesetArray(nameset src,
                               nameset dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToNamesetArray(elementAtNamesetArray(index, src), dest);
         index = index + 1;
      }
   }
}
expfun void deepExtendNamesetArray(nameset src,
                                   nameset dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToNamesetArray(textNew(elementAtNamesetArray(index, src)), dest);
         index = index + 1;
      }
   }
}
expfun void printNameset(text Offset,
                         nameset n,
                         FILE* fp,
                         c3po_bool showEmpty)
{
   text increment = " ";
   text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
   if (new_Offset == NULL) {
      fprintf(fp, "NULL\n");
      return;
   }
   sprintf(new_Offset, "%s%s", Offset, increment);

   if (n == NULL)
   {
      if (showEmpty == true)
      {
         fprintf(fp, "%s{\n", Offset);
         fprintf(fp, "%sNULL\n", new_Offset);
         fprintf(fp, "%s}\n", Offset);
      }
   }
   else
   {
      fprintf(fp, "%s{\n", Offset);

      if (n->array == NULL)
      {
         if (showEmpty == true)
            fprintf(fp, "%stext array : NULL\n", new_Offset);
      }
      else
      {
         if ((isEmptyNamesetArray(n) == false) || (showEmpty == true))
         {
            size_t index;
            fprintf(fp, "%sint size : %lu\n", new_Offset, (unsigned long)n->size);

            index = 0;
            while(index < n->size)
            {
               if (n->array[index] ne NULL)
               fprintf(fp, "%stext array[%lu] : %s\n", new_Offset, (unsigned long)index, n->array[index]);
               else if (showEmpty == true)
                  fprintf(fp, "%stext array[%lu] : NULL\n", new_Offset, (unsigned long)index);
               index = index + 1;
            }
         }
      }

      fprintf(fp, "%s}\n", Offset);
   }
   free(new_Offset);
}
expfun void freeNameset(nameset n,
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
