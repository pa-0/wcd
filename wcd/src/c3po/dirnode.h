#ifndef DIRNODE_INCLUDED
#define DIRNODE_INCLUDED
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
   extern dirnode dirnodeNew(dirnode parent,
                             dirnode up,
                             dirnode down);
   extern dirnode copyDirnode(dirnode src,
                              c3po_bool Deep);
   extern text dirnodeGetName(dirnode d);
   extern int dirnodeGetX(dirnode d);
   extern int dirnodeGetY(dirnode d);
   extern dirnode dirnodeGetParent(dirnode d);
   extern dirnode* dirnodeGetSubdirs(dirnode d);
   extern dirnode dirnodeGetUp(dirnode d);
   extern dirnode dirnodeGetDown(dirnode d);
   extern c3po_bool dirnodeFold(dirnode d);
   extern dirnode dirnodeSetName(text name,
                                 dirnode d);
   extern dirnode dirnodeSetX(int x,
                              dirnode d);
   extern dirnode dirnodeSetY(int y,
                              dirnode d);
   extern dirnode dirnodeSetParent(dirnode parent,
                                   dirnode d);
   extern dirnode dirnodeSetUp(dirnode up,
                               dirnode d);
   extern dirnode dirnodeSetDown(dirnode down,
                                 dirnode d);
   extern dirnode dirnodeSetFold(c3po_bool fold,
                                 dirnode d);
   extern c3po_bool dirnodeHasName(dirnode d);
   extern c3po_bool dirnodeHasParent(dirnode d);
   extern c3po_bool dirnodeHasSubdirs(dirnode d);
   extern c3po_bool dirnodeHasUp(dirnode d);
   extern c3po_bool dirnodeHasDown(dirnode d);
   extern dirnode endOfRecursionOfDirnodeParent(dirnode d);
   extern dirnode endOfRecursionOfDirnodeUp(dirnode d);
   extern dirnode endOfRecursionOfDirnodeDown(dirnode d);
   extern int getSizeOfDirnode(dirnode d);
   extern void setSizeOfDirnode(dirnode d,
                                int size);
   extern c3po_bool isEmptyDirnode(dirnode d);
   extern void addToDirnode(dirnode d,
                            dirnode set);
   extern void putElementAtDirnode(dirnode d,
                                   int position,
                                   dirnode set);
   extern void insertElementAtDirnode(dirnode d,
                                      int position,
                                      dirnode set);
   extern void removeElementAtDirnode(int position,
                                      dirnode set,
                                      c3po_bool FreeAtPos,
                                      c3po_bool Deep);
   extern dirnode elementAtDirnode(int position,
                                   dirnode set);
   extern void extendDirnode(dirnode src,
                             dirnode dest);
   extern void deepExtendDirnode(dirnode src,
                                 dirnode dest);
   extern void printDirnode(text Offset,
                            dirnode d,
                            FILE* fp,
                            c3po_bool showEmpty);
   extern void freeDirnode(dirnode d,
                           c3po_bool Deep);
#endif
