/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 2010, 2012 David E. O'Brien                                    │
│ Copyright (c) 1980, 1992, 1993                                               │
│      The Regents of the University of California.  All rights reserved.      │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions           │
│ are met:                                                                     │
│  1. Redistributions of source code must retain the above copyright           │
│     notice, this list of conditions and the following disclaimer.            │
│  2. Redistributions in binary form must reproduce the above copyright        │
│     notice, this list of conditions and the following disclaimer in the      │
│     documentation and/or other materials provided with the distribution.     │
│  3. Neither the name of the University nor the names of its contributors     │
│     may be used to endorse or promote products derived from this software    │
│     without specific prior written permission.                               │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND      │
│ ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE        │
│ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   │
│ ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE     │
│ FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   │
│ DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS      │
│ OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)        │
│ HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   │
│ LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    │
│ OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       │
│ SUCH DAMAGE.                                                                 │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bswap.h"
#include "libc/log/bsd.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/select.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/termios.h"
#include "libc/time/time.h"
#include "third_party/getopt/getopt.internal.h"
// clang-format off

/**
 * @fileoverview Terminal Screencast Recorder / Player, e.g.
 *
 *     make o//examples/script.com
 *     o//examples/script.com -r
 *     # type stuff..
 *     # CTRL-D
 *     o//examples/script.com -p typescript
 *
 * @note works on Linux, OpenBSD, NetBSD, FreeBSD, MacOS
 * @see https://asciinema.org/
 */

asm(".ident\t\"\\n\\n\
FreeBSD Script (BSD-3 License)\\n\
Copyright (c) 2010, 2012 David E. O'Brien\\n\
Copyright (c) 1980, 1992, 1993\\n\
\tThe Regents of the University of California.\\n\
\tAll rights reserved.\"");
asm(".include \"libc/disclaimer.inc\"");

#define DEF_BUF 65536

struct stamp {
	uint64_t scr_len;	/* amount of data */
	uint64_t scr_sec;	/* time it arrived in seconds... */
	uint32_t scr_usec;	/* ...and microseconds */
	uint32_t scr_direction; /* 'i', 'o', etc (also indicates endianness) */
};

static FILE *fscript;
static int master, slave;
static int child;
static const char *fname;
static char *fmfname;
static int qflg, ttyflg;
static int usesleep, rawout, showexit;

static struct termios tt;

static void done(int) wontreturn;
static void doshell(char **);
static void finish(void);
static void record(FILE *, char *, size_t, int);
static void consume(FILE *, off_t, char *, int);
static void playback(FILE *) wontreturn;
static void usage(void);

