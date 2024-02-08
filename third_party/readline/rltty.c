/* rltty.c -- functions to prepare and restore the terminal for readline's
   use. */

/* Copyright (C) 1992-2022 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library (Readline), a library
   for reading lines of text with interactive input and history editing.

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "rldefs.h"

#include "rltty.h"
#if defined (HAVE_SYS_IOCTL_H)
#  include <sys/ioctl.h>		/* include for declaration of ioctl */
#endif

#include "readline.h"
#include "rlprivate.h"

#if !defined (errno)
extern int errno;
#endif /* !errno */

rl_vintfunc_t *rl_prep_term_function = rl_prep_terminal;
rl_voidfunc_t *rl_deprep_term_function = rl_deprep_terminal;

static void set_winsize (int);

/* **************************************************************** */
/*								    */
/*		      Saving and Restoring the TTY	    	    */
/*								    */
/* **************************************************************** */

/* Non-zero means that the terminal is in a prepped state.  There are several
   flags that are OR'd in to denote whether or not we have sent various
   init strings to the terminal. */
#define TPX_PREPPED	0x01
#define TPX_BRACKPASTE	0x02
#define TPX_METAKEY	0x04

static int terminal_prepped;

static _RL_TTY_CHARS _rl_tty_chars, _rl_last_tty_chars;

/* If non-zero, means that this process has called tcflow(fd, TCOOFF)
   and output is suspended. */
#if defined (__ksr1__)
static int ksrflow;
#endif

/* Dummy call to force a backgrounded readline to stop before it tries
   to get the tty settings. */
static void
set_winsize (tty)
     int tty;
{
#if defined (TIOCGWINSZ)
  struct winsize w;

  if (ioctl (tty, TIOCGWINSZ, &w) == 0)
      (void) ioctl (tty, TIOCSWINSZ, &w);
#endif /* TIOCGWINSZ */
}

#if defined (NO_TTY_DRIVER)
/* Nothing */
#elif defined (NEW_TTY_DRIVER)

/* Values for the `flags' field of a struct bsdtty.  This tells which
   elements of the struct bsdtty have been fetched from the system and
   are valid. */
#define SGTTY_SET	0x01
#define LFLAG_SET	0x02
#define TCHARS_SET	0x04
#define LTCHARS_SET	0x08

struct bsdtty {
  struct sgttyb sgttyb;	/* Basic BSD tty driver information. */
  int lflag;		/* Local mode flags, like LPASS8. */
#if defined (TIOCGETC)
  struct tchars tchars;	/* Terminal special characters, including ^S and ^Q. */
#endif
#if defined (TIOCGLTC)
  struct ltchars ltchars; /* 4.2 BSD editing characters */
#endif
  int flags;		/* Bitmap saying which parts of the struct are valid. */
};

#define TIOTYPE struct bsdtty

static TIOTYPE otio;

static void save_tty_chars (TIOTYPE *);
static int _get_tty_settings (int, TIOTYPE *);
static int get_tty_settings (int, TIOTYPE *);
static int _set_tty_settings (int, TIOTYPE *);
static int set_tty_settings (int, TIOTYPE *);

static void prepare_terminal_settings (int, TIOTYPE, TIOTYPE *);

static void set_special_char (Keymap, TIOTYPE *, int, rl_command_func_t *);

