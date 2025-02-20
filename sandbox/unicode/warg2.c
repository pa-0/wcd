#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <stdarg.h>


void my_wprintf( const wchar_t* format, ... ) {
   wchar_t wstr[1024];
   va_list args;

   HANDLE stduit =GetStdHandle(STD_OUTPUT_HANDLE);
   int len;


   va_start( args, format );

   len = sizeof(wstr);
   vsnwprintf( wstr, len, format, args);
   WriteConsoleW(stduit, wstr, wcslen(wstr), NULL, NULL);
   //WriteConsoleW(stduit, L"\n\r", 1, NULL, NULL);
   va_end( args );
}


// This program can take Unicode arguments on Windows NT
// even if the system/console code page is 8 bit.
int main (int argc, char ** argv) {

    wchar_t *cmdstr;
    wchar_t **wargv;
    int i;
    wchar_t j[2];

    printf("system cp=%d\n",GetACP());
    printf("console cp=%d\n",GetConsoleOutputCP());


    // print parameters, including Unicode.
    cmdstr = GetCommandLineW();
    wargv = CommandLineToArgvW(cmdstr, &argc);
    for (i = 0; i < argc; i++) {
        my_wprintf(L"wargv[%d]: %s\n", i, wargv[i]);
    }

  printf("fwide=%d\n",fwide(stdout,0));
  my_wprintf(L"e with accent=%s\n",L"\u00e9");
  my_wprintf(L"greek delta=%s\n",L"\u0394");
  my_wprintf(L"pound sign=%s\n",L"\u00a3");
  my_wprintf(L"cyrillic Ya=%s\n",L"\u044f");
  my_wprintf(L"Dutch IJ=%s\n",L"\u0132");      // Latin extended-A
  my_wprintf(L"Dutch ij=%s\n",L"\u0133");      // Latin extended-A
  my_wprintf(L"Dutch Florin=%s\n",L"\u0192");  // Latin extended-B

  // Hebrew combining characters.
  my_wprintf(L" Hebrew combining characters\n");
  j[1] = 0x0;
  j[0] = 0x0591;
  while (j[0] < 0x05c6)
  {
     my_wprintf(L" %s",j);
     j[0]++;
     if (j[0]%16 == 0)
        my_wprintf(L"\n");
  }
  j[0]= 0x05c7;
  my_wprintf(L" %s",j);

  // Hebrew characters.
  my_wprintf(L"\n");
  my_wprintf(L" Hebrew characters\n");
  j[0]= 0x05c6;
  my_wprintf(L" %s",j);
  j[0] = 0x05d0;
  while (j[0] < 0x05eb)
  {
     my_wprintf(L" %s",j);
     j[0]++;
     if (j[0]%16 == 0)
        my_wprintf(L"\n");
  }
  j[0] = 0x05f0;
  while (j[0] < 0x05f5)
  {
     my_wprintf(L" %s",j);
     j[0]++;
     if (j[0]%16 == 0)
        my_wprintf(L"\n");
  }

  return 0;
}
