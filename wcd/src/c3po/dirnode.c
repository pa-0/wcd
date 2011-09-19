#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "std_macr.h"
#include "structur.h"

#include "Error.h"
#include "Text.h"
#include "dirnode.h"

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
expfun dirnode dirnodeNew(dirnode parent,
                          dirnode up,
                          dirnode down)
{
   dirnode d = (dirnode) malloc (sizeof(dirnode_struct));
   if (d != NULL)
   {
      d->parent = parent;
      d->up = up;
      d->down = down;
      d->name = NULL;
      d->x = 0;
      d->y = 0;
      d->fold = false;

      d->subdirs = NULL;
      d->size = 0;
   }
   else
      malloc_error("dirnodeNew()");

   return d;
}
expfun dirnode copyDirnode(dirnode src,
                           c3po_bool Deep)
{
   dirnode d;

   if (src == NULL)
      return NULL;
   else
   {
      d = (dirnode) malloc (sizeof(dirnode_struct));
      if (d != NULL)
      {
         if (Deep == true)
         {
            d->name = textNew(src->name);
            d->x = src->x;
            d->y = src->y;
            d->parent = src->parent;
            d->up = src->up;
            d->down = src->down;
            d->fold = src->fold;
         }
         else
         {
            d->name = src->name;
            d->x = src->x;
            d->y = src->y;
            d->parent = src->parent;
            d->up = src->up;
            d->down = src->down;
            d->fold = src->fold;
         }

         d->subdirs = NULL;
         d->size = 0;

         if (Deep == true)
            deepExtendDirnode(src, d);
         else
            extendDirnode(src, d);
      }
      else
         malloc_error("copyDirnode()");

      return d;
   }
}
expfun text dirnodeGetName(dirnode d)
{
   if (d != NULL)
      return d->name;
   else
      return NULL;
}
expfun int dirnodeGetX(dirnode d)
{
   if (d != NULL)
      return d->x;
   else
      return 0;
}
expfun int dirnodeGetY(dirnode d)
{
   if (d != NULL)
      return d->y;
   else
      return 0;
}
expfun dirnode dirnodeGetParent(dirnode d)
{
   if (d != NULL)
      return d->parent;
   else
      return NULL;
}
expfun dirnode* dirnodeGetSubdirs(dirnode d)
{
   if (d != NULL)
      return d->subdirs;
   else
      return NULL;
}
expfun dirnode dirnodeGetUp(dirnode d)
{
   if (d != NULL)
      return d->up;
   else
      return NULL;
}
expfun dirnode dirnodeGetDown(dirnode d)
{
   if (d != NULL)
      return d->down;
   else
      return NULL;
}
expfun c3po_bool dirnodeFold(dirnode d)
{
   if (d != NULL)
      return d->fold;
   else
      return false;
}
expfun dirnode dirnodeSetName(text name,
                              dirnode d)
{
   if (d != NULL)
      d->name = name;
   return d;
}
expfun dirnode dirnodeSetX(int x,
                           dirnode d)
{
   if (d != NULL)
      d->x = x;
   return d;
}
expfun dirnode dirnodeSetY(int y,
                           dirnode d)
{
   if (d != NULL)
      d->y = y;
   return d;
}
expfun dirnode dirnodeSetParent(dirnode parent,
                                dirnode d)
{
   if (d != NULL)
      d->parent = parent;
   return d;
}
expfun dirnode dirnodeSetUp(dirnode up,
                            dirnode d)
{
   if (d != NULL)
      d->up = up;
   return d;
}
expfun dirnode dirnodeSetDown(dirnode down,
                              dirnode d)
{
   if (d != NULL)
      d->down = down;
   return d;
}
expfun dirnode dirnodeSetFold(c3po_bool fold,
                              dirnode d)
{
   if (d != NULL)
      d->fold = fold;
   return d;
}
expfun c3po_bool dirnodeHasName(dirnode d)
{
   if (dirnodeGetName(d) != NULL)
      return true;
   else
      return false;
}
expfun c3po_bool dirnodeHasParent(dirnode d)
{
   if (dirnodeGetParent(d) != NULL)
      return true;
   else
      return false;
}
expfun c3po_bool dirnodeHasSubdirs(dirnode d)
{
   if (isEmptyDirnode(d) == false)
      return true;
   else
      return false;
}
expfun c3po_bool dirnodeHasUp(dirnode d)
{
   if (dirnodeGetUp(d) != NULL)
      return true;
   else
      return false;
}
expfun c3po_bool dirnodeHasDown(dirnode d)
{
   if (dirnodeGetDown(d) != NULL)
      return true;
   else
      return false;
}
expfun dirnode endOfRecursionOfDirnodeParent(dirnode d)
{
   if (dirnodeHasParent(d) == true)
      return endOfRecursionOfDirnodeParent(dirnodeGetParent(d));
   else
      return d;
}
expfun dirnode endOfRecursionOfDirnodeUp(dirnode d)
{
   if (dirnodeHasUp(d) == true)
      return endOfRecursionOfDirnodeUp(dirnodeGetUp(d));
   else
      return d;
}
expfun dirnode endOfRecursionOfDirnodeDown(dirnode d)
{
   if (dirnodeHasDown(d) == true)
      return endOfRecursionOfDirnodeDown(dirnodeGetDown(d));
   else
      return d;
}
expfun size_t getSizeOfDirnode(dirnode d)
{
   if (d != NULL)
      return d->size;
   else
      return 0;
}
expfun void setSizeOfDirnode(dirnode d,
                             size_t size)
{
   size_t index;
   if (d != NULL)
   {
      if (size <= 0)
      {
         if (isEmptyDirnode(d) eq false)
         {
            free((void *) d->subdirs);
            d->subdirs = NULL;
            d->size = 0;
         }
      }
      else if (size ne d->size)
      {
         if (isEmptyDirnode(d) eq true)
            d->subdirs = (dirnode*) malloc(sizeof(dirnode) * size);
         else
            d->subdirs = (dirnode*) realloc((void *) d->subdirs, sizeof(dirnode) * size);

         if (d->subdirs ne NULL)
         {
            index = d->size;
            while(index < size)
            {
               d->subdirs[index] = NULL;
               index = index + 1;
            }
            d->size = size;
         }
         else
         {
            d->size = 0;
            malloc_error("setSizeOfDirnode(d, size)");
         }
      }
   }
}
expfun c3po_bool isEmptyDirnode(dirnode d)
{
   if (getSizeOfDirnode(d) > 0)
      return false;
   else
      return true;
}
expfun void addToDirnode(dirnode d,
                         dirnode set)
{
   if (set != NULL)
   {
      setSizeOfDirnode(set, set->size + 1);
      if (set->subdirs != NULL)
         set->subdirs[set->size - 1] = d;
      else
         malloc_error("addToDirnode()");
   }
}
expfun void putElementAtDirnode(dirnode d,
                                size_t position,
                                dirnode set)
{
   if (set != NULL)
   {
      if (position >= set->size)
         setSizeOfDirnode(set, position + 1);
      if (set->subdirs != NULL)
         set->subdirs[position] = d;
      else
         malloc_error("putElementAtDirnode(d, position, set)");
   }
}
expfun void insertElementAtDirnode(dirnode d,
                                   size_t position,
                                   dirnode set)
{
   size_t index;
   if (set != NULL)
   {
      index = set->size;
      while(index > position)
      {
         putElementAtDirnode(set->subdirs[index-1], index, set);
         index = index - 1;
      }
      putElementAtDirnode(d, position, set);
   }
}
expfun void removeElementAtDirnode(size_t position,
                                   dirnode set,
                                   c3po_bool FreeAtPos,
                                   c3po_bool Deep)
{
   size_t index;
   if (set != NULL)
   {
      if (position < set->size)
      {
         if (FreeAtPos eq true)
            freeDirnode(set->subdirs[position], Deep);

         index = position + 1;
         while(index < set->size)
         {
            putElementAtDirnode(set->subdirs[index], index - 1, set);
            index = index + 1;
         }
         setSizeOfDirnode(set, set->size - 1);
      }
   }
}
expfun dirnode elementAtDirnode(size_t position,
                                dirnode set)
{
   if (set != NULL)
   {
      if (position < set->size)
         return set->subdirs[position];
   }

   return NULL;
}
expfun void extendDirnode(dirnode src,
                          dirnode dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToDirnode(elementAtDirnode(index, src), dest);
         index = index + 1;
      }
   }
}
expfun void deepExtendDirnode(dirnode src,
                              dirnode dest)
{
   if (src != NULL)
   {
      size_t index = 0;
      while(index < src->size)
      {
         addToDirnode(copyDirnode(elementAtDirnode(index, src), true), dest);
         index = index + 1;
      }
   }
}
expfun void printDirnode(text Offset,
                         dirnode d,
                         FILE* fp,
                         c3po_bool showEmpty)
{
   size_t index;

   text increment = " ";
   text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
   sprintf(new_Offset, "%s%s", Offset, increment);

   if (d == NULL)
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

      if ((dirnodeHasName(d) == true) || (showEmpty == true))
      {
         if (d->name ne NULL)
            fprintf(fp, "%stext name : %s\n", new_Offset, d->name);
         else if (showEmpty == true)
            fprintf(fp, "%stext name : NULL\n", new_Offset);
      }

      fprintf(fp, "%sint x : %d\n", new_Offset, d->x);

      fprintf(fp, "%sint y : %d\n", new_Offset, d->y);

      if ((dirnodeHasParent(d) == true) || (showEmpty == true))
         fprintf(fp, "%sdirnode parent : %lu (reference)\n", new_Offset, (long) d->parent);

      if ((dirnodeHasUp(d) == true) || (showEmpty == true))
         fprintf(fp, "%sdirnode up : %lu (reference)\n", new_Offset, (long) d->up);

      if ((dirnodeHasDown(d) == true) || (showEmpty == true))
         fprintf(fp, "%sdirnode down : %lu (reference)\n", new_Offset, (long) d->down);

      fprintf(fp, "%sc3po_bool fold : %d\n", new_Offset, d->fold);

      if (d->subdirs == NULL)
      {
         if (showEmpty == true)
            fprintf(fp, "%sdirnode subdirs : NULL\n", new_Offset);
      }
      else
      {
         if ((isEmptyDirnode(d) == false) || (showEmpty == true))
         {
            fprintf(fp, "%sint size : %d\n", new_Offset, d->size);

            index = 0;
            while(index < d->size)
            {
               fprintf(fp, "%sdirnode subdirs[%d],\n", new_Offset, index);
               printDirnode(new_Offset, d->subdirs[index], fp, showEmpty);
               fprintf(fp, "%s\\end dirnode[%d]\n", new_Offset, index);
               index = index + 1;
            }
         }
      }

      fprintf(fp, "%s}\n", Offset);
   }
}
expfun void freeDirnode(dirnode d,
                        c3po_bool Deep)
{
   size_t index;

   if (d ne NULL)
   {
      if (Deep eq true)
      {

         if (d->name ne NULL)
         {
            free((void *) d->name);
            d->name = NULL;
         }

         index = 0;
         while(index < d->size)
         {
            freeDirnode(d->subdirs[index], Deep);

            index = index + 1;
         }
      }

      if (d->subdirs ne NULL)
      {
         free((void *) d->subdirs);
         d->subdirs = NULL;
         d->size = 0;
      }

      free((void *) d);
   }
}