static void
save_tty_chars (TIOTYPE *tiop)
{
  _rl_last_tty_chars = _rl_tty_chars;

  if (tiop->flags & SGTTY_SET)
    {
      _rl_tty_chars.t_erase = tiop->sgttyb.sg_erase;
      _rl_tty_chars.t_kill = tiop->sgttyb.sg_kill;
    }

  if (tiop->flags & TCHARS_SET)
    {
      _rl_intr_char = _rl_tty_chars.t_intr = tiop->tchars.t_intrc;
      _rl_quit_char = _rl_tty_chars.t_quit = tiop->tchars.t_quitc;

      _rl_tty_chars.t_start = tiop->tchars.t_startc;
      _rl_tty_chars.t_stop = tiop->tchars.t_stopc;
      _rl_tty_chars.t_eof = tiop->tchars.t_eofc;
      _rl_tty_chars.t_eol = '\n';
      _rl_tty_chars.t_eol2 = tiop->tchars.t_brkc;
    }

  if (tiop->flags & LTCHARS_SET)
    {
      _rl_susp_char = _rl_tty_chars.t_susp = tiop->ltchars.t_suspc;

      _rl_tty_chars.t_dsusp = tiop->ltchars.t_dsuspc;
      _rl_tty_chars.t_reprint = tiop->ltchars.t_rprntc;
      _rl_tty_chars.t_flush = tiop->ltchars.t_flushc;
      _rl_tty_chars.t_werase = tiop->ltchars.t_werasc;
      _rl_tty_chars.t_lnext = tiop->ltchars.t_lnextc;
    }

  _rl_tty_chars.t_status = -1;
}

static int
get_tty_settings (int tty, TIOTYPE *tiop)
{
  set_winsize (tty);

  tiop->flags = tiop->lflag = 0;

  errno = 0;
  if (ioctl (tty, TIOCGETP, &(tiop->sgttyb)) < 0)
    return -1;
  tiop->flags |= SGTTY_SET;

#if defined (TIOCLGET)
  if (ioctl (tty, TIOCLGET, &(tiop->lflag)) == 0)
    tiop->flags |= LFLAG_SET;
#endif

#if defined (TIOCGETC)
  if (ioctl (tty, TIOCGETC, &(tiop->tchars)) == 0)
    tiop->flags |= TCHARS_SET;
#endif

#if defined (TIOCGLTC)
  if (ioctl (tty, TIOCGLTC, &(tiop->ltchars)) == 0)
    tiop->flags |= LTCHARS_SET;
#endif

  return 0;
}

static int
set_tty_settings (int tty, TIOTYPE *tiop)
{
  if (tiop->flags & SGTTY_SET)
    {
      ioctl (tty, TIOCSETN, &(tiop->sgttyb));
      tiop->flags &= ~SGTTY_SET;
    }
  _rl_echoing_p = 1;

#if defined (TIOCLSET)
  if (tiop->flags & LFLAG_SET)
    {
      ioctl (tty, TIOCLSET, &(tiop->lflag));
      tiop->flags &= ~LFLAG_SET;
    }
#endif

#if defined (TIOCSETC)
  if (tiop->flags & TCHARS_SET)
    {
      ioctl (tty, TIOCSETC, &(tiop->tchars));
      tiop->flags &= ~TCHARS_SET;
    }
#endif

#if defined (TIOCSLTC)
  if (tiop->flags & LTCHARS_SET)
    {
      ioctl (tty, TIOCSLTC, &(tiop->ltchars));
      tiop->flags &= ~LTCHARS_SET;
    }
#endif

  return 0;
}

