/* File: vms_export_symbol.c
 *
 * Some programs need special environment variables deported as DCL
 * DCL symbols.
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <descrip.h>
#include <stsdef.h>
#include <ssdef.h>
#include <unixlib.h>
#include <libclidef.h>

#pragma member_alignment save
#pragma nomember_alignment longword
struct item_list_3
{
  unsigned short len;
  unsigned short code;
  void * bufadr;
  unsigned short * retlen;
};


#pragma member_alignment

int
LIB$GET_SYMBOL (const struct dsc$descriptor_s * symbol,
                struct dsc$descriptor_s * value,
                unsigned short * value_len,
                const unsigned long * table);

int
LIB$SET_SYMBOL (const struct dsc$descriptor_s * symbol,
                const struct dsc$descriptor_s * value,
                const unsigned long * table);

int
LIB$DELETE_SYMBOL (const struct dsc$descriptor_s * symbol,
                   const unsigned long * table);

#define MAX_DCL_SYMBOL_LEN (255)
#if __CRTL_VER >= 70302000 && !defined(__VAX)
# define MAX_DCL_SYMBOL_VALUE (8192)
#else
# define MAX_DCL_SYMBOL_VALUE (1024)
#endif

struct dcl_symbol
{
  struct dcl_symbol * link;
  struct dsc$descriptor_s name_desc;
  struct dsc$descriptor_s value_desc;
  char name[MAX_DCL_SYMBOL_LEN + 1];    /* + 1 byte for null terminator */
  char value[MAX_DCL_SYMBOL_VALUE +1];  /* + 1 byte for null terminator */
  char pad[3]; /* Pad structure to longword allignment */
};

static struct dcl_symbol * vms_dcl_symbol_head = NULL;

/* Restore symbol state to original condition. */
static unsigned long
clear_dcl_symbol (struct dcl_symbol * symbol)
{

  const unsigned long symtbl = LIB$K_CLI_LOCAL_SYM;
  int status;

  if (symbol->value_desc.dsc$w_length == (unsigned short)-1)
    status = LIB$DELETE_SYMBOL (&symbol->name_desc, &symtbl);
  else
    status = LIB$SET_SYMBOL (&symbol->name_desc,
                             &symbol->value_desc, &symtbl);
  return status;
}


/* Restore all exported symbols to their original conditions */
static void
clear_exported_symbols (void)
{

  struct dcl_symbol * symbol;

  symbol = vms_dcl_symbol_head;

  /* Walk the list of symbols.  This is done durring exit,
   * so no need to free memory.
   */
  while (symbol != NULL)
  {
    clear_dcl_symbol (symbol);
    symbol = symbol->link;
  }

}


/* Restore the symbol back to the original value
 * symbol name is either a plain name or of the form "symbol=name" where
 * the name portion is ignored.
 */
void
vms_restore_symbol (const char * string)
{

  struct dcl_symbol * symbol;
  char name[MAX_DCL_SYMBOL_LEN + 1];
  int status;
  char * value;
  int name_len;

  symbol = vms_dcl_symbol_head;

  /* Isolate the name from the value */
  value = strchr (string, '=');
  if (value != NULL)
    {
      /* Copy the name from the string */
      name_len = (value - string);
    }
  else
    name_len = strlen (string);

  if (name_len > MAX_DCL_SYMBOL_LEN)
    name_len = MAX_DCL_SYMBOL_LEN;

  strncpy (name, string, name_len);
  name[name_len] = 0;

  /* Walk the list of symbols.  The saved symbol is not freed
   * symbols are likely to be overwritten multiple times, so this
   * saves time in saving them each time.
   */
  while (symbol != NULL)
    {
      int result;
      result = strcmp (symbol->name, name);
      if (result == 0)
        {
          clear_dcl_symbol (symbol);
          break;
        }
      symbol = symbol->link;
    }
}

