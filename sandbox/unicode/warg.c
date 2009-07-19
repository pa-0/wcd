#include <stdio.h>
#include <wchar.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <stdarg.h>

// Portable printf for Unicode characters.
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

// greek delta \u0394
// pound sign \u00a3
// Cyrillic Ya \u044f

// This program can take Unicode arguments.
int main (int argc, char ** argv) {

	wchar_t *cmdstr;
	wchar_t **wargv;
    int i;


#ifdef WIN32
    printf("system cp=%d\n",GetACP());
    printf("console cp=%d\n",GetConsoleOutputCP());
#endif


    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    cmdstr = GetCommandLineW();
    wargv = CommandLineToArgvW(cmdstr, &argc);
	
    for (i = 0; i < argc; i++) {
        wcd_wprintf(L"wargv[%d]: %s\n", i, wargv[i]);
    }
    wcd_wprintf(L"delta=%s\n",L"\u0394");
    wcd_wprintf(L"pound sign=%s\n",L"\u00a3");
    wcd_wprintf(L"cyrillic Ya=%s\n",L"\u044f");
	return 0;
}
