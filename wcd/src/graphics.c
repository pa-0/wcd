/*
Copyright (C) 2000-2008 Erwin Waterlander

Ideas and source code of NCD (Ninux Czo Directory) have been
used in the WCD graphical interface.

Ninux Change Directory
Copyright (C) 1995 Borja Etxebarria
<borja@bips.bi.ehu.es> or <jtbecgob@s835cc.bi.ehu.es>
http://www.ibiblio.org/pub/Linux/utils/shell/ncd-0.9.8.tgz

Additions on NCD were made by Olivier Sirol (Czo)

Ninux Czo Directory
Copyright (C) 1995 Borja Etxebarria
Copyright (C) 1996 Olivier Sirol
Olivier Sirol <sirol@ecoledoc.ibp.fr>
http://www.ibiblio.org/pub/Linux/utils/file/managers/ncd1_205.tgz


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
#include <signal.h>
#include "std_macr.h"
#include "structur.h"
#include "nameset.h"
#include "Text.h"
#include "Error.h"
#include "dirnode.h"
#include "graphics.h"


#include "wcd.h"
#include "dosdir.h"
#include "display.h"
#include "wfixpath.h"
#include "match.h"
#include "config.h"
#include "colors.h"             /* add colors for the tree on MS platform */

# define WCD_SEL_ON 6           /* selection on/off codes */
# define WCD_SEL_OFF 7

#define WCD_ONESUBDIR  "---"
#define WCD_SPLITDIR   "-+-"
#define WCD_SUBDIR    "  |-"
#define WCD_MOREDIR   "  |  "
#define WCD_ENDDIR    "  `-"
#define WCD_OVERDIR   "     "

#define WCD_GRAPH_MAX_LINE_LENGTH  DD_MAXPATH * 2

#ifdef WCD_USECURSES

struct wcdwin /* structure with window information */
{
   WINDOW *scrollWin;
   int scrollWinHeight;
   int scrollWinLen;
   WINDOW *inputWin;
   dirnode curNode;
   char str[WCD_MAX_INPSTR];
   int mode;
   int graphics_mode;
   dirnode zoomStack;
} ;

struct wcdwin wcd_cwin; /*global variable wcd curses windows */


void dataRefresh(int ydiff, int init);

#if defined(SIGWINCH)
#  define CAN_RESIZE 1
#else
#  define CAN_RESIZE 0
#endif

#if CAN_RESIZE
/* The most portable way of resizing seems to be
 * to just rebuild the windows from scratch */
void ioResize()
{
   /* end curses mode */
   endwin();
   refresh();  /* start curses */

   wcd_cwin.scrollWinHeight = LINES - INPUT_WIN_HEIGHT;
   wcd_cwin.scrollWinLen = COLS;

   /* free resources */
   delwin(wcd_cwin.scrollWin);
   delwin(wcd_cwin.inputWin);

   /* create new windows */
   wcd_cwin.scrollWin = newpad(wcd_cwin.scrollWinHeight,COLS);
   wcd_cwin.inputWin  = newpad(INPUT_WIN_HEIGHT,COLS);

   if (wcd_cwin.graphics_mode & WCD_GRAPH_COLOR)
   {
      colorbox (wcd_cwin.scrollWin, MENU_COLOR, 0);     /* BillyC add colors */
      colorbox (wcd_cwin.inputWin,  BODY_COLOR, 0);     /* BillyC add colors */
   }
   scrollok(wcd_cwin.scrollWin, TRUE);

   refresh();
   dataRefresh(0, 1); 
}

void signalSigwinch (int sig)
{
  ioResize ();
  signal(SIGWINCH,signalSigwinch);
}
#endif

#endif /* WCD_USECURSES */

/*****************************************************************
 *
 * dirHasSubdirs()
 * Wrapper for API dirnodeHasSubdirs()
 * When directory is folded it looks like there are no subdirectories.
 *
 *************************************************************** */

c3po_bool dirHasSubdirs(dirnode d)
{
   if (dirnodeFold(d) eq true)
      return false;
   else
      return dirnodeHasSubdirs(d);
}



/* this function is called by other functions only on directories that are last
 * on a line */
char* getTreeLine(dirnode d, int y, int *y_orig, dirnode curNode, c3po_bool fold)
{
   static text line = NULL;
   static text tline = NULL;
   dirnode n;
   int i,len;

   if (d == NULL)
      return(NULL);

   len = strlen(dirnodeGetName(d));

   if (line == NULL)
   {
      line = textNewSize(WCD_GRAPH_MAX_LINE_LENGTH);
      line[0] = '\0';
   }
   if (tline == NULL)
   {
      tline = textNewSize(WCD_GRAPH_MAX_LINE_LENGTH);
      tline[0] = '\0';
   }

   /* buffer overflow prevention:
    *
    * The max length of `line' and `tline' is the length of `line' increased
    * by the namelength of the dirnode and at most 8 characters
    */
   if ((strlen(line) + len + 8) > WCD_GRAPH_MAX_LINE_LENGTH)
   {
      sprintf(line,_("Wcd: error: line too long in function getTreeLine()"));
      return(line);
   }

   if (dirHasSubdirs(d) eq true)
   {
      if(getSizeOfDirnode(d) == 1)
      {
         if (dirnodeGetY(d) == *y_orig)
         {
            strcpy(tline," ");
            if (d == curNode)
               tline[strlen (tline) - 1] = WCD_SEL_ON;
            
            strcat(tline,dirnodeGetName(d));
            
            strcat(tline," ");
            if (d == curNode)
               tline[strlen (tline) - 1] = WCD_SEL_OFF;
               
            strcat(tline,WCD_ONESUBDIR);

            n = elementAtDirnode(0,d);
            if (fold eq true)
               tline[strlen(tline)-1] = '+';

         }
         else
         {
            for (i=0;i<len;i++)
               tline[i] = ' ';
            tline[len] = '\0';
            strcat(tline,WCD_OVERDIR);
         }
      }
      else
      {
         if (y == dirnodeGetY(d))
         {
            if (dirnodeGetY(d) == *y_orig)
            {
               strcpy(tline," ");
               if (d == curNode)
                  tline[strlen (tline) - 1] = WCD_SEL_ON;
               
               strcat(tline,dirnodeGetName(d));
               
               strcat(tline," ");
               if (d == curNode)
                  tline[strlen (tline) - 1] = WCD_SEL_OFF;
               
               strcat(tline,WCD_SPLITDIR);

               n = elementAtDirnode(0,d);
               if (fold eq true)
                  tline[strlen(tline)-1] = '+';

            }
            else
            {
               for (i=0;i<len;i++)
                  tline[i] = ' ';
               tline[len] = '\0';
               strcat(tline,WCD_MOREDIR);
            }
         }
         else
         {
            for (i=0;i<len;i++)
               tline[i] = ' ';
            tline[len] = '\0';
            
            if (y == dirnodeGetY(elementAtDirnode(getSizeOfDirnode(d)-1,d)))
            {
               if (y == *y_orig)
               {
                  strcat(tline,WCD_ENDDIR);
                  n = elementAtDirnode(0,d);
                  if (fold eq true)
                     tline[strlen(tline)-1] = '+';
               }
               else
                  strcat(tline,WCD_OVERDIR);
            }
            else
            {
               if (y == *y_orig)
               {
                  strcat(tline,WCD_SUBDIR);
                  n = elementAtDirnode(0,d);
                  if (fold eq true)
                     tline[strlen(tline)-1] = '+';
               }
               else 
                  strcat(tline,WCD_MOREDIR);
            }
         }

      }
      
   }
   else
   {
      strcpy(tline," ");
      if (d == curNode)
         tline[strlen (tline) - 1] = WCD_SEL_ON;
      
      strcat(tline,dirnodeGetName(d));
      
      strcat(tline," ");
      if (d == curNode)
         tline[strlen (tline) - 1] = WCD_SEL_OFF;
      
      line[0] = '\0';
   }

   strcat(tline,line);
   strcpy(line,tline);

   if (dirnodeHasParent(d) eq true)
   {
      n = dirnodeGetParent(d);
      return(getTreeLine(n,dirnodeGetY(d),y_orig,curNode,d->fold));
   }
   
   if (*y_orig != 0)
   {
      strcpy(tline," ");
      strcat(tline,line);
      strcpy(line,tline);
   }
      
   return line;
}

