include
{
prepend '/*\
Copyright (C) 1997-2000 Ondrej Popp\
This code is generated with Ondrej Popp\'s C3PO.\
\
This program is free software; you can redistribute it and/or\
modify it under the terms of the GNU General Public License\
as published by the Free Software Foundation; either version 2\
of the License, or (at your option) any later version.\
\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\
GNU General Public License for more details.\
\
You should have received a copy of the GNU General Public License\
along with this program; if not, write to the Free Software\
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\
\
*/'
}


<WcdStack : (int) i->maxsize
           (int)->lastadded
           (int)->current
           ((text)->dir)+
          >;

/* recursive directory tree structure */
<dirnode : (text)->name
          (int)->x
          (int)->y
          dirnode->parent &
          (dirnode->subdirs)+
          dirnode->up &
          dirnode->down &
          (bool)->fold
          >;

