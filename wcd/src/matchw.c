/*---------------------------------------------------------------------------

  match.c

  The match() routine recursively compares a string to a "pattern" (regular
  expression), returning TRUE if a match is found or FALSE if not.  This
  version is specifically for use with unzip.c:  as did the previous match()
  routines from SEA and J. Kercheval, it leaves the case (upper, lower, or
  mixed) of the string alone, but converts any uppercase characters in the
  pattern to lowercase if indicated by the global var pInfo->lcflag (which
  is to say, string is assumed to have been converted to lowercase already,
  if such was necessary).

  GRR:  reversed order of text, pattern in matche() (now same as match());
        added ignore_case/ic flags, Case() macro.

  PK:   replaced matche() with recmatch() from Zip, modified to have an
        ignore_case argument; replaced test frame with simpler one.

  ---------------------------------------------------------------------------

  matchw.c : A modified version of match.c with Unicode matching.

  EW:   * Option to ignore diacritics.
        * Unicode normalistion.

  ---------------------------------------------------------------------------

  Copyright on recmatch() from Zip's util.c (although recmatch() was almost
  certainly written by Mark Adler...ask me how I can tell :-) ):

     Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
     Kai Uwe Rommel and Igor Mandrichenko.
     Copyright (C) 2011 Erwin Waterlander

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

  ---------------------------------------------------------------------------

   Info-ZIP's home WWW site is listed on Yahoo and is at:

   http://www.cdrom.com/pub/infozip/

   e-mail : Zip-Bugs@lists.wku.edu


  ---------------------------------------------------------------------------

  Match the pattern (wildcard) against the string (fixed):

     match(string, pattern, ignore_case);

  returns TRUE if string matches pattern, FALSE otherwise.  In the pattern:

     `*' matches any sequence of characters (zero or more)
     `?' matches any character
     [SET] matches any character in the specified set,
     [!SET] or [^SET] matches any character not in the specified set.

  A set is composed of characters or ranges; a range looks like ``character
  hyphen character'' (as in 0-9 or A-Z).  [0-9a-zA-Z_] is the minimal set of
  characters allowed in the [..] pattern construct.  Other characters are
  allowed (ie. 8 bit characters) if your system will support them.

  To suppress the special syntactic significance of any of ``[]*?!^-\'', in-
  side or outside a [..] construct and match the character exactly, precede
  it with a ``\'' (backslash).

  Note that "*.*" and "*." are treated specially under MS-DOS if DOSWILD is
  defined.  See the DOSWILD section below for an explanation.

  ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#ifdef WCD_UNINORM
#  include <uninorm.h>  /* part of libunistring */
#endif
#include "wcd.h"
#include "display.h"
#include "finddirs.h"
#include "matchw.h" /* define ToLower() in here (for Unix, define ToLower
                       * to be macro (using isupper()); otherwise just use
                       * tolower() */
#define Case(x)  (ic? towlower(x) : (wint_t)(x))

