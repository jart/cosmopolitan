/* --------------- Moved here from job.c ---------------
   This file must be #included in job.c, as it accesses static functions.

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

#include <string.h>
#include <descrip.h>
#include <clidef.h>

/* TODO - VMS specific header file conditionally included in makeint.h */

#include <stsdef.h>
#include <ssdef.h>
void
decc$exit (int status);

/* Lowest legal non-success VMS exit code is 8 */
/* GNU make only defines codes 0, 1, 2 */
/* So assume any exit code > 8 is a VMS exit code */

#ifndef MAX_EXPECTED_EXIT_CODE
# define MAX_EXPECTED_EXIT_CODE 7
#endif


#if __CRTL_VER >= 70302000 && !defined(__VAX)
# define MAX_DCL_LINE_LENGTH 4095
# define MAX_DCL_CMD_LINE_LENGTH 8192
#else
# define MAX_DCL_LINE_LENGTH 255
# define MAX_DCL_CMD_LINE_LENGTH 1024
#endif
#define MAX_DCL_TOKEN_LENGTH 255
#define MAX_DCL_TOKENS 127

enum auto_pipe { nopipe, add_pipe, dcl_pipe };

char *vmsify (char *name, int type);

static int vms_jobsefnmask = 0;

/* returns whether path is assumed to be a unix like shell. */
int
_is_unixy_shell (const char *path)
{
  return vms_gnv_shell;
}

#define VMS_GETMSG_MAX 256
static char vms_strsignal_text[VMS_GETMSG_MAX + 2];

char *
vms_strsignal (int status)
{
  if (status <= MAX_EXPECTED_EXIT_CODE)
    sprintf (vms_strsignal_text, "lib$spawn returned %x", status);
  else
    {
      int vms_status;
      unsigned short * msg_len;
      unsigned char out[4];
      vms_status = SYS$GETMSG (status, &msg_len,
                               vms_strsignal_text, 7, *out);
    }

  return vms_strsignal_text;
}


/* Wait for nchildren children to terminate */
static void
vmsWaitForChildren (int *status)
{
  while (1)
    {
      if (!vms_jobsefnmask)
        {
          *status = 0;
          return;
        }

      *status = sys$wflor (32, vms_jobsefnmask);
    }
  return;
}

static int ctrlYPressed= 0;
/* This is called at main or AST level. It is at AST level for DONTWAITFORCHILD
   and at main level otherwise. In any case it is called when a child process
   terminated. At AST level it won't get interrupted by anything except a
   inner mode level AST.
*/
static int
vmsHandleChildTerm (struct childbase *cbase)
{
  struct child *child = (struct child*)cbase;
  struct child *lastc, *c;
  int child_failed;
  int exit_code;

  /* The child efn is 0 when a built-in or null command is executed
     successfully with out actually creating a child.
  */
  if (child->efn > 0)
  {
    vms_jobsefnmask &= ~(1 << (child->efn - 32));

    lib$free_ef (&child->efn);
  }
  if (child->comname)
    {
      if (!ISDB (DB_JOBS) && !ctrlYPressed)
        unlink (child->comname);
      free (child->comname);
    }

  (void) sigblock (fatal_signal_mask);

  /* First check to see if this is a POSIX exit status and handle */
  if ((child->cstatus & VMS_POSIX_EXIT_MASK) == VMS_POSIX_EXIT_MASK)
    {
      exit_code = (child->cstatus >> 3) & 255;
      if (exit_code != MAKE_SUCCESS)
        child_failed = 1;
    }
  else
    {
      child_failed = !$VMS_STATUS_SUCCESS (child->cstatus);
      if (child_failed)
        exit_code = child->cstatus;
    }

  /* Search for a child matching the deceased one.  */
  lastc = 0;
#if defined(RECURSIVEJOBS)
  /* I've had problems with recursive stuff and process handling */
  for (c = children; c != 0 && c != child; lastc = c, c = c->next)
    ;
#else
  c = child;
#endif

  if ($VMS_STATUS_SUCCESS (child->vms_launch_status))
    {
      /* Convert VMS success status to 0 for UNIX code to be happy */
      child->vms_launch_status = 0;
    }

  /* Set the state flag to say the commands have finished.  */
  c->file->command_state = cs_finished;
  notice_finished_file (c->file);

  (void) sigsetmask (sigblock (0) & ~(fatal_signal_mask));

  return 1;
}

/* VMS:
   Spawn a process executing the command in ARGV and return its pid. */

/* local helpers to make ctrl+c and ctrl+y working, see below */
#include <iodef.h>
#include <libclidef.h>
#include <ssdef.h>

static int ctrlMask= LIB$M_CLI_CTRLY;
static int oldCtrlMask;
static int setupYAstTried= 0;
static unsigned short int chan= 0;

static void
reEnableAst(void)
{
  lib$enable_ctrl (&oldCtrlMask,0);
}

static int
astYHandler (void)
{
  struct child *c;
  for (c = children; c != 0; c = c->next)
    if (c->pid > 0)
      sys$delprc (&c->pid, 0, 0);
  ctrlYPressed= 1;
  kill (getpid(),SIGQUIT);
  return SS$_NORMAL;
}