static void
prepare_terminal_settings (int meta_flag, TIOTYPE oldtio, TIOTYPE *tiop)
{
  _rl_echoing_p = (oldtio.sgttyb.sg_flags & ECHO);
  _rl_echoctl = (oldtio.sgttyb.sg_flags & ECHOCTL);

  /* Copy the original settings to the structure we're going to use for
     our settings. */
  tiop->sgttyb = oldtio.sgttyb;
  tiop->lflag = oldtio.lflag;
#if defined (TIOCGETC)
  tiop->tchars = oldtio.tchars;
#endif
#if defined (TIOCGLTC)
  tiop->ltchars = oldtio.ltchars;
#endif
  tiop->flags = oldtio.flags;

  /* First, the basic settings to put us into character-at-a-time, no-echo
     input mode. */
  tiop->sgttyb.sg_flags &= ~(ECHO | CRMOD);
  tiop->sgttyb.sg_flags |= CBREAK;

  /* If this terminal doesn't care how the 8th bit is used, then we can
     use it for the meta-key.  If only one of even or odd parity is
     specified, then the terminal is using parity, and we cannot. */
#if !defined (ANYP)
#  define ANYP (EVENP | ODDP)
#endif
  if (((oldtio.sgttyb.sg_flags & ANYP) == ANYP) ||
      ((oldtio.sgttyb.sg_flags & ANYP) == 0))
    {
      tiop->sgttyb.sg_flags |= ANYP;

      /* Hack on local mode flags if we can. */
#if defined (TIOCLGET)
#  if defined (LPASS8)
      tiop->lflag |= LPASS8;
#  endif /* LPASS8 */
#endif /* TIOCLGET */
    }

#if defined (TIOCGETC)
#  if defined (USE_XON_XOFF)
  /* Get rid of terminal output start and stop characters. */
  tiop->tchars.t_stopc = -1; /* C-s */
  tiop->tchars.t_startc = -1; /* C-q */

  /* If there is an XON character, bind it to restart the output. */
  if (oldtio.tchars.t_startc != -1)
    rl_bind_key (oldtio.tchars.t_startc, rl_restart_output);
#  endif /* USE_XON_XOFF */

  /* If there is an EOF char, bind _rl_eof_char to it. */
  if (oldtio.tchars.t_eofc != -1)
    _rl_eof_char = oldtio.tchars.t_eofc;

#  if defined (NO_KILL_INTR)
  /* Get rid of terminal-generated SIGQUIT and SIGINT. */
  tiop->tchars.t_quitc = -1; /* C-\ */
  tiop->tchars.t_intrc = -1; /* C-c */
#  endif /* NO_KILL_INTR */
#endif /* TIOCGETC */

#if defined (TIOCGLTC)
  /* Make the interrupt keys go away.  Just enough to make people happy. */
  tiop->ltchars.t_dsuspc = -1;	/* C-y */
  tiop->ltchars.t_lnextc = -1;	/* C-v */
#endif /* TIOCGLTC */
}

#else  /* !defined (NEW_TTY_DRIVER) */

#if !defined (VMIN)
#  define VMIN VEOF
#endif

#if !defined (VTIME)
#  define VTIME VEOL
#endif

#if defined (TERMIOS_TTY_DRIVER)
#  define TIOTYPE struct termios
#  define DRAIN_OUTPUT(fd)	tcdrain (fd)
#  define GETATTR(tty, tiop)	(tcgetattr (tty, tiop))
#  ifdef M_UNIX
#    define SETATTR(tty, tiop)	(tcsetattr (tty, TCSANOW, tiop))
#  else
#    define SETATTR(tty, tiop)	(tcsetattr (tty, TCSADRAIN, tiop))
#  endif /* !M_UNIX */
#else
#  define TIOTYPE struct termio
#  define DRAIN_OUTPUT(fd)
#  define GETATTR(tty, tiop)	(ioctl (tty, TCGETA, tiop))
#  define SETATTR(tty, tiop)	(ioctl (tty, TCSETAW, tiop))
#endif /* !TERMIOS_TTY_DRIVER */

static TIOTYPE otio;

static void save_tty_chars (TIOTYPE *);
static int _get_tty_settings (int, TIOTYPE *);
static int get_tty_settings (int, TIOTYPE *);
static int _set_tty_settings (int, TIOTYPE *);
static int set_tty_settings (int, TIOTYPE *);

static void prepare_terminal_settings (int, TIOTYPE, TIOTYPE *);

static void set_special_char (Keymap, TIOTYPE *, int, rl_command_func_t *);
static void _rl_bind_tty_special_chars (Keymap, TIOTYPE);

#if defined (FLUSHO)
#  define OUTPUT_BEING_FLUSHED(tp)  (tp->c_lflag & FLUSHO)
#else
#  define OUTPUT_BEING_FLUSHED(tp)  0
#endif