static wint_t match_C[0x250] =
{
   0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07,  0x08,  0x09,  0x0a,  0x0b,  0x0c,  0x0d,  0x0e,  0x0f,
   0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x1a,  0x1b,  0x1c,  0x1d,  0x1e,  0x1f,
   0x20,  0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2a,  0x2b,  0x2c,  0x2d,  0x2e,  0x2f,
   0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3a,  0x3b,  0x3c,  0x3d,  0x3e,  0x3f,
   0x40,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4a,  0x4b,  0x4c,  0x4d,  0x4e,  0x4f,
   0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5a,  0x5b,  0x5c,  0x5d,  0x5e,  0x5f,
   0x60,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6a,  0x6b,  0x6c,  0x6d,  0x6e,  0x6f,
   0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7a,  0x7b,  0x7c,  0x7d,  0x7e,  0x7f,
   0x80,  0x81,  0x82,  0x83,  0x84,  0x85,  0x86,  0x87,  0x88,  0x89,  0x8a,  0x8b,  0x8c,  0x8d,  0x8e,  0x8f,
   0x90,  0x91,  0x92,  0x93,  0x94,  0x95,  0x96,  0x97,  0x98,  0x99,  0x9a,  0x9b,  0x9c,  0x9d,  0x9e,  0x9f,
   0xa0,  0xa1,  0xa2,  0xa3,  0xa4,  0xa5,  0xa6,  0xa7,  0xa8,  0xa9,  0xaa,  0xab,  0xac,  0xad,  0xae,  0xaf,
   0xb0,  0xb1,  0xb2,  0xb3,  0xb4,  0xb5,  0xb6,  0xb7,  0xb8,  0xb9,  0xba,  0xbb,  0xbc,  0xbd,  0xbe,  0xbf,
   0xc0,  0xc1,  0xc2,  0xc3,  0xc4,  0xc5,  0xc6,  0xc7,  0xc8,  0xc9,  0xca,  0xcb,  0xcc,  0xcd,  0xce,  0xcf,
   0xd0,  0xd1,  0xd2,  0xd3,  0xd4,  0xd5,  0xd6,  0xd7,  0xd8,  0xd9,  0xda,  0xdb,  0xdc,  0xdd,  0xde,  0xdf,
   0xe0,  0xe1,  0xe2,  0xe3,  0xe4,  0xe5,  0xe6,  0xe7,  0xe8,  0xe9,  0xea,  0xeb,  0xec,  0xed,  0xee,  0xef,
   0xf0,  0xf1,  0xf2,  0xf3,  0xf4,  0xf5,  0xf6,  0xf7,  0xf8,  0xf9,  0xfa,  0xfb,  0xfc,  0xfd,  0xfe,  0xff,
  0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10a, 0x10b, 0x10c, 0x10d, 0x10e, 0x10f,
  0x110, 0x111, 0x112, 0x113, 0x114, 0x115, 0x116, 0x117, 0x118, 0x119, 0x11a, 0x11b, 0x11c, 0x11d, 0x11e, 0x11f,
  0x120, 0x121, 0x122, 0x123, 0x124, 0x125, 0x126, 0x127, 0x128, 0x129, 0x12a, 0x12b, 0x12c, 0x12d, 0x12e, 0x12f,
  0x130, 0x131, 0x132, 0x133, 0x134, 0x135, 0x136, 0x137, 0x138, 0x139, 0x13a, 0x13b, 0x13c, 0x13d, 0x13e, 0x13f,
  0x140, 0x141, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147, 0x148, 0x149, 0x14a, 0x14b, 0x14c, 0x14d, 0x14e, 0x14f,
  0x150, 0x151, 0x152, 0x153, 0x154, 0x155, 0x156, 0x157, 0x158, 0x159, 0x15a, 0x15b, 0x15c, 0x15d, 0x15e, 0x15f,
  0x160, 0x161, 0x162, 0x163, 0x164, 0x165, 0x166, 0x167, 0x168, 0x169, 0x16a, 0x16b, 0x16c, 0x16d, 0x16e, 0x16f,
  0x170, 0x171, 0x172, 0x173, 0x174, 0x175, 0x176, 0x177, 0x178, 0x179, 0x17a, 0x17b, 0x17c, 0x17d, 0x17e, 0x17f,
  0x180, 0x181, 0x182, 0x183, 0x184, 0x185, 0x186, 0x187, 0x188, 0x189, 0x18a, 0x18b, 0x18c, 0x18d, 0x18e, 0x18f,
  0x190, 0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199, 0x19a, 0x19b, 0x19c, 0x19d, 0x19e, 0x19f,
  0x1a0, 0x1a1, 0x1a2, 0x1a3, 0x1a4, 0x1a5, 0x1a6, 0x1a7, 0x1a8, 0x1a9, 0x1aa, 0x1ab, 0x1ac, 0x1ad, 0x1ae, 0x1af,
  0x1b0, 0x1b1, 0x1b2, 0x1b3, 0x1b4, 0x1b5, 0x1b6, 0x1b7, 0x1b8, 0x1b9, 0x1ba, 0x1bb, 0x1bc, 0x1bd, 0x1be, 0x1bf,
  0x1c0, 0x1c1, 0x1c2, 0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7, 0x1c8, 0x1c9, 0x1ca, 0x1cb, 0x1cc, 0x1cd, 0x1ce, 0x1cf,
  0x1d0, 0x1d1, 0x1d2, 0x1d3, 0x1d4, 0x1d5, 0x1d6, 0x1d7, 0x1d8, 0x1d9, 0x1da, 0x1db, 0x1dc, 0x1dd, 0x1de, 0x1df,
  0x1e0, 0x1e1, 0x1e2, 0x1e3, 0x1e4, 0x1e5, 0x1e6, 0x1e7, 0x1e8, 0x1e9, 0x1ea, 0x1eb, 0x1ec, 0x1ed, 0x1ee, 0x1ef,
  0x1f0, 0x1f1, 0x1f2, 0x1f3, 0x1f4, 0x1f5, 0x1f6, 0x1f7, 0x1f8, 0x1f9, 0x1fa, 0x1fb, 0x1fc, 0x1fd, 0x1fe, 0x1ff,
  0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209, 0x20a, 0x20b, 0x20c, 0x20d, 0x20e, 0x20f,
  0x210, 0x211, 0x212, 0x213, 0x214, 0x215, 0x216, 0x217, 0x218, 0x219, 0x21a, 0x21b, 0x21c, 0x21d, 0x21e, 0x21f,
  0x220, 0x221, 0x222, 0x223, 0x224, 0x225, 0x226, 0x227, 0x228, 0x229, 0x22a, 0x22b, 0x22c, 0x22d, 0x22e, 0x22f,
  0x230, 0x231, 0x232, 0x233, 0x234, 0x235, 0x236, 0x237, 0x238, 0x239, 0x23a, 0x23b, 0x23c, 0x23d, 0x23e, 0x23f,
  0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x247, 0x248, 0x249, 0x24a, 0x24b, 0x24c, 0x24d, 0x24e, 0x24f
};

