#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#endif

int main () {

  int i;
  char mbs[16];
  wchar_t wcs[3];
  wchar_t wcs32[2];

  setlocale(LC_ALL, "");

  // U+2070E
  wcs[0] = 0xd841;
  wcs[1] = 0xdf0e;
  wcs[2] = L'\0';

  //i = wcstombs(mbs,L"\ud841\udf0e",sizeof(mbs));
  i = wcstombs(mbs,wcs,sizeof(mbs));
  printf("i = %d\n",i);

  printf("wcstombs UTF-16: ");

  if (i > 0 )
  {
    i =0;
    while (mbs[i] != '\0') {
     printf(" %x",(unsigned char)mbs[i]);
     i++;
    }
    printf("<\n");

    printf("%s\n",mbs);
  }
  else
    printf ("failed\n");

#if !defined(_WIN32) && !defined(__CYGWIN__)

  // Linux

  // decode surrogate pair.
  wcs32[0] = 0x10000;
  wcs32[0] += (wcs[0] & 0x03FF) << 10;
  wcs32[0] += (wcs[1] & 0x03FF);
  wcs32[1] = L'\0';
  printf(" %X\n",wcs32[0]);

  i = wcstombs(mbs,wcs32,sizeof(mbs));
  printf("i = %d\n",i);

  printf("wcstombs UTF-32: ");

  if (i > 0 )
  {
    i =0;
    while (mbs[i] != '\0') {
     printf(" %x",(unsigned char)mbs[i]);
     i++;
    }
    printf("<\n");

    printf("%s\n",mbs);
  }
  else
    printf ("failed\n");

#endif

#if defined(_WIN32) || defined(__CYGWIN__)
  i = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, mbs, sizeof(mbs), NULL, NULL) -1;
  printf("i = %d\n",i);

  if (i > 0 )
  {
    printf("WideCharToMultiByte: ");
    i =0;
    while (mbs[i] != '\0') {
     printf(" %x",(unsigned char)mbs[i]);
     i++;
    }
    printf("<\n");

  printf("%s\n",mbs);
  }
#endif

  return 0;
}
