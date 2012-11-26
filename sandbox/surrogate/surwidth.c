#include <stdio.h>
#include <wchar.h>

int main (int argc, char ** argv) {
  wchar_t wcs[3];

  // U+2070E
  wcs[0] = 0xd841;
  wcs[1] = 0xdf0e;
  wcs[2] = L'\0';

  printf("width %X = %d\n", wcs[0], wcwidth(wcs[0]));
  printf("width %X = %d\n", wcs[1], wcwidth(wcs[1]));

  return(0);
}
