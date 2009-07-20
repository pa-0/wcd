#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <stdarg.h>

// Portable wprintf for Unicode characters.
void wcd_wprintf( const wchar_t* format, ... ) {
	wchar_t wstr[1024];
	va_list args;
#ifdef WIN32
	HANDLE stduit =GetStdHandle(STD_OUTPUT_HANDLE); 
	int len;
#endif

	va_start( args, format );
#ifdef WIN32
	len = sizeof(wstr);
	vsnwprintf( wstr, len, format, args);
   	WriteConsoleW(stduit, wstr, wcslen(wstr), NULL, NULL);
   	//WriteConsoleW(stduit, L"\n\r", 1, NULL, NULL);  
#else
	vwprintf( format, args );
#endif
	va_end( args );
}


// This program can take Unicode arguments on Windows NT
// even if the system/console code page is 8 bit.
int main (int argc, char ** argv) {

    wchar_t *cmdstr;
    wchar_t **wargv;
    wchar_t *wstr = L"\u0394"; /* greek delta */
    wchar_t wc ;
    int i;
    FILE *out;
#ifdef WIN32

    printf("system cp=%d\n",GetACP());
    printf("console cp=%d\n",GetConsoleOutputCP());
#endif


#ifdef WIN32
    // print parameters, including Unicode.
    cmdstr = GetCommandLineW();
    wargv = CommandLineToArgvW(cmdstr, &argc);
    for (i = 0; i < argc; i++) {
        wcd_wprintf(L"wargv[%d]: %s\n", i, wargv[i]);
    }
#else
    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
#endif
	
    wcd_wprintf(L"greek delta=%s\n",L"\u0394");
    wcd_wprintf(L"pound sign=%s\n",L"\u00a3");
    wcd_wprintf(L"cyrillic Ya=%s\n",L"\u044f");

  // Write unicode to a file.
  out = fopen("out.txt","wb");
  fwprintf (out, L"%s\n", wstr);
  fclose(out);

  // Read unicode back from file.
  out = fopen("out.txt","rb");
  wc = getwc(out);
  fclose(out);
  wcd_wprintf(L"wide char=%c\n",wc);
  wcd_wprintf(L"wide char=%x\n",wc);

  return 0;
}
