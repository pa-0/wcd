/*
Copyright (C) 1997-2006 Erwin Waterlander

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WCD_UTF8
#define __USE_XOPEN
#include <wchar.h>
#endif
#include "std_macr.h"
#include "structur.h"
#include "nameset.h"
#include "display.h"
#include "wcd.h"
#include "config.h"
#include "dosdir.h"


/*
 * int str_columns (char *s)
 *
 * Return number of colums a string takes. In case
 * of erroneous multi-byte sequence, return number of
 * 8 bit characters.
 */

int str_columns (char *s)
{
#ifdef WCD_UTF8
   static wchar_t wstr[DD_MAXPATH];
   int i;

   /* convert to wide characters. i = nr. of characters */
   i= mbstowcs(wstr,s,DD_MAXPATH);
   if ( i < 0)
      return(strlen(s));
   else
   {
      i = wcswidth(wstr,DD_MAXPATH);
      /* i =  nr. of columns */
      if ( i < 0)
         return(strlen(s));
      else
         return(i);
   }
#else
   return(strlen(s));
#endif
}

/************************************************************************
 *
 *  swap(), ssort() and sort_list()
 *
 *  Sort a nameset list alphabeticly
 *
 ************************************************************************/

void swap(nameset list, int i, int j)
{  text temp;

   temp = list->array[i];
   list->array[i] = list->array[j];
   list->array[j] = temp;
}

void ssort (nameset list, int left, int right)
{
 int i, last;
#ifdef WCD_UTF8
 static wchar_t wstr_left[DD_MAXPATH];
 static wchar_t wstr_right[DD_MAXPATH];
 int len1,len2;
#endif

  if (left >= right) return; /* fewer than 2 elements */

  swap(list, left, (left + right)/2);
  last = left;

  for (i = left+1; i <=right; i++)
  {
#ifdef WCD_UTF8
   len1 = mbstowcs(wstr_left, list->array[left],DD_MAXPATH);
   len2 = mbstowcs(wstr_right,list->array[i],DD_MAXPATH);
   if ((len1<0)||(len2<0))
   {
      /* Erroneous UTF-8 sequence */
      /* Try 8 bit characters */
#  ifdef ENABLE_NLS
      if  (strcoll(list->array[i],list->array[left])<0)
#  else
      if  (strcmp(list->array[i],list->array[left])<0)
#  endif
         swap(list, ++last, i);
   } else {
      if  (wcscoll(wstr_right,wstr_left)<0)
         swap(list, ++last, i);
   }
#else
#  ifdef ENABLE_NLS
  if  (strcoll(list->array[i],list->array[left])<0)
#  else
  if  (strcmp(list->array[i],list->array[left])<0)
#  endif
     swap(list, ++last, i);
#endif
  }

  swap(list, left, last);
  ssort(list, left, last-1);
  ssort(list, last+1, right);
}

void sort_list(nameset list)
{
 ssort(list,0,(list->size)-1);
}

/************************************************************************
 *
 *  maxLength()
 *  Get the longest string in a nameset list.
 *
 ************************************************************************/

#if (defined(WCD_USECONIO) || defined(WCD_USECURSES))
int maxLength(nameset list)
{
   int i,len,maxlen = 0;

   if (list == NULL)
   {
      fprintf(stderr,_("Wcd: error in maxLength(), list == NULL\n"));
      return 32 ;
   }

   for (i=0;i<list->size;i++)
   {
    if( (len=str_columns(list->array[i])) > maxlen)
       maxlen=len;
   }
   if (maxlen > 32)
      return(maxlen);
   else
      return 32 ;     /* minimal width for help screen */
}
int maxLengthStack(WcdStack s)
{
   int i,len,maxlen = 0;

   if (s == NULL)
   {
      fprintf(stderr,_("Wcd: error in maxLengthStack(), s == NULL\n"));
      return 32 ;
   }

   for (i=0;i<s->size;i++)
   {
    if( (len=str_columns(s->dir[i])) > maxlen)
       maxlen=len;
   }
   if (maxlen > 32)
      return(maxlen);
   else
      return 32 ;     /* minimal width for help screen */
}
#endif

/************************************************************************
 *
 *  display_list(nameset list)
 *
 *  Display a match list on screen.
 *
 *  There are three versions of the function:
 *
 *  1) stdout version
 *     This version will work on any platform. Default for Unix and Windows.
 *     Has to make use of the scroll-back capability of the terminal.
 *
 *  2) CONIO version
 *     Default used for DOS, optional for Windows NT.
 *     Scroll back is programmed in.
 *
 *  3) CURSES version
 *     Optional for Unix, DOS or Windows.
 *     Scroll back is programmed in.
 *
 ************************************************************************/

#ifdef WCD_USECONIO

void printLine(nameset n, int i, int y, int xoffset, int *use_numbers, int screenWidth)
{
   wcd_char *s;
   int len, j, nr_offset;

   s = n->array[i];

   if (s != NULL)
   {
      len = strlen(s);
      if (*use_numbers == 0)
         nr_offset = 2;
      else
         nr_offset = 3;

      for(j=xoffset;(j<len)&&((nr_offset+j-xoffset)<(screenWidth-1));j++)
      {
         cprintf("%c",s[j]);
      }
   }
}

