#ifndef INTSET_INCLUDED
#define INTSET_INCLUDED
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
   extern intset intsetNew(void);
   extern intset copyIntset(intset src);
   extern size_t* intsetGetArray(intset i);
   extern c3po_bool intsetHasArray(intset i);
   extern size_t getSizeOfIntset(intset i);
   extern void setSizeOfIntset(intset i,
                               size_t size);
   extern c3po_bool isEmptyIntset(intset i);
   extern void addToIntset(size_t i,
                           intset set);
   extern void putElementAtIntset(size_t i,
                                  size_t position,
                                  intset set);
   extern void insertElementAtIntset(size_t i,
                                     size_t position,
                                     intset set);
   extern void removeElementAtIntset(size_t position,
                                     intset set);
   extern size_t elementAtIntset(size_t position,
                              intset set);
   extern void extendIntset(intset src,
                            intset dest);
   extern void printIntset(text Offset,
                           intset i,
                           FILE* fp,
                           c3po_bool showEmpty);
   extern void freeIntset(intset i);
#endif
