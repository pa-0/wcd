/*
Copyright (C) 2000-2013 Erwin Waterlander

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

#ifndef _WCD_GRAPHICS_H
#define _WCD_GRAPHICS_H

dirnode createRootNode(void);
void buildTreeFromFile(char *filename, dirnode d, int quiet);
void buildTreeFromNameset(nameset set, dirnode d);
void sortTree(dirnode d);
void setXYTree(dirnode d, const int *graphics_mode);
void dumpTree(dirnode d, const int *graphics_mode);
char *selectANode(dirnode tree, int *use_HOME, int ignore_case, int graphics_mode,int ignore_diacritics);
void setFold(dirnode n, c3po_bool f, int *ymax);

#define WCD_GRAPH_NO     0 /* no graphics */
#define WCD_GRAPH_NORMAL 1 /* graphics mode */
#define WCD_GRAPH_DUMP   2 /* dump graphics */
#define WCD_GRAPH_ALT    4 /* alternative navigating */
#define WCD_GRAPH_COLOR  8 /* colors */
#define WCD_GRAPH_CENTER 16 /* center current directory */
#define WCD_GRAPH_ASCII  32 /* draw tree with ASCII characters */
#define WCD_GRAPH_COMPACT 64 /* draw compact tree */
#define WCD_GRAPH_CJK    128 /* East-Asian CJK font with double width line drawing symbols */

#if defined(WCD_UNICODE) || defined(WCD_WINDOWS)
int wcd_wcwidth(wchar_t c);
int wcd_wcswidth(const wchar_t *pwcs, size_t n);
#endif

#endif