void printStackLine(WcdStack ws, int i, int y, int xoffset, int *use_numbers, int screenWidth)
{
   wcd_char *s;
   int len, j, nr_offset;

   s = ws->dir[i];

   if (s != NULL)
   {
      len = strlen(s);
      if (*use_numbers == 0)
         nr_offset = 2;
      else
         nr_offset = 3;

      for(j=xoffset;(j<len)&&((nr_offset+j-xoffset)<(screenWidth-1));j++)
      {
         cprintf("%c",s[j]);
      }
      if ((i == ws->current) && ((nr_offset+j-xoffset+2)<(screenWidth-1)))
           cprintf(" *");
   }
}

void print_list_normal(int lines_per_page, int line, nameset list, int top, int bottom, int use_numbers, int xoffset, int screenWidth)
{
   int i;

   for (i=top;i<=bottom;i++)
   {
      gotoxy(1,line);
      if (use_numbers == 0)
         cprintf("%c ",(char)(((i-top)%lines_per_page) + 'a'));
      else
         cprintf("%2d ",((i-top)%lines_per_page) + 1);
      printLine(list, i, line, xoffset, &use_numbers, screenWidth);
      line++;
   }
}
/**************************************************/

void print_list_stack(int lines_per_page, int line, WcdStack ws, int start, int top, int bottom, int use_numbers, int xoffset, int screenWidth) 
{
   int i,j;

   if (use_numbers == 0)
   {
      for (i=top;i<=bottom;i++)
      {
        j = (i + start)%(ws->size);
        gotoxy(1,line);
        cprintf("%c ",(char)(((i-top)%lines_per_page) + 'a'));
        printStackLine(ws, j, line, xoffset, &use_numbers, screenWidth);
        line++;
      }
   }
   else
   {
      for (i=top;i<=bottom;i++)
      {
        j = (i + start)%(ws->size);
        gotoxy(1,line);
        cprintf("%2d ",(i-top)%lines_per_page + 1);
        printStackLine(ws, j, line, xoffset, &use_numbers, screenWidth);
        line++;
      }
   }
}

void print_list(int lines_per_page,int line, nameset list, WcdStack ws, int start, int top, int bottom, int use_numbers, int xoffset, int screenWidth) 
{
   clrscr();
   if (list != NULL)
      print_list_normal(lines_per_page,line,list,top,bottom,use_numbers,xoffset,screenWidth);
   else
      if (ws != NULL)
         print_list_stack(lines_per_page,line,ws,start,top,bottom,use_numbers,xoffset,screenWidth);
}

/****************************************************************************/