static void
save_tty_chars (TIOTYPE *tiop)
{
  _rl_last_tty_chars = _rl_tty_chars;

  _rl_tty_chars.t_eof = tiop->c_cc[VEOF];
  _rl_tty_chars.t_eol = tiop->c_cc[VEOL];
#ifdef VEOL2
  _rl_tty_chars.t_eol2 = tiop->c_cc[VEOL2];
#endif
  _rl_tty_chars.t_erase = tiop->c_cc[VERASE];
#ifdef VWERASE
  _rl_tty_chars.t_werase = tiop->c_cc[VWERASE];
#endif
  _rl_tty_chars.t_kill = tiop->c_cc[VKILL];
#ifdef VREPRINT
  _rl_tty_chars.t_reprint = tiop->c_cc[VREPRINT];
#endif
  _rl_intr_char = _rl_tty_chars.t_intr = tiop->c_cc[VINTR];
  _rl_quit_char = _rl_tty_chars.t_quit = tiop->c_cc[VQUIT];
#ifdef VSUSP
  _rl_susp_char = _rl_tty_chars.t_susp = tiop->c_cc[VSUSP];
#endif
#ifdef VDSUSP
  _rl_tty_chars.t_dsusp = tiop->c_cc[VDSUSP];
#endif
#ifdef VSTART
  _rl_tty_chars.t_start = tiop->c_cc[VSTART];
#endif
#ifdef VSTOP
  _rl_tty_chars.t_stop = tiop->c_cc[VSTOP];
#endif
#ifdef VLNEXT
  _rl_tty_chars.t_lnext = tiop->c_cc[VLNEXT];
#endif
#ifdef VDISCARD
  _rl_tty_chars.t_flush = tiop->c_cc[VDISCARD];
#endif
#ifdef VSTATUS
  _rl_tty_chars.t_status = tiop->c_cc[VSTATUS];
#endif
}

#if defined (_AIX) || defined (_AIX41)
/* Currently this is only used on AIX */
static void
rltty_warning (char *msg)
{
  _rl_errmsg ("warning: %s", msg);
}
#endif

#if defined (_AIX)
void
setopost (TIOTYPE *tp)
{
  if ((tp->c_oflag & OPOST) == 0)
    {
      _rl_errmsg ("warning: turning on OPOST for terminal\r");
      tp->c_oflag |= OPOST|ONLCR;
    }
}
#endif

static int
_get_tty_settings (int tty, TIOTYPE *tiop)
{
  int ioctl_ret;

  while (1)
    {
      ioctl_ret = GETATTR (tty, tiop);
      if (ioctl_ret < 0)
	{
	  if (errno != EINTR)
	    return -1;
	  else
	    continue;
	}
      if (OUTPUT_BEING_FLUSHED (tiop))
	{
#if defined (FLUSHO)
	  _rl_errmsg ("warning: turning off output flushing");
	  tiop->c_lflag &= ~FLUSHO;
	  break;
#else
	  continue;
#endif
	}
      break;
    }

  return 0;
}

static int
get_tty_settings (int tty, TIOTYPE *tiop)
{
  set_winsize (tty);

  errno = 0;
  if (_get_tty_settings (tty, tiop) < 0)
    return -1;

#if defined (_AIX)
  setopost(tiop);
#endif

  return 0;
}

static int
_set_tty_settings (int tty, TIOTYPE *tiop)
{
  while (SETATTR (tty, tiop) < 0)
    {
      if (errno != EINTR)
	return -1;
      errno = 0;
    }
  return 0;
}

static int
set_tty_settings (int tty, TIOTYPE *tiop)
{
  if (_set_tty_settings (tty, tiop) < 0)
    return -1;
    
#if 0

#if defined (TERMIOS_TTY_DRIVER)
#  if defined (__ksr1__)
  if (ksrflow)
    {
      ksrflow = 0;
      tcflow (tty, TCOON);
    }
#  else /* !ksr1 */
  tcflow (tty, TCOON);		/* Simulate a ^Q. */
#  endif /* !ksr1 */
#else
  ioctl (tty, TCXONC, 1);	/* Simulate a ^Q. */
#endif /* !TERMIOS_TTY_DRIVER */

#endif /* 0 */

  return 0;
}

