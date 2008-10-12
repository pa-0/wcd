#ifndef FILE_INCLUDED
#define FILE_INCLUDED
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
   extern FILE * openR(text filename);
   extern FILE * openRwarn(text filename,
                           c3po_bool Warn);
   extern c3po_bool existsFile(text filename);
   extern FILE * openW(text filename);
   extern FILE * openWBackup(text filename,
                             c3po_bool Backup);
   extern FILE * Append(text filename);
   extern void Close(FILE* fp);
   extern unsigned long getFileSize(text filename);
#endif