/* upper left corner is (1,1) */
int display_list_conio(nameset list,WcdStack ws, int perfect,int use_numbers)
{
int i, n=0, line, top, bottom, c=0, extended, gtxt=0;
int start=0, size;
int scrollWinHeight, page, len, shift=0 ;
int lines_per_page ; /* number of lines to print per page */
char number_str[WCD_MAX_INPSTR];
char *buffer;
struct text_info ti;

   gettextinfo(&ti);

   buffer = (char *) malloc(ti.screenwidth * ti.screenheight * 2);

   if (buffer!=NULL)   /* get total screen */
     gtxt = gettext(1,1,ti.screenwidth,ti.screenheight,buffer);

   if (list != NULL)    /* normal list */
   {
      sort_list(list);
      size = list->size;
   }
   else
      if (ws != NULL)   /* stack */
      {
         if( ((ws->size) <= 0) || ((ws->size) > ws->maxsize) || ((ws->size) > ws->maxsize) )
            return(WCD_ERR_LIST); /* in case stack file was corrupt */
         else
         {
            size = ws->size;

            if (ws->size < ws->maxsize)
               start = 0;
            else
               start = ws->lastadded + 1;

            if (ws->lastadded >= ws->maxsize)
               start = 0;
         }
      }
      else
         return(WCD_ERR_LIST);  /* no list or stack */

   i= WCD_ERR_LIST;
   number_str[n] = '\0';

   scrollWinHeight = ti.screenheight - INPUT_WIN_HEIGHT;
   lines_per_page = scrollWinHeight ;

   if (use_numbers == 0)
   {
      if (scrollWinHeight > SCROLL_WIN_HEIGHT)
         lines_per_page = SCROLL_WIN_HEIGHT;
   }
   else
   {
      if (scrollWinHeight > 99)
         lines_per_page = 99; /* stay below 3 digits */
   }

   if (list != NULL)
      len = maxLength(list);
   else
      if (ws != NULL)
         len = maxLengthStack(ws);
      else
         return(WCD_ERR_LIST);

   top = 0;

   bottom = size -1;
   top = size - lines_per_page;
   if (top < 0)
      top = 0;

   if (bottom < (lines_per_page -1) )
      line = scrollWinHeight - bottom; /* screen line nr. where to start printing */
   else
      line = scrollWinHeight - lines_per_page + 1;

   window(1,1,ti.screenwidth,scrollWinHeight);

   print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);

   page = bottom / lines_per_page + 1 ;

   window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);

   clrscr();

   cprintf("\n\r");
   if (list != NULL)
   {
      if(perfect)
         cprintf("Perfect ");
      else
         cprintf("Wild ");
      cprintf("match for %d directories.\n\r",size);
   }
   else
      cprintf("\n\r");

   cprintf("Please choose one (<Enter> to abort): ");
   fflush(stdout);
   gotoxy (PAGEOFFSET, 2);
   cprintf("w=up x=down ?=help  Page %d/%d      ",page,(size -1)/lines_per_page +1);
   gotoxy (OFFSET + n, 3);

   while ((c != 13 )&&(( c < 'a' ) || ( c > ('a'+scrollWinHeight-1) || ( c > 'v' ) )))
   {

     c = getch();

     switch(c)
     {
     case 'x':
     case 'w':
     case 'z':
     case ',': /* 1 left */
     case '.': /* 1 right */
     case 1  : /* Ctrl-A, Home */
     case 5  : /* Ctrl-E, End */
     case '?': /* Help */
     case 0:   /* extended key */

      if(c==0)
        extended = getch();
      else
      {
       switch (c)
       {
          case 'w': extended = 73;  /* Page Up */
             break;
          case 'x': extended = 81;  /* Page Down */
             break;
          case 'z': extended = 81;  /* Page Down */
             break;
          case ',': extended = 75;  /* Key Left */
             break;
          case '.': extended = 77;  /* Key Right */
             break;
          case 1  : extended = 71;  /* Home */
             break;
          case 5  : extended = 79;  /* End */
             break;
          case '?': extended = 59;  /* F1, Help */
             break;
          default : extended = 0;
             break;
       }
      }


      if ((extended == 73) || /* Page Up */
         (extended == 72))   /* Arrow Up */
      {
        window(1,1,ti.screenwidth,scrollWinHeight);

        if(bottom > (lines_per_page -1))
        {
         bottom = bottom - lines_per_page ;
         top = top - lines_per_page ;
        }

        if (top<0) top = 0;

        if (bottom < (lines_per_page -1) )
           line = scrollWinHeight - bottom;
        else
           line = scrollWinHeight - lines_per_page + 1;
        
        print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);

         page = bottom / lines_per_page + 1 ;

         window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
         gotoxy (PAGEOFFSET, 2);
         cprintf("w=up x=down ?=help  Page %d/%d      ",page,(size -1)/lines_per_page +1);
         gotoxy (OFFSET + n, 3);


      } /* Page Up */

      if ((extended == 81) || /* Page down */
         (extended == 80))   /* Arrow down */
      {
        window(1,1,ti.screenwidth,scrollWinHeight);

        if(bottom < (size - 1))
        {
         bottom = bottom + lines_per_page ;
         top = bottom - lines_per_page + 1;
        }

        if (bottom < (lines_per_page -1) )
           line = scrollWinHeight - bottom;
        else
           line = scrollWinHeight - lines_per_page + 1;

        print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);

         page = bottom / lines_per_page + 1 ;

         window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
         gotoxy (PAGEOFFSET, 2);
         cprintf("w=up x=down ?=help  Page %d/%d      ",page,(size -1)/lines_per_page +1);
         gotoxy (OFFSET + n, 3);
      }/* Page down */

      if (extended == 75) /* Key Left */
      {
          if (shift > 0)
             shift--;
          window(1,1,ti.screenwidth,scrollWinHeight);
          print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);
      }
      if (extended == 77) /* Key Right */
      {
          if (shift < len)
            shift++;
          window(1,1,ti.screenwidth,scrollWinHeight);
          print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);
      }
      if (extended == 71) /* Home */
      {
          shift = 0;
          window(1,1,ti.screenwidth,scrollWinHeight);
          print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);
      }
      if (extended == 79) /* End */
      {
          shift = len - ti.screenwidth/2;
          if (shift < 0)
             shift=0;
          window(1,1,ti.screenwidth,scrollWinHeight);
          print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);
      }
      if (extended == 59) /* Help */
      {
          window(1,1,ti.screenwidth,scrollWinHeight);
          clrscr();
          if (scrollWinHeight < 17)
          {
             gotoxy(1,1);
             cprintf("Screenheight must be > 20 for help.");
          }
          else
          {
             gotoxy(1,1);
             cprintf("w or <Up>         Page Up.\r\n");
             cprintf("x or z or <Down>  Page Down.\r\n");
             cprintf(", or <Left>       Scroll 1 left.\r\n");
             cprintf(". or <Right>      Scroll 1 right.\r\n");
             cprintf("< or [            Scroll 10 left.\r\n");
             cprintf("> or ]            Scroll 10 right.\r\n");
             cprintf("CTRL-a or <HOME>  Scroll to beginning.\r\n");
             cprintf("CTRL-e or <END>   Scroll to end.\r\n");
             cprintf("CTRL-c or <Esc>   Abort.\r\n");
             cprintf("<Enter>           Abort.\r\n");
             cprintf("Type w or x to quit help.\r\n");
          }
      }

   window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
   gotoxy (OFFSET + n, 3);
     break;
     case '<':
     case '[':
        shift -=10;
        if (shift < 0)
           shift=0;
        window(1,1,ti.screenwidth,scrollWinHeight);
        print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);
        window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
        gotoxy (OFFSET + n, 3);
       break;
     case ']':
     case '>':
        shift +=10;
        if (shift > len)
           shift=len;
        window(1,1,ti.screenwidth,scrollWinHeight);
        print_list(lines_per_page,line,list, ws, start, top, bottom, use_numbers,shift,ti.screenwidth);
        window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
        gotoxy (OFFSET + n, 3);
        break;
     case 8:  /* backspace */
      window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
      if(n>0) n--;
      number_str[n] = '\0';
      gotoxy(OFFSET + n, 3);
      cprintf(" ");
      gotoxy(OFFSET + n, 3);
      break;
     case 3:  /* Control-C */
     case 27: /* Escape */
      c = 13;
      i = WCD_ERR_LIST;
      number_str[0] = '\0';
      break;
     case 13: /* Enter */
      c = 13;
      i = WCD_ERR_LIST;
      break;
     default:
      if (( c >= '0') && ( c <= '9') && (n < WCD_MAX_INPSTR)) /* numbers */
      {
         number_str[n] = (char)c;
         window (1,scrollWinHeight+1,ti.screenwidth,ti.screenheight);
         gotoxy (OFFSET + n++, 3);
         cprintf("%c",(char)c);
         number_str[n] = '\0';
      
         /* Notice that one has to choose a number from 1 to max 22 */
      if (((bottom - top) < 9) /* displayed list is 9 or less matches */
          || (n == 2)           /* second number typed */
          || (c >= '3')         /* 3-9 is typed */
          || ((c == '2')&&((bottom - top) < 19)) /* displayed list is 19 or less matches */
         )
         c = 13;      /* do an <Enter> */
         
      }
      else
         i=c+top-'a'+1;
       break;
     }
   }
   window (1,1,ti.screenwidth,ti.screenheight);
   gotoxy (ti.curx, ti.cury);

   printf("\n");

   if (gtxt ==1)
     puttext(1,1,ti.screenwidth,ti.screenheight,buffer);

   if (buffer!=NULL)
     free(buffer);