static wint_t match_Unicode[0x250] =
{
  0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07,  0x08,  0x09,  0x0a,  0x0b,  0x0c,  0x0d,  0x0e,  0x0f,
  0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,  0x1a,  0x1b,  0x1c,  0x1d,  0x1e,  0x1f,
  0x20,  0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2a,  0x2b,  0x2c,  0x2d,  0x2e,  0x2f,
  0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3a,  0x3b,  0x3c,  0x3d,  0x3e,  0x3f,
  0x40,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4a,  0x4b,  0x4c,  0x4d,  0x4e,  0x4f,
  0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5a,  0x5b,  0x5c,  0x5d,  0x5e,  0x5f,
  0x60,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6a,  0x6b,  0x6c,  0x6d,  0x6e,  0x6f,
  0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7a,  0x7b,  0x7c,  0x7d,  0x7e,  0x7f,
  0x80,  0x81,  0x82,  0x83,  0x84,  0x85,  0x86,  0x87,  0x88,  0x89,  0x8a,  0x8b,  0x8c,  0x8d,  0x8e,  0x8f, /* Latin-1 */
  0x90,  0x91,  0x92,  0x93,  0x94,  0x95,  0x96,  0x97,  0x98,  0x99,  0x9a,  0x9b,  0x9c,  0x9d,  0x9e,  0x9f,
  0xa0,  0xa1,  0xa2,  0xa3,  0xa4,  0xa5,  0xa6,  0xa7,  0xa8,  0xa9,  0xaa,  0xab,  0xac,  0xad,  0xae,  0xaf,
  0xb0,  0xb1,  0xb2,  0xb3,  0xb4,  0xb5,  0xb6,  0xb7,  0xb8,  0xb9,  0xba,  0xbb,  0xbc,  0xbd,  0xbe,  0xbf,
  0x41,  0x41,  0x41,  0x41,  0x41,  0x41,  0xc6,  0x43,  0x45,  0x45,  0x45,  0x45,  0x49,  0x49,  0x49,  0x49,
  0x44,  0x4e,  0x4f,  0x4f,  0x4f,  0x4f,  0x4f,  0xd7,  0x4f,  0x55,  0x55,  0x55,  0x55,  0x59,  0xde,  0xdf,
  0x61,  0x61,  0x61,  0x61,  0x61,  0x61,  0xe6,  0x63,  0x65,  0x65,  0x65,  0x65,  0x69,  0x69,  0x69,  0x69,
  0xf0,  0x6e,  0x6f,  0x6f,  0x6f,  0x6f,  0x6f,  0xf7,  0x6f,  0x75,  0x75,  0x75,  0x75,  0x79,  0xfe,  0x79,
  0x41,  0x61,  0x41,  0x61,  0x41,  0x61,  0x43,  0x63,  0x43,  0x63,  0x43,  0x63,  0x43,  0x63,  0x44,  0x64, /* Latin Extended-A */
  0x44,  0x64,  0x45,  0x65,  0x45,  0x65,  0x45,  0x65,  0x45,  0x65,  0x45,  0x65,  0x47,  0x67,  0x47,  0x67,
  0x47,  0x67,  0x47,  0x67,  0x48,  0x68,  0x48,  0x68,  0x49,  0x69,  0x49,  0x69,  0x49,  0x69,  0x49,  0x69,
  0x49,  0x69, 0x132, 0x133,  0x4a,  0x6a,  0x4b,  0x6b, 0x138,  0x4c,  0x6c,  0x4c,  0x6c,  0x4c,  0x6c,  0x4c,
  0x6c,  0x4c,  0x6c,  0x4e,  0x6e,  0x4e,  0x6e,  0x4e,  0x6e,  0x6e,  0x4e,  0x6e,  0x4f,  0x6f,  0x4f,  0x6f,
  0x4f,  0x6f, 0x152, 0x153,  0x52,  0x72,  0x52,  0x72,  0x52,  0x72,  0x53,  0x73,  0x53,  0x73,  0x53,  0x73,
  0x53,  0x73,  0x54,  0x74,  0x54,  0x74,  0x54,  0x74,  0x55,  0x75,  0x55,  0x75,  0x55,  0x75,  0x55,  0x75,
  0x55,  0x75,  0x55,  0x75,  0x57,  0x77,  0x59,  0x79,  0x59,  0x5a,  0x7a,  0x5a,  0x7a,  0x5a,  0x7a, 0x17f,
  0x62,  0x42,  0x42,  0x62, 0x184, 0x185, 0x186,  0x43,  0x63,  0x44,  0x44,  0x44,  0x64, 0x18d, 0x18e, 0x18f, /* Latin Extended-B */
  0x45,  0x46,  0x66,  0x47, 0x194, 0x195,  0x49, 0x197,  0x4b,  0x6b, 0x19a, 0x19b, 0x19c,  0x4e,  0x6e,  0x4f,
  0x4f,  0x6f, 0x1a2, 0x1a3,  0x50,  0x70, 0x1a6, 0x1a7, 0x1a8, 0x1a9, 0x1aa,  0x74,  0x54,  0x74,  0x54,  0x55,
  0x75, 0x1b1,  0x56,  0x59,  0x79,  0x5a,  0x7a, 0x1b7, 0x1b8, 0x1b9, 0x1ba,  0x32, 0x1bc, 0x1bd, 0x1be, 0x1bf,
 0x1c0, 0x1c1, 0x1c2, 0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7, 0x1c8, 0x1c9, 0x1ca, 0x1cb, 0x1cc,  0x41,  0x61,  0x49,
  0x69,  0x4f,  0x6f,  0x55,  0x75,  0x55,  0x75,  0x55,  0x75,  0x55,  0x75,  0x55,  0x75, 0x1dd,  0x41,  0x61,
  0x41,  0x61, 0x1e2, 0x1e3,  0x47,  0x67,  0x47,  0x67,  0x4b,  0x6b,  0x4f,  0x6f,  0x4f,  0x6f, 0x1ee, 0x1ef,
  0x6a, 0x1f1, 0x1f2, 0x1f3,  0x47,  0x67, 0x1f6, 0x1f7,  0x4e,  0x6e,  0x41,  0x61, 0x1fc, 0x1fd,  0x4f,  0x6f,
  0x41,  0x61,  0x41,  0x61,  0x45,  0x65,  0x45,  0x65,  0x49,  0x69,  0x49,  0x69,  0x4f,  0x6f,  0x4f,  0x6f,
  0x52,  0x72,  0x52,  0x72,  0x55,  0x75,  0x55,  0x75,  0x53,  0x73,  0x54,  0x74, 0x21c, 0x21d,  0x48,  0x68,
  0x4e,  0x64, 0x222, 0x223,  0x5a,  0x7a,  0x41,  0x61,  0x45,  0x65,  0x4f,  0x6f,  0x4f,  0x6f,  0x4f,  0x6f,
  0x4f,  0x6f,  0x59,  0x79,  0x6c,  0x6e,  0x74,  0x6a, 0x238, 0x239,  0x41,  0x43,  0x63,  0x4c,  0x54,  0x73,
  0x7a, 0x241, 0x242,  0x42,  0x55, 0x245,  0x45,  0x65,  0x4a,  0x6a,  0x71,  0x71,  0x52,  0x72,  0x59,  0x79
};


