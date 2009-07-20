#include <stdio.h>
#include <wchar.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <stdarg.h>


#ifdef WIN32
# define WCDL (wchar_t*)
# define WCD_PRINTF wcd_wprintf
#else
# define WCDL 
# define WCD_PRINTF printf
#endif


// Portable wprintf for Unicode characters.
void wcd_wprintf( const wchar_t* format, ... ) {
	wchar_t wstr[1024];
	va_list args;
#ifdef WIN32
	HANDLE stduit =GetStdHandle(STD_OUTPUT_HANDLE); 
	int len;
#endif

        if ( fwide(stdout,0) < 0 )
	{
           printf ("stdio is byte oriented.\n");
           printf ("wide-char streams not permitted.\n");
           printf ("wprintf will not work.\n");
	}
        else
	{
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
	
#ifdef WIN32
    printf("fwide=%d\n",fwide(stdout,0));
    WCD_PRINTF(L"greek delta=%s\n",L"\u0394");
    WCD_PRINTF(L"pound sign=%s\n",L"\u00a3");
    WCD_PRINTF(L"cyrillic Ya=%s\n",L"\u044f");
#else
    WCD_PRINTF("greek delta=%s\n","\u0394");
    WCD_PRINTF("pound sign=%s\n","\u00a3");
    WCD_PRINTF("cyrillic Ya=%s\n","\u044f");
#endif


#ifdef WIN32
  // Write unicode to a file.
  out = fopen("out.txt","wb");
  fwprintf (out, L"%s", L"\ufeff");  /* UTF-16LE BOM */
  fwprintf (out, L"%s\r\n", wstr);
  fwprintf (out, L"\r\n"); /* new line */
  fwprintf (out, L"%s", L"a");
  fwprintf (out, L"%s", L"\u00df"); /* German sharp s */
  fwprintf (out, L"\r\n"); /* new line */
  fclose(out);

  // Read unicode back from file.
  out = fopen("out.txt","rb");
  wc = getwc(out); /* UTF-16LE BOM */
  wc = getwc(out);
  fclose(out);
  wcd_wprintf(L"wide char=%c\n",wc);
  wcd_wprintf(L"wide char=%4x\n",wc);
#endif

  return 0;
}