/*
   cprintf("window left      %2d\r\n",ti.winleft);
   cprintf("window top       %2d\r\n",ti.wintop);
   cprintf("window right     %2d\r\n",ti.winright);
   cprintf("window bottom    %2d\r\n",ti.winbottom);
   cprintf("attribute        %2d\r\n",ti.attribute);
   cprintf("normal attribute %2d\r\n",ti.normattr);
   cprintf("current mode     %2d\r\n",ti.currmode);
   cprintf("screen height    %2d\r\n",ti.screenheight);
   cprintf("screen width     %2d\r\n",ti.screenwidth);

   cprintf("current x        %2d\r\n",ti.curx);
   cprintf("current y        %2d\r\n",ti.cury); */

   if (strcmp(number_str,"") != 0) /* a number was typed */
      i=atoi(number_str) + top; 

   if((ws != NULL)&&(list == NULL)) /* stack */
   {
      if (( i <=0)||(i > ws->size)) /* fail */
      {
        return(WCD_ERR_LIST);
       }
      else    /* succes */
      {
        i = ( i - 1 + start)%(ws->size);
        ws->current = i;
      }
   }
   return i;
}

#endif

#ifdef WCD_USECURSES

void wcd_mvwaddstr(WINDOW *win, int x, int y, char *str)
{
#ifdef WCD_UTF8
   static wchar_t wstr[DD_MAXPATH];
   int i;

   /* convert to wide characters. i = nr. of characters */
   i= mbstowcs(wstr,str,DD_MAXPATH);
   if ( i < 0)
   {
      /* Erroneous UTF-8 sequence */
      /* Try 8 bit characters */
      mvwaddstr(win, x, y, str);
   } else {
      mvwaddwstr(win, x, y, wstr);
   }
#else
   mvwaddstr(win, x, y, str);
#endif
}

void printLine(WINDOW *win, nameset n, int i, int y, int xoffset, int *use_numbers)
{
   wcd_char *s;
   int len, j, nr_offset;
#ifdef WCD_UTF8
   static wchar_t wstr[DD_MAXPATH];
   int width, c;
#endif

   s = (wcd_char *)n->array[i];

   if (s != NULL)
   {
#ifdef WCD_UTF8
      len = mbstowcs(wstr,(char *)s,DD_MAXPATH); /* number of wide characters */
#else
      len = strlen((char *)s);
#endif
      if (*use_numbers == 0)
         nr_offset = 2;
      else
         nr_offset = 3;

      wmove(win,y,nr_offset);

#ifdef WCD_UTF8
      if (len<0)
      {
         /* Erroneous UTF-8 sequence */
         /* Try 8 bit characters */
         len = strlen((char *)s);
         for(j=xoffset;(j<len)&&((nr_offset+j-xoffset)<(COLS-1));j++)
         {
            waddch(win,s[j]);
         }
      } else {
         c = 0; /* count characters with width > 0 from beginning of string. */
         j = 0;
         while ((j<len)&&(c<xoffset))
         {
            if (wcwidth(wstr[j]) != 0 )
               c++;
            j++; /* j advances over combining characters */
         }
         while ((j<len)&&(wcwidth(wstr[j]) == 0 ))  /* Skip combining characters */
           j++;
         width = wcwidth(wstr[j]);
         while ((j<len)&&((nr_offset+width)<(COLS-1)))
         {
            waddnwstr(win,wstr+j,1);
            j++;
            width = width + wcwidth(wstr[j]);
         }
      }
#else
      for(j=xoffset;(j<len)&&((nr_offset+j-xoffset)<(COLS-1));j++)
      {
         waddch(win,s[j]);
      }
#endif
   }
}