/* dd_matchwcs() is a shell to recmatch() to return only Boolean values. */
static int recmatchwcs(wchar_t *pattern, wchar_t *string, int ignore_case, wint_t *CPTable);

int dd_matchmbs(const char *string, const char *pattern, int ignore_case, int ignore_diacritics)
{
    static wchar_t wstr_string[WCD_MAXPATH];
    static wchar_t wstr_pattern[WCD_MAXPATH];
    size_t len1,len2;

    len1 = MBSTOWCS(wstr_string , string ,(size_t)WCD_MAXPATH);
    len2 = MBSTOWCS(wstr_pattern, pattern,(size_t)WCD_MAXPATH);
    if ((len1 == (size_t)(-1)) || (len2 == (size_t)(-1)))
        return 0;
    return(dd_matchwcs(wstr_string, wstr_pattern, ignore_case, ignore_diacritics));
}

int dd_matchwcs(const wchar_t *string,const wchar_t *pattern,int ignore_case, int ignore_diacritics)
{
#if (defined(__MSDOS__) && defined(DOSWILD))
    wchar_t *dospattern;
    size_t j = wcslen(pattern);
#endif
    wint_t *CPTable;
    int result;

#ifdef WCD_UNINORM
    static wchar_t string_buffer[WCD_MAXPATH];
    static wchar_t pattern_buffer[WCD_MAXPATH];
    size_t lengthp = WCD_MAXPATH;
    wchar_t *string_normalized;
    wchar_t *pattern_normalized;

#  if defined(_WIN32) || defined(__CYGWIN__)
    /* Normalization. Composition, such that we can ignore diacritics. */
    string_normalized  = u16_normalize (UNINORM_NFKC, string,  wcslen(string) +1, string_buffer,  &lengthp);
    if (string_normalized == NULL)
        return(0);
    pattern_normalized = u16_normalize (UNINORM_NFKC, pattern, wcslen(pattern) +1, pattern_buffer, &lengthp);
    if (pattern_normalized == NULL)
        return(0);
#  else
    string_normalized  = u32_normalize (UNINORM_NFKC, string,  wcslen(string) +1, string_buffer,  &lengthp);
    if (string_normalized == NULL)
        return(0);
    pattern_normalized = u32_normalize (UNINORM_NFKC, pattern, wcslen(pattern) +1, pattern_buffer, &lengthp);
    if (pattern_normalized == NULL)
        return(0);
#  endif
#else
    /* No normalization */
    const wchar_t *string_normalized;
    const wchar_t *pattern_normalized;

    string_normalized = string;
    pattern_normalized = pattern;
#endif
    if (ignore_diacritics == 0)
        CPTable = match_C;
    else
        CPTable = match_Unicode;

#if (defined(__MSDOS__) && defined(DOSWILD))
/*---------------------------------------------------------------------------
    Optional MS-DOS preprocessing section:  compare last three chars of the
    wildcard to "*.*" and translate to "*" if found; else compare the last
    two characters to "*." and, if found, scan the non-wild string for dots.
    If in the latter case a dot is found, return failure; else translate the
    "*." to "*".  In either case, continue with the normal (Unix-like) match
    procedure after translation.  (If not enough memory, default to normal
    match.)  This causes "a*.*" and "a*." to behave as MS-DOS users expect.
  ---------------------------------------------------------------------------*/

    if ((dospattern = (wchar_t *)malloc((j+1) * sizeof(wchar_t))) != NULL) {
        wcscpy(dospattern, pattern_normalized);
        if (!wcscmp(dospattern+j-3, L"*.*")) {
            dospattern[j-2] = '\0';                    /* nuke the ".*" */
        } else if (!wcscmp(dospattern+j-2, L"*.")) {
            wchar_t *p = wcschr(string_normalized, L'.');

            if (p) {   /* found a dot:  match fails */
                free(dospattern);
                return 0;
            }
            dospattern[j-1] = '\0';                    /* nuke the end "." */
        }
        result = recmatchwcs((wchar_t *)dospattern, (wchar_t *)string_normalized, ignore_case, CPTable);
        free(dospattern);
#ifdef WCD_UNINORM
        if (string_normalized != string_buffer)
            free(string_normalized);
        if (pattern_normalized != pattern_buffer)
            free(pattern_normalized);
#endif
        return result == 1;
    } else
#endif /* __MSDOS__ && DOSWILD */
    result = recmatchwcs((wchar_t *)pattern_normalized, (wchar_t *)string_normalized, ignore_case, CPTable) == 1;
#ifdef WCD_UNINORM
    if (string_normalized != string_buffer)
        free(string_normalized);
    if (pattern_normalized != pattern_buffer)
        free(pattern_normalized);
#endif
    return(result);
}


