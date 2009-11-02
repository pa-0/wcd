#ifndef NAMESET_INCLUDED
#define NAMESET_INCLUDED
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
   extern namesetw namesetwNew(void);
   extern namesetw copyNamesetw(namesetw src,
                              c3po_bool Deep);
   extern textw* namesetwGetArray(namesetw n);
   extern c3po_bool namesetwHasArray(namesetw n);
   extern int getSizeOfNamesetwArray(namesetw n);
   extern void setSizeOfNamesetwArray(namesetw n,
                                     int size);
   extern c3po_bool isEmptyNamesetwArray(namesetw n);
   extern void addToNamesetwArray(textw t,
                                 namesetw set);
   extern void putElementAtNamesetwArray(textw t,
                                        int position,
                                        namesetw set);
   extern void insertElementAtNamesetwArray(textw t,
                                           int position,
                                           namesetw set);
   extern void removeElementAtNamesetwArray(int position,
                                           namesetw set,
                                           c3po_bool FreeAtPos);
   extern textw elementAtNamesetwArray(int position,
                                     namesetw set);
   extern void extendNamesetwArray(namesetw src,
                                  namesetw dest);
   extern void deepExtendNamesetwArray(namesetw src,
                                      namesetw dest);
   extern void printNamesetw(textw Offset,
                            namesetw n,
                            FILE* fp,
                            c3po_bool showEmpty);
   extern void freeNamesetw(namesetw n,
                           c3po_bool Deep);
#endif
