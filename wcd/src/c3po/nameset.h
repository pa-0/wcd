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
   extern nameset namesetNew(void);
   extern nameset copyNameset(nameset src,
                              c3po_bool Deep);
   extern text* namesetGetArray(nameset n);
   extern c3po_bool namesetHasArray(nameset n);
   extern int getSizeOfNamesetArray(nameset n);
   extern void setSizeOfNamesetArray(nameset n,
                                     int size);
   extern c3po_bool isEmptyNamesetArray(nameset n);
   extern void addToNamesetArray(text t,
                                 nameset set);
   extern void putElementAtNamesetArray(text t,
                                        int position,
                                        nameset set);
   extern void insertElementAtNamesetArray(text t,
                                           int position,
                                           nameset set);
   extern void removeElementAtNamesetArray(int position,
                                           nameset set,
                                           c3po_bool FreeAtPos);
   extern text elementAtNamesetArray(int position,
                                     nameset set);
   extern void extendNamesetArray(nameset src,
                                  nameset dest);
   extern void deepExtendNamesetArray(nameset src,
                                      nameset dest);
   extern void printNameset(text Offset,
                            nameset n,
                            FILE* fp,
                            c3po_bool showEmpty);
   extern void freeNameset(nameset n,
                           c3po_bool Deep);
#endif