static int recmatchwcs(wchar_t *p,wchar_t *s,int ic, wint_t *CPTable)
   /*  wchar_t *p;  			 sh pattern to match */
   /*  wchar_t *s;  			 string to which to match it */
   /*  int ic;  			 true for case insensitivity */
/* Recursively compare the sh pattern p with the string s and return 1 if
 * they match, and 0 or 2 if they don't or if there is a syntax error in the
 * pattern.  This routine recurses on itself no more deeply than the number
 * of characters in the pattern. */
{
    wint_t c;       /* pattern char or start of range in [-] loop */
    int result;

    /* Get first character, the pattern for new recmatch calls follows */
    c = *p++;

    /* If that was the end of the pattern, match if string empty too */
    if (c == 0)
        return *s == 0;

    /* '?' (or '%') matches any character (but not an empty string) */
#ifdef VMS
    if (c == L'%')         /* GRR:  make this conditional, too? */
#else /* !VMS */
    if (c == L'?')
#endif /* ?VMS */
        return *s ? recmatchwcs(p, s + 1, ic, CPTable) : 0;

    /* '*' matches any number of characters, including zero */
#ifdef AMIGA
    if (c == L'#' && *p == L'?')     /* "#?" is Amiga-ese for "*" */
        c = L'*', p++;
#endif /* AMIGA */
    if (c == L'*') {
        if (*p == 0)
            return 1;
        for (; *s; s++)
            if ((result = recmatchwcs(p, s, ic, CPTable)) != 0)
                return result;
        return 2;       /* 2 means give up--match will return false */
    }

#ifndef VMS             /* No bracket matching in VMS */
    /* Parse and process the list of characters and ranges in brackets */
    if (c == L'[') {
        wint_t e;      /* flag true if next char to be taken literally */
        wchar_t *q;    /* pointer to end of [-] group */
        wint_t r;      /* flag true to match anything but the range */

        if (*s == 0)                           /* need a character to match */
            return 0;
        p += (r = (*p == L'!' || *p == L'^'));   /* see if reverse */
        for (q = p, e = 0; *q; q++)            /* find closing bracket */
            if (e)
                e = 0;
            else
                if (*q == '\\')      /* GRR:  change to ^ for MS-DOS, OS/2? */
                    e = 1;
                else if (*q == L']')
                    break;
        if (*q != L']')               /* nothing matches if bad syntax */
            return 0;
        for (c = 0, e = *p == L'-'; p < q; p++) {  /* go through the list */
            if (e == 0 && *p == L'\\')             /* set escape flag if \ */
                e = 1;
            else if (e == 0 && *p == L'-')         /* set start of range if - */
                c = *(p-1);
            else {
                wint_t cc = Case((wint_t)*s);

                if (*(p+1) != L'-')
                    for (c = c ? c : (wint_t)*p; c <= (wint_t)*p; c++)  /* compare range */
                    {
                        if ((c < 0x250) && (cc < 0x250))
                            result = Case(CPTable[c]) == Case(CPTable[cc]);
                        else
                            result = Case(c) == Case(cc);
                        if (result)
                            return r ? 0 : recmatchwcs(q + 1, s + 1, ic, CPTable);
                    }
                c = e = 0;   /* clear range, escape flags */
            }
        }
        return r ? recmatchwcs(q + 1, s + 1, ic, CPTable) : 0;  /* bracket match failed */
    }
#endif /* !VMS */

    /* if escape ('\'), just compare next character */
    if (c == '\\' && (c = *p++) == 0)     /* if \ at end, then syntax error */
        return 0;

    /* just a character--compare it */
    if ((c < 0x250) && (*s < 0x250))
        result = Case(CPTable[c]) == Case(CPTable[*s]);
    else
        result = Case(c) == Case((wint_t)*s);
    return result ? recmatchwcs(p, ++s, ic, CPTable) : 0;

} /* end function recmatch() */