void dumpTree(dirnode d)
{
   int index, size;
   dirnode n;
   int y;

   
   index = 0;
   
   if(isEmptyDirnode(d) eq false)
   {
         size = getSizeOfDirnode(d);
         while(index < size)
         {
            n = elementAtDirnode(index,d);
            dumpTree(n);
            index++;
         }
   }
   else
   {
      y = dirnodeGetY(d);
      printf("%s\n",getTreeLine(d,y,&y,NULL,false));
   }


}

/* 
 * Set the x,y values of the dirnodes
 * in the tree
 */
 
void setXYTree(dirnode d)
{
   int index, x, len, size;
   static int y;
   dirnode n;


   if(dirHasSubdirs(d) eq true)
   {
      len = strlen(dirnodeGetName(d));
      y = dirnodeGetY(d);
      index = 0;
      size = getSizeOfDirnode(d);
      while(index < size)
      {
         n = elementAtDirnode(index,d);
         x = dirnodeGetX(d) + len + 5;
         dirnodeSetX(x,n);
         dirnodeSetY(y,n);
         setXYTree(n);
         index++;
         y = y + 1;
      }
      /* y is 1 too large */
      y = y - 1;
   }
}

c3po_bool eqTextDirnode(text t, dirnode d)
{
   return(eqText(t,dirnodeGetName(d)));
}

int inDirnode(text t, dirnode d)
{
   int index, size;

   index = 0;

   if(dirHasSubdirs(d) eq true)
   {
      size = getSizeOfDirnode(d);
      while(index < size)
      {
         if (eqTextDirnode(t,elementAtDirnode(index,d)) eq true)
            return index;
         index++;
      }
   }
   return -1;
}

void addPath(text path, dirnode d)
{
   char *s;
   dirnode n,n_up;
   text t;
   int index;
#if (defined(WIN32) || defined(__CYGWIN__))
   static char buf[DD_MAXPATH] = "//" ;

   if ( (path != NULL) &&
        (*path == '/') &&
        (*(path+1) == '/') )
   {
      s = strtok(path,"/");
      if (s != NULL)
      {
         buf[2] = '\0';
         strcat(buf, s);
         s = buf;
      }
   }
   else
#endif
      s = strtok(path,"/");

   if (s != NULL)
   {
      index = inDirnode(s,d);
      if (index < 0)
      {
         n = dirnodeNew(d,NULL,NULL);
         t = textNew(s);
         dirnodeSetName(t,n);
         if (d->size > 0)
         {
            n_up = d->subdirs[d->size -1];
            n_up->down = n;
            n->up = n_up;
         }

         addToDirnode(n,d);
      }
      else
      {
         n = elementAtDirnode(index,d);
      }
      addPath(NULL,n);
   }
}

void buildTreeFromNameset(nameset set, dirnode d)
{
   int i;

   if ((set == NULL)||(d == NULL))
      return;
   
   sort_list(set);

   for (i=0;i < set->size;i++)
   {
      addPath(set->array[i],d);
   }

}

/**********************************************************
 *
 * NCD (Ninux Czo Directory) can read a tree and build
 * a data structure faster than WCD because:
 * - NCD has a more compact treefile, optimised for graphical tree.
 * - NCD just adds all dirs. No checking for duplicates.
 *
 * WCD is slower because:
 * - WCD has larger treefile, optimised for string matching.
 * - WCD has to split every path in directories.
 * - WCD searches every path in data structure.
 *
 * WCD takes slower speed for granted because:
 * - no extra file needed for graphical interface.
 * - string matching is more important feature.
 * - duplicates are skipped
 * - These functions can be used on all WCD treefiles
 *   and match lists. This enables a graphical match list.
 *   
 ***********************************************************/

void buildTreeFromFile(char *filename, dirnode d)
{
   nameset dirs;

   dirs = namesetNew();

   if(dirs != NULL)
      read_treefile(filename,dirs,0);
   else
      return;
   
   if (d == NULL)
      return;

   buildTreeFromNameset(dirs,d);

   freeNameset(dirs, 1);
}

/*****************************************************/

dirnode createRootNode()
{
   dirnode rootNode;

   rootNode = dirnodeNew(NULL,NULL,NULL);

   if (rootNode != NULL)
   {
      dirnodeSetName(ROOTDIR,rootNode);
      dirnodeSetX(0,rootNode);
      dirnodeSetY(0,rootNode);
   }

   return(rootNode);
}

/************************************************
 *
 * Search for dirnode with with path `path'.
 * If dirnode is not found return closest parent
 * dirnode.
 *
 ************************************************/

dirnode searchNodeForDir(text path, dirnode d, dirnode rNode)
{
   char *s;
   dirnode n;
   int index;
   dirnode rootnode;
#if (defined(WIN32) || defined(__CYGWIN__))
   static char buf[DD_MAXPATH] = "//" ;
#endif
   
   if (d == NULL)
      return(NULL);

   if (rNode == NULL)
   {
      rootnode = endOfRecursionOfDirnodeParent(d);
      return searchNodeForDir(path, rootnode, rootnode); /* start on top of tree */
   }

#if (defined(WIN32) || defined(__CYGWIN__))
   if ( (path != NULL) &&
        (*path == '/') &&
        (*(path+1) == '/') )
   {
      s = strtok(path,"/");
      if (s != NULL)
      {
         buf[2] = '\0';
         strcat(buf, s);
         s = buf;
      }
   }
   else
#endif
      s = strtok(path,"/");

   if (s != NULL)
   {
      index = inDirnode(s,d);
      if (index < 0)
      {
         return(d); 
      }
      else
      {
         n = elementAtDirnode(index,d);
         return(searchNodeForDir(NULL,n,rNode));
      }
   }

   return(d);
}

/*****************************************************/

dirnode locatePathOrSo(text path, dirnode d)
{
   return(searchNodeForDir(path,d,NULL));
}

/*****************************************************/

