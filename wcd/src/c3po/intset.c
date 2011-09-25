#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"

#include "Error.h"
#include "intset.h"

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
expfun intset intsetNew(void)
{
   intset i = (intset) malloc (sizeof(intset_struct));
   if (i != NULL)
   {
      i->array = NULL;
      i->size = 0;
   }
   else
      malloc_error("intsetNew()");

   return i;
}
expfun intset copyIntset(intset src)
{
   intset i;

   if (src == NULL)
      return NULL;
   else
   {
      i = (intset) malloc (sizeof(intset_struct));
      if (i != NULL)
      {
         i->array = NULL;
         i->size = 0;

         extendIntset(src, i);
      }
      else
         malloc_error("copyIntset()");

      return i;
   }
}
expfun size_t* intsetGetArray(intset i)
{
   if (i != NULL)
      return i->array;
   else
      return NULL;
}
expfun c3po_bool intsetHasArray(intset i)
{
   if (isEmptyIntset(i) == false)
      return true;
   else
      return false;
}
expfun size_t getSizeOfIntset(intset i)
{
   if (i != NULL)
      return i->size;
   else
      return 0;
}
expfun void setSizeOfIntset(intset i,
                            size_t size)
{
   size_t index;
   if (i != NULL)
   {
      if (size == 0)
      {
         if (isEmptyIntset(i) eq false)
         {
            free((void *) i->array);
            i->array = NULL;
            i->size = 0;
         }
      }
      else if (size ne i->size)
      {
         if (isEmptyIntset(i) eq true)
            i->array = (size_t*) malloc(sizeof(size_t) * size);
         else
            i->array = (size_t*) realloc((void *) i->array, sizeof(size_t) * size);

         if (i->array ne NULL)
         {
            index = i->size;
            while(index < size)
            {
               i->array[index] = 0;
               index = index + 1;
            }
            i->size = size;
         }
         else
         {
            i->size = 0;
            malloc_error("setSizeOfIntset(i, size)");
         }
      }
   }
}
expfun c3po_bool isEmptyIntset(intset i)
{
   if (getSizeOfIntset(i) > 0)
      return false;
   else
      return true;
}
expfun void addToIntset(size_t i,
                        intset set)
{
   if (set != NULL)
   {
      setSizeOfIntset(set, set->size + 1);
      if (set->array != NULL)
         set->array[set->size - 1] = i;
      else
         malloc_error("addToIntset()");
   }
}
expfun void putElementAtIntset(size_t i,
                               size_t position,
                               intset set)
{
   if (set != NULL)
   {
      if (position >= set->size)
         setSizeOfIntset(set, position + 1);
      if (set->array != NULL)
         set->array[position] = i;
      else
         malloc_error("putElementAtIntset(i, position, set)");
   }
}
expfun void insertElementAtIntset(size_t i,
                                  size_t position,
                                  intset set)
{
   size_t index;
   if (set != NULL)
   {
      index = set->size;
      while(index > position)
      {
         putElementAtIntset(set->array[index-1], index, set);
         index = index - 1;
      }
      putElementAtIntset(i, position, set);
   }
}
expfun void removeElementAtIntset(size_t position,
                                  intset set)
{
   size_t index;
   if (set != NULL)
   {
      if (position < set->size)
      {
         index = position + 1;
         while(index < set->size)
         {
            putElementAtIntset(set->array[index], index - 1, set);
            index = index + 1;
         }
         setSizeOfIntset(set, set->size - 1);
      }
   }
}
expfun size_t elementAtIntset(size_t position,
                           intset set)
{
   if (set != NULL)
   {
      if (position < set->size)
         return set->array[position];
   }

   return 0;
}
expfun void extendIntset(intset src,
                         intset dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToIntset(elementAtIntset(index, src), dest);
         index = index + 1;
      }
   }
}
expfun void printIntset(text Offset,
                        intset i,
                        FILE* fp,
                        c3po_bool showEmpty)
{
   size_t index;

   text increment = " ";
   text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
   sprintf(new_Offset, "%s%s", Offset, increment);

   if (i == NULL)
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

      if (i->array == NULL)
      {
         if (showEmpty == true)
            fprintf(fp, "%sint array : NULL\n", new_Offset);
      }
      else
      {
         if ((isEmptyIntset(i) == false) || (showEmpty == true))
         {
            fprintf(fp, "%sint size : %lu\n", new_Offset, (unsigned long)i->size);

            index = 0;
            while(index < i->size)
            {
               fprintf(fp, "%sint array[%lu] : %lu\n", new_Offset, (unsigned long)index, (unsigned long)i->array[index]);
               index = index + 1;
            }
         }
      }

      fprintf(fp, "%s}\n", Offset);
   }
}
expfun void freeIntset(intset i)
{
   if (i ne NULL)
   {

      free((void *) i);
   }
}