static void
tryToSetupYAst(void)
{
  $DESCRIPTOR(inputDsc,"SYS$COMMAND");
  int     status;
  struct {
    short int       status, count;
    int     dvi;
  } iosb;
  unsigned short int loc_chan;

  setupYAstTried++;

  if (chan)
    loc_chan= chan;
  else
    {
      status= sys$assign(&inputDsc,&loc_chan,0,0);
      if (!(status&SS$_NORMAL))
        {
          lib$signal(status);
          return;
        }
    }
  status= sys$qiow (0, loc_chan, IO$_SETMODE|IO$M_CTRLYAST,&iosb,0,0,
                    astYHandler,0,0,0,0,0);
  if (status==SS$_NORMAL)
    status= iosb.status;
  if (status!=SS$_NORMAL)
    {
      if (!chan)
        sys$dassgn(loc_chan);
      if (status!=SS$_ILLIOFUNC && status!=SS$_NOPRIV)
        lib$signal(status);
      return;
    }

  /* called from AST handler ? */
  if (setupYAstTried>1)
    return;
  if (atexit(reEnableAst))
    fprintf (stderr,
             _("-warning, you may have to re-enable CTRL-Y handling from DCL.\n"));
  status= lib$disable_ctrl (&ctrlMask, &oldCtrlMask);
  if (!(status&SS$_NORMAL))
    {
      lib$signal(status);
      return;
    }
  if (!chan)
    chan = loc_chan;
}

/* Check if a token is too long */
#define INC_TOKEN_LEN_OR_RETURN(x) {token->length++; \
  if (token->length >= MAX_DCL_TOKEN_LENGTH) \
    { token->cmd_errno = ERANGE; return x; }}

#define INC_TOKEN_LEN_OR_BREAK {token->length++; \
  if (token->length >= MAX_DCL_TOKEN_LENGTH) \
    { token->cmd_errno = ERANGE; break; }}

#define ADD_TOKEN_LEN_OR_RETURN(add_len, x) {token->length += add_len; \
  if (token->length >= MAX_DCL_TOKEN_LENGTH) \
    { token->cmd_errno = ERANGE; return x; }}

/* Check if we are out of space for more tokens */
#define V_NEXT_TOKEN { if (cmd_tkn_index < MAX_DCL_TOKENS) \
  cmd_tokens[++cmd_tkn_index] = NULL; \
  else { token.cmd_errno = E2BIG; break; } \
  token.length = 0;}


#define UPDATE_TOKEN {cmd_tokens[cmd_tkn_index] = strdup(token.text); \
  V_NEXT_TOKEN;}

#define EOS_ERROR(x) { if (*x == 0) { token->cmd_errno = ERANGE; break; }}

struct token_info
  {
    char *text;       /* Parsed text */
    int length;       /* Length of parsed text */
    char *src;        /* Pointer to source text */
    int cmd_errno;    /* Error status of parse */
    int use_cmd_file; /* Force use of a command file */
  };


/* Extract a Posix single quoted string from input line */
static char *
posix_parse_sq (struct token_info *token)
{
  /* A Posix quoted string with no expansion unless in a string
     Unix simulation means no lexical functions present.
  */
  char * q;
  char * p;
  q = token->text;
  p = token->src;

  *q++ = '"';
  p++;
  INC_TOKEN_LEN_OR_RETURN (p);

  while (*p != '\'' && (token->length < MAX_DCL_TOKEN_LENGTH))
    {
      EOS_ERROR (p);
      if (*p == '"')
        {
          /* Embedded double quotes need to be doubled */
          *q++ = '"';
          INC_TOKEN_LEN_OR_BREAK;
          *q = '"';
        }
      else
        *q = *p;

      q++;
      p++;
      INC_TOKEN_LEN_OR_BREAK;
    }
  *q++ = '"';
  p++;
  INC_TOKEN_LEN_OR_RETURN (p);
  *q = 0;
  return p;
}

/* Extract a Posix double quoted string from input line */
static char *
posix_parse_dq (struct token_info *token)
{
  /* Unix mode:  Any imbedded \" becomes doubled.
                 \t is tab, \\, \$ leading character stripped.
                 $ character replaced with \' unless escaped.
  */
  char * q;
  char * p;
  q = token->text;
  p = token->src;
  *q++ = *p++;
  INC_TOKEN_LEN_OR_RETURN (p);
  while (*p != 0)
    {
      if (*p == '\\')
        {
          switch(p[1])
            {
            case 't':     /* Convert tabs */
              *q = '\t';
              p++;
              break;
            case '\\':     /* Just remove leading backslash */
            case '$':
              p++;
              *q = *p;
              break;
            case '"':
              p++;
              *q = *p;
              *q++ = '"';
              INC_TOKEN_LEN_OR_BREAK;
            default:      /* Pass through unchanged */
              *q++ = *p++;
              INC_TOKEN_LEN_OR_BREAK;
            }
          INC_TOKEN_LEN_OR_BREAK;
        }
      else if (*p == '$' && isalpha (p[1]))
        {
          /* A symbol we should be able to substitute */
          *q++ = '\'';
          INC_TOKEN_LEN_OR_BREAK;
          *q = '\'';
          INC_TOKEN_LEN_OR_BREAK;
          token->use_cmd_file = 1;
        }
      else
        {
          *q = *p;
          INC_TOKEN_LEN_OR_BREAK;
          if (*p == '"')
            {
              p++;
              q++;
              break;
            }
        }
      p++;
      q++;
    }
  *q = 0;
  return p;
}