char *getNodeFullPath(dirnode node)
{
   static text line = NULL;
   static text tline = NULL;
   dirnode n;
   int len;

   len = strlen(dirnodeGetName(node));

   /* The reconstructed path will not be longer than
    * it was original in the treedata file. The max length
    * is DD_MAXPATH. See also function `read_treefile()'
    *
    * The reconstructed path has only an extra slash "/"
    * in the beginning on DOS/Windows if environment
    * variable HOME or WCDHOME is set
    * */
   
   if (line == NULL)
   {
      line = textNewSize(DD_MAXPATH+1);
   }
   if (tline == NULL)
   {
      tline = textNewSize(DD_MAXPATH+1);
   }
   
   line[0] = '\0';
   
   n = node;

   if (dirnodeHasParent(n) eq false)
      strcpy(line,"/");
   else
   {
      while (dirnodeHasParent(n) eq true)
      {
         strcpy(tline,"/");
         strcat(tline,dirnodeGetName(n));

         strcat(tline,line);
         strcpy(line,tline);

         n = dirnodeGetParent(n);
      }
   }

   return(line);
}
   
/*****************************************************
 *
 * dirnode Left()
 *
 * Return Left node, which is parent node 
 *****************************************************/

dirnode Left(dirnode node)
{
   return dirnodeGetParent(node);
}

/*****************************************************
 *
 * dirnode Down()
 *
 * Return node Down, which is next node in parent directory
 *****************************************************/

dirnode Down(dirnode node)
{
   return node->down;
}

/*****************************************************
 *
 * dirnode Right()
 *
 * Return Right node, which is first sub node 
 *****************************************************/

dirnode Right(dirnode node)
{
   
   if(dirnodeFold(node) eq false)
      return elementAtDirnode(0,node);
   else
      return(NULL);
}

/*****************************************************
 *
 * dirnode Up()
 *
 * Return node Up, which is previous node in parent directory
 *****************************************************/

dirnode Up(dirnode node)
{
   return node->up;
}

/******************************************************
 *
 *  return first node in a certain level
 *
 ******************************************************/

dirnode getAnyNodeInLevel(dirnode node, int level)
{
   dirnode n, ndown;
   dirnode rootnode;

   if (node == NULL)
      return(NULL);
   
   rootnode = endOfRecursionOfDirnodeParent(node);

   n = rootnode; /* start on top of tree */
   
   while(n != NULL)
   {
      ndown = Down(n);
      while ((ndown != NULL) && (dirnodeGetY(ndown)<=level))
      {
         n = ndown;
         ndown=Down(ndown);
      }

      if (dirnodeGetY(n) == level)
         return(n);

      n = Right(n);
   }

   return(NULL); /* level is larger than tree or <0 */
}

/******************************************************
 *
 *  return first node in a certain level
 *
 ******************************************************/

dirnode getFirstNodeInLevel(dirnode node, int level)
{
   return getAnyNodeInLevel(node, level);
}

/******************************************************/

dirnode getLastNodeInSameLevel(dirnode node)
{
   if (node == NULL)
      return(NULL);

   if (dirHasSubdirs(node) eq false)
      return(node);
   else
      return getLastNodeInSameLevel(elementAtDirnode(0,node));
}

/******************************************************/

dirnode getLastNodeInLevel(dirnode node, int level)
{
   return getLastNodeInSameLevel(getAnyNodeInLevel(node,level));
}

/******************************************************
 *
 * getNodeCursUp(dirnode curNode)
 *
 * Get node up in the tree.
 * If node is a valid pointer to a node this function
 * will return always a node.
 ******************************************************/
dirnode getNodeCursUp(dirnode curNode)
{
   dirnode node;

   
   if ((node = Up(curNode)) == NULL) /* there is no Up node, try left node */
   {
      if ((node = Left(curNode)) == NULL)
         return(curNode); /* there is no Up or Left node, return same node */
   }

   return(node);
}
/******************************************************
 *
 * getNodeCursDown(dirnode curNode)
 *
 * Get node down in the tree.
 * If node is a valid pointer to a node this function
 * will return always a node.
 ******************************************************/
dirnode getNodeCursDown(dirnode curNode)
{
   dirnode node;

   if ((node = Down(curNode)) == NULL)
   {
      if ((node = Right(curNode)) == NULL)
      {
         node = getFirstNodeInLevel(curNode,dirnodeGetY(curNode)+1);
         if (node == NULL)
            return(curNode);
      }
   }

   return(node);
}
/******************************************************
 *
 * getNodeCursLeft(dirnode curNode)
 *
 * Get node left in the tree.
 * If node is a valid pointer to a node this function
 * will return always a node.
 ******************************************************/
dirnode getNodeCursLeft(dirnode curNode)
{
   dirnode node;

   if ((node = Left(curNode)) == NULL)
      return(curNode); /* there is no Left node, return same node */

   return(node);
}
/******************************************************
 *
 * getNodeCursRight(dirnode curNode)
 *
 * Get node right in the tree.
 * If node is a valid pointer to a node this function
 * will return always a node.
 ******************************************************/
dirnode getNodeCursRight(dirnode curNode, int graphics_mode)
{
   dirnode node;

   if ((node = Right(curNode)) == NULL)
   {
      if (graphics_mode & WCD_GRAPH_ALT) /* stop at end of line */
         return(curNode);

      if ((node = Down(curNode)) == NULL) /* try to go one down */
      {
         /* try to go one up and one down */
         node = Left(curNode);

         while (node != NULL)
         {
            if(Down(node) != NULL)
               return(Down(node));
            else
               node = Left(node);
         }
         /* node == NULL */
         return(curNode);
      }
   }

   return(node);
}

/******************************************************/

dirnode getNodeLnUp(dirnode curNode)
{
   dirnode dn;

   dn = getLastNodeInLevel(curNode, dirnodeGetY(curNode)-1);

   if(dn == NULL)
   {
      return(curNode);
   }
   else
   {
      return(dn);
   }
}

/******************************************************/

dirnode getNodeLnDn(dirnode curNode)
{
   dirnode dn;

   dn = getFirstNodeInLevel(curNode,dirnodeGetY(curNode)+1);

   if(dn == NULL)
      return(curNode);
   else
      return(dn);
}

/******************************************************/

dirnode getNodePrev(dirnode curNode)
{
   dirnode node;

   if (curNode == NULL)
      return(NULL);

   node = Left(curNode);
   
   if ((node != NULL) && (dirnodeGetY(node)==dirnodeGetY(curNode)))
   {
      return(node);
   }
   
   return(getNodeLnUp(curNode));
}

/******************************************************/
dirnode getLastDescendant(dirnode node)
{
   dirnode n;
   dirnode rootnode;

   if (node == NULL)
      return(NULL);

   rootnode = endOfRecursionOfDirnodeParent(node);

   n = rootnode; /* start on top of tree */

   while(n != NULL)
   {
      while (Down(n) != NULL)
         n=Down(n);

      if (Right(n) == NULL)
         return(n);

      n = Right(n);
   }
   return(NULL);
}

/******************************************************/

dirnode prevNodeCiclic(dirnode curNode)
{
   dirnode rootnode;

   if (curNode == NULL)
      return NULL;
   
   rootnode = endOfRecursionOfDirnodeParent(curNode);

   if (curNode == rootnode)
      return(getLastDescendant(curNode));

   return(getNodePrev(curNode));
}