void printStackLine(WINDOW *win, WcdStack ws, int i, int y, int xoffset, int *use_numbers)
{
   wcd_char *s;
   int len, j, nr_offset;
#ifdef WCD_UTF8
   static wchar_t wstr[DD_MAXPATH];
   int width, c;
#endif

   s = (wcd_char *)ws->dir[i];

   if (s != NULL)
   {
#ifdef WCD_UTF8
      len = mbstowcs(wstr,(char *)s,DD_MAXPATH); /* number of wide characters */
#else
      len = strlen((char *)s);
#endif
      if (*use_numbers == 0)
         nr_offset = 2;
      else
         nr_offset = 3;

      wmove(win,y,nr_offset);

#ifdef WCD_UTF8
      if (len<0)
      {
         /* Erroneous UTF-8 sequence */
         /* Try 8 bit characters */
         len = strlen((char *)s);
         for(j=xoffset;(j<len)&&((nr_offset+j-xoffset)<(COLS-1));j++)
         {
            waddch(win,s[j]);
         }
         if ((i == ws->current) && ((nr_offset+j-xoffset+2)<(COLS-1)))
            wprintw(win," *");
      } else {
         c = 0; /* count characters with width > 0 from beginning of string. */
         j = 0;
         while ((j<len)&&(c<xoffset))
         {
            if (wcwidth(wstr[j]) != 0 )
               c++;
            j++; /* j advances over combining characters */
         }
         while ((j<len)&&(wcwidth(wstr[j]) == 0 ))  /* Skip combining characters */
           j++;
         width = wcwidth(wstr[j]);
         while ((j<len)&&((nr_offset+width)<(COLS-1)))
         {
            waddnwstr(win,wstr+j,1);
            j++;
            width = width + wcwidth(wstr[j]);
         }
         if ((i == ws->current) && ((nr_offset+width-wcwidth(wstr[j])+2)<(COLS-1)))
            wprintw(win," *");
      }
#else
      for(j=xoffset;(j<len)&&((nr_offset+j-xoffset)<(COLS-1));j++)
      {
         waddch(win,s[j]);
      }
      if ((i == ws->current) && ((nr_offset+j-xoffset+2)<(COLS-1)))
         wprintw(win," *");
#endif
   }
}
/**************************************************/

void print_list_normal(WINDOW *scrollWin, int lines_per_page,int line, nameset list, int top, int bottom, int use_numbers, int xoffset) 
{
   int i;

   for (i=top;i<=bottom;i++)
   {
      if (use_numbers == 0)
         mvwprintw(scrollWin,line,0,"%c ",(char)(((i-top)%lines_per_page) + 'a'));
      else
         mvwprintw(scrollWin,line,0,"%2d ",((i-top)%lines_per_page) + 1);
      printLine(scrollWin, list, i, line, xoffset, &use_numbers);
      line++;
   }
}

/**************************************************/

void print_list_stack(WINDOW *scrollWin, int lines_per_page,int line, WcdStack ws, int start, int top, int bottom, int use_numbers, int xoffset) 
{
   int i,j;

   if (use_numbers == 0)
   {
      for (i=top;i<=bottom;i++)
      {
        j = (i + start)%(ws->size);
        mvwprintw(scrollWin,line,0,"%c ",(char)(((i-top)%lines_per_page) + 'a'));
        printStackLine(scrollWin, ws, j, line, xoffset, &use_numbers);
        line++;
      }
   }
   else
   {
      for (i=top;i<=bottom;i++)
      {
        j = (i + start)%(ws->size);
        /* mvwprintw(scrollWin,line,0,"%d  %s",i + 1,ws->dir[j]); */
        mvwprintw(scrollWin,line,0,"%2d ",(i-top)%lines_per_page + 1);
        printStackLine(scrollWin, ws, j, line, xoffset, &use_numbers);
        line++;
      }
   }
}

void print_list(WINDOW *scrollWin, int lines_per_page,int line, nameset list, WcdStack ws, int start, int top, int bottom, int use_numbers, int xoffset) 
{
   wclear(scrollWin);
   if (list != NULL)
      print_list_normal(scrollWin,lines_per_page,line,list,top,bottom,use_numbers,xoffset);
   else
      if (ws != NULL)
         print_list_stack(scrollWin,lines_per_page,line,ws,start,top,bottom,use_numbers,xoffset);
}

