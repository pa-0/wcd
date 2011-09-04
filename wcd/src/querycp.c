/* The code in this file is Public Domain */

#ifdef DJGPP

#include <dpmi.h>
#include <go32.h>
#include <stdio.h>

// ----------------------------------------------------------------------
// Tuesday, May 5, 2009    1:40pm
// rugxulo _AT_ gmail _DOT_ com
//
// This file is (obviously?) not copyrighted, "nenies propra�o" !!
//
// Tested successfully on DR-DOS 7.03, FreeDOS 1.0++, and MS-DOS 6.22.
// (Doesn't work on XP or Vista, though.)
// ----------------------------------------------------------------------
//
// unsigned short query_con_codepage(void);
//
// gets currently selected display CON codepage
//
// int 21h, 6601h ("chcp") needs NLSFUNC.EXE + COUNTRY.SYS, but many
//    obscure codepages (e.g. FD's cp853 from EGA.CPX (CPIX.ZIP) or
//    Kosta Kostis' cp913 from ISOLATIN.CPI (ISOCP101.ZIP) have no
//    relevant data inside COUNTRY.SYS.
//
// int 21h, 440Ch 6Ah only works in MS-DOS and DR-DOS (not FreeDOS) because
//    FreeDOS DISPLAY is an .EXE TSR, not a device driver, and hence doesn't
//    fully support IOCTL, so they use the undocumented int 2Fh, 0AD02h
//    (which doesn't work in DR-DOS!). But DR-DOS' DISPLAY doesn't respond
//    to the typical install check i.d. anyways. FreeDOS currently only
//    supports COUNTRY.SYS in their "unstable" kernel 2037, but at least
//    their KEYB, "�oje", supports cp853 too (thanks, Henrique!).
//
// P.S. For MS or DR: ren ega.cpx *.com ; upx -d ega.com ; ren ega.com *.cpi
// ----------------------------------------------------------------------

unsigned short query_con_codepage(void) {
   __dpmi_regs regs;

   unsigned short param_block[2] = { 0, 437 };

   regs.d.eax = 0x440C;                // GENERIC IO FOR HANDLES
   regs.d.ebx = 1;                     // STDOUT
   regs.d.ecx = 0x036A;                // 3 = CON, 0x6A = QUERY SELECTED CP
   regs.x.ds = __tb >> 4;              // using transfer buffer for low mem.
   regs.x.dx = __tb & 0x0F;            // (suggested by DJGPP FAQ, hi Eli!)
   regs.x.flags |= 1;                  // preset carry for potential failure
   __dpmi_int (0x21, &regs);

   if (!(regs.x.flags & 1))            // if succeed (carry flag not set)
     dosmemget( __tb, 4, param_block);
   else {                              // (undocumented method)
     regs.x.ax = 0xAD02;               // 440C -> MS-DOS or DR-DOS only
     regs.x.bx = 0xFFFE;               // AD02 -> MS-DOS or FreeDOS only
     regs.x.flags |= 1;
     __dpmi_int(0x2F, &regs);

     if ((!(regs.x.flags & 1)) && (regs.x.bx < 0xFFFE))
       param_block[1] = regs.x.bx;
   }

   return param_block[1];
}
#elif defined(__WATCOMC__) && defined(MSDOS)

/* rugxulo _AT_ gmail _DOT_ com */

#include <stdio.h>
#include <dos.h>
#include <i86.h>

unsigned short query_con_codepage(void) {
   union REGS regs;
   unsigned short param_block[2] = { 0, 437 };

   regs.x.ax = 0x440C;           // GENERIC IO FOR HANDLES
   regs.x.bx = 1;                // STDOUT
   regs.x.cx = 0x036A;           // 3 = CON, 0x6A = QUERY SELECTED CP
   regs.x.dx = (unsigned short)param_block;
   regs.x.cflag |= 1;            // preset carry for potential failure
   int86(0x21, &regs, &regs);

   if (regs.x.cflag)             // if not succeed (carry flag set)
   {
     regs.x.ax = 0xAD02;         // 440C -> MS-DOS or DR-DOS only
     regs.x.bx = 0xFFFE;         // AD02 -> MS-DOS or FreeDOS only
     regs.x.cflag |= 1;
     int86(0x2F, &regs, &regs);
   }

     if ((!(regs.x.cflag)) && (regs.x.bx < 0xFFFE))
       param_block[1] = regs.x.bx;

   return param_block[1];

}



#elif defined (WIN32) && !defined(__CYGWIN__)

/* Erwin Waterlander */

#include <windows.h>
unsigned short query_con_codepage(void) {
   return((unsigned short)GetACP());
}
#else

unsigned short query_con_codepage(void) {
   return(0);
}
#endif

#ifdef TEST
int main() {
  printf("\nCP%u\n",query_con_codepage() );  // should be same result as
  return 0;                                  //   "mode con cp /status"
}
#endif

// EOF
