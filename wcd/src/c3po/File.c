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
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "std_macr.h"
#include "structur.h"
#include "Constants.h"

expfun FILE * openR(text filename)
{
   FILE *fi = fopen(filename, "r");

   return fi;
}
expfun FILE * openRwarn(text filename,
                        c3po_bool Warn)
{
   FILE *fi = fopen(filename, "r");
   if ((fi == NULL) && (Warn == true))
      fprintf(stderr, "\n** WARNING : couldn't open '%s', (ignored) ...\n", filename);

   return fi;
}
expfun c3po_bool existsFile(text filename)
{
   FILE *fp = openR(filename);
   if (fp != NULL)
   {
      fclose(fp);
      return true;
   }
   else
      return false;
}
expfun FILE * openW(text filename)
{
   FILE *fo = fopen(filename, "w");
   if (fo != NULL)
      fprintf(stderr, "\n- generating '%s'\n", filename);
   else
   {
      fprintf(stderr, "\n- couldn't open '%s', using stdout...\n", filename);
      fo = stdout;
   }

   return fo;
}
expfun FILE * openWBackup(text filename,
                          c3po_bool Backup)
{
   int index;
   char moveName[STRING_SIZE];
   if ((existsFile(filename) == true) &&
       (Backup == true))
   {
      sprintf(moveName, "%s.tmp", filename);
      index = 1;
      while(existsFile(moveName) == true)
      {
         sprintf(moveName, "%s.tmp%d", filename, index);
         index = index + 1;
      }

      fprintf(stderr, "\n- existing file '%s' has been moved to '%s',", filename, moveName);
      rename(filename, moveName);
   }

   return openW(filename);
}
expfun FILE * Append(text filename)
{
   FILE *fo = fopen(filename, "a");
   if (fo == NULL)
   {
      fprintf(stderr, "\n- couldn't append to '%s', using stdout...\n", filename);
      fo = stdout;
   }

   return fo;
}
expfun void Close(FILE* fp)
{
   if ((fp != NULL) &&
       (fp != stdout) &&
       (fp != stderr))
      fclose(fp);
}
expfun unsigned long getFileSize(text filename)
{
   struct stat s;
   if (stat(filename, &s) == 0)
      return (unsigned long) s.st_size;
   else
      return 0;
}
