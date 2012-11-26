#include <stdio.h>
#include <wchar.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main (int argc, char ** argv) {
  wchar_t wcs[8];
  int i;
  char mbs[16];

  // U+2070E
  wcs[0] = 0xd841;
  wcs[1] = 0xdf0e;
  wcs[2] = L'\0';

  printf("width %X = %d\n", wcs[0], wcwidth(wcs[0]));
  printf("width %X = %d\n", wcs[1], wcwidth(wcs[1]));

#ifdef _WIN32
  i = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, mbs, sizeof(mbs), NULL, NULL) -1;
  printf("i = %d multi byte chars\n",i);

  i = MultiByteToWideChar(CP_UTF8, 0, mbs, -1, wcs, sizeof(wcs)) -1;
  printf("i = %d wide chars\n",i);
  /* MultiByteToWideChar returns the number of UTF-16 chars: 2.
   * So a surrogate pair counts as 2 */
#endif

  return(0);
}