int
vms_export_dcl_symbol (const char * name, const char * value)
{

  struct dcl_symbol * symbol;
  struct dcl_symbol * next;
  struct dcl_symbol * link;
  int found;
  const unsigned long symtbl = LIB$K_CLI_LOCAL_SYM;
  struct dsc$descriptor_s value_desc;
  int string_len;
  int status;
  char new_value[MAX_DCL_SYMBOL_VALUE + 1];
  char * dollarp;

  next = vms_dcl_symbol_head;
  link = vms_dcl_symbol_head;

  /* Is symbol already exported? */
  found = 0;
  while ((found == 0) && (link != NULL))
    {
      int x;
      found = !strncasecmp (link->name, name, MAX_DCL_SYMBOL_LEN);
      if (found)
        symbol = link;
      next = link;
      link = link->link;
    }

  /* New symbol, set it up */
  if (found == 0)
    {
      symbol = malloc (sizeof (struct dcl_symbol));
      if (symbol == NULL)
        return SS$_INSFMEM;

      /* Construct the symbol descriptor, used for both saving
       * the old symbol and creating the new symbol.
       */
      symbol->name_desc.dsc$w_length = strlen (name);
      if (symbol->name_desc.dsc$w_length > MAX_DCL_SYMBOL_LEN)
        symbol->name_desc.dsc$w_length = MAX_DCL_SYMBOL_LEN;

      strncpy (symbol->name, name, symbol->name_desc.dsc$w_length);
      symbol->name[symbol->name_desc.dsc$w_length] = 0;
      symbol->name_desc.dsc$a_pointer = symbol->name;
      symbol->name_desc.dsc$b_dtype = DSC$K_DTYPE_T;
      symbol->name_desc.dsc$b_class = DSC$K_CLASS_S;

      /* construct the value descriptor, used only for saving
       * the old symbol.
       */
      symbol->value_desc.dsc$a_pointer = symbol->value;
      symbol->value_desc.dsc$w_length = MAX_DCL_SYMBOL_VALUE;
      symbol->value_desc.dsc$b_dtype = DSC$K_DTYPE_T;
      symbol->value_desc.dsc$b_class = DSC$K_CLASS_S;
    }

  if (found == 0)
    {
      unsigned long old_symtbl;
      unsigned short value_len;

      /* Look up the symbol */
      status = LIB$GET_SYMBOL (&symbol->name_desc, &symbol->value_desc,
                               &value_len, &old_symtbl);
      if (!$VMS_STATUS_SUCCESS (status))
        value_len = (unsigned short)-1;
      else if (old_symtbl != symtbl)
        value_len = (unsigned short)-1;

      symbol->value_desc.dsc$w_length = value_len;

      /* Store it away */
      if (value_len != (unsigned short) -1)
        symbol->value[value_len] = 0;

      /* Make sure atexit scheduled */
      if (vms_dcl_symbol_head == NULL)
        {
          vms_dcl_symbol_head = symbol;
          atexit (clear_exported_symbols);
        }
      else
        {
          /* Extend the chain */
          next->link = symbol;
        }
    }

  /* Create or replace a symbol */
  value_desc.dsc$a_pointer = new_value;
  string_len = strlen (value);
  if (string_len > MAX_DCL_SYMBOL_VALUE)
    string_len = MAX_DCL_SYMBOL_VALUE;

  strncpy (new_value, value, string_len);
  new_value[string_len] = 0;

  /* Special handling for GNU Make.  GNU Make doubles the dollar signs
   * in environment variables read in from getenv().  Make exports symbols
   * with the dollar signs already doubled.  So all $$ must be converted
   * back to $.
   * If the first $ is not doubled, then do not convert at all.
   */
  dollarp = strchr (new_value, '$');
  while (dollarp && dollarp[1] == '$')
    {
      int left;
      dollarp++;
      left = string_len - (dollarp - new_value - 1);
      string_len--;
      if (left > 0)
        {
          memmove (dollarp, &dollarp[1], left);
          dollarp = strchr (&dollarp[1], '$');
        }
      else
        {
          /* Ended with $$, simple case */
          dollarp[1] = 0;
          break;
        }
    }
  value_desc.dsc$w_length = string_len;
  value_desc.dsc$b_dtype = DSC$K_DTYPE_T;
  value_desc.dsc$b_class = DSC$K_CLASS_S;
  status = LIB$SET_SYMBOL (&symbol->name_desc, &value_desc, &symtbl);
  return status;
}