/******************************************************/

dirnode nextNodeCiclic(dirnode curNode)
{
   dirnode node;
   dirnode rootnode;

   if (curNode == NULL)
      return NULL;
   
   rootnode = endOfRecursionOfDirnodeParent(curNode);

   node = getNodeCursRight(curNode, WCD_GRAPH_NORMAL);

   if (node == curNode)
      return(rootnode);
   else
      return(node);
}

/******************************************************/

int getNodeDepth(dirnode curNode)
{
   dirnode node;
   int thisdepth = 0;

   node = curNode;

   while ((node = Left(node)) != NULL)
      thisdepth++;

   return(thisdepth);
}
   
/******************************************************/

dirnode getNodeCursDownNatural(dirnode curNode, int graphics_mode)
{
   dirnode node,oldnode;
   int thisdepth;

   if(Down(curNode) != NULL)
      return(Down(curNode));  /* go down */

   if (graphics_mode & WCD_GRAPH_ALT)
      return(curNode); /* stop */

   thisdepth = getNodeDepth(curNode);
   oldnode = curNode;
   node = getNodeCursRight(curNode, graphics_mode);

   /* There was no direct down node, try to find down node on same level */
   
   while ( (node != NULL) &&
           (node !=oldnode) &&
           (getNodeDepth(node) != thisdepth))
   {
      oldnode = node;
      node = getNodeCursRight(node, WCD_GRAPH_NORMAL);
   }

   /* node==NULL  or  
    * node==oldnode or
    * dept node == dept oldnode */

   if (node != NULL)
   {
      if (oldnode != node)
         return(node);
      else
         return getNodeCursRight(curNode, WCD_GRAPH_NORMAL);
   }
   else
      return(curNode);
}

/******************************************************/

dirnode getNodeCursUpNatural(dirnode curNode, int graphics_mode)
{
   dirnode node,oldnode;
   int thisdepth;

   if(Up(curNode) != NULL)
      return(Up(curNode));  /* go up */

   if (graphics_mode & WCD_GRAPH_ALT)
      return(curNode); /* stop */

   thisdepth = getNodeDepth(curNode);
   oldnode = curNode;
   node = getNodePrev(curNode);

   while ( (node != NULL) &&
           (node !=oldnode) &&
           (getNodeDepth(node) != thisdepth))
   {
      oldnode = node;
      node = getNodePrev(node);
   }

   /* node==NULL  or  
    * node==oldnode or
    * dept node == dept oldnode */

   if (node != NULL)
   {
      if(oldnode != node)
         return(node);
      else
         return(getNodePrev(curNode));
   }
   else
      return(curNode);
}

/******************************************************/

int validSearchDir(char *dir, dirnode curNode, int exact, int ignore_case)
{
   char path[WCD_MAX_INPSTR+2];
   char *name;

   if ((dir == NULL)||(curNode == NULL))
      return(0);

   /* The string length of *dir is guaranteed <= WCD_MAX_INPSTR
    * *path is max two characters longer
    * */

   strcpy(path,"*");
   strcat(path,dir);
   if (exact == 0)
      strcat(path,"*");


   if( (name = strrchr(path,DIR_SEPARATOR)) != NULL)
      name++;
   else
      name = path + 1;

   /* matching with wildcard support !!! */
   if ( (dd_match(dirnodeGetName(curNode),name,ignore_case)) &&
        (dd_match(getNodeFullPath(curNode),path,ignore_case))
         )
      return(1);
   else
      return(0);
}

/******************************************************/

dirnode findDirInCiclePrev(char *dir, dirnode curNode, int exact, int ignore_case)
{
   dirnode node;
   int valid;

   node = curNode;
   
   do{
      node = prevNodeCiclic(node);
      valid = validSearchDir(dir,node,exact,ignore_case);
   } while((!valid)&&(node!=curNode));
   
   return(node);
}

/******************************************************/

dirnode findDirInCicle(char *dir, dirnode curNode, int exact, int ignore_case)
{
   dirnode node;
   int valid;

   node = curNode;
   
   do{
      node = nextNodeCiclic(node);
      valid = validSearchDir(dir,node,exact,ignore_case);
   } while((!valid)&&(node!=curNode));
   
   return(node);
}

/************************************************************
 *
 * updateLine()
 * i = nodelevel
 * y = screen level
 * 
 * **********************************************************/

#ifdef WCD_USECURSES
void updateLine(WINDOW *win, dirnode n, int i, int y, dirnode curNode, int xoffset)
{
   wcd_char *s;
   int len, j;

   s = getTreeLine(getLastNodeInLevel(n,i),i,&i,curNode,false);

   if (s != NULL)
   {
      len = strlen(s);
      wmove(win,y,0);

      for(j=xoffset;(j<len)&&((j-xoffset)<(COLS-1));j++)
      {
         switch(s[j])
         {
            case WCD_SEL_ON:
                  wattron(win,A_REVERSE);
                  waddch(win,'<');
               break;
            case WCD_SEL_OFF:
                  waddch(win,'>');
                  wattroff(win,A_REVERSE);
               break;
            default:
               waddch(win,s[j]);
         }
      }
   }
}

#define WCD_NAV 0
#define WCD_SEARCH 1

/************************************************************/

char *getZoomStackPath(dirnode stack)
{
   static text line = NULL;
   int i, size;
   text name;

   
   if (line == NULL)
   {
      line = textNewSize(DD_MAXPATH);
   }
   
   line[0] = '\0';
   
   size = getSizeOfDirnode(stack);
   
   for (i = 0; i < size ; i++)
   {
      if(i != 0)
         strcat(line,"/");
      name = dirnodeGetName(elementAtDirnode(i,stack));
      if((strlen(line)+strlen(name)) < DD_MAXPATH)
         strcat(line,name);
   }

            
   return(line);
}

