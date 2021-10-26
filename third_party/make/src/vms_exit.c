/* vms_exit.c
 *
 * Wrapper for the VMS exit() command to tranlate UNIX codes to be
 * encoded for POSIX, but also have VMS severity levels.
 * The posix_exit() variant only sets a severity level for status code 1.
 *
 * Author: John E. Malmberg
 */

/* Copyright (C) 2014-2020 Free Software Foundation, Inc.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */


/* Per copyright assignment agreement with the Free Software Foundation
   this software may be available under under other license agreements
   and copyrights. */

#include <makeint.h>

#include <stsdef.h>
void
decc$exit (int status);
#ifndef C_FACILITY_NO
# define C_FACILITY_NO 0x350000
#endif

/* Lowest legal non-success VMS exit code is 8 */
/* GNU make only defines codes 0, 1, 2 */
/* So assume any exit code > 8 is a VMS exit code */

#ifndef MAX_EXPECTED_EXIT_CODE
# define MAX_EXPECTED_EXIT_CODE 7
#endif

/* Build a Posix Exit with VMS severity */
void
vms_exit (int status)
{
  int vms_status;
  /* Fake the __posix_exit with severity added */
  /* Undocumented correct way to do this. */
  vms_status = 0;

  /* The default DECC definition is not compatible with doing a POSIX_EXIT */
  /* So fix it. */
  if (status == EXIT_FAILURE)
    status = MAKE_FAILURE;

  /* Trivial case exit success */
  if (status == 0)
    decc$exit (STS$K_SUCCESS);

  /* Is this a VMS status then just take it */
  if (status > MAX_EXPECTED_EXIT_CODE)
    {
      /* Make sure that the message inhibit is set since message has */
      /* already been displayed. */
      vms_status = status | STS$M_INHIB_MSG;
      decc$exit (vms_status);
    }

  /* Unix status codes are limited to 1 byte, so anything larger */
  /* is a probably a VMS exit code and needs to be passed through */
  /* A lower value can be set for a macro. */
  /* Status 0 is always passed through as it is converted to SS$_NORMAL */
  /* Always set the message inhibit bit */
  vms_status = C_FACILITY_NO | 0xA000 | STS$M_INHIB_MSG;
  vms_status |= (status << 3);

  /* STS$K_ERROR is for status that stops makefile that a simple */
  /* Rerun of the makefile will not fix. */

  if (status == MAKE_FAILURE)
    vms_status |= STS$K_ERROR;
  else if (status == MAKE_TROUBLE)
    {
      /* Make trouble is for when make was told to do nothing and */
      /* found that a target was not up to date.  Since a second */
      /* of make will produce the same condition, this is of */
      /* Error severity */
      vms_status |= STS$K_ERROR;
    }
  decc$exit (vms_status);
}
