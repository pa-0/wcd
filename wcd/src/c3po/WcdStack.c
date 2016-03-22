#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"

#include "Error.h"
#include "Text.h"
#include "WcdStack.h"

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
expfun WcdStack WcdStackNew(int maxsize)
{
   WcdStack w = (WcdStack) malloc (sizeof(WcdStack_struct));
   if (w != NULL)
   {
      w->maxsize = maxsize;
      w->lastadded = 0;
      w->current = 0;

      w->dir = NULL;
      w->size = 0;
   }
   else
      malloc_error("WcdStackNew()");

   return w;
}
expfun WcdStack copyWcdStack(WcdStack src,
                             c3po_bool Deep)
{
   WcdStack w;

   if (src == NULL)
      return NULL;
   else
   {
      w = (WcdStack) malloc (sizeof(WcdStack_struct));
      if (w != NULL)
      {
         if (Deep == true)
         {
            w->maxsize = src->maxsize;
            w->lastadded = src->lastadded;
            w->current = src->current;
         }
         else
         {
            w->maxsize = src->maxsize;
            w->lastadded = src->lastadded;
            w->current = src->current;
         }

         w->dir = NULL;
         w->size = 0;

         if (Deep == true)
            deepExtendWcdStackDir(src, w);
         else
            extendWcdStackDir(src, w);
      }
      else
         malloc_error("copyWcdStack()");

      return w;
   }
}
expfun int WcdStackGetMaxsize(WcdStack w)
{
   if (w != NULL)
      return w->maxsize;
   else
      return 0;
}
expfun int WcdStackGetLastadded(WcdStack w)
{
   if (w != NULL)
      return w->lastadded;
   else
      return 0;
}
expfun int WcdStackGetCurrent(WcdStack w)
{
   if (w != NULL)
      return w->current;
   else
      return 0;
}
expfun text* WcdStackGetDir(WcdStack w)
{
   if (w != NULL)
      return w->dir;
   else
      return NULL;
}
expfun WcdStack WcdStackSetMaxsize(int maxsize,
                                   WcdStack w)
{
   if (w != NULL)
      w->maxsize = maxsize;
   return w;
}
expfun WcdStack WcdStackSetLastadded(int lastadded,
                                     WcdStack w)
{
   if (w != NULL)
      w->lastadded = lastadded;
   return w;
}
expfun WcdStack WcdStackSetCurrent(int current,
                                   WcdStack w)
{
   if (w != NULL)
      w->current = current;
   return w;
}
expfun c3po_bool WcdStackHasDir(WcdStack w)
{
   if (isEmptyWcdStackDir(w) == false)
      return true;
   else
      return false;
}
expfun size_t getSizeOfWcdStackDir(WcdStack w)
{
   if (w != NULL)
      return w->size;
   else
      return 0;
}
expfun void setSizeOfWcdStackDir(WcdStack w,
                                 size_t size)
{
   size_t index;
   if (w != NULL)
   {
      if (size == 0)
      {
         if (isEmptyWcdStackDir(w) eq false)
         {
            free((void *) w->dir);
            w->dir = NULL;
            w->size = 0;
         }
      }
      else if (size ne w->size)
      {
         if (isEmptyWcdStackDir(w) eq true)
            w->dir = (text*) malloc(sizeof(text) * size);
         else
            w->dir = (text*) realloc((void *) w->dir, sizeof(text) * size);

         if (w->dir ne NULL)
         {
            index = w->size;
            while(index < size)
            {
               w->dir[index] = NULL;
               index = index + 1;
            }
            w->size = size;
         }
         else
         {
            w->size = 0;
            malloc_error("setSizeOfWcdStackDir(w, size)");
         }
      }
   }
}
expfun c3po_bool isEmptyWcdStackDir(WcdStack w)
{
   if (getSizeOfWcdStackDir(w) > 0)
      return false;
   else
      return true;
}
expfun void addToWcdStackDir(text t,
                             WcdStack set)
{
   if (set != NULL)
   {
      setSizeOfWcdStackDir(set, set->size + 1);
      if (set->dir != NULL)
         set->dir[set->size - 1] = t;
      else
         malloc_error("addToWcdStackDir()");
   }
}
expfun void putElementAtWcdStackDir(text t,
                                    size_t position,
                                    WcdStack set)
{
   if (set != NULL)
   {
      if (position >= set->size)
         setSizeOfWcdStackDir(set, position + 1);
      if (set->dir != NULL)
         set->dir[position] = t;
      else
         malloc_error("putElementAtWcdStackDir(t, position, set)");
   }
}
expfun void insertElementAtWcdStackDir(text t,
                                       size_t position,
                                       WcdStack set)
{
   size_t index;
   if (set != NULL)
   {
      index = set->size;
      while(index > position)
      {
         putElementAtWcdStackDir(set->dir[index-1], index, set);
         index = index - 1;
      }
      putElementAtWcdStackDir(t, position, set);
   }
}
expfun void removeElementAtWcdStackDir(size_t position,
                                       WcdStack set,
                                       c3po_bool FreeAtPos)
{
   size_t index;
   if (set != NULL)
   {
      if (position < set->size)
      {
         if (FreeAtPos eq true)
         {
            if (set->dir[position] ne NULL)
               free((void *) set->dir[position]);
         }

         index = position + 1;
         while(index < set->size)
         {
            putElementAtWcdStackDir(set->dir[index], index - 1, set);
            index = index + 1;
         }
         setSizeOfWcdStackDir(set, set->size - 1);
      }
   }
}
expfun text elementAtWcdStackDir(size_t position,
                                 WcdStack set)
{
   if (set != NULL)
   {
      if (position < set->size)
         return set->dir[position];
   }

   return NULL;
}
expfun void extendWcdStackDir(WcdStack src,
                              WcdStack dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToWcdStackDir(elementAtWcdStackDir(index, src), dest);
         index = index + 1;
      }
   }
}
expfun void deepExtendWcdStackDir(WcdStack src,
                                  WcdStack dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToWcdStackDir(textNew(elementAtWcdStackDir(index, src)), dest);
         index = index + 1;
      }
   }
}
expfun void printWcdStack(text Offset,
                          WcdStack w,
                          FILE* fp,
                          c3po_bool showEmpty)
{
   size_t index;

   text increment = " ";
   text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
   sprintf(new_Offset, "%s%s", Offset, increment);

   if (w == NULL)
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

      fprintf(fp, "%sint maxsize : %d\n", new_Offset, w->maxsize);

      fprintf(fp, "%sint lastadded : %d\n", new_Offset, w->lastadded);

      fprintf(fp, "%sint current : %d\n", new_Offset, w->current);

      if (w->dir == NULL)
      {
         if (showEmpty == true)
            fprintf(fp, "%stext dir : NULL\n", new_Offset);
      }
      else
      {
         if ((isEmptyWcdStackDir(w) == false) || (showEmpty == true))
         {
            fprintf(fp, "%sint size : %lu\n", new_Offset, (unsigned long)w->size);

            index = 0;
            while(index < w->size)
            {
               if (w->dir[index] ne NULL)
               fprintf(fp, "%stext dir[%lu] : %s\n", new_Offset, (unsigned long)index, w->dir[index]);
               else if (showEmpty == true)
                  fprintf(fp, "%stext dir[%lu] : NULL\n", new_Offset, (unsigned long)index);
               index = index + 1;
            }
         }
      }

      fprintf(fp, "%s}\n", Offset);
   }
   free(new_Offset);
}
expfun void freeWcdStack(WcdStack w,
                         c3po_bool Deep)
{
   size_t index;

   if (w ne NULL)
   {
      if (Deep eq true)
      {

         index = 0;
         while(index < w->size)
         {
            if (w->dir[index] ne NULL)
               free((void *) w->dir[index]);

            index = index + 1;
         }
      }

      if (w->dir ne NULL)
      {
         free((void *) w->dir);
         w->dir = NULL;
         w->size = 0;
      }

      free((void *) w);
   }
}
