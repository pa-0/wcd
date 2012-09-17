#ifndef STD_MACRO_INCLUDED
#define STD_MACRO_INCLUDED
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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#define    eq        ==             /* Logical 'equals'                       */
#define    ne        !=             /* Logical inequality                     */
#define    not       !              /* Logical 'NOT'                          */
#define    bnot      ~              /* Bitwise 'NOT'  (one's complement)      */
#define    cand      &&             /* Conditional 'AND'                      */
#define    and       &&             /* Logical 'AND'                          */
#define    band      &              /* Bitwise 'AND'                          */
#define    cor       ||             /* Conditional 'OR'                       */
#define    or        ||             /* Logical 'OR'                           */
#define    bor       |              /* Bitwise 'OR'                           */
#define    bxor      ^              /* Bitwise exclusive 'OR'                 */
/* Definition of 'mod' gives problems with WinBase.h (MS Visual C++)  */
/*#define    mod       %               Remainder operator                     */

#ifndef    NULL
#define    NULL      0              /* Null pointer                           */
#endif

#ifndef    EOF
#define    EOF      -1              /* end of file                            */
#endif

#define    true       1
#define    false      0

#define    expvar                   /* export variable                        */
#define    expfun                   /* export function                        */

#ifdef __cplusplus
#define c3po_bool _c3po_bool
#endif
typedef    int c3po_bool;                /* pseudo c3po_boolean                         */

#ifdef SETTRACE
#define TRACE(x) x
#else
#define TRACE(x)
#endif

#endif