/* Extract a VMS quoted string or substitution string from input line */
static char *
vms_parse_quotes (struct token_info *token)
{
  /* VMS mode, the \' means that a symbol substitution is starting
     so while you might think you can just copy until the next
     \'.  Unfortunately the substitution can be a lexical function
     which can contain embedded strings and lexical functions.
     Messy, so both types need to be handled together.
  */
  char * q;
  char * p;
  q = token->text;
  p = token->src;
  int parse_level[MAX_DCL_TOKENS + 1];
  int nest = 0;

  parse_level[0] = *p;
  if (parse_level[0] == '\'')
    token->use_cmd_file = 1;

  *q++ = *p++;
  INC_TOKEN_LEN_OR_RETURN (p);


  /* Copy everything until after the next single quote at nest == 0 */
  while (token->length < MAX_DCL_TOKEN_LENGTH)
    {
      EOS_ERROR (p);
      *q = *p;
      INC_TOKEN_LEN_OR_BREAK;
      if ((*p == parse_level[nest]) && (p[1] != '"'))
        {
          if (nest == 0)
            {
              *q++ = *p++;
              break;
            }
          nest--;
        }
      else
        {
          switch(*p)
            {
            case '\\':
              /* Handle continuation on to next line */
              if (p[1] != '\n')
                break;
              p++;
              p++;
              *q = *p;
              break;
            case '(':
              /* Parenthesis only in single quote level */
              if (parse_level[nest] == '\'')
                {
                  nest++;
                  parse_level[nest] == ')';
                }
              break;
            case '"':
              /* Double quotes only in parenthesis */
              if (parse_level[nest] == ')')
                {
                  nest++;
                  parse_level[nest] == '"';
                }
              break;
            case '\'':
              /* Symbol substitution ony in double quotes */
              if ((p[1] == '\'') && (parse_level[nest] == '"'))
                {
                  nest++;
                  parse_level[nest] == '\'';
                  *p++ = *q++;
                  token->use_cmd_file = 1;
                  INC_TOKEN_LEN_OR_BREAK;
                  break;
                }
              *q = *p;
            }
        }
      p++;
      q++;
      /* Pass through doubled double quotes */
      if ((*p == '"') && (p[1] == '"') && (parse_level[nest] == '"'))
      {
        *p++ = *q++;
        INC_TOKEN_LEN_OR_BREAK;
        *p++ = *q++;
        INC_TOKEN_LEN_OR_BREAK;
      }
    }
  *q = 0;
  return p;
}

/* Extract a $ string from the input line */
static char *
posix_parse_dollar (struct token_info *token)
{
  /* $foo becomes 'foo' */
  char * q;
  char * p;
  q = token->text;
  p = token->src;
  token->use_cmd_file = 1;

  p++;
  *q++ = '\'';
  INC_TOKEN_LEN_OR_RETURN (p);

  while ((isalnum (*p)) || (*p == '_'))
    {
      *q++ = *p++;
      INC_TOKEN_LEN_OR_BREAK;
    }
  *q++ = '\'';
  while (1)
    {
      INC_TOKEN_LEN_OR_BREAK;
      break;
    }
  *q = 0;
  return p;
}

const char *vms_filechars = "0123456789abcdefghijklmnopqrstuvwxyz" \
   "ABCDEFGHIJKLMNOPQRSTUVWXYZ[]<>:/_-.$";

/* Simple text copy */
static char *
parse_text (struct token_info *token, int assignment_hack)
{
  char * q;
  char * p;
  int str_len;
  q = token->text;
  p = token->src;

  /* If assignment hack, then this text needs to be double quoted. */
  if (vms_unix_simulation && (assignment_hack == 2))
    {
      *q++ = '"';
      INC_TOKEN_LEN_OR_RETURN (p);
    }

  *q++ = *p++;
  INC_TOKEN_LEN_OR_RETURN (p);

  while (*p != 0)
    {
      str_len = strspn (p, vms_filechars);
      if (str_len == 0)
        {
          /* Pass through backslash escapes in Unix simulation
             probably will not work anyway.
             All any character after a ^ otherwise to support EFS.
          */
          if (vms_unix_simulation && (p[0] == '\\') && (p[1] != 0))
            str_len = 2;
          else if ((p[0] == '^') && (p[1] != 0))
            str_len = 2;
          else if (!vms_unix_simulation && (p[0] == ';'))
            str_len = 1;

          if (str_len == 0)
            {
              /* If assignment hack, then this needs to be double quoted. */
              if (vms_unix_simulation && (assignment_hack == 2))
              {
                *q++ = '"';
                INC_TOKEN_LEN_OR_RETURN (p);
              }
              *q = 0;
              return p;
            }
        }
      if (str_len > 0)
        {
          ADD_TOKEN_LEN_OR_RETURN (str_len, p);
          strncpy (q, p, str_len);
          p += str_len;
          q += str_len;
          *q = 0;
        }
    }
  /* If assignment hack, then this text needs to be double quoted. */
  if (vms_unix_simulation && (assignment_hack == 2))
    {
      *q++ = '"';
      INC_TOKEN_LEN_OR_RETURN (p);
    }
  return p;
}

/* single character copy */
static char *
parse_char (struct token_info *token, int count)
{
  char * q;
  char * p;
  q = token->text;
  p = token->src;

  while (count > 0)
    {
      *q++ = *p++;
      INC_TOKEN_LEN_OR_RETURN (p);
      count--;
    }
  *q = 0;
  return p;
}