int
main(int argc, char *argv[])
{
	int cc;
	struct termios rtt, stt;
	struct winsize win;
	struct timeval tv, *tvp;
	time_t tvec, start;
	static char obuf[BUFSIZ];
	static char ibuf[BUFSIZ];
	fd_set rfd;
	int fm_fd;
	int aflg, Fflg, kflg, pflg, ch, k, n;
	int flushtime, readstdin;

	aflg = Fflg = kflg = pflg = 0;
	usesleep = 1;
	rawout = 0;
	flushtime = 30;
	fm_fd = -1;	/* Shut up stupid "may be used uninitialized" GCC
			   warning. (not needed w/clang) */
	showexit = 0;

	(void)fm_fd;

	while ((ch = getopt(argc, argv, "adeFfkpqrt:")) != -1)
		switch(ch) {
		case 'a':
			aflg = 1;
			break;
		case 'd':
			usesleep = 0;
			break;
		case 'e':	/* Default behavior, accepted for linux compat */
			break;
		case 'F':
			Fflg = 1;
			break;
		case 'k':
			kflg = 1;
			break;
		case 'p':
			pflg = 1;
			break;
		case 'q':
			qflg = 1;
			break;
		case 'r':
			rawout = 1;
			break;
		case 't':
			flushtime = atoi(optarg);
			if (flushtime < 0)
				err(1, "invalid flush time %d", flushtime);
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc > 0) {
		fname = argv[0];
		argv++;
		argc--;
	} else
		fname = "typescript";

	if ((fscript = fopen(fname, pflg ? "r" : aflg ? "a" : "w")) == NULL)
		err(1, "%s", fname);

	if (pflg)
		playback(fscript);

	if (tcgetattr(STDIN_FILENO, &tt) == -1 ||
	    tcgetwinsize(STDIN_FILENO, &win) == -1) {
		if (errno != ENOTTY) /* For debugger. */
			err(1, "tcgetattr/ioctl");
		if (openpty(&master, &slave, NULL, NULL, NULL) == -1)
			err(1, "openpty");
	} else {
		if (openpty(&master, &slave, NULL, &tt, &win) == -1)
			err(1, "openpty");
		ttyflg = 1;
	}

	if (rawout)
		record(fscript, NULL, 0, 's');

	if (!qflg) {
		tvec = time(NULL);
		printf("Script started, output file is %s\n", fname);
		if (!rawout) {
			fprintf(fscript, "Script started on %s",
			    ctime(&tvec));
			if (argv[0]) {
				showexit = 1;
				fprintf(fscript, "Command: ");
				for (k = 0 ; argv[k] ; ++k)
					fprintf(fscript, "%s%s", k ? " " : "",
						argv[k]);
				fprintf(fscript, "\n");
			}
		}
		fflush(fscript);
	}
	if (ttyflg) {
		rtt = tt;
		cfmakeraw(&rtt);
		rtt.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &rtt);
	}

	child = fork();
	if (child < 0) {
		warn("fork");
		done(1);
	}
	if (child == 0) {
		doshell(argv);
	}
	close(slave);

	start = tvec = time(0);
	readstdin = 1;
	for (;;) {
		FD_ZERO(&rfd);
		FD_SET(master, &rfd);
		if (readstdin)
			FD_SET(STDIN_FILENO, &rfd);
		if (!readstdin && ttyflg) {
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			tvp = &tv;
			readstdin = 1;
		} else if (flushtime > 0) {
			tv.tv_sec = flushtime - (tvec - start);
			tv.tv_usec = 0;
			tvp = &tv;
		} else {
			tvp = NULL;
		}
		n = select(master + 1, &rfd, 0, 0, tvp);
		if (n < 0 && errno != EINTR)
			break;
		if (n > 0 && FD_ISSET(STDIN_FILENO, &rfd)) {
			cc = read(STDIN_FILENO, ibuf, BUFSIZ);
			if (cc < 0)
				break;
			if (cc == 0) {
				if (tcgetattr(master, &stt) == 0 &&
				    (stt.c_lflag & ICANON) != 0) {
					write(master, &stt.c_cc[VEOF], 1);
				}
				readstdin = 0;
			}
			if (cc > 0) {
				if (rawout)
					record(fscript, ibuf, cc, 'i');
				write(master, ibuf, cc);
				if (kflg && tcgetattr(master, &stt) >= 0 &&
				    ((stt.c_lflag & ECHO) == 0)) {
					fwrite(ibuf, 1, cc, fscript);
				}
			}
		}
		if (n > 0 && FD_ISSET(master, &rfd)) {
			cc = read(master, obuf, sizeof (obuf));
			if (cc <= 0)
				break;
			write(STDOUT_FILENO, obuf, cc);
			if (rawout)
				record(fscript, obuf, cc, 'o');
			else
				fwrite(obuf, 1, cc, fscript);
		}
		tvec = time(0);
		if (tvec - start >= flushtime) {
			fflush(fscript);
			start = tvec;
		}
		if (Fflg)
			fflush(fscript);
	}
	finish();
	done(0);
}

static void
usage(void)
{
	fprintf(stderr,
		"usage: script [-adfkpqr] [-t time] [file [command ...]]\n");
	exit(1);
}

static void
finish(void)
{
	int e, status;

	if (waitpid(child, &status, 0) == child) {
		if (WIFEXITED(status))
			e = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			e = WTERMSIG(status);
		else /* can't happen */
			e = 1;
		done(e);
	}
}

static void
doshell(char **av)
{
	const char *shell;

	shell = getenv("SHELL");
	if (shell == NULL)
		shell = _PATH_BSHELL;

	close(master);
	fclose(fscript);
	free(fmfname);
	login_tty(slave);
	setenv("SCRIPT", fname, 1);
	if (av[0]) {
		execvp(av[0], av);
		warn("%s", av[0]);
	} else {
		execl(shell, shell, "-i", 0);
		warn("%s", shell);
	}
	exit(1);
}

static void
done(int eno)
{
	time_t tvec;

	if (ttyflg)
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tt);
	tvec = time(NULL);
	if (rawout)
		record(fscript, NULL, 0, 'e');
	if (!qflg) {
		if (!rawout) {
			if (showexit)
				fprintf(fscript, "\nCommand exit status:"
				    " %d", eno);
			fprintf(fscript,"\nScript done on %s",
			    ctime(&tvec));
		}
		printf("\nScript done, output file is %s\n", fname);
	}
	fclose(fscript);
	close(master);
	exit(eno);
}

