/* Error handling for Windows
Copyright (C) 1996-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdlib.h>
#include <windows.h>
#include "makeint.h"
#include "w32err.h"

/*
 * Description: the windows32 version of perror()
 *
 * Returns:  a pointer to a static error
 *
 * Notes/Dependencies:  I got this from
 *      comp.os.ms-windows.programmer.win32
 */
const char *
map_windows32_error_to_string (DWORD ercode) {
/*
 * We used to have an MSVC-specific '__declspec (thread)' qualifier
 * here, with the following comment:
 *
 * __declspec (thread) necessary if you will use multiple threads on MSVC
 *
 * However, Make was never multithreaded on Windows (except when
 * Ctrl-C is hit, in which case the main thread is stopped
 * immediately, so it doesn't matter in this context).  The functions
 * on sub_proc.c that started and stopped additional threads were
 * never used, and are now #ifdef'ed away.  Until we need more than
 * one thread, we have no problems with the following buffer being
 * static.  (If and when we do need it to be in thread-local storage,
 * the corresponding GCC qualifier is '__thread'.)
 */
    static char szMessageBuffer[128];
        /* Fill message buffer with a default message in
         * case FormatMessage fails
         */
    wsprintf (szMessageBuffer, "Error %ld\n", ercode);

        /*
         *  Special code for winsock error handling.
         */
        if (ercode > WSABASEERR) {
#if 0
                HMODULE hModule = GetModuleHandle("wsock32");
                if (hModule != NULL) {
                        FormatMessage(FORMAT_MESSAGE_FROM_HMODULE,
                                hModule,
                                ercode,
                                LANG_NEUTRAL,
                                szMessageBuffer,
                                sizeof(szMessageBuffer),
                                NULL);
                        FreeLibrary(hModule);
                }
#else
                O (fatal, NILF, szMessageBuffer);
#endif
        } else {
                /*
                 *  Default system message handling
                 */
                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        ercode,
                        LANG_NEUTRAL,
                        szMessageBuffer,
                        sizeof(szMessageBuffer),
                        NULL);
        }
    return szMessageBuffer;
}