/* ****************************************************************** */
/* upper left corner is (0,0) */
/* ****************************************************************** */

#define Key_CTRL(x)      ((x) & 31)

int display_list_curses(nameset list, WcdStack ws, int perfect,int use_numbers)
{
  int i, n=0, line, top, bottom, c=0;
  int start=0, size,displayed_list;
  int scrollWinHeight, page, len, shift=0, err ;
  int inputWinLen ;
  int lines_per_page ; /* number of lines to print per page */
  char number_str[WCD_MAX_INPSTR];
  char buf[WCD_MAX_INPSTR];
  int offset, pageoffset;
  WINDOW *scrollWin, *inputWin ;
#ifndef __PDCURSES__
  SCREEN *sp;
#endif

/* Notice that list->size > 1 when this function is called. */

   if (list != NULL)    /* normal list */
   {
      sort_list(list);
      size = list->size;
   }
   else
      if (ws != NULL)   /* stack */
      {
         if( ((ws->size) <= 0) || ((ws->size) > ws->maxsize) || ((ws->size) > ws->maxsize) )
            return(WCD_ERR_LIST); /* in case stack file was corrupt */
         else
         {
            size = ws->size;

            if (ws->size < ws->maxsize)
               start = 0;
            else
               start = ws->lastadded + 1;

            if (ws->lastadded >= ws->maxsize)
               start = 0;
         }
      }
      else
         return(WCD_ERR_LIST);  /* no list or stack */

   i= WCD_ERR_LIST;
   number_str[n] = '\0';

#ifdef __PDCURSES__
   initscr();
#else
   sp = newterm(NULL,stdout,stdin);
   if (sp == NULL)
   {  
      fprintf(stderr,_("Wcd: warning: Error opening terminal, falling back to stdout interface.\n"));
      return WCD_ERR_CURSES;
   }
#endif

   keypad(stdscr, TRUE);
   intrflush(stdscr, FALSE);
   cbreak();
   noecho();
   nonl();
   scrollok(stdscr, TRUE); /* enable scrolling */

   if (LINES < 4)
   {
      endwin();
#ifdef XCURSES
      XCursesExit();
#endif
      fprintf(stderr,_("Wcd: error: screen height must be larger than 3 lines.\n"));
      return WCD_ERR_CURSES;
   }

   scrollWinHeight = LINES - INPUT_WIN_HEIGHT;
   lines_per_page = scrollWinHeight;
   if (use_numbers == 0)
   {
      if (scrollWinHeight > SCROLL_WIN_HEIGHT)
         lines_per_page = SCROLL_WIN_HEIGHT;
   }
   else
   {
      if (scrollWinHeight > 99)
         lines_per_page = 99; /* stay below 3 digits */
   }

   if (list != NULL)
      len = maxLength(list);
   else
      if (ws != NULL)
         len = maxLengthStack(ws);
      else
         return(WCD_ERR_LIST);
         
   refresh();

   scrollWin = newpad(scrollWinHeight,COLS);
   if (scrollWin == NULL)
   {
      endwin();
#ifdef XCURSES
      XCursesExit();
#endif
      fprintf(stderr,_("Wcd: error creating scroll window.\n"));
      return WCD_ERR_CURSES;
   }

   scrollok(scrollWin, TRUE);

   bottom = size -1; /* lowest match to print */
   top = size - lines_per_page; /* top match to print */
   if (top < 0)
      top = 0;

   if (bottom < (lines_per_page -1) )
      line = scrollWinHeight - bottom - 1; /* screen line nr. where to start printing */
   else
      line = scrollWinHeight - lines_per_page;

   print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);


   if (COLS < INPUT_WIN_LEN)
      inputWinLen = INPUT_WIN_LEN;
   else 
      inputWinLen = COLS;

   inputWin = newpad(INPUT_WIN_HEIGHT,inputWinLen);
   if (inputWin == NULL)
   {
      endwin();
#ifdef XCURSES
      XCursesExit();
#endif
      fprintf(stderr,_("Wcd: error creating input window.\n"));
      return WCD_ERR_CURSES;
   }

   wclear(inputWin);
   scrollok(inputWin, TRUE);

   if (list != NULL)
   {
      if(perfect)
        wcd_mvwaddstr(inputWin,1,0,_("Perfect "));
      else
        wcd_mvwaddstr(inputWin,1,0,_("Wild "));
      wprintw(inputWin,_("match for %d directories."),size);
   }

   page = bottom / lines_per_page + 1 ;

   sprintf(buf,_(" w=up x=down ?=help  Page %d/%d "),page,(size -1)/lines_per_page +1);
   pageoffset = COLS - str_columns(buf);
   if (pageoffset < 0)
      pageoffset = 0;
   wmove (inputWin, 0, pageoffset);
   wprintw(inputWin,"%s",buf);

   sprintf(buf,_("Please choose one (<Enter> to abort): "));
   wcd_mvwaddstr(inputWin,2,0,buf);
   offset = str_columns(buf) ;
   wmove (inputWin, 2, offset);
   err = prefresh(scrollWin,0,0,0,0,scrollWinHeight-1,COLS-1);
   err = prefresh(inputWin,0,0,scrollWinHeight,0,scrollWinHeight+INPUT_WIN_HEIGHT-1,COLS-1);

   while ((c != 13 )&&(( c < 'a' ) || ( c > ('a'+scrollWinHeight-1) || ( c > 'v' ) )))
   {

     c = getch();

     switch(c)
     {
     case 'w':
     case KEY_UP:  /* Arrow Up */
     case KEY_PPAGE: /* Page Up */

        if(bottom > (lines_per_page -1))
        {
         bottom = bottom - lines_per_page ;
         top = top - lines_per_page ;
        }

        if (top<0) top = 0;

        if (bottom < (lines_per_page -1) )
           line = scrollWinHeight - bottom - 1;
        else
           line = scrollWinHeight - lines_per_page;

         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);

         page = bottom / lines_per_page + 1 ;

         sprintf(buf,_(" w=up x=down ?=help  Page %d/%d "),page,(size -1)/lines_per_page +1);
         pageoffset = COLS - str_columns(buf);
         if (pageoffset < 0)
            pageoffset = 0;
         wmove (inputWin, 0, pageoffset);
         wprintw(inputWin,"%s",buf);
         wmove (inputWin, 2, offset + n);

         break;

      case 'x':
      case 'z':
      case KEY_DOWN: /* Arrow down */
      case KEY_NPAGE: /* Page down */

        if(bottom < (size - 1))
        {
         bottom = bottom + lines_per_page ;
         top = bottom - lines_per_page + 1;
        }

        if (bottom < (lines_per_page -1) )
           line = scrollWinHeight - bottom - 1;
        else
           line = scrollWinHeight - lines_per_page;

         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);

         page = bottom / lines_per_page + 1 ;

         sprintf(buf,_(" w=up x=down ?=help  Page %d/%d "),page,(size -1)/lines_per_page +1);
         pageoffset = COLS - str_columns(buf);
         if (pageoffset < 0)
            pageoffset = 0;
         wmove (inputWin, 0, pageoffset);
         wprintw(inputWin,"%s",buf);
         wmove (inputWin, 2, offset + n);
     break;

      case ',':
      case KEY_LEFT:
          if (shift > 0)
             shift--;
         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);
         break;
      case '.':
      case KEY_RIGHT:
         if (shift < len)
            shift++;
         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);
         break;
       case '<':
       case '[':
          shift -=10;
          if (shift < 0)
             shift=0;
         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);
         break;
      case ']':
      case '>':
         shift +=10;
         if (shift > len)
            shift=len;
         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);
         break;
      case Key_CTRL ('a'):
      case KEY_HOME:
         shift = 0;
         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);
         break;
      case Key_CTRL ('e'):