static void
prepare_terminal_settings (int meta_flag, TIOTYPE oldtio, TIOTYPE *tiop)
{
  int sc;
  Keymap kmap;

  _rl_echoing_p = (oldtio.c_lflag & ECHO);
#if defined (ECHOCTL)
  _rl_echoctl = (oldtio.c_lflag & ECHOCTL);
#endif

  tiop->c_lflag &= ~(ICANON | ECHO);

  if ((unsigned char) oldtio.c_cc[VEOF] != (unsigned char) _POSIX_VDISABLE)
    _rl_eof_char = oldtio.c_cc[VEOF];

#if defined (USE_XON_XOFF)
#if defined (IXANY)
  tiop->c_iflag &= ~(IXON | IXANY);
#else
  /* `strict' Posix systems do not define IXANY. */
  tiop->c_iflag &= ~IXON;
#endif /* IXANY */
#endif /* USE_XON_XOFF */

  /* Only turn this off if we are using all 8 bits. */
  if (((tiop->c_cflag & CSIZE) == CS8) || meta_flag)
    tiop->c_iflag &= ~(ISTRIP | INPCK);

  /* Make sure we differentiate between CR and NL on input. */
  tiop->c_iflag &= ~(ICRNL | INLCR);

#if !defined (HANDLE_SIGNALS)
  tiop->c_lflag &= ~ISIG;
#else
  tiop->c_lflag |= ISIG;
#endif

  tiop->c_cc[VMIN] = 1;
  tiop->c_cc[VTIME] = 0;

#if defined (FLUSHO)
  if (OUTPUT_BEING_FLUSHED (tiop))
    {
      tiop->c_lflag &= ~FLUSHO;
      oldtio.c_lflag &= ~FLUSHO;
    }
#endif

  /* Turn off characters that we need on Posix systems with job control,
     just to be sure.  This includes ^Y and ^V.  This should not really
     be necessary.  */
#if defined (TERMIOS_TTY_DRIVER) && defined (_POSIX_VDISABLE)

#if defined (VLNEXT)
  tiop->c_cc[VLNEXT] = _POSIX_VDISABLE;
#endif

#if defined (VDSUSP)
  tiop->c_cc[VDSUSP] = _POSIX_VDISABLE;
#endif

  /* Conditionally disable some other tty special characters if there is a
     key binding for them in the current keymap.  Readline ordinarily doesn't
     bind these characters, but an application or user might. */
#if defined (VI_MODE)
      kmap = (rl_editing_mode == vi_mode) ? vi_insertion_keymap : _rl_keymap;
#else
      kmap = _rl_keymap;
#endif
#if defined (VDISCARD)
  sc = tiop->c_cc[VDISCARD];
  if (sc != _POSIX_VDISABLE && kmap[(unsigned char)sc].type == ISFUNC)
    tiop->c_cc[VDISCARD] = _POSIX_VDISABLE;
#endif /* VDISCARD */

#endif /* TERMIOS_TTY_DRIVER && _POSIX_VDISABLE */
}
#endif  /* !NEW_TTY_DRIVER */

/* Put the terminal in CBREAK mode so that we can detect key presses. */
#if defined (NO_TTY_DRIVER)
void
rl_prep_terminal (int meta_flag)
{
  _rl_echoing_p = 1;
}

void
rl_deprep_terminal (void)
{
}