/************************************************************/
void dataRefresh(int ydiff, int init)
{
  int i, yoffset, xo, len;
  static int xoffset = 0;
  static int yposition = -1;  /* -1 : not initialized */
  wcd_char *s;

  /*
     yoffset is the y-offset in the whole tree of the current node.
     ydiff is the y-difference in the whole tree with the previous node.
     yposition is the y-offset in the window (0 .. scrollWinHeight-1)
  */
  if ((yposition < 0)|| init)
     yposition = (wcd_cwin.scrollWinHeight /2);

  wclear(wcd_cwin.scrollWin);
  wclear(wcd_cwin.inputWin);

  if (ydiff < 0 )
  {
    /* going down */
    if( yposition-ydiff < (wcd_cwin.scrollWinHeight-1) )
       yposition -= ydiff;
    else
       yposition = (wcd_cwin.scrollWinHeight -2);
  }
  else
  {
    /* going up */
    if( (yposition-ydiff) > 0 )
       yposition -= ydiff;
    else
       yposition = 1;
  }

  yoffset = dirnodeGetY(wcd_cwin.curNode) - yposition;

  if (yoffset < 0) yoffset = 0;

  len=dirnodeGetX(wcd_cwin.curNode)+strlen(dirnodeGetName(wcd_cwin.curNode))+3;
  if (len > COLS)
  {
    xo = len - COLS;
    if ((xo > xoffset) || (xoffset > (dirnodeGetX(wcd_cwin.curNode)-1)))
    {
       xoffset = xo;
    }
  }
  else
      xoffset = 0;

  for (i = 0; i < wcd_cwin.scrollWinHeight; i++)
    updateLine(wcd_cwin.scrollWin, wcd_cwin.curNode, i+yoffset, i,
                                            wcd_cwin.curNode, xoffset);

  for (i = 0; i < COLS; i++)
  {
    wmove(wcd_cwin.inputWin, 0, i);
    waddch(wcd_cwin.inputWin, '-');
  }

  /* mvwprintw(inputWin, 1,0,"%s",getNodeFullPath(curNode)); */
  s = getZoomStackPath(wcd_cwin.zoomStack); /* s has size DD_MAXPATH */
  strcat(s, getNodeFullPath(wcd_cwin.curNode));
  wcd_fixpath(s, DD_MAXPATH);

  if (s != NULL)
  {
    len = strlen(s);
    wmove(wcd_cwin.inputWin, 1, 0);

    for (i = 0; (i < len) && (i < (COLS - 1)); i++)
      waddch(wcd_cwin.inputWin, s[i]);
  }

  if (wcd_cwin.mode == WCD_NAV)
    mvwprintw(wcd_cwin.inputWin, 2, 0, "/ = search forward,  ? = search backward,  : = help");
  else
  {
    mvwprintw(wcd_cwin.inputWin, 2, 0, "Search: %s", wcd_cwin.str);
  }

  prefresh(wcd_cwin.scrollWin, 0, 0, 0, 0, wcd_cwin.scrollWinHeight-1, COLS-1);
  prefresh(wcd_cwin.inputWin, 0, 0, wcd_cwin.scrollWinHeight, 0,
                          wcd_cwin.scrollWinHeight+INPUT_WIN_HEIGHT-1, COLS-1);
}

/************************************************************/
void showHelp(WINDOW *win, int height)
{
   wclear(win);
   if (height > 20)
   {
      mvwprintw(win, 0,0,_("NAVIGATE MODE:"));
      mvwprintw(win, 1,0,_("h or <Left>       go left."));
      mvwprintw(win, 2,0,_("j or <Down>       go down."));
      mvwprintw(win, 3,0,_("k or <Up>         go up."));
      mvwprintw(win, 4,0,_("l or <Right>      go right."));
      mvwprintw(win, 5,0,_("* or v or <Space> go forward to dir with same name."));
      mvwprintw(win, 6,0,_("# or p or <BS>    go backward to dir with same name."));
      mvwprintw(win, 7,0,_("^ or a            go to beginning of line."));
      mvwprintw(win, 8,0,_("$ or e            go to end of line."));
      mvwprintw(win, 9,0,_("1                 go to root dir."));
      mvwprintw(win,10,0,_("g or G            go to last dir."));
      mvwprintw(win,11,0,_("f                 go page forward."));
      mvwprintw(win,12,0,_("b                 go page backward."));
      mvwprintw(win,13,0,_("u                 go half page up."));
      mvwprintw(win,14,0,_("d                 go half page down."));
      mvwprintw(win,15,0,_("<Esc> or q        Abort."));
      mvwprintw(win,16,0,_("/                 Search forward."));
      mvwprintw(win,17,0,_("?                 Search backward."));
      mvwprintw(win,18,0,_("n                 Repeat latest / or ? search."));
      mvwprintw(win,19,0,_("<Enter>           Select directory."));
      mvwprintw(win,20,0,_("Press any key."));

      prefresh(win,0,0,0,0,height-1,COLS-1);
      getch();
      wclear(win);

      mvwprintw(win, 0,0,_("SEARCH MODE with wildcard and subdir support:"));
      mvwprintw(win, 1,0,_("<Left>            go left."));
      mvwprintw(win, 2,0,_("<Down>            go down."));
      mvwprintw(win, 3,0,_("<Up>              go up."));
      mvwprintw(win, 4,0,_("<Right>           go right."));
      mvwprintw(win, 5,0,_("CTRL-v            go forward to dir with same name."));
      mvwprintw(win, 6,0,_("CTRL-p            go backward to dir with same name."));
      mvwprintw(win, 7,0,_("CTRL-a            go to beginning of line."));
      mvwprintw(win, 8,0,_("CTRL-e            go to end of line."));
      mvwprintw(win, 9,0,_("CTRL-g            go to last dir."));
      mvwprintw(win,10,0,_("CTRL-f            go page forward."));
      mvwprintw(win,11,0,_("CTRL-b            go page backward."));
      mvwprintw(win,12,0,_("CTRL-u            go half page up."));
      mvwprintw(win,13,0,_("CTRL-d            go half page down."));
      mvwprintw(win,14,0,_("<Esc> or CTRL-x   Abort SEARCH MODE."));
      mvwprintw(win,15,0,_("CTRL-n            Repeat latest / or ? search."));
      mvwprintw(win,16,0,_("<Enter>           Select directory."));
      mvwprintw(win,18,0,_("Press any key."));

      prefresh(win,0,0,0,0,height-1,COLS-1);
      getch();
      wclear(win);

      mvwprintw(win, 0,0,_("ZOOMING:"));
      mvwprintw(win, 1,0,_("z or i or CTRL-i  zoom in."));
      mvwprintw(win, 2,0,_("Z or o or CTRL-o  zoom out."));
      mvwprintw(win, 3,0,_("c                 condense: fold current level."));
      mvwprintw(win, 4,0,_("C                 condense: fold subdir level."));
      mvwprintw(win, 5,0,_("w                 condense: fold current and sub levels."));
      mvwprintw(win, 6,0,_("y or CTRL-y       uncondense: unfold current and sub levels."));
      mvwprintw(win, 7,0,_("r or CTRL-r       uncondense: unfold all directories."));
      mvwprintw(win, 8,0,_("-                 fold directory."));
      mvwprintw(win, 9,0,_("+ or =            unfold directory."));
      mvwprintw(win,10,0,_("l or <Right>      unfold and go right."));
      mvwprintw(win,11,0,_("Press any key."));
   }
   else
      mvwprintw(win, 0,0,_("Screenheight must be > 20 for help"));


   prefresh(win,0,0,0,0,height-1,COLS-1);
   getch();
}