#ifdef KEY_END
      case KEY_END:
#endif
         shift = len - COLS/2;
          if (shift < 0)
             shift=0;
         print_list(scrollWin,lines_per_page,line,list,ws,start,top,bottom,use_numbers,shift);
         break;
      case KEY_F (1):
      case '?':

         wclear(scrollWin);
         if (scrollWinHeight < 17)
            wcd_mvwaddstr(scrollWin,0,0,_("Screenheight must be > 20 for help."));
         else
         {
            wcd_mvwaddstr(scrollWin, 0,0,_("w or <Up>         Page Up."));
            wcd_mvwaddstr(scrollWin, 1,0,_("x or z or <Down>  Page Down."));
            wcd_mvwaddstr(scrollWin, 2,0,_(", or <Left>       Scroll 1 left."));
            wcd_mvwaddstr(scrollWin, 3,0,_(". or <Right>      Scroll 1 right."));
            wcd_mvwaddstr(scrollWin, 4,0,_("< or [            Scroll 10 left."));
            wcd_mvwaddstr(scrollWin, 5,0,_("> or ]            Scroll 10 right."));
            wcd_mvwaddstr(scrollWin, 6,0,_("CTRL-a or <HOME>  Scroll to beginning."));
            wcd_mvwaddstr(scrollWin, 7,0,_("CTRL-e or <END>   Scroll to end."));
            wcd_mvwaddstr(scrollWin, 8,0,_("CTRL-c or <Esc>   Abort."));
            wcd_mvwaddstr(scrollWin, 9,0,_("<Enter>           Abort."));
            wcd_mvwaddstr(scrollWin,11,0,_("Type w or x to quit help."));
         }
         break;
     case 3:  /* Control-C */
     case 27: /* Escape */
      c = 13;
      i = WCD_ERR_LIST;
      number_str[0] = '\0';
      break;
     case 13: /* Enter */
     case KEY_ENTER:
      c = 13;
      i = WCD_ERR_LIST;
      break;
     case 8:  /* backspace */
     case KEY_BACKSPACE:
     case 127: /* delete */
            if(n>0) n--;
            number_str[n] = '\0';
            wmove (inputWin, 2, offset + n);
            wprintw(inputWin," ");
            wmove (inputWin, 2, offset + n);
     break;
     default:
         if (( c >= '0') && ( c <= '9') && (n < WCD_MAX_INPSTR)) /* numbers */
         {
            number_str[n] = (char)c;
            wmove (inputWin, 2, offset + n++);
            number_str[n] = '\0';
            wprintw(inputWin,"%c",(char)c);
         
            displayed_list = bottom - top;
            /* Notice that one has to choose a number from 1 to max 99 */
         if ((displayed_list < 9) /* displayed list is 9 or less matches */
             || (n == 2)           /* second number typed */
             || ((c == '2')&&( displayed_list < 19)) /* displayed list is 19 or less matches */
             || ((c == '3')&&( displayed_list < 29)) /* displayed list is 29 or less matches */
             || ((c == '4')&&( displayed_list < 39)) /* displayed list is 39 or less matches */
             || ((c == '5')&&( displayed_list < 49)) /* displayed list is 49 or less matches */
             || ((c == '6')&&( displayed_list < 59)) /* displayed list is 59 or less matches */
             || ((c == '7')&&( displayed_list < 69)) /* displayed list is 69 or less matches */
             || ((c == '8')&&( displayed_list < 79)) /* displayed list is 79 or less matches */
             || ((c == '9')&&( displayed_list < 89)) /* displayed list is 89 or less matches */
            )
            c = 13;      /* do an <Enter> */
            
         }
         else
            i=c+top-'a'+1;

     break;
     }
     err = prefresh(scrollWin,0,0,0,0,scrollWinHeight-1,COLS-1);
     err = prefresh(inputWin,0,0,scrollWinHeight,0,scrollWinHeight+INPUT_WIN_HEIGHT-1,COLS-1);
   }

   endwin();