#else /* ! NO_TTY_DRIVER */
void
rl_prep_terminal (int meta_flag)
{
  int tty, nprep;
  TIOTYPE tio;

  if (terminal_prepped)
    return;

  /* Try to keep this function from being INTerrupted. */
  _rl_block_sigint ();

  tty = rl_instream ? fileno (rl_instream) : fileno (stdin);

  if (get_tty_settings (tty, &tio) < 0)
    {
#if defined (ENOTSUP)
      /* MacOS X and Linux, at least, lie about the value of errno if
	 tcgetattr fails. */
      if (errno == ENOTTY || errno == EINVAL || errno == ENOTSUP)
#else
      if (errno == ENOTTY || errno == EINVAL)
#endif
	_rl_echoing_p = 1;		/* XXX */

      _rl_release_sigint ();
      return;
    }

  otio = tio;

  if (_rl_bind_stty_chars)
    {
#if defined (VI_MODE)
      /* If editing in vi mode, make sure we restore the bindings in the
	 insertion keymap no matter what keymap we ended up in. */
      if (rl_editing_mode == vi_mode)
	rl_tty_unset_default_bindings (vi_insertion_keymap);
      else
#endif
	rl_tty_unset_default_bindings (_rl_keymap);
    }
  save_tty_chars (&otio);
  RL_SETSTATE(RL_STATE_TTYCSAVED);
  if (_rl_bind_stty_chars)
    {
#if defined (VI_MODE)
      /* If editing in vi mode, make sure we set the bindings in the
	 insertion keymap no matter what keymap we ended up in. */
      if (rl_editing_mode == vi_mode)
	_rl_bind_tty_special_chars (vi_insertion_keymap, tio);
      else
#endif
	_rl_bind_tty_special_chars (_rl_keymap, tio);
    }

  prepare_terminal_settings (meta_flag, otio, &tio);

  if (set_tty_settings (tty, &tio) < 0)
    {
      _rl_release_sigint ();
      return;
    }

  if (_rl_enable_keypad)
    _rl_control_keypad (1);

  nprep = TPX_PREPPED;

  if (_rl_enable_bracketed_paste)
    {
      fprintf (rl_outstream, BRACK_PASTE_INIT);
      nprep |= TPX_BRACKPASTE;
    }

  fflush (rl_outstream);
  terminal_prepped = nprep;
  RL_SETSTATE(RL_STATE_TERMPREPPED);

  _rl_release_sigint ();
}

/* Restore the terminal's normal settings and modes. */
void
rl_deprep_terminal (void)
{
  int tty;

  if (terminal_prepped == 0)
    return;

  /* Try to keep this function from being interrupted. */
  _rl_block_sigint ();

  tty = rl_instream ? fileno (rl_instream) : fileno (stdin);

  if (terminal_prepped & TPX_BRACKPASTE)
    {
      fprintf (rl_outstream, BRACK_PASTE_FINI);
      /* Since the last character in BRACK_PASTE_FINI is \r */
      _rl_last_c_pos = 0;
      if (rl_eof_found && (RL_ISSTATE (RL_STATE_TIMEOUT) == 0))
 	fprintf (rl_outstream, "\n");
      else if (_rl_echoing_p == 0)
 	fprintf (rl_outstream, "\n");
    }

  if (_rl_enable_keypad)
    _rl_control_keypad (0);

  fflush (rl_outstream);

  if (set_tty_settings (tty, &otio) < 0)
    {
      _rl_release_sigint ();
      return;
    }

  terminal_prepped = 0;
  RL_UNSETSTATE(RL_STATE_TERMPREPPED);

  _rl_release_sigint ();
}
#endif /* !NO_TTY_DRIVER */

/* Set readline's idea of whether or not it is echoing output to the terminal,
   returning the old value. */
int
rl_tty_set_echoing (int u)
{
  int o;

  o = _rl_echoing_p;
  _rl_echoing_p = u;
  return o;
}

/* **************************************************************** */
/*								    */
/*			Bogus Flow Control      		    */
/*								    */
/* **************************************************************** */

