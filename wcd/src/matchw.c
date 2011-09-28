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

  matchw.c : A modified version of match.c with Unicode locale matching.

  EW:   Characters with accents a.s.o. match
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
#include <wctype.h>
#include "matchw.h" /* define ToLower() in here (for Unix, define ToLower
                       * to be macro (using isupper()); otherwise just use
                       * tolower() */
#define Case(x)  (ic? towlower(x) : (wint_t)(x))

static wchar_t match_C[256] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

static wchar_t match_Unicode[256] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
  0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xc6, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
  0x44, 0x4e, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0xd7, 0x4f, 0x55, 0x55, 0x55, 0x55, 0x59, 0xde, 0xdf,
  0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0xe6, 0x87, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,
  0xf0, 0x6e, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0xf7, 0x6f, 0x75, 0x75, 0x75, 0x75, 0x79, 0xfe, 0x79,
};


/* dd_matchwcs() is a shell to recmatch() to return only Boolean values. */
static int recmatchwcs(wchar_t *pattern, wchar_t *string, int ignore_case, wchar_t *CPTable);

int dd_matchwcs(const wchar_t *string,const wchar_t *pattern,int ignore_case)
{
#if (defined(MSDOS) && defined(DOSWILD))
    wchar_t *dospattern;
    size_t j = wcslen(pattern);
#endif
    int exact = 1;
    wchar_t *CPTable;


    if (exact)
        CPTable = match_C;
    else
        CPTable = match_Unicode;

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

    if ((dospattern = (wchar_t *)malloc((j+1) * sizeof(wchar_t))) != NULL) {
        wcscpy(dospattern, pattern);
        if (!wcscmp(dospattern+j-3, L"*.*")) {
            dospattern[j-2] = '\0';                    /* nuke the ".*" */
        } else if (!wcscmp(dospattern+j-2, L"*.")) {
            wchar_t *p = wcschr(string, L'.');

            if (p) {   /* found a dot:  match fails */
                free(dospattern);
                return 0;
            }
            dospattern[j-1] = '\0';                    /* nuke the end "." */
        }
        j = recmatchwcs((wchar_t *)dospattern, (wchar_t *)string, ignore_case, CPTable);
        free(dospattern);
        return j == 1;
    } else
#endif /* MSDOS && DOSWILD */
    return recmatchwcs((wchar_t *)pattern, (wchar_t *)string, ignore_case, CPTable) == 1;
}


static int recmatchwcs(wchar_t *p,wchar_t *s,int ic, wchar_t *CPTable)
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
                wint_t cc = Case(*s);

                if (*(p+1) != L'-')
                    for (c = c ? c : *p; c <= *p; c++)  /* compare range */
                    {
                        if ((c <= 256) && (cc <= 256))
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
    if ((c <= 256) && (*s <= 256))
        result = Case(CPTable[c]) == Case(CPTable[*s]);
    else
        result = Case(c) == Case(*s);
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