/************************************************************/
dirnode pushZoom(dirnode zoomStack, dirnode curNode, int *ymax)
{
   dirnode znew; /* new element on zoom stack */
   dirnode top;
   
   if ((zoomStack == NULL)||(curNode == NULL))
      return(NULL);

   top = endOfRecursionOfDirnodeParent(curNode);
   if (curNode == top) /* we are at the top */
      return(curNode);

   /* store Parent, Up and Down pointers in znew */
   znew = dirnodeNew(dirnodeGetParent(curNode),dirnodeGetUp(curNode),dirnodeGetDown(curNode));
   
   dirnodeSetName(textNew(getNodeFullPath(curNode)),znew);
   
   addToDirnode (znew,zoomStack);

   /* curNode will be the new top node */
   dirnodeSetParent(NULL,curNode);
   dirnodeSetUp(NULL,curNode);
   dirnodeSetDown(NULL,curNode);
   dirnodeSetX(0,curNode);
   dirnodeSetY(0,curNode);

   setXYTree(curNode);

   *ymax = dirnodeGetY(getLastDescendant(curNode));
   
   return(curNode);
}
/************************************************************/
dirnode popZoom(dirnode zoomStack, dirnode curNode, int *ymax)
{
   dirnode zlast;  /* element on zoom stack */
   dirnode top; /* the current top node */
   dirnode newtop; /* the new top node */
   int size;

   if ((zoomStack == NULL)||(curNode == NULL))
      return(NULL);

   size = getSizeOfDirnode(zoomStack);
   if( size == 0)
      return(NULL); /* stack is empty */
   else
      zlast = elementAtDirnode(size - 1,zoomStack); /* get last element on stack */

   top = endOfRecursionOfDirnodeParent(curNode);
   
   /* restore pointers on top node */
   dirnodeSetParent(dirnodeGetParent(zlast),top);
   dirnodeSetUp(dirnodeGetUp(zlast),top);
   dirnodeSetDown(dirnodeGetDown(zlast),top);

   /* remove last element from stack */
   removeElementAtDirnode(size - 1, zoomStack, false, false);

   newtop = endOfRecursionOfDirnodeParent(top);
   dirnodeSetX(0,newtop);
   dirnodeSetY(0,newtop);
   setXYTree(newtop);

   *ymax = dirnodeGetY(getLastDescendant(newtop));

   return(newtop);
}

/****************************************************************
 * setFold()
 * Unfold or fold a dirnode
 *
 ****************************************************************/
void setFold(dirnode n, c3po_bool f, int *ymax)
{
   dirnode d;

   if (n == NULL)
      return;
   else
   {
      if (n->size > 0) /* only (un)fold directories that have subdirs */
      {
         dirnodeSetFold(f,n);

         d = endOfRecursionOfDirnodeParent(n);
         setXYTree(d);
         *ymax = dirnodeGetY(getLastDescendant(d));
      }
   }

   return;
}
/****************************************************************
 * setfold_tree()
 * Fold or unfold all dirnodes in tree
 *
 ****************************************************************/
void setFold_tree(dirnode d, c3po_bool *f)
{
   int index,size;
   dirnode n;

   if(dirnodeHasSubdirs(d) eq true) /* only (un)fold directories that have subdirs */
   {
      d->fold = *f;
      index = 0;
      size = getSizeOfDirnode(d);
      while(index < size)
      {
         n = elementAtDirnode(index,d);
         setFold_tree(n,f);
         index++;
      }
   }
   return;
}

/*****************************************************************
 * setfold_sub()
 * Fold or unfold all dirnode from a sub dirnode
 *****************************************************************/

void setFold_sub(dirnode d, c3po_bool f, int *ymax)
{
   dirnode top;

   if (d == NULL)
      return;
   else
   {
      setFold_tree(d,&f);
      d->fold = false;
      top = endOfRecursionOfDirnodeParent(d);
      setXYTree(top);
      *ymax = dirnodeGetY(getLastDescendant(top));
      return;
   }
}

/*****************************************************************
 * setfold_all()
 * Fold or unfold all dirnode from top dirnode
 *****************************************************************/

void setFold_all(dirnode n, c3po_bool f, int *ymax)
{
   dirnode top;

   if (n == NULL)
      return;
   else
   {
      top = endOfRecursionOfDirnodeParent(n);
      setFold_sub(top,f,ymax);
      return;
   }
}
/****************************************************************
 *
 * condenseSubdirs()
 * This function folds all subdirectories of a dirnode.
 * 
 *****************************************************************/
void condenseSubdirs(dirnode n, int *ymax)
{
   int i;
   dirnode d;

   if ((n == NULL)||(dirHasSubdirs(n) eq false))
      return;
   
   for(i=0;i<n->size;i++)
   {
      d = elementAtDirnode(i,n);
      if (d-> size > 0)
         d->fold = true;
   }
   
   d = endOfRecursionOfDirnodeParent(n);
   setXYTree(d);
   *ymax = dirnodeGetY(getLastDescendant(d));

   return;
}

/***************************************************************/
void condense(dirnode n, int *ymax)
{
   if((n == NULL) || (dirnodeHasParent(n) eq false))
      return;

   condenseSubdirs(dirnodeGetParent(n),ymax);
}
   
/***************************************************************/

#define Key_CTRL(x)      ((x) & 31)

