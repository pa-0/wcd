/*
Copyright (C) 1997-1998 Erwin Waterlander

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

#ifndef _STACK_H
#define _STACK_H


#define WCD_STACK_SIZE 10

int stack_read(WcdStack ws,char *stackfilename);
int stack_print(WcdStack ws, int use_numbers, int use_stdout);
int stack_write(WcdStack ws,char *stackfilename);
int stack_add(WcdStack ws, char *dir);
char* stack_push(WcdStack ws, int push_ntimes);
char* stack_pop(WcdStack ws, int pop_ntimes);

#endif