/* Build a command string from the collected tokens
   and process built-ins now
*/
static struct dsc$descriptor_s *
build_vms_cmd (char **cmd_tokens,
               enum auto_pipe use_pipe_cmd,
               int append_token)
{
  struct dsc$descriptor_s *cmd_dsc;
  int cmd_tkn_index;
  char * cmd;
  int cmd_len;
  int semicolon_seen;

  cmd_tkn_index = 0;
  cmd_dsc = xmalloc (sizeof (struct dsc$descriptor_s));

  /* Empty command? */
  if (cmd_tokens[0] == NULL)
    {
      cmd_dsc->dsc$a_pointer = NULL;
      cmd_dsc->dsc$w_length = 0;
      return cmd_dsc;
    }

  /* Max DCL command + 1 extra token and trailing space */
  cmd = xmalloc (MAX_DCL_CMD_LINE_LENGTH + 256);

  cmd[0] = '$';
  cmd[1] = 0;
  cmd_len = 1;

  /* Handle real or auto-pipe */
  if (use_pipe_cmd == add_pipe)
    {
      /* We need to auto convert to a pipe command */
      strcat (cmd, "pipe ");
      cmd_len += 5;
    }

  semicolon_seen = 0;
  while (cmd_tokens[cmd_tkn_index] != NULL)
    {

      /* Check for buffer overflow */
      if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
        {
          errno = E2BIG;
          break;
        }

      /* Eliminate double ';' */
      if (semicolon_seen && (cmd_tokens[cmd_tkn_index][0] == ';'))
        {
          semicolon_seen = 0;
          free (cmd_tokens[cmd_tkn_index++]);
          if (cmd_tokens[cmd_tkn_index] == NULL)
            break;
        }

      /* Special handling for CD built-in */
      if (strncmp (cmd_tokens[cmd_tkn_index], "builtin_cd", 11) == 0)
        {
          int result;
          semicolon_seen = 0;
          free (cmd_tokens[cmd_tkn_index]);
          cmd_tkn_index++;
          if (cmd_tokens[cmd_tkn_index] == NULL)
            break;
          DB(DB_JOBS, (_("BUILTIN CD %s\n"), cmd_tokens[cmd_tkn_index]));

          /* TODO: chdir fails with some valid syntaxes */
          result = chdir (cmd_tokens[cmd_tkn_index]);
          if (result != 0)
            {
              /* TODO: Handle failure better */
              free (cmd);
              while (cmd_tokens[cmd_tkn_index] == NULL)
                free (cmd_tokens[cmd_tkn_index++]);
              cmd_dsc->dsc$w_length = -1;
              cmd_dsc->dsc$a_pointer = NULL;
              return cmd_dsc;
            }
        }
      else if (strncmp (cmd_tokens[cmd_tkn_index], "exit", 5) == 0)
        {
          /* Copy the exit command */
          semicolon_seen = 0;
          strcpy (&cmd[cmd_len], cmd_tokens[cmd_tkn_index]);
          cmd_len += strlen (cmd_tokens[cmd_tkn_index]);
          free (cmd_tokens[cmd_tkn_index++]);
          if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
            {
              errno = E2BIG;
              break;
            }

          /* Optional whitespace */
          if (isspace (cmd_tokens[cmd_tkn_index][0]))
            {
              strcpy (&cmd[cmd_len], cmd_tokens[cmd_tkn_index]);
              cmd_len += strlen (cmd_tokens[cmd_tkn_index]);
              free (cmd_tokens[cmd_tkn_index++]);
              if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
              {
                errno = E2BIG;
                break;
              }
            }

          /* There should be a status, but it is optional */
          if (cmd_tokens[cmd_tkn_index][0] == ';')
            continue;

          /* If Unix simulation, add '((' */
          if (vms_unix_simulation)
            {
              strcpy (&cmd[cmd_len], "((");
              cmd_len += 2;
              if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
                {
                  errno = E2BIG;
                  break;
                }
            }

          /* Add the parameter */
          strcpy (&cmd[cmd_len], cmd_tokens[cmd_tkn_index]);
          cmd_len += strlen (cmd_tokens[cmd_tkn_index]);
          free (cmd_tokens[cmd_tkn_index++]);
          if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
            {
              errno = E2BIG;
              break;
            }

          /* Add " * 8) .and. %x7f8) .or. %x1035a002" */
          if (vms_unix_simulation)
            {
              const char *end_str = " * 8) .and. %x7f8) .or. %x1035a002";
              strcpy (&cmd[cmd_len], end_str);
              cmd_len += strlen (end_str);
              if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
                {
                  errno = E2BIG;
                  break;
                }
            }
          continue;
        }

      /* auto pipe needs spaces before semicolon */
      if (use_pipe_cmd == add_pipe)
        if (cmd_tokens[cmd_tkn_index][0] == ';')
          {
            cmd[cmd_len++] = ' ';
            semicolon_seen = 1;
            if (cmd_len > MAX_DCL_CMD_LINE_LENGTH)
              {
                errno = E2BIG;
                break;
              }
          }
        else
          {
            char ch;
            ch = cmd_tokens[cmd_tkn_index][0];
            if (!(ch == ' ' || ch == '\t'))
              semicolon_seen = 0;
          }

      strcpy (&cmd[cmd_len], cmd_tokens[cmd_tkn_index]);
      cmd_len += strlen (cmd_tokens[cmd_tkn_index]);

      free (cmd_tokens[cmd_tkn_index++]);

      /* Skip the append tokens if they exist */
      if (cmd_tkn_index == append_token)
        {
          free (cmd_tokens[cmd_tkn_index++]);
          if (isspace (cmd_tokens[cmd_tkn_index][0]))
            free (cmd_tokens[cmd_tkn_index++]);
          free (cmd_tokens[cmd_tkn_index++]);
        }
    }

  cmd[cmd_len] = 0;
  cmd_dsc->dsc$w_length = cmd_len;
  cmd_dsc->dsc$a_pointer = cmd;
  cmd_dsc->dsc$b_dtype = DSC$K_DTYPE_T;
  cmd_dsc->dsc$b_class = DSC$K_CLASS_S;

  return cmd_dsc;
}

