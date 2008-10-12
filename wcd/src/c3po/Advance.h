#ifndef ADVANCE_INCLUDED
#define ADVANCE_INCLUDED
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
   template<TARGS> static void advanceHookWcdStack(WcdStack w,
                                                   text Offset,
                                                    ARGS);
   template<TARGS> static void advanceWcdStack(WcdStack w,
                                               text Offset,
                                                ARGS);
   template<TARGS> static void advanceHookDirnode(dirnode d,
                                                  text Offset,
                                                   ARGS);
   template<TARGS> static void advanceDirnode(dirnode d,
                                              text Offset,
                                               ARGS);
   template<TARGS> static void advanceHookNameset(nameset n,
                                                  text Offset,
                                                   ARGS);
   template<TARGS> static void advanceNameset(nameset n,
                                              text Offset,
                                               ARGS);
   template<TARGS> static void advanceHookIntset(intset i,
                                                 text Offset,
                                                  ARGS);
   template<TARGS> static void advanceIntset(intset i,
                                             text Offset,
                                              ARGS);
   template<TARGS> static void advanceHookWcdStack(WcdStack w,
                                                   text Offset,
                                                    ARGS)
   {
      int index;

      index = 0;
      while(index < w->getSizeOfWcdStackDir)
      {

         index = index + 1;
      }
   }
   template<TARGS> static void advanceWcdStack(WcdStack w,
                                               text Offset,
                                                ARGS)
   {
      text increment = "   ";
      text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
      sprintf(new_Offset, "%s%s", Offset, increment);
      TRACE(fprintf(stdout, "%sAdvance : WcdStack\n", Offset);)
      TRACE(fprintf(stdout, "%s{\n", Offset);)

      if (w ne NULL)
         advanceHookWcdStack(w, new_Offset, ARGSREF);

      TRACE(fprintf(stdout, "%s} <- end Advance WcdStack\n", Offset);)

      free(new_Offset);
   }
   template<TARGS> static void advanceHookDirnode(dirnode d,
                                                  text Offset,
                                                   ARGS)
   {
      int index;

      index = 0;
      while(index < d->getSizeOfDirnode)
      {
         advanceDirnode(d->subdirs[index], Offset, ARGSREF);

         index = index + 1;
      }
   }
   template<TARGS> static void advanceDirnode(dirnode d,
                                              text Offset,
                                               ARGS)
   {
      text increment = "   ";
      text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
      sprintf(new_Offset, "%s%s", Offset, increment);
      TRACE(fprintf(stdout, "%sAdvance : dirnode\n", Offset);)
      TRACE(fprintf(stdout, "%s{\n", Offset);)

      if (d ne NULL)
         advanceHookDirnode(d, new_Offset, ARGSREF);

      TRACE(fprintf(stdout, "%s} <- end Advance dirnode\n", Offset);)

      free(new_Offset);
   }
   template<TARGS> static void advanceHookNameset(nameset n,
                                                  text Offset,
                                                   ARGS)
   {
      int index;

      index = 0;
      while(index < n->getSizeOfNamesetArray)
      {

         index = index + 1;
      }
   }
   template<TARGS> static void advanceNameset(nameset n,
                                              text Offset,
                                               ARGS)
   {
      text increment = "   ";
      text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
      sprintf(new_Offset, "%s%s", Offset, increment);
      TRACE(fprintf(stdout, "%sAdvance : nameset\n", Offset);)
      TRACE(fprintf(stdout, "%s{\n", Offset);)

      if (n ne NULL)
         advanceHookNameset(n, new_Offset, ARGSREF);

      TRACE(fprintf(stdout, "%s} <- end Advance nameset\n", Offset);)

      free(new_Offset);
   }
   template<TARGS> static void advanceHookIntset(intset i,
                                                 text Offset,
                                                  ARGS)
   {
      int index;

      index = 0;
      while(index < i->getSizeOfIntset)
      {

         index = index + 1;
      }
   }
   template<TARGS> static void advanceIntset(intset i,
                                             text Offset,
                                              ARGS)
   {
      text increment = "   ";
      text new_Offset = (text) malloc(sizeof(char) * (strlen(Offset) + strlen(increment) + 1));
      sprintf(new_Offset, "%s%s", Offset, increment);
      TRACE(fprintf(stdout, "%sAdvance : intset\n", Offset);)
      TRACE(fprintf(stdout, "%s{\n", Offset);)

      if (i ne NULL)
         advanceHookIntset(i, new_Offset, ARGSREF);

      TRACE(fprintf(stdout, "%s} <- end Advance intset\n", Offset);)

      free(new_Offset);
   }
#endif
