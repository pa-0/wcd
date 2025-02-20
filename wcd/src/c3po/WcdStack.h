#ifndef WCDSTACK_INCLUDED
#define WCDSTACK_INCLUDED
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
   extern WcdStack WcdStackNew(int maxsize);
   extern WcdStack copyWcdStack(WcdStack src,
                                c3po_bool Deep);
   extern int WcdStackGetMaxsize(WcdStack w);
   extern int WcdStackGetLastadded(WcdStack w);
   extern int WcdStackGetCurrent(WcdStack w);
   extern text* WcdStackGetDir(WcdStack w);
   extern WcdStack WcdStackSetMaxsize(int maxsize,
                                      WcdStack w);
   extern WcdStack WcdStackSetLastadded(int lastadded,
                                        WcdStack w);
   extern WcdStack WcdStackSetCurrent(int current,
                                      WcdStack w);
   extern c3po_bool WcdStackHasDir(WcdStack w);
   extern size_t getSizeOfWcdStackDir(WcdStack w);
   extern void setSizeOfWcdStackDir(WcdStack w,
                                    size_t size);
   extern c3po_bool isEmptyWcdStackDir(WcdStack w);
   extern void addToWcdStackDir(text t,
                                WcdStack set);
   extern void putElementAtWcdStackDir(text t,
                                       size_t position,
                                       WcdStack set);
   extern void insertElementAtWcdStackDir(text t,
                                          size_t position,
                                          WcdStack set);
   extern void removeElementAtWcdStackDir(size_t position,
                                          WcdStack set,
                                          c3po_bool FreeAtPos);
   extern text elementAtWcdStackDir(size_t position,
                                    WcdStack set);
   extern void extendWcdStackDir(WcdStack src,
                                 WcdStack dest);
   extern void deepExtendWcdStackDir(WcdStack src,
                                     WcdStack dest);
   extern void printWcdStack(text Offset,
                             WcdStack w,
                             FILE* fp,
                             c3po_bool showEmpty);
   extern void freeWcdStack(WcdStack w,
                            c3po_bool Deep);
#endif