#ifdef WILD_STAT_BUG   /* Turbo/Borland C, Watcom C, VAX C, Atari MiNT libs */

int dd_iswildl(const wchar_t *p)
{
    for (; *p; ++p)
        if (*p == '\\' && *(p+1))
            ++p;
#ifdef VMS
        else if (*p == L'%' || *p == L'*')
#else /* !VMS */
#ifdef AMIGA
        else if (*p == L'?' || *p == L'*' || (*p==L'#' && p[1]==L'?') || *p == L'[')
#else /* !AMIGA */
        else if (*p == L'?' || *p == L'*' || *p == L'[')
#endif /* ?AMIGA */
#endif /* ?VMS */
            return 1;

    return 0;

} /* end function dd_iswild() */

#endif /* WILD_STAT_BUG */




#ifdef TEST_MATCH
/* replaced gets() with fgets(). gets() can be dangerous,
   has known to give security problems, because it does not
   check for bufferoverflow.
   Erwin Waterlander, Jul 22 1998 */

#define put(s) { fputs(s, stdout); fflush(stdout); }

void main(void)
{
    wchar_t pat[256], str[256];

    for (;;) {
        put("Pattern (return to exit): ");
        fgetws(pat,100,stdin);
        if (!pat[0])
            break;
        for (;;) {
            put("String (return for new pattern): ");
            fgetws(str,100,stdin);
            if (!str[0])
                break;
            printf("Case sensitive: %s  insensitive: %s\n",
              dd_matchwcs(str, pat, 0) ? "YES" : "NO",
              dd_matchwcs(str, pat, 1) ? "YES" : "NO");
        }
    }
    exit(0);
}

#endif /* TEST_MATCH */
