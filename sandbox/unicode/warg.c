#include <stdio.h>
#include <wchar.h>
#ifdef WIN32
#include <windows.h>
#include <iconv.h>
#endif
#include <stdarg.h>


#ifdef WIN32
# define CAT(a,b)   a##b
# define L_(String) CAT(L,String)
# define WCD_PRINTF wcd_wprintf
#else
# define L_(String) String
# define WCD_PRINTF printf
#endif

/* wide char to UTF-8 */
int wcstoutf8(wchar_t *wcstr, char *mbstr, int len)
{
   return(WideCharToMultiByte(CP_UTF8, 0, wcstr, -1, mbstr, len, NULL, NULL));
}

/* Print an UTF-8 multi-byte string */
void wcd_printf( const char* format, ... ) {
   wchar_t wstr[1024];
   char buf[1024];
   va_list args;
#ifdef WIN32
   HANDLE stduit =GetStdHandle(STD_OUTPUT_HANDLE);
   int len;
#endif

   va_start( args, format );
#ifdef WIN32
   len = sizeof(wstr);
   vsnprintf( buf, len, format, args);
   if (MultiByteToWideChar(CP_UTF8,0, buf, -1, wstr,1024) > 0  )
      WriteConsoleW(stduit, wstr, wcslen(wstr), NULL, NULL);
   //WriteConsoleW(stduit, L"\n\r", 1, NULL, NULL);
#else
   if ( fwide(stdout,0) < 0 )
   {
      printf ("stdio is byte oriented.\n");
      printf ("wide-char streams not permitted.\n");
      printf ("wprintf will not work.\n");
   }
   else
   {
      vwprintf( format, args );
   }
#endif
   va_end( args );
}


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
   if ( fwide(stdout,0) < 0 )
   {
      printf ("stdio is byte oriented.\n");
      printf ("wide-char streams not permitted.\n");
      printf ("wprintf will not work.\n");
   }
   else
   {
      vwprintf( format, args );
   }
#endif
   va_end( args );
}


// This program can take Unicode arguments on Windows NT
// even if the system/console code page is 8 bit.
int main (int argc, char ** argv) {

    wchar_t *cmdstr;
    wchar_t **wargv;
    wchar_t *wstr = L"\u0394"; /* greek delta */
    wchar_t *BOM_UTF16LE = L"\ufeff";
    wchar_t wc ;
    wchar_t wstring[128];
    char    cstring[128];
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

  printf("fwide=%d\n",fwide(stdout,0));
  WCD_PRINTF(L_("greek delta=%s\n"),L_("\u0394"));
  WCD_PRINTF(L_("pound sign=%s\n"),L_("\u00a3"));
  WCD_PRINTF(L_("cyrillic Ya=%s\n"),L_("\u044f"));

  wcscpy(wstring,L"DELTA=\u0394");
  wcstoutf8(wstring, cstring, sizeof(cstring));
  wcd_printf("UTF-8 %s\n",cstring);
  wcd_printf("UTF-8 \u044f\n");


#ifdef WIN32
  // Write unicode to a file.
  out = fopen("out.txt","wb");
  fwprintf (out, L"%s", BOM_UTF16LE);  /* UTF-16LE BOM */
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
  WCD_PRINTF(L_("wide char=%c\n"),wc);
  WCD_PRINTF(L_("wide char=%4x\n"),wc);
#endif

  return 0;
}
