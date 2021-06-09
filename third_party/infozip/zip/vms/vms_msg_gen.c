/*
 * VMS Message Source File Generator.
 *
 * 2007-01-29 SMS.
 *
 * Generates a VMS error message source file from data in "ziperr.h".
 *
 * On a VMS system, the standard builders should do the work.  On a
 * non-VMS system:
 *
 *    cc -I. vms/vms_msg_gen.c -o vms_msg_gen
 *    ./vms_msg_gen > vms/zip_msg.msg
 *    rm ./vms_msg_gen
 */

#include <stdio.h>
#include <string.h>

#define GLOBALS         /* Include data for ziperrors[] in ziperr.h. */
#include "ziperr.h"

main()
{
    int base_prev;
    int code_vms;
    int code_zip;
    int i;

    char *sev_str[ 8] = {
     "/WARNING",
     "/SUCCESS",
     "/ERROR",
     "/INFORMATIONAL",
     "/FATAL",
     "/??????",
     "/???????",
     "/????????"
    };

    char *text1[] = {
"!    VMS Error Message Source File for Zip",
"!",
"! Because the facility code was formally assigned by HP, the .FACILITY",
"! directive below specifies /SYSTEM.  Because the messages are, in",
"! general, specific to Zip, this file is not compiled with /SHARED.",
"! For example:",
"!",
"!    MESSAGE /OBJECT = [.dest]ZIP_MSG.OBJ /NOSYMBOLS [.VMS]ZIP_MSG.MSG",
"!",
"!    LINK /SHAREABLE = [.dest]ZIP_MSG.EXE [.dest]ZIP_MSG.OBJ",
"!",
"!-----------------------------------------------------------------------",
"",
".TITLE  Info-ZIP Zip Error Messages",
".FACILITY IZ_ZIP, 1955 /SYSTEM",
NULL                                            /* End-of-text marker. */
};

    /* Initialize the .BASE counter. */
    base_prev = -2;

    /* Put out the header text. */
    for (i = 0; text1[ i] != NULL; i++)
    {
        printf( "%s\n", text1[ i]);
    }
    printf( ".IDENT '%s'\n", VMS_MSG_IDENT);
    printf( "\n");

    /* Put out the error messages. */
    for (code_zip = 0; code_zip <= ZE_MAXERR; code_zip++)
    {
        if ((ziperrors[ code_zip].string != NULL) &&
         (strlen(ziperrors[ code_zip].string) != 0))
        {
            code_vms = 2* code_zip;     /* 4-bit left-shift, not 3. */
            if (code_vms != base_prev+ 1)
            {
                printf( ".BASE %d\n", code_vms);
            }
            printf( "%-7s %-13s <%s>\n",
             ziperrors[ code_zip].name,
             sev_str[ ziperrors[ code_zip].severity & 0x07],
             ziperrors[ code_zip].string);
            base_prev = code_vms;
        }
    }
    /* Put out the .END directive. */
    printf( "\n");
    printf( ".END\n");
}