#ifdef XCURSES
   XCursesExit();
#endif
   if (strcmp(number_str,"") != 0) /* a number was typed */
      i=atoi(number_str) + top; 

   printf("\n"); /* Extra newline for curses, pdcurses and when ncurses doesn't restore screen */

   if ((ws != NULL)&&(list == NULL)) /* stack */
   {
      if (( i <=0)||(i > ws->size)) /* fail */
      {
        return(WCD_ERR_LIST);
       }
      else    /* succes */
      {
        i = ( i - 1 + start)%(ws->size);
        ws->current = i;
      }
   }
   return i;
}

#endif


 /* stdout version */
int display_list_stdout(nameset list,WcdStack ws, int perfect, int use_stdout)
{
   int i;
   int k, start, j;

   if (list != NULL) /* normal list */
   {
      sort_list(list);

      if ( use_stdout & WCD_STDOUT_DUMP )
      {
         for (i=0;i<list->size;i++)
            printf("%s\n", list->array[i]);
      }
      else
      {
         for (i=0;i<list->size;i++)
            printf("%d  %s\n",i+1,list->array[i]);
      }

      if ( use_stdout & WCD_STDOUT_DUMP )
         return(WCD_ERR_LIST);
      else
      {
         if(perfect)
           printf(_("\nPerfect "));
         else
           printf(_("\nWild "));
         printf(_("match for %d directories.\n"),list->size);
         printf(_("Please choose one (<Enter> to abort): "));
      }

      return wcd_get_int();
   }
   else
      if (ws != NULL)  /* stack */
      {
   /* printWcdStack("XXX ", ws, stdout); */
      if(ws->maxsize <= 0)
         return (WCD_ERR_LIST);
      else
         if( ((ws->size) <= 0) || ((ws->size) > ws->maxsize) )
         return (WCD_ERR_LIST);
         else
         {

            if (ws->size < ws->maxsize)
                start = 0;
             else
                 start = ws->lastadded + 1;

             if (ws->lastadded >= ws->maxsize)
                 start = 0;

            k=1;
            for(i=0; i < (ws->size) ; i++)
            {
               j = (i + start)%(ws->size);
               if ( !(use_stdout & WCD_STDOUT_DUMP) )
                  printf("%2d ",k);
               k++;

               printf("%s",ws->dir[j]);
               if (j == ws->current)
                  printf(" *");
               printf("\n");
            }

            if ( use_stdout & WCD_STDOUT_DUMP )
              return(WCD_ERR_LIST);

            printf(_("\nPlease choose one (<Enter> to abort): "));
            i = wcd_get_int();

            if (( i <=0)||(i > ws->size)) /* fail */
            {
              return(WCD_ERR_LIST);
             }
            else    /* succes */
            {
              i = ( i - 1 + start)%(ws->size);
              ws->current = i;
              return(i);
            }
         }
      }
      else
         return WCD_ERR_LIST;
}


int display_list(nameset list,int perfect, int use_numbers, int use_stdout)
{
#ifdef WCD_USECONIO
   if (use_stdout == WCD_STDOUT_NO)
      return display_list_conio(list,NULL,perfect,use_numbers);
   else
      return display_list_stdout(list,NULL,perfect, use_stdout);
#else
# ifdef WCD_USECURSES
   int i;
   if ((use_stdout == WCD_STDOUT_NO) && ((i = display_list_curses(list,NULL,perfect,use_numbers)) != WCD_ERR_CURSES))
      return i;
   else
      return display_list_stdout(list,NULL,perfect, use_stdout);
# else
   return display_list_stdout(list,NULL,perfect, use_stdout);
# endif
#endif
}
