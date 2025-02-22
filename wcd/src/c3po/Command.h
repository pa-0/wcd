#ifndef COMMAND_INCLUDED
#define COMMAND_INCLUDED
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
   extern text getBasename(text input_file);
   extern text getCore(text input_file);
   extern c3po_bool inArgList(text name,
                         int argc,
                         char** argv);
   extern text getArg(text name,
                      int argc,
                      char** argv);
   extern nameset getArgs(text name,
                          int argc,
                          char** argv);
   extern c3po_bool isInt(text string);
   extern c3po_bool isFloat(text string);
   extern void dumpCommandline(text file,
                               int argc,
                               char** argv);
#endif
