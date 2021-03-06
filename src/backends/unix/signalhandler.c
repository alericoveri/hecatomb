/*
 * Copyright (C) 2013 Alejandro Ricoveri
 * Copyright (C) 2011 Yamagi Burmeister
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This is a signal handler for printing some hints to debug problem in
 * the case of a crash. On Linux a backtrace is printed.
 *
 * =======================================================================
 */

 #include "prereqs.h"

 #if HT_HAVE_EXECINFO
 # include <execinfo.h>
 #endif

 /* ========================================================================= */
 void
 Sys_Backtrace ( q_int32_t sig )
 {
 #if HT_HAVE_EXECINFO
   void *array[15];
   size_t size;
   char **strings;
   q_int32_t i;
   size = backtrace ( array, 15 );
   strings = backtrace_symbols ( array, size );
 #endif
   printf ( "%s - v%s\n", HT_DEV_NAME, HT_VERSION);
   printf ( "OS       => %s\n", HT_OS_NAME );
   printf ( "arch     => %s\n", HT_ARCH_NAME );
   printf ( "cc       => %s-%d.%d.%d\n", HT_CC_ID, HT_CC_VERSION_MAJOR,
                                  HT_CC_VERSION_MINOR, HT_CC_VERSION_PATCH );
   printf ( "cflags   => %s\n", HT_CC_CFLAGS);
   printf ( "signal   => %s\n", strsignal(sig) );
#ifdef HT_WITH_GIT
   printf ("git ref  => %s\n", HT_GIT_REFSPEC);
   printf ("git hash => %s\n", HT_GIT_SHA1);
#endif
   printf ( "\nBacktrace:\n" );
 #if HT_HAVE_EXECINFO
   for ( i = 0; i < size; i++ ) {
     printf ( "[%d]  %s\n", i, strings[i] );
   }
 #else
   printf ( "[Not available on this plattform.]\n" );
 #endif
 }

 /* ========================================================================= */
 void
 Sys_SigHandler ( q_int32_t sig )
 {
   printf ( "\n=======================================================\n" );
   printf ( "%s HAS CRASHED! This is embarassing indeed...\n", HT_PRODUCT_NAME );
   printf ( "\nMake sure that you're using the last version. It can\n" );
   printf ( "be found at %s. If you do,\n", HT_URL );
   printf ( "open up an issue on %s and include:\n\n", HT_URL );
   printf ( " - This output\n" );
   printf ( " - The conditions that triggered the crash\n" );
   printf ( " - How to reproduce the crash (if known)\n" );
   printf ( " - The following files. None of them contains private\n" );
   printf ( "   data. They're necessary to analyze the backtrace:\n\n" );
   printf ( "    - %s (the executable / binary)\n\n", HT_DEV_NAME );
   printf ( "    - %s (the game DLL of the mod you were playing\n", HT_GAME_DLL );
   printf ( "      when the game crashed, e.g. %s/gamex86.so for the\n", HT_DIR_BASE );
   printf ( "      main game on *nix x86 build)\n\n" );
   printf ( " - Any other data which you think might be usefull\n" );
   printf ( "\nThank you very much for your help, making %s\n", HT_PRODUCT_NAME );
   printf ( "an even better source port. It's much appreciated.\n" );
   printf ( "=======================================================\n\n" );
   Sys_Backtrace ( sig );

   /* make sure this is written */
   fflush ( stdout );

   /* reset signalhandler */
   signal ( SIGSEGV, SIG_DFL );
   signal ( SIGILL, SIG_DFL );
   signal ( SIGFPE, SIG_DFL );
   signal ( SIGABRT, SIG_DFL );

   /* pass signal to the os */
   raise ( sig );
 }
