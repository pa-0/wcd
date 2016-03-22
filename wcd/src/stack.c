/*
Copyright (C) 1997-2014 Erwin Waterlander

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "std_macr.h"
#include "structur.h"
#include "nameset.h"
#include "display.h"
#include "dosdir.h"
#include "WcdStack.h"
#include "Text.h"
#include "wcd.h"
#include "stack.h"
#include "config.h"
#include "wcddir.h"


/********************************************************************
 *
 *                 stack_add
 *
 ********************************************************************/

int stack_add(WcdStack ws, char *dir)
{
	ws->lastadded++;

	if(ws->lastadded == ws->maxsize)
		ws->lastadded = 0;

	ws->current = ws->lastadded;

	/* printWcdStack("add ", ws, stdout); */

	/* free old dir string if present */
	if (ws->dir != NULL)
	if((ws->dir[ws->lastadded] != NULL) && (ws->size == (size_t)ws->maxsize))
	free (ws->dir[ws->lastadded]);

	putElementAtWcdStackDir(textNew(dir), (size_t)ws->lastadded, ws);

	return(0);
}
/********************************************************************
 *
 *                 stack_read
 *
 ********************************************************************/

int stack_read(WcdStack ws,const char *stackfilename)
{

	FILE *infile;
	int line_nr=1;


	/* open stack-file */
	if  ((ws->maxsize <= 0)||((infile = wcd_fopen(stackfilename,"r",1)) == NULL))
	{
		/* print_error("Unable to read file %s\n",stackfilename); */
		ws->lastadded = -1;
		ws->current = -1;
	}
	else
	{
		if(fscanf(infile,"%d %d",&ws->lastadded,&ws->current)==2)
		{

	                char tmp[DD_MAXPATH];
			while( !feof(infile) && !ferror(infile) && (ws->size < (size_t)ws->maxsize) )
			{
			int len ;
			/* read a line */
			len = wcd_getline(tmp,DD_MAXPATH,infile,stackfilename,&line_nr);
			++line_nr;

			if (len > 0 )
				addToWcdStackDir(textNew(tmp), ws);
			}
		}
		else
		{
			print_error("%s", _("Error parsing stack\n"));
			ws->lastadded = -1;
			ws->current = -1;
		}
		if (ferror(infile)) {
			wcd_read_error(stackfilename);
		}

 		wcd_fclose(infile, stackfilename, "r", "stack_read: ");

		if (ws->lastadded >= (int)ws->size)
		ws->lastadded = 0;
		if (ws->current >= (int)ws->size)
		ws->current = 0;
	}

/*	printWcdStack("READ ", ws, stdout); */
	return(0);
}

/*******************************************************************/

int stack_print(WcdStack ws, int use_numbers, int use_stdout)
{
#ifdef WCD_USECONIO
	if (use_stdout == WCD_STDOUT_NO)
		return display_list_conio(NULL,ws,0,use_numbers);
	else
		return display_list_stdout(NULL,ws,0, use_stdout);
#else
# ifdef WCD_USECURSES
	int i;
	if ((use_stdout == WCD_STDOUT_NO) && ((i = display_list_curses(NULL,ws,0,use_numbers)) != WCD_ERR_CURSES))
		return i;
	else
		return display_list_stdout(NULL,ws,0, use_stdout);
# else
	return display_list_stdout(NULL,ws,0, use_stdout);
# endif
#endif
}
/********************************************************************
 *
 *                 stack_push
 *
 ********************************************************************/

char* stack_push(WcdStack ws, int push_ntimes)
{

	int  new_stack_nr;

	if(ws == NULL)
	return (NULL);
	else
		if( ((ws->size) == 0) || ((ws->size) > (size_t)ws->maxsize) )
		return (NULL);
		else
		{

			push_ntimes = push_ntimes % (int)ws->size;

			new_stack_nr = ws->current - push_ntimes;

			if(new_stack_nr < 0)
				new_stack_nr = (int)ws->size + new_stack_nr;

			ws->current = new_stack_nr;

			return(ws->dir[ws->current]);
		}
}
/********************************************************************
 *
 *                 stack_pop
 *
 *
 *
 ********************************************************************/

char* stack_pop(WcdStack ws, int pop_ntimes)
{

	int  new_stack_nr;


	if(ws == NULL)
	return (NULL);
	else
		if( ((ws->size) == 0) || ((ws->size) > (size_t)ws->maxsize) )
			return (NULL);
		else
		{
			pop_ntimes = pop_ntimes % (int)ws->size;

			new_stack_nr = ws->current + pop_ntimes;

			if(new_stack_nr > (int)(ws->size -1))
				new_stack_nr =  new_stack_nr - (int)ws->size;

			ws->current = new_stack_nr;
			return(ws->dir[ws->current]);
		}
}
/********************************************************************
 *
 *                 stack_write
 *
 ********************************************************************/

int stack_write(WcdStack ws,const char *stackfilename)
{
	if (ws->maxsize <= 0)
		return(0);
	else
	{
		/* create directory for stack file if it doesn't exist */
		create_dir_for_file(stackfilename);

	        FILE *outfile;
		if ( (outfile = wcd_fopen(stackfilename,"w",0)) == NULL)
		{
			return(0);
		}
		else
		{
			wcd_fprintf(outfile,"%d %d\n",ws->lastadded,ws->current);
			for(int i=0;((i<(int)ws->size)&&(i<ws->maxsize));i++)
			{
			/* printf("writing line %d\n",i);  */
				wcd_fprintf(outfile,"%s\n",ws->dir[i]);
			}
			wcd_fclose(outfile, stackfilename, "w", "stack_write: ");
		}
		return(0);
	}
}