char *selectANode(dirnode tree, int *use_HOME, int ignore_case, int graphics_mode)
{
  int c = 0, n =0, y, ymax;
  int ydiff;
  char curPath[DD_MAXPATH];
  char *ptr, *ptr2;
#ifndef __PDCURSES__
  SCREEN *sp;
#endif

  wcd_cwin.mode = WCD_NAV;
  wcd_cwin.graphics_mode = graphics_mode;
  ydiff = 0;

  if (tree == NULL)
     return NULL;

   ptr=NULL;   
  
   getCurPath(curPath,DD_MAXPATH,use_HOME);
   wcd_cwin.curNode = locatePathOrSo(curPath,tree);

   wcd_cwin.zoomStack = dirnodeNew(NULL,NULL,NULL);

#if CAN_RESIZE
   signal (SIGWINCH, signalSigwinch);
#endif
  
#ifdef __PDCURSES__
   initscr();
#else
   sp = newterm(NULL,stdout,stdin);
   if (sp == NULL)
   {  
      fprintf(stderr,_("Wcd: warning: Error opening terminal, falling back to stdout interface.\n"));
      return NULL;
   }
#endif

   if (graphics_mode & WCD_GRAPH_COLOR)
      initcolor();

   keypad(stdscr, TRUE);
   intrflush(stdscr, FALSE);
   cbreak();
   noecho();
   nonl();
   scrollok(stdscr, TRUE); /* enable scrolling */


   wcd_cwin.scrollWinHeight = LINES - INPUT_WIN_HEIGHT;
   wcd_cwin.scrollWinLen = COLS;
   wcd_cwin.scrollWin = newpad(wcd_cwin.scrollWinHeight,COLS);

   wcd_cwin.inputWin = newpad(INPUT_WIN_HEIGHT,COLS);

   if (graphics_mode & WCD_GRAPH_COLOR)
   {
      colorbox (wcd_cwin.scrollWin, MENU_COLOR, 0);     /* BillyC add colors */
      colorbox (wcd_cwin.inputWin,  BODY_COLOR, 0);     /* BillyC add colors */
   }
   if (wcd_cwin.scrollWin == NULL)
   {
      endwin();
#ifdef XCURSES
      XCursesExit();
#endif
      fprintf(stderr,_("Wcd: error creating scroll window.\n"));
      return NULL;
   }

   scrollok(wcd_cwin.scrollWin, TRUE);

   wcd_cwin.str[n] = '\0';

   refresh();
   dataRefresh(0, 1);

   ymax = dirnodeGetY(getLastDescendant(wcd_cwin.curNode));

   while (c != 13 )
   {

      c = getch();
      ydiff = wcd_cwin.curNode->y;

     switch(c)
     {
      case 'y':
         if (wcd_cwin.mode == WCD_NAV)
         {
            if (dirnodeHasParent(wcd_cwin.curNode) eq false)
               setFold_sub(wcd_cwin.curNode,false,&ymax); /* unfold tree from current dirnode */
            else
               setFold_sub(dirnodeGetParent(wcd_cwin.curNode),false,&ymax); /* unfold tree from parent dirnode */
         }
         break;
      case Key_CTRL ('y'):
         if (dirnodeHasParent(wcd_cwin.curNode) eq false)
            setFold_sub(wcd_cwin.curNode,false,&ymax); /* unfold tree from current dirnode */
         else
            setFold_sub(dirnodeGetParent(wcd_cwin.curNode),false,&ymax); /* unfold tree from parent dirnode */
         break;
      case 'r':
         if (wcd_cwin.mode == WCD_NAV)
         {
            setFold_all(wcd_cwin.curNode,false,&ymax); /* unfold complete tree */
         }
         break;
      case Key_CTRL('r'):
         setFold_all(wcd_cwin.curNode,false,&ymax); /* unfold complete tree */
         break;
      case '-':
         if (wcd_cwin.mode == WCD_NAV)
         {
            setFold(wcd_cwin.curNode,true,&ymax); /* fold */
         }
         break;
      case '+':
      case '=':
         if (wcd_cwin.mode == WCD_NAV)
         {
            setFold(wcd_cwin.curNode,false,&ymax); /* unfold */
         }
         break;
#ifdef PADMINUS
      case PADMINUS:
         setFold(wcd_cwin.curNode,true,&ymax); /* fold */
         break;
#endif
#ifdef PADPLUS
      case PADPLUS:
         setFold(wcd_cwin.curNode,false,&ymax); /* unfold */
         break;
#endif
      case 'c':
         if (wcd_cwin.mode == WCD_NAV)
         {
            condense(wcd_cwin.curNode,&ymax); /* condense */
         }
         break;
      case 'C':
         if (wcd_cwin.mode == WCD_NAV)
         {
            condenseSubdirs(wcd_cwin.curNode,&ymax); /* condense */
         }
         break;
      case 'w':
         if (wcd_cwin.mode == WCD_NAV)
         {
            if (dirnodeHasParent(wcd_cwin.curNode) eq false)
               setFold_sub(wcd_cwin.curNode,true,&ymax); /* fold tree from current dirnode */
            else
               setFold_sub(dirnodeGetParent(wcd_cwin.curNode),true,&ymax); /* fold tree from parent dirnode */
         }
         break;
      case 'b':
         if (wcd_cwin.mode == WCD_NAV)
         {
           y = dirnodeGetY(wcd_cwin.curNode) - wcd_cwin.scrollWinHeight;
           if (y < 0)
              y = 0;
           wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         }
         break;
      case Key_CTRL ('b'):
      case KEY_PPAGE: /* Page Up */
#ifdef KEY_A3
      case KEY_A3:    /* Num-pad Page Up */
#endif
         y = dirnodeGetY(wcd_cwin.curNode) - wcd_cwin.scrollWinHeight;
         if (y < 0)
            y = 0;
         wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         break;
      case 'f':
         if (wcd_cwin.mode == WCD_NAV)
         {
           y = dirnodeGetY(wcd_cwin.curNode) + wcd_cwin.scrollWinHeight;
           if (y > ymax)
              y = ymax;
           wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         }
         break;
      case Key_CTRL ('f'):
      case KEY_NPAGE: /* Page down */
#ifdef KEY_C3
      case KEY_C3:   /*  Num-pad  page down */
#endif
         y = dirnodeGetY(wcd_cwin.curNode) + wcd_cwin.scrollWinHeight;
         if (y > ymax)
           y = ymax;
         wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         break;
      case 'u':
         if (wcd_cwin.mode == WCD_NAV)
         {
           y = dirnodeGetY(wcd_cwin.curNode) - (wcd_cwin.scrollWinHeight/2);
           if (y < 0)
              y = 0;
           wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         }
         break;
      case Key_CTRL ('u'):
         y = dirnodeGetY(wcd_cwin.curNode) - (wcd_cwin.scrollWinHeight/2);
         if (y < 0)
            y = 0;
         wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         break;
      case 'd':
         if (wcd_cwin.mode == WCD_NAV)
         {
           y = dirnodeGetY(wcd_cwin.curNode) + (wcd_cwin.scrollWinHeight/2);
           if (y > ymax)
              y = ymax;
           wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         }
         break;
      case Key_CTRL ('d'):
         y = dirnodeGetY(wcd_cwin.curNode) + (wcd_cwin.scrollWinHeight/2);
         if (y > ymax)
           y = ymax;
         wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,y);
         break;
      case '$':
      case 'e':
         if (wcd_cwin.mode == WCD_NAV)
           wcd_cwin.curNode = getLastNodeInSameLevel(wcd_cwin.curNode);
         break;
      case Key_CTRL ('e'):
#ifdef KEY_END
      case KEY_END:
#endif
#ifdef KEY_C1
      case KEY_C1:  /*  Num-pad END */
#endif
           wcd_cwin.curNode = getLastNodeInSameLevel(wcd_cwin.curNode);
         break;
      case '^':
      case 'a':
         if (wcd_cwin.mode == WCD_NAV)
           wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,dirnodeGetY(wcd_cwin.curNode));
         break;
      case Key_CTRL ('a'):
      case KEY_HOME:
#ifdef KEY_A1
      case KEY_A1:  /*  Num-pad  HOME */
#endif
           wcd_cwin.curNode = getFirstNodeInLevel(wcd_cwin.curNode,dirnodeGetY(wcd_cwin.curNode));
         break;
      case 'k':
         if (wcd_cwin.mode == WCD_NAV)
         {
            wcd_cwin.curNode = getNodeCursUpNatural(wcd_cwin.curNode, graphics_mode);
         }
         break;
      case KEY_UP:  /* Arrow Up */
#ifdef KEY_A2
      case KEY_A2:  /*  Num-pad ARROW UP */
#endif
            wcd_cwin.curNode = getNodeCursUpNatural(wcd_cwin.curNode, graphics_mode);
         break;
      case 'j':
         if (wcd_cwin.mode == WCD_NAV)
         {
            wcd_cwin.curNode = getNodeCursDownNatural(wcd_cwin.curNode, graphics_mode);
         }
         break;
      case KEY_DOWN: /* Arrow down */
#ifdef KEY_C2
      case KEY_C2:   /* Num-pad  Arrow down */
#endif
            wcd_cwin.curNode = getNodeCursDownNatural(wcd_cwin.curNode, graphics_mode);
         break;
      case ',':
      case 'h':
         if (wcd_cwin.mode == WCD_NAV)
         {
            wcd_cwin.curNode = getNodeCursLeft(wcd_cwin.curNode);
         }
         break;
      case KEY_LEFT:
#ifdef KEY_B1
      case KEY_B1:   /* Num-pad  Arrow left */
#endif
            wcd_cwin.curNode = getNodeCursLeft(wcd_cwin.curNode);
         break;
      case '.':
      case 'l':
         if (wcd_cwin.mode == WCD_NAV)
         {
            if(dirnodeFold(wcd_cwin.curNode) eq true)
            {
               setFold(wcd_cwin.curNode,false,&ymax);
            }
            wcd_cwin.curNode = getNodeCursRight(wcd_cwin.curNode, graphics_mode);
         }
         break;
      case KEY_RIGHT:
#ifdef KEY_B3
      case KEY_B3:   /* Num-pad  key_right  */
#endif
            if(dirnodeFold(wcd_cwin.curNode) eq true)
            {
               setFold(wcd_cwin.curNode,false,&ymax);
            }
            wcd_cwin.curNode = getNodeCursRight(wcd_cwin.curNode, graphics_mode);
         break;
      case '1': /* goto rootnode */
         if (wcd_cwin.mode == WCD_NAV)
            wcd_cwin.curNode = endOfRecursionOfDirnodeParent(wcd_cwin.curNode);
         break;
      case 'G': /* goto last node */
      case 'g':
         if (wcd_cwin.mode == WCD_NAV)
            wcd_cwin.curNode = getLastDescendant(wcd_cwin.curNode);
         break;
      case Key_CTRL ('g'):
         wcd_cwin.curNode = getLastDescendant(wcd_cwin.curNode);
         break;
      case 'p':
      case '#':
         if (wcd_cwin.mode == WCD_NAV)
            wcd_cwin.curNode = findDirInCiclePrev(dirnodeGetName(wcd_cwin.curNode),wcd_cwin.curNode,1,ignore_case);
         break;
      case Key_CTRL ('p'):
         wcd_cwin.curNode = findDirInCiclePrev(dirnodeGetName(wcd_cwin.curNode),wcd_cwin.curNode,1,ignore_case);
         break;
      case ' ':
      case 'v':
      case '*':
         if (wcd_cwin.mode == WCD_NAV)
            wcd_cwin.curNode = findDirInCicle(dirnodeGetName(wcd_cwin.curNode),wcd_cwin.curNode,1,ignore_case);
         break;
      case Key_CTRL ('v'):
         wcd_cwin.curNode = findDirInCicle(dirnodeGetName(wcd_cwin.curNode),wcd_cwin.curNode,1,ignore_case);
         break;
      case '/':
         if (wcd_cwin.mode == WCD_NAV)
         {
            wcd_cwin.mode = WCD_SEARCH;
            wcd_cwin.str[0] = '\0';
            n = 0;
         }
         break;
      case '?':
         if (wcd_cwin.mode == WCD_NAV)
         {
            wcd_cwin.mode = WCD_SEARCH;
            wcd_cwin.str[0] = '\0';
            n = 0;
         }
         break;
      case Key_CTRL ('n'):
         ptr2 = wcd_cwin.str + 1;
         if (n>1)
         {
            if (wcd_cwin.str[0] == '/')
               wcd_cwin.curNode = findDirInCicle(ptr2,wcd_cwin.curNode,0,ignore_case);
            else
               wcd_cwin.curNode = findDirInCiclePrev(ptr2,wcd_cwin.curNode,0,ignore_case);
         }
         break;
      case 'n':
         if (wcd_cwin.mode == WCD_NAV)
         {
            ptr2 = wcd_cwin.str + 1;
            if (n>1)
            {
               if (wcd_cwin.str[0] == '/')
                  wcd_cwin.curNode = findDirInCicle(ptr2,wcd_cwin.curNode,0,ignore_case);
               else
                  wcd_cwin.curNode = findDirInCiclePrev(ptr2,wcd_cwin.curNode,0,ignore_case);
            }
         }
         break;
      case KEY_F (1):
      case ':':
         if (wcd_cwin.mode == WCD_NAV)
            showHelp(wcd_cwin.scrollWin,wcd_cwin.scrollWinHeight);
         break;
      case 'q':
         if (wcd_cwin.mode == WCD_NAV)
            c = 13;
      break;
      case Key_CTRL ('i'):
            pushZoom(wcd_cwin.zoomStack,wcd_cwin.curNode,&ymax);
         break;   
      case 'i':
      case 'z':
         if (wcd_cwin.mode == WCD_NAV)
            pushZoom(wcd_cwin.zoomStack,wcd_cwin.curNode,&ymax);
         break;
      case Key_CTRL ('o'):
            popZoom(wcd_cwin.zoomStack,wcd_cwin.curNode,&ymax);
         break;
      case 'o':
      case 'Z':
         if (wcd_cwin.mode == WCD_NAV)
            popZoom(wcd_cwin.zoomStack,wcd_cwin.curNode,&ymax);
         break;
      case 3:  /* Control-C */
      case 27: /* Escape */
      case Key_CTRL (']'):
      case Key_CTRL ('x'):
      case Key_CTRL ('q'):
         if (wcd_cwin.mode == WCD_SEARCH)
            wcd_cwin.mode = WCD_NAV;
         else
            c = 13;
      break;
     case 13: /* Enter */
     case KEY_ENTER:
            c = 13;
            ptr = getZoomStackPath(wcd_cwin.zoomStack); /* s has size DD_MAXPATH */
            strcat(ptr,getNodeFullPath(wcd_cwin.curNode));
            wcd_fixpath(ptr,DD_MAXPATH);
      break;
     case 8:  /* backspace */
     case KEY_BACKSPACE:
         if (wcd_cwin.mode == WCD_NAV)
            wcd_cwin.curNode = findDirInCiclePrev(dirnodeGetName(wcd_cwin.curNode),wcd_cwin.curNode,1,ignore_case);

            if(n==1)
               wcd_cwin.mode = WCD_NAV;
            if(n>1) n--;
            wcd_cwin.str[n] = '\0';
     break;
     case 127: /* delete */
            if(n==1)
               wcd_cwin.mode = WCD_NAV;
            if(n>1) n--;
            wcd_cwin.str[n] = '\0';
     break;
     default:
     break;
    }

    ydiff -= (wcd_cwin.curNode->y);

      if ((wcd_cwin.mode == WCD_SEARCH) && ( c >= ' ') && ( c <= '~') && (n < WCD_MAX_INPSTR)) /* numbers */
      {
         wcd_cwin.str[n] = (char)c;
         n++;
         wcd_cwin.str[n] = '\0';
         ptr2 = wcd_cwin.str + 1;
         if (n>1)
         {
            if (wcd_cwin.str[0] == '/')
               wcd_cwin.curNode = findDirInCicle(ptr2,wcd_cwin.curNode,0,ignore_case);
            else
               wcd_cwin.curNode = findDirInCiclePrev(ptr2,wcd_cwin.curNode,0,ignore_case);
         }
      }
      dataRefresh(ydiff, 0);
   }

   endwin();
#ifdef XCURSES
   XCursesExit();
#endif
   
   printf("\n"); /* Extra newline for curses, pdcurses and when ncurses doesn't restore screen */

#ifdef MSDOS
   /* remove '/' before drive letter */
   if ((ptr != NULL)&&(*use_HOME)&&
         (*(ptr+1) != '/'))  /* UNC path */
      ptr++;
#endif
   wcd_fixpath(ptr,DD_MAXPATH);
   return(ptr);
   
}

#endif /* WCD_USECURSES */
