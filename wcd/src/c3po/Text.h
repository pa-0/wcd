#ifndef TEXT_INCLUDED
#define TEXT_INCLUDED
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
   extern text textNewSize(int size);
   extern text textNew(text string);
   extern c3po_bool eqText(text a,
                      text b);
   extern int inNameset(text name,
                        nameset set);
   extern intset matchNameset(text name,
                              nameset set);
   extern int matchCountNameset(text name,
                                nameset set);
   extern text concat(text a,
                      text b);
   extern text concat3(text a,
                       text b,
                       text c);
   extern text concat4(text a,
                       text b,
                       text c,
                       text d);
   extern text repeatOnBuffer(text pattern,
                              int amount,
                              int bufferNr);
   extern text repeat(text pattern,
                      int amount);
   extern text spacesOnBuffer(int amount,
                              int bufferNr);
   extern text spaces(int amount);
   extern text TabOnBuffer(int amount,
                           int bufferNr);
   extern text Tab(int amount);
   extern text intToString(int i);
   extern text doubleToString(double d);
#endif
