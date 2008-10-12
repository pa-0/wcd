#ifndef STRUCTURES_INCLUDED
#define STRUCTURES_INCLUDED
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

#ifndef C3PO_BASIC_TYPES_INCLUDED
#define C3PO_BASIC_TYPES_INCLUDED

   typedef char *text;

   typedef struct nameset_struct
   {
      text *array;
      int size;
   } nameset_struct, *nameset;

   typedef struct intset_struct
   {
      int *array;
      int size;
   } intset_struct, *intset;

#endif

   typedef struct WcdStack_struct
   {
      int maxsize;
      int lastadded;
      int current;
      text *dir;
      int size;
   } WcdStack_struct, *WcdStack;

   typedef struct dirnode_struct
   {
      text name;
      int x;
      int y;
      struct dirnode_struct *parent;
      struct dirnode_struct **subdirs;
      int size;
      struct dirnode_struct *up;
      struct dirnode_struct *down;
      c3po_bool fold;
   } dirnode_struct, *dirnode;

#endif