int
rl_restart_output (int count, int key)
{
#if defined (__MINGW32__)
  return 0;
#else /* !__MING32__ */

  int fildes = fileno (rl_outstream);
#if defined (TIOCSTART)
#if defined (apollo)
  ioctl (&fildes, TIOCSTART, 0);
#else
  ioctl (fildes, TIOCSTART, 0);
#endif /* apollo */

#else /* !TIOCSTART */
#  if defined (TERMIOS_TTY_DRIVER)
#    if defined (__ksr1__)
  if (ksrflow)
    {
      ksrflow = 0;
      tcflow (fildes, TCOON);
    }
#    else /* !ksr1 */
  tcflow (fildes, TCOON);		/* Simulate a ^Q. */
#    endif /* !ksr1 */
#  else /* !TERMIOS_TTY_DRIVER */
#    if defined (TCXONC)
  ioctl (fildes, TCXONC, TCOON);
#    endif /* TCXONC */
#  endif /* !TERMIOS_TTY_DRIVER */
#endif /* !TIOCSTART */

  return 0;
#endif /* !__MINGW32__ */
}

int
rl_stop_output (int count, int key)
{
#if defined (__MINGW32__)
  return 0;
#else

  int fildes = fileno (rl_instream);

#if defined (TIOCSTOP)
# if defined (apollo)
  ioctl (&fildes, TIOCSTOP, 0);
# else
  ioctl (fildes, TIOCSTOP, 0);
# endif /* apollo */
#else /* !TIOCSTOP */
# if defined (TERMIOS_TTY_DRIVER)
#  if defined (__ksr1__)
  ksrflow = 1;
#  endif /* ksr1 */
  tcflow (fildes, TCOOFF);
# else
#   if defined (TCXONC)
  ioctl (fildes, TCXONC, TCOON);
#   endif /* TCXONC */
# endif /* !TERMIOS_TTY_DRIVER */
#endif /* !TIOCSTOP */

  return 0;
#endif /* !__MINGW32__ */
}

/* **************************************************************** */
/*								    */
/*			Default Key Bindings			    */
/*								    */
/* **************************************************************** */

#if !defined (NO_TTY_DRIVER)
#define SET_SPECIAL(sc, func)	set_special_char(kmap, &ttybuff, sc, func)
#endif

#if defined (NO_TTY_DRIVER)

#define SET_SPECIAL(sc, func)
#define RESET_SPECIAL(c)

#elif defined (NEW_TTY_DRIVER)
static void
set_special_char (Keymap kmap, TIOTYPE *tiop, int sc, rl_command_func_t *func)
{
  if (sc != -1 && kmap[(unsigned char)sc].type == ISFUNC)
    kmap[(unsigned char)sc].function = func;
}

#define RESET_SPECIAL(c) \
  if (c != -1 && kmap[(unsigned char)c].type == ISFUNC) \
    kmap[(unsigned char)c].function = rl_insert;

static void
_rl_bind_tty_special_chars (Keymap kmap, TIOTYPE ttybuff)
{
  if (ttybuff.flags & SGTTY_SET)
    {
      SET_SPECIAL (ttybuff.sgttyb.sg_erase, rl_rubout);
      SET_SPECIAL (ttybuff.sgttyb.sg_kill, rl_unix_line_discard);
    }

#  if defined (TIOCGLTC)
  if (ttybuff.flags & LTCHARS_SET)
    {
      SET_SPECIAL (ttybuff.ltchars.t_werasc, rl_unix_word_rubout);
      SET_SPECIAL (ttybuff.ltchars.t_lnextc, rl_quoted_insert);
    }
#  endif /* TIOCGLTC */
}

#else /* !NEW_TTY_DRIVER */
static void
set_special_char (Keymap kmap, TIOTYPE *tiop, int sc, rl_command_func_t *func)
{
  unsigned char uc;

  uc = tiop->c_cc[sc];
  if (uc != (unsigned char)_POSIX_VDISABLE && kmap[uc].type == ISFUNC)
    kmap[uc].function = func;
}

/* used later */
#define RESET_SPECIAL(uc) \
  if (uc != (unsigned char)_POSIX_VDISABLE && kmap[uc].type == ISFUNC) \
    kmap[uc].function = rl_insert;