/* export a DCL symbol using a string in the same syntax as putenv */
int
vms_putenv_symbol (const char * string)
{

  char name[MAX_DCL_SYMBOL_LEN + 1];
  int status;
  char * value;
  int name_len;

  /* Isolate the name from the value */
  value = strchr (string, '=');
  if (value == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  /* Copy the name from the string */
  name_len = (value - string);
  if (name_len > MAX_DCL_SYMBOL_LEN)
    name_len = MAX_DCL_SYMBOL_LEN;

  strncpy (name, string, name_len);
  name[name_len] = 0;

  /* Skip past the "=" */
  value++;

  /* Export the symbol */
  status = vms_export_dcl_symbol (name, value);

  /* Convert the error to Unix format */
  if (!$VMS_STATUS_SUCCESS (status))
    {
      errno = EVMSERR;
      vaxc$errno = status;
      return -1;
    }
  return 0;
}

#if __CRTL_VER >= 70301000
# define transpath_parm transpath
#else
static char transpath[MAX_DCL_SYMBOL_VALUE];
#endif

/* Helper callback routine for converting Unix paths to VMS */
static int
to_vms_action (char * vms_spec, int flag, char * transpath_parm)
{
  strncpy (transpath, vms_spec, MAX_DCL_SYMBOL_VALUE - 1);
  transpath[MAX_DCL_SYMBOL_VALUE - 1] = 0;
  return 0;
}

#ifdef __DECC
# pragma message save
  /* Undocumented extra parameter use triggers a ptrmismatch warning */
# pragma message disable ptrmismatch
#endif

/* Create a foreign command only visible to children */
int
create_foreign_command (const char * command, const char * image)
{
  char vms_command[MAX_DCL_SYMBOL_VALUE + 1];
  int status;

  vms_command[0] = '$';
  vms_command[1] = 0;
  if (image[0] == '/')
    {
#if __CRTL_VER >= 70301000
      /* Current decc$to_vms is reentrant */
      decc$to_vms (image, to_vms_action, 0, 1, &vms_command[1]);
#else
      /* Older decc$to_vms is not reentrant */
      decc$to_vms (image, to_vms_action, 0, 1);
      strncpy (&vms_command[1], transpath, MAX_DCL_SYMBOL_VALUE - 1);
      vms_command[MAX_DCL_SYMBOL_VALUE] = 0;
#endif
    }
  else
    {
      strncpy (&vms_command[1], image, MAX_DCL_SYMBOL_VALUE - 1);
      vms_command[MAX_DCL_SYMBOL_VALUE] = 0;
    }
  status = vms_export_dcl_symbol (command, vms_command);

  return status;
}
#ifdef __DECC
# pragma message restore
#endif


#ifdef DEBUG

int
main(int argc, char ** argv, char **env)
{

  char value[MAX_DCL_SYMBOL_VALUE +1];
  int status = 0;
  int putenv_status;
  int vms_status;
  struct dsc$descriptor_s name_desc;
  struct dsc$descriptor_s value_desc;
  const unsigned long symtbl = LIB$K_CLI_LOCAL_SYM;
  unsigned short value_len;
  unsigned long old_symtbl;
  int result;
  const char * vms_command = "vms_export_symbol";
  const char * vms_image = "test_image.exe";
  const char * vms_symbol1 = "test_symbol1";
  const char * value1 = "test_value1";
  const char * vms_symbol2 = "test_symbol2";
  const char * putenv_string = "test_symbol2=value2";
  const char * value2 = "value2";

  /* Test creating a foreign command */
  vms_status = create_foreign_command (vms_command, vms_image);
  if (!$VMS_STATUS_SUCCESS (vms_status))
    {
      printf("Create foreign command failed: %d\n", vms_status);
      status = 1;
    }

  name_desc.dsc$a_pointer = (char *)vms_command;
  name_desc.dsc$w_length = strlen (vms_command);
  name_desc.dsc$b_dtype = DSC$K_DTYPE_T;
  name_desc.dsc$b_class = DSC$K_CLASS_S;

  value_desc.dsc$a_pointer = value;
  value_desc.dsc$w_length = MAX_DCL_SYMBOL_VALUE;
  value_desc.dsc$b_dtype = DSC$K_DTYPE_T;
  value_desc.dsc$b_class = DSC$K_CLASS_S;

  vms_status = LIB$GET_SYMBOL (&name_desc, &value_desc,
                               &value_len, &old_symtbl);
  if (!$VMS_STATUS_SUCCESS (vms_status))
    {
      printf ("lib$get_symbol for command failed: %d\n", vms_status);
      status = 1;
    }

  value[value_len] = 0;
  result = strncasecmp (&value[1], vms_image, value_len - 1);
  if (result != 0)
    {
      printf ("create_foreign_command failed!  expected '%s', got '%s'\n",
              vms_image, &value[1]);
      status = 1;
    }

  /* Test exporting a symbol */
  vms_status = vms_export_dcl_symbol (vms_symbol1, value1);
  if (!$VMS_STATUS_SUCCESS (vms_status))
    {
      printf ("vms_export_dcl_symbol for command failed: %d\n", vms_status);
      status = 1;
    }

  name_desc.dsc$a_pointer = (char *)vms_symbol1;
  name_desc.dsc$w_length = strlen (vms_symbol1);
  vms_status = LIB$GET_SYMBOL(&name_desc, &value_desc,
                              &value_len, &old_symtbl);
  if (!$VMS_STATUS_SUCCESS(vms_status))
    {
      printf ("lib$get_symbol for command failed: %d\n", vms_status);
      status = 1;
    }

  value[value_len] = 0;
  result = strncmp (value, value1, value_len);
  if (result != 0)
    {
      printf ("vms_export_dcl_symbol failed!  expected '%s', got '%s'\n",
              value1, value);
      status = 1;
    }

  /* Test putenv for DCL symbols */
  putenv_status = vms_putenv_symbol (putenv_string);
  if (putenv_status != 0)
    {
      perror ("vms_putenv_symbol");
      status = 1;
    }

  name_desc.dsc$a_pointer = (char *)vms_symbol2;
  name_desc.dsc$w_length = strlen(vms_symbol2);
  vms_status = LIB$GET_SYMBOL (&name_desc, &value_desc,
                               &value_len, &old_symtbl);
  if (!$VMS_STATUS_SUCCESS (vms_status))
    {
      printf ("lib$get_symbol for command failed: %d\n", vms_status);
      status = 1;
    }

  value[value_len] = 0;
  result = strncmp (value, value2, value_len);
  if (result != 0)
  {
    printf ("vms_putenv_symbol failed!  expected '%s', got '%s'\n",
            value2, value);
    status = 1;
  }

  vms_restore_symbol (putenv_string);
  vms_status = LIB$GET_SYMBOL (&name_desc, &value_desc,
                               &value_len, &old_symtbl);
  if ($VMS_STATUS_SUCCESS (vms_status))
    {
      printf ("lib$get_symbol for command succeeded, should have failed\n");
      status = 1;
    }

  exit (status);
}

#endif
