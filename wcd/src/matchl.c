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

  matchl.c : A modified version of match.c with locale matching.

  EW:   For several 8 bit encodings characters with accents a.s.o. match
        the corresponding ASCII chararcter without accent .

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

#include <string.h>
#include <ctype.h>
#include "querycp.h"
#include "matchl.h" /* define ToLower() in here (for Unix, define ToLower
                       * to be macro (using isupper()); otherwise just use
                       * tolower() */
#define Case(x)  (ic? tolower(x) : (x))

static uch match_C[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

static uch match_CP437[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x43', '\x75', '\x65', '\x61', '\x61', '\x61', '\x61', '\x63', '\x65', '\x65', '\x65', '\x69', '\x69', '\x69', '\x41', '\x41',
  '\x45', '\x91', '\x92', '\x6f', '\x6f', '\x6f', '\x75', '\x75', '\x79', '\x47', '\x55', '\x63', '\x9c', '\x9d', '\x9e', '\x9f',
  '\x61', '\x69', '\x6f', '\x75', '\x6e', '\x4e', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

static uch match_CP850[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x43', '\x75', '\x65', '\x61', '\x61', '\x61', '\x61', '\x63', '\x65', '\x65', '\x65', '\x69', '\x69', '\x69', '\x41', '\x41',
  '\x45', '\x91', '\x92', '\x6f', '\x6f', '\x6f', '\x75', '\x75', '\x79', '\x4f', '\x55', '\x6f', '\x9c', '\x4f', '\x9e', '\x9f',
  '\x61', '\x69', '\x6f', '\x75', '\x6e', '\x4e', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\x41', '\x41', '\x41', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\x61', '\x41', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\x45', '\x45', '\x45', '\x69', '\x49', '\x49', '\x49', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\x49', '\xdf',
  '\x4f', '\xe1', '\x4f', '\x4f', '\x6f', '\x4f', '\xe6', '\xe7', '\xe8', '\x55', '\x55', '\x55', '\x79', '\x59', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

static uch match_CP1252[256] =
{
  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x53', '\x8b', '\x8c', '\x8d', '\x5a', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x73', '\x9b', '\x9c', '\x9d', '\x7a', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\x41', '\x41', '\x41', '\x41', '\x41', '\x41', '\xc6', '\x43', '\x45', '\x45', '\x45', '\x45', '\x49', '\x49', '\x49', '\x49',
  '\x44', '\x4e', '\x4f', '\x4f', '\x4f', '\x4f', '\x4f', '\xd7', '\x4f', '\x55', '\x55', '\x55', '\x55', '\x59', '\xde', '\xdf',
  '\x61', '\x61', '\x61', '\x61', '\x61', '\x61', '\xe6', '\x87', '\x65', '\x65', '\x65', '\x65', '\x69', '\x69', '\x69', '\x69',
  '\xf0', '\x6e', '\x6f', '\x6f', '\x6f', '\x6f', '\x6f', '\xf7', '\x6f', '\x75', '\x75', '\x75', '\x75', '\x79', '\xfe', '\x79',
};

/* dd_matchl() is a shell to recmatch() to return only Boolean values. */
static int recmatchl(uch *pattern, uch *string, int ignore_case, uch *CPTable);

int dd_matchl(const char *string,const char *pattern,int ignore_case)
{
#if (defined(MSDOS) && defined(DOSWILD))
    char *dospattern;
    int j = strlen(pattern);
#endif
    int code_page;
    uch *CPTable;

    code_page = (int)query_con_codepage();

    switch (code_page)
    {
      case 437: /* DOS Latin-US */
        CPTable = match_CP437;
        break;
      case 850: /* DOS Latin-1 */
        CPTable = match_CP850;
        break;
      case 1252: /* Windows Latin-1 */
        CPTable = match_CP1252;
        break;
      default: /* C */
        CPTable = match_C;
    }

#if (defined(MSDOS) && defined(DOSWILD))
/*---------------------------------------------------------------------------
    Optional MS-DOS preprocessing section:  compare last three chars of the
    wildcard to "*.*" and translate to "*" if found; else compare the last
    two characters to "*." and, if found, scan the non-wild string for dots.
    If in the latter case a dot is found, return failure; else translate the
    "*." to "*".  In either case, continue with the normal (Unix-like) match
    procedure after translation.  (If not enough memory, default to normal
    match.)  This causes "a*.*" and "a*." to behave as MS-DOS users expect.
  ---------------------------------------------------------------------------*/

    if ((dospattern = (char *)malloc(j+1)) != NULL) {
        strcpy(dospattern, pattern);
        if (!strcmp(dospattern+j-3, "*.*")) {
            dospattern[j-2] = '\0';                    /* nuke the ".*" */
        } else if (!strcmp(dospattern+j-2, "*.")) {
            char *p = strchr(string, '.');

            if (p) {   /* found a dot:  match fails */
                free(dospattern);
                return 0;
            }
            dospattern[j-1] = '\0';                    /* nuke the end "." */
        }
        j = recmatchl((uch *)dospattern, (uch *)string, ignore_case, CPTable);
        free(dospattern);
        return j == 1;
    } else
#endif /* MSDOS && DOSWILD */
    return recmatchl((uch *)pattern, (uch *)string, ignore_case, CPTable) == 1;
}


static int recmatchl(uch *p,uch *s,int ic, uch *CPTable)
   /*  uch *p;  			 sh pattern to match */
   /*  uch *s;  			 string to which to match it */
   /*  int ic;  			 true for case insensitivity */
/* Recursively compare the sh pattern p with the string s and return 1 if
 * they match, and 0 or 2 if they don't or if there is a syntax error in the
 * pattern.  This routine recurses on itself no more deeply than the number
 * of characters in the pattern. */
{
    unsigned int c;       /* pattern char or start of range in [-] loop */

    /* Get first character, the pattern for new recmatch calls follows */
    c = *p++;

    /* If that was the end of the pattern, match if string empty too */
    if (c == 0)
        return *s == 0;

    /* '?' (or '%') matches any character (but not an empty string) */
#ifdef VMS
    if (c == '%')         /* GRR:  make this conditional, too? */
#else /* !VMS */
    if (c == '?')
#endif /* ?VMS */
        return *s ? recmatchl(p, s + 1, ic, CPTable) : 0;

    /* '*' matches any number of characters, including zero */
#ifdef AMIGA
    if (c == '#' && *p == '?')     /* "#?" is Amiga-ese for "*" */
        c = '*', p++;
#endif /* AMIGA */
    if (c == '*') {
        if (*p == 0)
            return 1;
        for (; *s; s++)
            if ((c = recmatchl(p, s, ic, CPTable)) != 0)
                return (int)c;
        return 2;       /* 2 means give up--match will return false */
    }

#ifndef VMS             /* No bracket matching in VMS */
    /* Parse and process the list of characters and ranges in brackets */
    if (c == '[') {
        int e;          /* flag true if next char to be taken literally */
        uch *q;         /* pointer to end of [-] group */
        int r;          /* flag true to match anything but the range */

        if (*s == 0)                           /* need a character to match */
            return 0;
        p += (r = (*p == '!' || *p == '^'));   /* see if reverse */
        for (q = p, e = 0; *q; q++)            /* find closing bracket */
            if (e)
                e = 0;
            else
                if (*q == '\\')      /* GRR:  change to ^ for MS-DOS, OS/2? */
                    e = 1;
                else if (*q == ']')
                    break;
        if (*q != ']')               /* nothing matches if bad syntax */
            return 0;
        for (c = 0, e = *p == '-'; p < q; p++) {  /* go through the list */
            if (e == 0 && *p == '\\')             /* set escape flag if \ */
                e = 1;
            else if (e == 0 && *p == '-')         /* set start of range if - */
                c = *(p-1);
            else {
                unsigned int cc = Case(*s);

                if (*(p+1) != '-')
                    for (c = c ? c : *p; c <= *p; c++)  /* compare range */
                        if (Case(c) == cc)
                            return r ? 0 : recmatchl(q + 1, s + 1, ic, CPTable);
                c = e = 0;   /* clear range, escape flags */
            }
        }
        return r ? recmatchl(q + 1, s + 1, ic, CPTable) : 0;  /* bracket match failed */
    }
#endif /* !VMS */

    /* if escape ('\'), just compare next character */
    if (c == '\\' && (c = *p++) == 0)     /* if \ at end, then syntax error */
        return 0;

    /* just a character--compare it */
    return Case(CPTable[c]) == Case(CPTable[*s]) ? recmatchl(p, ++s, ic, CPTable) : 0;

} /* end function recmatch() */




#ifdef WILD_STAT_BUG   /* Turbo/Borland C, Watcom C, VAX C, Atari MiNT libs */

int dd_iswildl(const char *p)
{
    for (; *p; ++p)
        if (*p == '\\' && *(p+1))
            ++p;
#ifdef VMS
        else if (*p == '%' || *p == '*')
#else /* !VMS */
#ifdef AMIGA
        else if (*p == '?' || *p == '*' || (*p=='#' && p[1]=='?') || *p == '[')
#else /* !AMIGA */
        else if (*p == '?' || *p == '*' || *p == '[')
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
    char pat[256], str[256];

    for (;;) {
        put("Pattern (return to exit): ");
        fgets(pat,100,stdin);
        if (!pat[0])
            break;
        for (;;) {
            put("String (return for new pattern): ");
            fgets(str,100,stdin);
            if (!str[0])
                break;
            printf("Case sensitive: %s  insensitive: %s\n",
              dd_matchl(str, pat, 0) ? "YES" : "NO",
              dd_matchl(str, pat, 1) ? "YES" : "NO");
        }
    }
    exit(0);
}

#endif /* TEST_MATCH */
