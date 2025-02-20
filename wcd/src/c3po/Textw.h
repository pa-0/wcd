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
   extern textw textwNewSize(size_t size);
   extern textw textwNew(textw string);
   extern c3po_bool eqTextw(textw a,
                      textw b);
   extern size_t inNamesetw(textw name,
                        namesetw set);
   extern intset matchNamesetw(textw name,
                              namesetw set);
   extern size_t matchCountNamesetw(textw name,
                                namesetw set);
   extern textw concatw(textw a,
                      textw b);
   extern textw concatw3(textw a,
                       textw b,
                       textw c);
   extern textw concatw4(textw a,
                       textw b,
                       textw c,
                       textw d);
   extern textw repeatOnBufferw(textw pattern,
                              size_t amount,
                              size_t bufferNr);
   extern textw repeatw(textw pattern,
                      size_t amount);
   extern textw spacesOnBufferw(size_t amount,
                              size_t bufferNr);
   extern textw spacesw(size_t amount);
   extern textw TabOnBufferw(size_t amount,
                           size_t bufferNr);
   extern textw Tabw(size_t amount);
   extern textw intToStringw(int i);
   extern textw doubleToStringw(double d);
#endif