static void
record(FILE *fp, char *buf, size_t cc, int direction)
{
	struct iovec iov[2];
	struct stamp stamp;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	stamp.scr_len = cc;
	stamp.scr_sec = tv.tv_sec;
	stamp.scr_usec = tv.tv_usec;
	stamp.scr_direction = direction;
	iov[0].iov_len = sizeof(stamp);
	iov[0].iov_base = &stamp;
	iov[1].iov_len = cc;
	iov[1].iov_base = buf;
	if (writev(fileno(fp), &iov[0], 2) == -1)
		err(1, "writev");
}

static void
consume(FILE *fp, off_t len, char *buf, int reg)
{
	size_t l;

	if (reg) {
		if (fseeko(fp, len, SEEK_CUR) == -1)
			err(1, NULL);
	}
	else {
		while (len > 0) {
			l = MIN(DEF_BUF, len);
			if (fread(buf, sizeof(char), l, fp) != l)
				err(1, "cannot read buffer");
			len -= l;
		}
	}
}

#define swapstamp(stamp) do { \
	if (stamp.scr_direction > 0xff) { \
		stamp.scr_len = bswap_64(stamp.scr_len); \
		stamp.scr_sec = bswap_64(stamp.scr_sec); \
		stamp.scr_usec = bswap_32(stamp.scr_usec); \
		stamp.scr_direction = bswap_32(stamp.scr_direction); \
	} \
} while (0/*CONSTCOND*/)

static void
termset(void)
{
	struct termios traw;

	if (tcgetattr(STDOUT_FILENO, &tt) == -1) {
		if (errno != ENOTTY) /* For debugger. */
			err(1, "tcgetattr");
		return;
	}
	ttyflg = 1;
	traw = tt;
	cfmakeraw(&traw);
	traw.c_lflag |= ISIG;
	tcsetattr(STDOUT_FILENO, TCSANOW, &traw);
}

static void
termreset(void)
{
	if (ttyflg) {
		tcsetattr(STDOUT_FILENO, TCSADRAIN, &tt);
		ttyflg = 0;
	}
}

static void
playback(FILE *fp)
{
	struct timespec tsi, tso;
	struct stamp stamp;
	struct stat pst;
	static char buf[DEF_BUF];
	off_t nread, save_len;
	size_t l;
	time_t tclock;
	int reg;

	if (fstat(fileno(fp), &pst) == -1)
		err(1, "fstat failed");

	reg = S_ISREG(pst.st_mode);

	for (nread = 0; !reg || nread < pst.st_size; nread += save_len) {
		if (fread(&stamp, sizeof(stamp), 1, fp) != 1) {
			if (reg)
				err(1, "reading playback header");
			else
				break;
		}
		swapstamp(stamp);
		save_len = sizeof(stamp);

		if (reg && stamp.scr_len >
		    (uint64_t)(pst.st_size - save_len) - nread)
			errx(1, "invalid stamp");

		save_len += stamp.scr_len;
		tclock = stamp.scr_sec;
		tso.tv_sec = stamp.scr_sec;
		tso.tv_nsec = stamp.scr_usec * 1000;

		switch (stamp.scr_direction) {
		case 's':
			if (!qflg)
			    printf("Script started on %s",
				ctime(&tclock));
			tsi = tso;
			consume(fp, stamp.scr_len, buf, reg);
			termset();
			atexit(termreset);
			break;
		case 'e':
			termreset();
			if (!qflg)
				printf("\nScript done on %s",
				    ctime(&tclock));
			consume(fp, stamp.scr_len, buf, reg);
			break;
		case 'i':
			/* throw input away */
			consume(fp, stamp.scr_len, buf, reg);
			break;
		case 'o':
			tsi.tv_sec = tso.tv_sec - tsi.tv_sec;
			tsi.tv_nsec = tso.tv_nsec - tsi.tv_nsec;
			if (tsi.tv_nsec < 0) {
				tsi.tv_sec -= 1;
				tsi.tv_nsec += 1000000000;
			}
			if (usesleep)
				nanosleep(&tsi, NULL);
			tsi = tso;
			while (stamp.scr_len > 0) {
				l = MIN(DEF_BUF, stamp.scr_len);
				if (fread(buf, sizeof(char), l, fp) != l)
					err(1, "cannot read buffer");

				write(STDOUT_FILENO, buf, l);
				stamp.scr_len -= l;
			}
			break;
		default:
			errx(1, "invalid direction");
		}
	}
	fclose(fp);
	exit(0);
}
