/*
Copyright (C) 2000-2005 Erwin Waterlander

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

#ifndef _WCD_GRAPHICS_H
#define _WCD_GRAPHICS_H

dirnode createRootNode();
void buildTreeFromFile(char *filename, dirnode d);
void buildTreeFromNameset(nameset set, dirnode d);
void setXYTree(dirnode d);
void dumpTree(dirnode d);
char *selectANode(dirnode tree, int *use_HOME, int ignore_case, int graphics_mode);

#define WCD_GRAPH_NO     0 /* no graphics */
#define WCD_GRAPH_NORMAL 1 /* graphics mode */
#define WCD_GRAPH_DUMP   2 /* dump graphics */
#define WCD_GRAPH_ALT    4 /* alternative navigating */
#define WCD_GRAPH_COLOR  8 /* colors */
	
#endif