static void
_rl_bind_tty_special_chars (Keymap kmap, TIOTYPE ttybuff)
{
  SET_SPECIAL (VERASE, rl_rubout);
  SET_SPECIAL (VKILL, rl_unix_line_discard);

#  if defined (VLNEXT) && defined (TERMIOS_TTY_DRIVER)
  SET_SPECIAL (VLNEXT, rl_quoted_insert);
#  endif /* VLNEXT && TERMIOS_TTY_DRIVER */

#  if defined (VWERASE) && defined (TERMIOS_TTY_DRIVER)
#    if defined (VI_MODE)
  if (rl_editing_mode == vi_mode)
    SET_SPECIAL (VWERASE, rl_vi_unix_word_rubout);
  else
#    endif
  SET_SPECIAL (VWERASE, rl_unix_word_rubout);
#  endif /* VWERASE && TERMIOS_TTY_DRIVER */
}

#endif /* !NEW_TTY_DRIVER */

/* Set the system's default editing characters to their readline equivalents
   in KMAP.  Should be static, now that we have rl_tty_set_default_bindings. */
void
rltty_set_default_bindings (Keymap kmap)
{
#if !defined (NO_TTY_DRIVER)
  TIOTYPE ttybuff;
  int tty;

  tty = fileno (rl_instream);

  if (get_tty_settings (tty, &ttybuff) == 0)
    _rl_bind_tty_special_chars (kmap, ttybuff);
#endif
}

/* New public way to set the system default editing chars to their readline
   equivalents. */
void
rl_tty_set_default_bindings (Keymap kmap)
{
  rltty_set_default_bindings (kmap);
}

/* Rebind all of the tty special chars that readline worries about back
   to self-insert.  Call this before saving the current terminal special
   chars with save_tty_chars().  This only works on POSIX termios or termio
   systems. */
void
rl_tty_unset_default_bindings (Keymap kmap)
{
  /* Don't bother before we've saved the tty special chars at least once. */
  if (RL_ISSTATE(RL_STATE_TTYCSAVED) == 0)
    return;

  RESET_SPECIAL (_rl_tty_chars.t_erase);
  RESET_SPECIAL (_rl_tty_chars.t_kill);

#  if defined (VLNEXT) && defined (TERMIOS_TTY_DRIVER)
  RESET_SPECIAL (_rl_tty_chars.t_lnext);
#  endif /* VLNEXT && TERMIOS_TTY_DRIVER */

#  if defined (VWERASE) && defined (TERMIOS_TTY_DRIVER)
  RESET_SPECIAL (_rl_tty_chars.t_werase);
#  endif /* VWERASE && TERMIOS_TTY_DRIVER */
}

#if defined (HANDLE_SIGNALS)

#if defined (NEW_TTY_DRIVER) || defined (NO_TTY_DRIVER)
int
_rl_disable_tty_signals (void)
{
  return 0;
}

int
_rl_restore_tty_signals (void)
{
  return 0;
}
#else

static TIOTYPE sigstty, nosigstty;
static int tty_sigs_disabled = 0;

int
_rl_disable_tty_signals (void)
{
  if (tty_sigs_disabled)
    return 0;

  if (_get_tty_settings (fileno (rl_instream), &sigstty) < 0)
    return -1;

  nosigstty = sigstty;

  nosigstty.c_lflag &= ~ISIG;
  nosigstty.c_iflag &= ~IXON;

  if (_set_tty_settings (fileno (rl_instream), &nosigstty) < 0)
    return (_set_tty_settings (fileno (rl_instream), &sigstty));

  tty_sigs_disabled = 1;
  return 0;
}

int
_rl_restore_tty_signals (void)
{
  int r;

  if (tty_sigs_disabled == 0)
    return 0;

  r = _set_tty_settings (fileno (rl_instream), &sigstty);

  if (r == 0)
    tty_sigs_disabled = 0;

  return r;
}
#endif /* !NEW_TTY_DRIVER */

#endif /* HANDLE_SIGNALS */
