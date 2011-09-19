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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/
   extern text textNewSize(size_t size);
   extern text textNew(text string);
   extern c3po_bool eqText(text a,
                      text b);
   extern size_t inNameset(text name,
                        nameset set);
   extern intset matchNameset(text name,
                              nameset set);
   extern size_t matchCountNameset(text name,
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
                              size_t amount,
                              size_t bufferNr);
   extern text repeat(text pattern,
                      size_t amount);
   extern text spacesOnBuffer(size_t amount,
                              size_t bufferNr);
   extern text spaces(size_t amount);
   extern text TabOnBuffer(size_t amount,
                           size_t bufferNr);
   extern text Tab(size_t amount);
   extern text intToString(int i);
   extern text doubleToString(double d);
#endif