pid_t
child_execute_job (struct childbase *child, int good_stdin UNUSED, char *argv)
{
  int i;

  static struct dsc$descriptor_s *cmd_dsc;
  static struct dsc$descriptor_s pnamedsc;
  int spflags = CLI$M_NOWAIT;
  int status;
  int comnamelen;
  char procname[100];

  char *p;
  char *cmd_tokens[(MAX_DCL_TOKENS * 2) + 1]; /* whitespace does not count */
  char token_str[MAX_DCL_TOKEN_LENGTH + 1];
  struct token_info token;
  int cmd_tkn_index;
  int paren_level = 0;
  enum auto_pipe use_pipe_cmd = nopipe;
  int append_token = -1;
  char *append_file = NULL;
  int unix_echo_cmd = 0;  /* Special handle Unix echo command */
  int assignment_hack = 0; /* Handle x=y command as piped command */

  /* Parse IO redirection.  */

  child->comname = NULL;

  DB (DB_JOBS, ("child_execute_job (%s)\n", argv));

  while (isspace ((unsigned char)*argv))
    argv++;

  if (*argv == 0)
    {
      /* Only a built-in or a null command - Still need to run term AST */
      child->cstatus = VMS_POSIX_EXIT_MASK;
      child->vms_launch_status = SS$_NORMAL;
      child->efn = 0;
      vmsHandleChildTerm (child);
      /* TODO what is this "magic number" */
      return 270163; /* Special built-in */
    }

  sprintf (procname, "GMAKE_%05x", getpid () & 0xfffff);
  pnamedsc.dsc$w_length = strlen (procname);
  pnamedsc.dsc$a_pointer = procname;
  pnamedsc.dsc$b_dtype = DSC$K_DTYPE_T;
  pnamedsc.dsc$b_class = DSC$K_CLASS_S;

  /* Old */
  /* Handle comments and redirection.
     For ONESHELL, the redirection must be on the first line. Any other
     redirection token is handled by DCL, that is, the pipe command with
     redirection can be used, but it should not be used on the first line
     for ONESHELL. */

  /* VMS parser notes:
     1. A token is any of DCL verbs, qualifiers, parameters, or punctuation.
     2. Only MAX_DCL_TOKENS per line in both one line or command file mode.
     3. Each token limited to MAC_DCL_TOKEN_LENGTH
     4. If the line to DCL is greater than MAX_DCL_LINE_LENGTH then a
        command file must be used.
     5. Currently a command file must be used symbol substitution is to
        be performed.
     6. Currently limiting command files to 2 * MAX_DCL_TOKENS.

     Build both a command file token list and command line token list
     until it is determined that the command line limits are exceeded.
  */

  cmd_tkn_index = 0;
  cmd_tokens[cmd_tkn_index] = NULL;
  p = argv;

  token.text = token_str;
  token.length = 0;
  token.cmd_errno = 0;
  token.use_cmd_file = 0;

  while (*p != 0)
    {
      /* We can not build this command so give up */
      if (token.cmd_errno != 0)
        break;

      token.src = p;

      switch (*p)
        {
        case '\'':
          if (vms_unix_simulation || unix_echo_cmd)
            {
              p = posix_parse_sq (&token);
              UPDATE_TOKEN;
              break;
            }

          /* VMS mode, the \' means that a symbol substitution is starting
             so while you might think you can just copy until the next
             \'.  Unfortunately the substitution can be a lexical function
             which can contain embedded strings and lexical functions.
             Messy.
          */
          p = vms_parse_quotes (&token);
          UPDATE_TOKEN;
          break;
        case '"':
          if (vms_unix_simulation)
            {
              p = posix_parse_dq (&token);
              UPDATE_TOKEN;
              break;
            }

          /* VMS quoted string, can contain lexical functions with
             quoted strings and nested lexical functions.
          */
          p = vms_parse_quotes (&token);
          UPDATE_TOKEN;
          break;

        case '$':
          if (vms_unix_simulation)
            {
              p = posix_parse_dollar (&token);
              UPDATE_TOKEN;
              break;
            }

          /* Otherwise nothing special */
          p = parse_text (&token, 0);
          UPDATE_TOKEN;
          break;
        case '\\':
          if (p[1] == '\n')
            {
              /* Line continuation, remove it */
              p += 2;
              break;
            }

          /* Ordinary character otherwise */
          if (assignment_hack != 0)
            assignment_hack++;
          if (assignment_hack > 2)
            {
              assignment_hack = 0;          /* Reset */
              if (use_pipe_cmd == nopipe)   /* force pipe use */
                use_pipe_cmd = add_pipe;
              token_str[0] = ';';              /* add ; token */
              token_str[1] = 0;
              UPDATE_TOKEN;
            }
          p = parse_text (&token, assignment_hack);
          UPDATE_TOKEN;
          break;
        case '!':
        case '#':
          /* Unix '#' is VMS '!' which comments out the rest of the line.
             Historically the rest of the line has been skipped.
             Not quite the right thing to do, as the f$verify lexical
             function works in comments.  But this helps keep the line
             lengths short.
          */
          unix_echo_cmd = 0;
          while (*p != '\n' && *p != 0)
            p++;
          break;
        case '(':
          /* Subshell, equation, or lexical function argument start */
          p = parse_char (&token, 1);
          UPDATE_TOKEN;
          paren_level++;
          break;
        case ')':
          /* Close out a paren level */
          p = parse_char (&token, 1);
          UPDATE_TOKEN;
          paren_level--;
          /* TODO: Should we diagnose if paren_level goes negative? */
          break;
        case '&':
          if (isalpha (p[1]) && !vms_unix_simulation)
            {
              /* VMS symbol substitution */
              p = parse_text (&token, 0);
              token.use_cmd_file = 1;
              UPDATE_TOKEN;
              break;
            }
          if (use_pipe_cmd == nopipe)
            use_pipe_cmd = add_pipe;
          if (p[1] != '&')
            p = parse_char (&token, 1);
          else
            p = parse_char (&token, 2);
          UPDATE_TOKEN;
          break;
        case '|':
          if (use_pipe_cmd == nopipe)
            use_pipe_cmd = add_pipe;
          if (p[1] != '|')
            p = parse_char (&token, 1);
          else
            p = parse_char (&token, 2);
          UPDATE_TOKEN;
          break;
        case ';':
          /* Separator - convert to a pipe command. */
          unix_echo_cmd = 0;
        case '<':
          if (use_pipe_cmd == nopipe)
            use_pipe_cmd = add_pipe;
          p = parse_char (&token, 1);
          UPDATE_TOKEN;
          break;
        case '>':
          if (use_pipe_cmd == nopipe)
            use_pipe_cmd = add_pipe;
          if (p[1] == '>')
            {
              /* Parsing would have been simple until support for the >>
                 append redirect was added.
                 Implementation needs:
                 * if not exist output file create empty
                 * open/append gnv$make_temp??? output_file
                 * define/user sys$output gnv$make_temp???
                 ** And all this done before the command previously tokenized.
                 * command previously tokenized
                 * close gnv$make_temp???
              */
              p = parse_char (&token, 2);
              append_token = cmd_tkn_index;
              token.use_cmd_file = 1;
            }
          else
            p = parse_char (&token, 1);
          UPDATE_TOKEN;
          break;
        case '/':
          /* Unix path or VMS option start, read until non-path symbol */
          if (assignment_hack != 0)
            assignment_hack++;
          if (assignment_hack > 2)
            {
              assignment_hack = 0;          /* Reset */
              if (use_pipe_cmd == nopipe)   /* force pipe use */
                use_pipe_cmd = add_pipe;
              token_str[0] = ';';              /* add ; token */
              token_str[1] = 0;
              UPDATE_TOKEN;
            }
          p = parse_text (&token, assignment_hack);
          UPDATE_TOKEN;
          break;
        case ':':
          if ((p[1] == 0) || isspace (p[1]))
            {
              /* Unix Null command - treat as comment until next command */
              unix_echo_cmd = 0;
              p++;
              while (*p != 0)
                {
                  if (*p == ';')
                    {
                      /* Remove Null command from pipeline */
                      p++;
                      break;
                    }
                  p++;
                }
              break;
            }

          /* String assignment */
          /* := :== or : */
          if (p[1] != '=')
            p = parse_char (&token, 1);
          else if (p[2] != '=')
            p = parse_char (&token, 2);
          else
            p = parse_char (&token, 3);
          UPDATE_TOKEN;
          break;
        case '=':
          /* = or == */
          /* If this is not an echo statement, this could be a shell
             assignment.  VMS requires the target to be quoted if it
             is not a macro substitution */
          if (!unix_echo_cmd && vms_unix_simulation && (assignment_hack == 0))
            assignment_hack = 1;
          if (p[1] != '=')
            p = parse_char (&token, 1);
          else
            p = parse_char (&token, 2);
          UPDATE_TOKEN;
          break;
        case '+':
        case '-':
        case '*':
          p = parse_char (&token, 1);
          UPDATE_TOKEN;
          break;
        case '.':
          /* .xxx. operation, VMS does not require the trailing . */
          p = parse_text (&token, 0);
          UPDATE_TOKEN;
          break;
        default:
          /* Skip repetitive whitespace */
          if (isspace (*p))
            {
              p = parse_char (&token, 1);

              /* Force to a space or a tab */
              if ((token_str[0] != ' ') ||
                  (token_str[0] != '\t'))
                token_str[0] = ' ';
              UPDATE_TOKEN;

              while (isspace (*p))
                p++;
              if (assignment_hack != 0)
                assignment_hack++;
              break;
            }

          if (assignment_hack != 0)
            assignment_hack++;
          if (assignment_hack > 2)
            {
              assignment_hack = 0;          /* Reset */
              if (use_pipe_cmd == nopipe)   /* force pipe use */
                use_pipe_cmd = add_pipe;
              token_str[0] = ';';              /* add ; token */
              token_str[1] = 0;
              UPDATE_TOKEN;
            }
          p = parse_text (&token, assignment_hack);
          if (strncasecmp (token.text, "echo", 4) == 0)
            unix_echo_cmd = 1;
          else if (strncasecmp (token.text, "pipe", 4) == 0)
            use_pipe_cmd = dcl_pipe;
          UPDATE_TOKEN;
          break;
        }
    }

  /* End up here with a list of tokens to build a command line.
     Deal with errors detected during parsing.
   */
  if (token.cmd_errno != 0)
    {
      while (cmd_tokens[cmd_tkn_index] == NULL)
        free (cmd_tokens[cmd_tkn_index++]);
      child->cstatus = VMS_POSIX_EXIT_MASK | (MAKE_TROUBLE << 3);
      child->vms_launch_status = SS$_ABORT;
      child->efn = 0;
      errno = token.cmd_errno;
      return -1;
    }

  /* Save any redirection to append file */
  if (append_token != -1)
    {
      int file_token;
      char * lastdot;
      char * lastdir;
      char * raw_append_file;
      file_token = append_token;
      file_token++;
      if (isspace (cmd_tokens[file_token][0]))
        file_token++;
      raw_append_file = vmsify (cmd_tokens[file_token], 0);
      /* VMS DCL needs a trailing dot if null file extension */
      lastdot = strrchr(raw_append_file, '.');
      lastdir = strrchr(raw_append_file, ']');
      if (lastdir == NULL)
        lastdir = strrchr(raw_append_file, '>');
      if (lastdir == NULL)
        lastdir = strrchr(raw_append_file, ':');
      if ((lastdot == NULL) || (lastdot > lastdir))
        {
          append_file = xmalloc (strlen (raw_append_file) + 1);
          strcpy (append_file, raw_append_file);
          strcat (append_file, ".");
        }
      else
        append_file = strdup(raw_append_file);
    }

  cmd_dsc = build_vms_cmd (cmd_tokens, use_pipe_cmd, append_token);
  if (cmd_dsc->dsc$a_pointer == NULL)
    {
      if (cmd_dsc->dsc$w_length < 0)
        {
          free (cmd_dsc);
          child->cstatus = VMS_POSIX_EXIT_MASK | (MAKE_TROUBLE << 3);
          child->vms_launch_status = SS$_ABORT;
          child->efn = 0;
          return -1;
        }

      /* Only a built-in or a null command - Still need to run term AST */
      free (cmd_dsc);
      child->cstatus = VMS_POSIX_EXIT_MASK;
      child->vms_launch_status = SS$_NORMAL;
      child->efn = 0;
      vmsHandleChildTerm (child);
      /* TODO what is this "magic number" */
      return 270163; /* Special built-in */
    }

  if (cmd_dsc->dsc$w_length > MAX_DCL_LINE_LENGTH)
    token.use_cmd_file = 1;

  DB(DB_JOBS, (_("DCL: %s\n"), cmd_dsc->dsc$a_pointer));

  /* Enforce the creation of a command file if "vms_always_use_cmd_file" is
     non-zero.
     Further, this way DCL reads the input stream and therefore does
     'forced' symbol substitution, which it doesn't do for one-liners when
     they are 'lib$spawn'ed.

     Otherwise the behavior is:

     Create a *.com file if either the command is too long for
     lib$spawn, or if a redirect appending to a file is desired, or
     symbol substitition.
  */

  if (vms_always_use_cmd_file || token.use_cmd_file)
    {
      FILE *outfile;
      int cmd_len;

      outfile = get_tmpfile (&child->comname,
                             "sys$scratch:gnv$make_cmdXXXXXX.com");
      /*                      123456789012345678901234567890 */
      if (outfile == 0)
        pfatal_with_name (_("fopen (temporary file)"));
      comnamelen = strlen (child->comname);

      /* The whole DCL "script" is executed as one action, and it behaves as
         any DCL "script", that is errors stop it but warnings do not. Usually
         the command on the last line, defines the exit code.  However, with
         redirections there is a prolog and possibly an epilog to implement
         the redirection.  Both are part of the script which is actually
         executed. So if the redirection encounters an error in the prolog,
         the user actions will not run; if in the epilog, the user actions
         ran, but output is not captured. In both error cases, the error of
         redirection is passed back and not the exit code of the actions. The
         user should be able to enable DCL "script" verification with "set
         verify". However, the prolog and epilog commands are not shown. Also,
         if output redirection is used, the verification output is redirected
         into that file as well. */
      fprintf (outfile, "$ gnv$$make_verify = \"''f$verify(0)'\"\n");
      fprintf (outfile, "$ gnv$$make_pid = f$getjpi(\"\",\"pid\")\n");
      fprintf (outfile, "$ on error then $ goto gnv$$make_error\n");

      /* Handle append redirection */
      if (append_file != NULL)
        {
          /* If file does not exist, create it */
          fprintf (outfile,
                   "$ gnv$$make_al = \"gnv$$make_append''gnv$$make_pid'\"\n");
          fprintf (outfile,
                   "$ if f$search(\"%s\") .eqs. \"\" then create %s\n",
                   append_file, append_file);

          fprintf (outfile,
                   "$ open/append 'gnv$$make_al' %s\n", append_file);

          /* define sys$output to that file */
          fprintf (outfile,
                   "$ define/user sys$output 'gnv$$make_al'\n");
          DB (DB_JOBS, (_("Append output to %s\n"), append_file));
          free(append_file);
        }

      fprintf (outfile, "$ gnv$$make_verify = f$verify(gnv$$make_verify)\n");

      /* TODO:
         Only for ONESHELL there will be several commands separated by
         '\n'. But there can always be multiple continuation lines.
      */

      fprintf (outfile, "%s\n", cmd_dsc->dsc$a_pointer);
      fprintf (outfile, "$ gnv$$make_status_2 = $status\n");
      fprintf (outfile, "$ goto gnv$$make_exit\n");

      /* Exit and clean up */
      fprintf (outfile, "$ gnv$$make_error: ! 'f$verify(0)\n");
      fprintf (outfile, "$ gnv$$make_status_2 = $status\n");

      if (append_token != -1)
        {
          fprintf (outfile, "$ deassign sys$output\n");
          fprintf (outfile, "$ close 'gnv$$make_al'\n");

          DB (DB_JOBS,
              (_("Append %.*s and cleanup\n"), comnamelen-3, child->comname));
        }
      fprintf (outfile, "$ gnv$$make_exit: ! 'f$verify(0)\n");
      fprintf (outfile,
             "$ exit 'gnv$$make_status_2' + (0*f$verify(gnv$$make_verify))\n");

      fclose (outfile);

      free (cmd_dsc->dsc$a_pointer);
      cmd_dsc->dsc$a_pointer = xmalloc (256 + 4);
      sprintf (cmd_dsc->dsc$a_pointer, "$ @%s", child->comname);
      cmd_dsc->dsc$w_length = strlen (cmd_dsc->dsc$a_pointer);

      DB (DB_JOBS, (_("Executing %s instead\n"), child->comname));
    }

  child->efn = 0;
  while (child->efn < 32 || child->efn > 63)
    {
      status = LIB$GET_EF ((unsigned long *)&child->efn);
      if (!$VMS_STATUS_SUCCESS (status))
        {
          if (child->comname)
            {
              if (!ISDB (DB_JOBS))
                unlink (child->comname);
              free (child->comname);
            }
          return -1;
        }
    }

  SYS$CLREF (child->efn);

  vms_jobsefnmask |= (1 << (child->efn - 32));

  /* Export the child environment into DCL symbols */
  if (child->environment != 0)
    {
      char **ep = child->environment;
      while (*ep != 0)
        {
          vms_putenv_symbol (*ep);
          *ep++;
        }
    }

  /*
    LIB$SPAWN  [command-string]
    [,input-file]
    [,output-file]
    [,flags]
    [,process-name]
    [,process-id] [,completion-status-address] [,byte-integer-event-flag-num]
    [,AST-address] [,varying-AST-argument]
    [,prompt-string] [,cli] [,table]
  */

#ifndef DONTWAITFORCHILD
  /*
   * Code to make ctrl+c and ctrl+y working.
   * The problem starts with the synchronous case where after lib$spawn is
   * called any input will go to the child. But with input re-directed,
   * both control characters won't make it to any of the programs, neither
   * the spawning nor to the spawned one. Hence the caller needs to spawn
   * with CLI$M_NOWAIT to NOT give up the input focus. A sys$waitfr
   * has to follow to simulate the wanted synchronous behaviour.
   * The next problem is ctrl+y which isn't caught by the crtl and
   * therefore isn't converted to SIGQUIT (for a signal handler which is
   * already established). The only way to catch ctrl+y, is an AST
   * assigned to the input channel. But ctrl+y handling of DCL needs to be
   * disabled, otherwise it will handle it. Not to mention the previous
   * ctrl+y handling of DCL needs to be re-established before make exits.
   * One more: At the time of LIB$SPAWN signals are blocked. SIGQUIT will
   * make it to the signal handler after the child "normally" terminates.
   * This isn't enough. It seems reasonable for simple command lines like
   * a 'cc foobar.c' spawned in a subprocess but it is unacceptable for
   * spawning make. Therefore we need to abort the process in the AST.
   *
   * Prior to the spawn it is checked if an AST is already set up for
   * ctrl+y, if not one is set up for a channel to SYS$COMMAND. In general
   * this will work except if make is run in a batch environment, but there
   * nobody can press ctrl+y. During the setup the DCL handling of ctrl+y
   * is disabled and an exit handler is established to re-enable it.
   * If the user interrupts with ctrl+y, the assigned AST will fire, force
   * an abort to the subprocess and signal SIGQUIT, which will be caught by
   * the already established handler and will bring us back to common code.
   * After the spawn (now /nowait) a sys$waitfr simulates the /wait and
   * enables the ctrl+y be delivered to this code. And the ctrl+c too,
   * which the crtl converts to SIGINT and which is caught by the common
   * signal handler. Because signals were blocked before entering this code
   * sys$waitfr will always complete and the SIGQUIT will be processed after
   * it (after termination of the current block, somewhere in common code).
   * And SIGINT too will be delayed. That is ctrl+c can only abort when the
   * current command completes. Anyway it's better than nothing :-)
   */

  if (!setupYAstTried)
    tryToSetupYAst();
  child->vms_launch_status = lib$spawn (cmd_dsc,               /* cmd-string */
                     NULL, /* input-file */
                     NULL, /* output-file */
                     &spflags,                                 /* flags */
                     &pnamedsc,                                /* proc name */
                     &child->pid, &child->cstatus, &child->efn,
                     0, 0,
                     0, 0, 0);

  status = child->vms_launch_status;
  if ($VMS_STATUS_SUCCESS (status))
    {
      status = sys$waitfr (child->efn);
      vmsHandleChildTerm (child);
    }
#else
  child->vms_launch_status = lib$spawn (cmd_dsc,
                      NULL,
                      NULL,
                      &spflags,
                      &pnamedsc,
                      &child->pid, &child->cstatus, &child->efn,
                      vmsHandleChildTerm, child,
                      0, 0, 0);
   status = child->vms_launch_status;
#endif

  /* Free the pointer if not a command file */
  if (!vms_always_use_cmd_file && !token.use_cmd_file)
    free (cmd_dsc->dsc$a_pointer);
  free (cmd_dsc);

  if (!$VMS_STATUS_SUCCESS (status))
    {
      switch (status)
        {
        case SS$_EXQUOTA:
          errno = EPROCLIM;
          break;
        default:
          errno = EFAIL;
        }
    }

  /* Restore the VMS symbols that were changed */
  if (child->environment != 0)
    {
      char **ep = child->environment;
      while (*ep != 0)
        {
          vms_restore_symbol (*ep);
          *ep++;
        }
    }

  /* TODO what is this "magic number" */
  return (status & 1) ? 270163 : -1 ;
}
