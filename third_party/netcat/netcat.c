/* $OpenBSD: netcat.c,v 1.226 2023/08/14 08:07:27 tb Exp $ */

/*
 * Copyright (c) 2001 Eric Jackson <ericj@monkey.org>
 * Copyright (c) 2015 Bob Beck.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Re-written nc(1) for OpenBSD. Original implementation by
 * *Hobbit* <hobbit@avian.org>.
 */

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/telnet.h>
#ifdef __linux__
# include <linux/in6.h>
#endif
#if defined(TCP_MD5SIG_EXT) && defined(TCP_MD5SIG_MAXKEYLEN)
# include <bsd/readpassphrase.h>
#endif

#ifndef IPTOS_LOWDELAY
# define IPTOS_LOWDELAY 0x10
# define IPTOS_THROUGHPUT 0x08
# define IPTOS_RELIABILITY 0x04
# define IPTOS_LOWCOST 0x02
# define IPTOS_MINCOST IPTOS_LOWCOST
#endif /* IPTOS_LOWDELAY */

# ifndef IPTOS_DSCP_AF11
# define	IPTOS_DSCP_AF11		0x28
# define	IPTOS_DSCP_AF12		0x30
# define	IPTOS_DSCP_AF13		0x38
# define	IPTOS_DSCP_AF21		0x48
# define	IPTOS_DSCP_AF22		0x50
# define	IPTOS_DSCP_AF23		0x58
# define	IPTOS_DSCP_AF31		0x68
# define	IPTOS_DSCP_AF32		0x70
# define	IPTOS_DSCP_AF33		0x78
# define	IPTOS_DSCP_AF41		0x88
# define	IPTOS_DSCP_AF42		0x90
# define	IPTOS_DSCP_AF43		0x98
# define	IPTOS_DSCP_EF		0xb8
#endif /* IPTOS_DSCP_AF11 */

#ifndef IPTOS_DSCP_CS0
# define	IPTOS_DSCP_CS0		0x00
# define	IPTOS_DSCP_CS1		0x20
# define	IPTOS_DSCP_CS2		0x40
# define	IPTOS_DSCP_CS3		0x60
# define	IPTOS_DSCP_CS4		0x80
# define	IPTOS_DSCP_CS5		0xa0
# define	IPTOS_DSCP_CS6		0xc0
# define	IPTOS_DSCP_CS7		0xe0
#endif /* IPTOS_DSCP_CS0 */

#ifndef IPTOS_DSCP_EF
# define	IPTOS_DSCP_EF		0xb8
#endif /* IPTOS_DSCP_EF */


#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_TLS
# include <tls.h>
#endif
#include <unistd.h>
#include <bsd/stdlib.h>
#include <bsd/string.h>
#include <cosmo.h>

#include "atomicio.h"

#define PORT_MAX	65535
#define UNIX_DG_TMP_SOCKET_SIZE	25

#define POLL_STDIN	0
#define POLL_NETOUT	1
#define POLL_NETIN	2
#define POLL_STDOUT	3
#define BUFSIZE		16384

#ifdef HAVE_TLS
# define TLS_NOVERIFY	(1 << 1)
# define TLS_NONAME	(1 << 2)
# define TLS_CCERT	(1 << 3)
# define TLS_MUSTSTAPLE	(1 << 4)
#endif

#define CONNECTION_SUCCESS 0
#define CONNECTION_FAILED 1
#define CONNECTION_TIMEOUT 2

#define UDP_SCAN_TIMEOUT 3			/* Seconds */

/* Command Line Options */
int	bflag;					/* Allow Broadcast */
int	dflag;					/* detached, no stdin */
int	Fflag;					/* fdpass sock to stdout */
unsigned int iflag;				/* Interval Flag */
int	kflag;					/* More than one connect */
int	lflag;					/* Bind to local port */
int	Nflag;					/* shutdown() network socket */
int	nflag;					/* Don't do name look up */
char   *Pflag;					/* Proxy username */
char   *pflag;					/* Localport flag */
int    qflag = -1;				/* Quit after some secs */
int	rflag;					/* Random ports flag */
char   *sflag;					/* Source Address */
int	tflag;					/* Telnet Emulation */
int	uflag;					/* UDP - Default to TCP */
int	dccpflag;				/* DCCP - Default to TCP */
int	vflag;					/* Verbosity */
int	xflag;					/* Socks proxy */
int	zflag;					/* Port Scan Flag */
int	Dflag;					/* sodebug */
int	Iflag;					/* TCP receive buffer size */
int	Oflag;					/* TCP send buffer size */
int	Sflag;					/* TCP MD5 signature option */
int	Tflag = -1;				/* IP Type of Service */
int	rtableid = -1;

#ifdef HAVE_TLS
int	usetls;					/* use TLS */
const char    *Cflag;				/* Public cert file */
const char    *Kflag;				/* Private key file */
const char    *oflag;				/* OCSP stapling file */
const char    *Rflag;				/* Root CA file */
int	tls_cachanged;				/* Using non-default CA file */
int	TLSopt;					/* TLS options */
char	*tls_expectname;			/* required name in peer cert */
char	*tls_expecthash;			/* required hash of peer cert */
char	*tls_ciphers;				/* TLS ciphers */
char	*tls_protocols;				/* TLS protocols */
FILE	*Zflag;					/* file to save peer cert */
#else
int	Cflag = 0;			/* CRLF line-ending */
#endif

#if defined(TCP_MD5SIG_EXT) && defined(TCP_MD5SIG_MAXKEYLEN)
char Sflag_password[TCP_MD5SIG_MAXKEYLEN];
#endif
int recvcount, recvlimit;
int timeout = -1;
int family = AF_UNSPEC;
char *portlist[PORT_MAX+1];
char *unix_dg_tmp_socket;
int ttl = -1;
int minttl = -1;

void	atelnet(int, unsigned char *, unsigned int);
int	strtoport(char *portstr, int udp);
void	build_ports(char **);
void	help(void) __attribute__((noreturn));
int	local_listen(const char *, const char *, struct addrinfo);
#ifdef HAVE_TLS
void	readwrite(int, struct tls *);
#else
void	readwrite(int);
#endif
void	fdpass(int nfd) __attribute__((noreturn));
int	remote_connect(const char *, const char *, struct addrinfo, char *);
#ifdef HAVE_TLS
int	timeout_tls(int, struct tls *, int (*)(struct tls *));
#endif
int	timeout_connect(int, const struct sockaddr *, socklen_t);
int	socks_connect(const char *, const char *, struct addrinfo,
	    const char *, const char *, struct addrinfo, int, const char *);
int	udptest(int);
int	unix_setup_sockaddr(char *, struct sockaddr_un *, int *);
void	connection_info(const char *, const char *, const char *, const char *);
int	unix_bind(char *, int);
int	unix_connect(char *);
int	unix_listen(char *);
void	set_common_sockopts(int, const struct sockaddr *);
int	process_tos_opt(char *, int *);
#ifdef HAVE_TLS
int	process_tls_opt(char *, int *);
void	save_peer_cert(struct tls *_tls_ctx, FILE *_fp);
#endif
void	report_sock(const char *, const struct sockaddr *, socklen_t, char *);
#ifdef HAVE_TLS
void	report_tls(struct tls *tls_ctx, char * host);
#endif
void	usage(int);
#ifdef HAVE_TLS
ssize_t drainbuf(int, unsigned char *, size_t *, struct tls *);
ssize_t fillbuf(int, unsigned char *, size_t *, struct tls *);
void	tls_setup_client(struct tls *, int, char *);
struct tls *tls_setup_server(struct tls *, int, char *);
#else
ssize_t drainbuf(int, unsigned char *, size_t *, int);
ssize_t fillbuf(int, unsigned char *, size_t *);
#endif

char *proto_name(int, int);
static int connect_with_timeout(int, const struct sockaddr *, socklen_t, int);
static void quit(int);

int
main(int argc, char *argv[])
{
	int ch, s = -1, ret, socksv;
	char *host, **uport;
	char ipaddr[NI_MAXHOST];
	struct addrinfo hints;
	socklen_t len;
	struct sockaddr_storage cliaddr;
	char *proxy = NULL, *proxyport = NULL;
	const char *errstr;
	struct addrinfo proxyhints;
	char unix_dg_tmp_socket_buf[UNIX_DG_TMP_SOCKET_SIZE];
#ifdef HAVE_TLS
	struct tls_config *tls_cfg = NULL;
	struct tls *tls_ctx = NULL;
	uint32_t protocols;
#endif

	ret = 1;
	socksv = 5;
	host = NULL;
	uport = NULL;
#ifdef HAVE_TLS
	Rflag = tls_default_ca_cert_file();
#endif

	signal(SIGPIPE, SIG_IGN);

	while ((ch = getopt(argc, argv,
#ifdef HAVE_TLS
	    "46bC:cDde:FH:hI:i:K:klM:m:NnO:o:P:p:q:R:rSs:T:tUuV:vW:w:X:x:Z:z"))
#else
	    "46bCDdFhI:i:klM:m:NnO:P:p:q:rSs:T:tUuV:vW:w:X:x:Zz"))
#endif
	    != -1) {
		switch (ch) {
		case '4':
			family = AF_INET;
			break;
		case '6':
			family = AF_INET6;
			break;
		case 'b':
#ifdef SO_BROADCAST
			bflag = 1;
#else
			errx(1, "no broadcast frame support available");
#endif
			break;
		case 'U':
			family = AF_UNIX;
			break;
		case 'X':
			if (strcasecmp(optarg, "connect") == 0)
				socksv = -1; /* HTTP proxy CONNECT */
			else if (strcmp(optarg, "4") == 0)
				socksv = 4; /* SOCKS v.4 */
			else if (strcmp(optarg, "5") == 0)
				socksv = 5; /* SOCKS v.5 */
			else
				errx(1, "unsupported proxy protocol");
			break;
#ifdef HAVE_TLS
		case 'C':
			Cflag = optarg;
			break;
		case 'c':
			usetls = 1;
			break;
#else
		case 'C':
			Cflag = 1;
			break;
#endif
		case 'd':
			dflag = 1;
			break;
#ifdef HAVE_TLS
		case 'e':
			tls_expectname = optarg;
			break;
#endif
		case 'F':
			Fflag = 1;
			break;
#ifdef HAVE_TLS
		case 'H':
			tls_expecthash = optarg;
			break;
#endif
		case 'h':
			help();
			break;
		case 'i':
			iflag = strtonum(optarg, 0, UINT_MAX, &errstr);
			if (errstr)
				errx(1, "interval %s: %s", errstr, optarg);
			break;
#ifdef HAVE_TLS
		case 'K':
			Kflag = optarg;
			break;
#endif
		case 'k':
			kflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'M':
			ttl = strtonum(optarg, 0, 255, &errstr);
			if (errstr)
				errx(1, "ttl is %s", errstr);
			break;
		case 'm':
			minttl = strtonum(optarg, 0, 255, &errstr);
			if (errstr)
				errx(1, "minttl is %s", errstr);
			break;
		case 'N':
			Nflag = 1;
			break;
		case 'n':
			nflag = 1;
			break;
		case 'P':
			Pflag = optarg;
			break;
		case 'p':
			pflag = optarg;
			break;
		case 'q':
			qflag = strtonum(optarg, INT_MIN, INT_MAX, &errstr);
			if (errstr)
				errx(1, "quit timer %s: %s", errstr, optarg);
			if (qflag >= 0)
				Nflag = 1;
			break;
#ifdef HAVE_TLS
		case 'R':
			tls_cachanged = 1;
			Rflag = optarg;
			break;
#endif
		case 'r':
			rflag = 1;
			break;
		case 's':
			sflag = optarg;
			break;
		case 't':
			tflag = 1;
			break;
		case 'u':
			uflag = 1;
			break;
		case 'Z':
#if defined(IPPROTO_DCCP) && defined(SOCK_DCCP)
			dccpflag = 1;
#else
			errx(1, "no DCCP support available");
#endif
			break;
		case 'V':
#ifdef RT_TABLEID_MAX
			rtableid = (int)strtonum(optarg, 0,
			    RT_TABLEID_MAX, &errstr);
			if (errstr)
				errx(1, "rtable %s: %s", errstr, optarg);
#else
			errx(1, "no alternate routing table support available");
#endif
			break;
		case 'v':
			vflag = 1;
			break;
		case 'W':
			recvlimit = strtonum(optarg, 1, INT_MAX, &errstr);
			if (errstr)
				errx(1, "receive limit %s: %s", errstr, optarg);
			break;
		case 'w':
			timeout = strtonum(optarg, 0, INT_MAX / 1000, &errstr);
			if (errstr)
				errx(1, "timeout %s: %s", errstr, optarg);
			timeout *= 1000;
			break;
		case 'x':
			xflag = 1;
			if ((proxy = strdup(optarg)) == NULL)
				err(1, NULL);
			break;
#ifdef HAVE_TLS
		case 'Z':
			if (strcmp(optarg, "-") == 0)
				Zflag = stderr;
			else if ((Zflag = fopen(optarg, "w")) == NULL)
				err(1, "can't open %s", optarg);
			break;
#endif
		case 'z':
			zflag = 1;
			break;
		case 'D':
			Dflag = 1;
			break;
		case 'I':
			Iflag = strtonum(optarg, 1, 65536 << 14, &errstr);
			if (errstr != NULL)
				errx(1, "TCP receive window %s: %s",
				    errstr, optarg);
			break;
		case 'O':
			Oflag = strtonum(optarg, 1, 65536 << 14, &errstr);
			if (errstr != NULL)
				errx(1, "TCP send window %s: %s",
				    errstr, optarg);
			break;
#ifdef HAVE_TLS
		case 'o':
			oflag = optarg;
			break;
#endif
		case 'S':
#if defined(TCP_MD5SIG_EXT) && defined(TCP_MD5SIG_MAXKEYLEN)
			if (readpassphrase("TCP MD5SIG password: ",
					Sflag_password, TCP_MD5SIG_MAXKEYLEN, RPP_REQUIRE_TTY) == NULL)
				errx(1, "Unable to read TCP MD5SIG password");
			Sflag = 1;
#else
			errx(1, "no TCP MD5 signature support available");
#endif
			break;
		case 'T':
			errstr = NULL;
			errno = 0;
#ifdef HAVE_TLS
			if (process_tls_opt(optarg, &TLSopt))
				break;
#endif
			if (process_tos_opt(optarg, &Tflag))
				break;
			if (strlen(optarg) > 1 && optarg[0] == '0' &&
			    optarg[1] == 'x')
				Tflag = (int)strtol(optarg, NULL, 16);
			else
				Tflag = (int)strtonum(optarg, 0, 255,
				    &errstr);
			if (Tflag < 0 || Tflag > 255 || errstr || errno)
#ifdef HAVE_TLS
				errx(1, "illegal tos/tls value %s", optarg);
#else
				errx(1, "illegal tos value %s", optarg);
#endif
			break;
		default:
			usage(1);
		}
	}
	argc -= optind;
	argv += optind;

#ifdef RT_TABLEID_MAX
	if (rtableid >= 0)
		if (setrtable(rtableid) == -1)
			err(1, "setrtable");
#endif

	/* Cruft to make sure options are clean, and used properly. */
	if (argc == 0 && lflag) {
		uport = &pflag;
		host = sflag;
	} else if (argc == 1 && !pflag &&
			/* `nc -l 12345` or `nc -U bar` or `nc -uU -s foo bar` */
			(!sflag || (family == AF_UNIX && uflag && !lflag))) {
		if (family == AF_UNIX) {
			host = argv[0];
			uport = NULL;
		} else if (lflag) {
			host  = NULL;
			uport = argv;
		}
	} else if (argc >= 2) {
		if (lflag && (pflag || sflag || argc > 2))
			usage(1); /* conflict */
		host = argv[0];
		uport = &argv[1];
	} else
		usage(1);

	if (family == AF_UNIX) {
#if defined(IPPROTO_DCCP) && defined(SOCK_DCCP)
		if (dccpflag)
			errx(1, "cannot use -Z and -U");
#endif
		if (uport && *uport)
			errx(1, "cannot use port with -U");
		if (!host)
			errx(1, "missing socket pathname");
	} else if (!uport || !*uport)
		errx(1, "missing port number");

	if (lflag && zflag)
		errx(1, "cannot use -z and -l");

#ifdef HAVE_TLS
	if (usetls) {
		if (Cflag && unveil(Cflag, "r") == -1)
			err(1, "unveil %s", Cflag);
		if (unveil(Rflag, "r") == -1)
			err(1, "unveil %s", Rflag);
		if (Kflag && unveil(Kflag, "r") == -1)
			err(1, "unveil %s", Kflag);
		if (oflag && unveil(oflag, "r") == -1)
			err(1, "unveil %s", oflag);
	} else if (family == AF_UNIX && uflag && lflag && !kflag) {
		/*
		 * After recvfrom(2) from client, the server connects
		 * to the client socket.  As the client path is determined
		 * during runtime, we cannot unveil(2).
		 */
	} else {
		if (family == AF_UNIX) {
			if (unveil(host, "rwc") == -1)
				err(1, "unveil %s", host);
			if (uflag && !kflag) {
				if (sflag) {
					if (unveil(sflag, "rwc") == -1)
						err(1, "unveil %s", sflag);
				} else {
					if (unveil("/tmp", "rwc") == -1)
						err(1, "unveil /tmp");
				}
			}
		} else {
			/* no filesystem visibility */
			if (unveil("/", "") == -1)
				err(1, "unveil /");
		}
	}
#endif

	if (!lflag && kflag)
		errx(1, "must use -l with -k");
#ifdef HAVE_TLS
	if (uflag && usetls)
		errx(1, "cannot use -c and -u");
	if ((family == AF_UNIX) && usetls)
		errx(1, "cannot use -c and -U");
#endif
	if ((family == AF_UNIX) && Fflag)
		errx(1, "cannot use -F and -U");
#ifdef HAVE_TLS
	if (Fflag && usetls)
		errx(1, "cannot use -c and -F");
	if (TLSopt && !usetls)
		errx(1, "you must specify -c to use TLS options");
	if (Cflag && !usetls)
		errx(1, "you must specify -c to use -C");
	if (Kflag && !usetls)
		errx(1, "you must specify -c to use -K");
	if (Zflag && !usetls)
		errx(1, "you must specify -c to use -Z");
	if (oflag && !Cflag)
		errx(1, "you must specify -C to use -o");
	if (tls_cachanged && !usetls)
		errx(1, "you must specify -c to use -R");
	if (tls_expecthash && !usetls)
		errx(1, "you must specify -c to use -H");
	if (tls_expectname && !usetls)
		errx(1, "you must specify -c to use -e");
#endif

	/* Get name of temporary socket for unix datagram client */
	if ((family == AF_UNIX) && uflag && !lflag) {
		if (sflag) {
			unix_dg_tmp_socket = sflag;
		} else {
			/* On Linux it's tempting to use abstract sockets here in
			 * order to limit bookkeeping and avoid cluttering /tmp.
			 * Unfortunately though this has security implications, as a
			 * second client could inject server responses if they
			 * manage to connect(2) to the temporary socket between the
			 * first client's bind(2) and connect(2) calls.  OTOH for
			 * pathname sockets the injection is only possible on Linux
			 * when write access to the socket is granted. */
			strlcpy(unix_dg_tmp_socket_buf, "/tmp/nc-XXXXXX",
			    UNIX_DG_TMP_SOCKET_SIZE);
			if (mkdtemp(unix_dg_tmp_socket_buf) == NULL)
				err(1, "mkdtemp");
			strlcat(unix_dg_tmp_socket_buf, "/recv.sock",
			    UNIX_DG_TMP_SOCKET_SIZE);
			unix_dg_tmp_socket = unix_dg_tmp_socket_buf;
		}
	}

	/* Initialize addrinfo structure. */
	if (family != AF_UNIX) {
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = family;
		if (uflag) {
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
		}
#if defined(IPPROTO_DCCP) && defined(SOCK_DCCP)
		else if (dccpflag) {
			hints.ai_socktype = SOCK_DCCP;
			hints.ai_protocol = IPPROTO_DCCP;
		}
#endif
		else {
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
		}
		if (nflag)
			hints.ai_flags |= AI_NUMERICHOST;
	}

	if (xflag) {
		if (uflag)
			errx(1, "no proxy support for UDP mode");
#if defined(IPPROTO_DCCP) && defined(SOCK_DCCP)
		if (dccpflag)
			errx(1, "no proxy support for DCCP mode");
#endif
		if (lflag)
			errx(1, "no proxy support for listen");

		if (family == AF_UNIX)
			errx(1, "no proxy support for unix sockets");

		if (sflag)
			errx(1, "no proxy support for local source address");

		if (*proxy == '[') {
			++proxy;
			proxyport = strchr(proxy, ']');
			if (proxyport == NULL)
				errx(1, "missing closing bracket in proxy");
			*proxyport++ = '\0';
			if (*proxyport == '\0')
				/* Use default proxy port. */
				proxyport = NULL;
			else {
				if (*proxyport == ':')
					++proxyport;
				else
					errx(1, "garbage proxy port delimiter");
			}
		} else {
			proxyport = strrchr(proxy, ':');
			if (proxyport != NULL)
				*proxyport++ = '\0';
		}

		memset(&proxyhints, 0, sizeof(struct addrinfo));
		proxyhints.ai_family = family;
		proxyhints.ai_socktype = SOCK_STREAM;
		proxyhints.ai_protocol = IPPROTO_TCP;
		if (nflag)
			proxyhints.ai_flags |= AI_NUMERICHOST;
	}

#ifdef HAVE_TLS
	if (usetls) {
		if ((tls_cfg = tls_config_new()) == NULL)
			errx(1, "unable to allocate TLS config");
		if (Rflag && tls_config_set_ca_file(tls_cfg, Rflag) == -1)
			errx(1, "%s", tls_config_error(tls_cfg));
		if (Cflag && tls_config_set_cert_file(tls_cfg, Cflag) == -1)
			errx(1, "%s", tls_config_error(tls_cfg));
		if (Kflag && tls_config_set_key_file(tls_cfg, Kflag) == -1)
			errx(1, "%s", tls_config_error(tls_cfg));
		if (oflag && tls_config_set_ocsp_staple_file(tls_cfg, oflag) == -1)
			errx(1, "%s", tls_config_error(tls_cfg));
		if (tls_config_parse_protocols(&protocols, tls_protocols) == -1)
			errx(1, "invalid TLS protocols `%s'", tls_protocols);
		if (tls_config_set_protocols(tls_cfg, protocols) == -1)
			errx(1, "%s", tls_config_error(tls_cfg));
		if (tls_config_set_ciphers(tls_cfg, tls_ciphers) == -1)
			errx(1, "%s", tls_config_error(tls_cfg));
		if (!lflag && (TLSopt & TLS_CCERT))
			errx(1, "clientcert is only valid with -l");
		if (TLSopt & TLS_NONAME)
			tls_config_insecure_noverifyname(tls_cfg);
		if (TLSopt & TLS_NOVERIFY) {
			if (tls_expecthash != NULL)
				errx(1, "-H and -T noverify may not be used "
				    "together");
			tls_config_insecure_noverifycert(tls_cfg);
		}
		if (TLSopt & TLS_MUSTSTAPLE)
			tls_config_ocsp_require_stapling(tls_cfg);

		if (Pflag) {
			if (pledge("stdio inet dns tty", NULL) == -1)
				err(1, "pledge");
		} else if (pledge("stdio inet dns", NULL) == -1)
			err(1, "pledge");
	}
#endif
	if (lflag) {
		ret = 0;

		if (family == AF_UNIX) {
			if (uflag)
				s = unix_bind(host, 0);
			else
				s = unix_listen(host);
		} else
			s = local_listen(host, *uport, hints);
		if (s < 0)
			err(1, NULL);

#ifdef HAVE_TLS
		if (usetls) {
			tls_config_verify_client_optional(tls_cfg);
			if ((tls_ctx = tls_server()) == NULL)
				errx(1, "tls server creation failed");
			if (tls_configure(tls_ctx, tls_cfg) == -1)
				errx(1, "tls configuration failed (%s)",
				    tls_error(tls_ctx));
		}
#endif
		/* Allow only one connection at a time, but stay alive. */
		for (;;) {
			if (uflag && kflag) {
				/*
				 * For UDP and -k, don't connect the socket,
				 * let it receive datagrams from multiple
				 * socket pairs.
				 */
#ifdef HAVE_TLS
				readwrite(s, NULL);
#else
				readwrite(s);
#endif
			} else if (uflag && !kflag) {
				/*
				 * For UDP and not -k, we will use recvfrom()
				 * initially to wait for a caller, then use
				 * the regular functions to talk to the caller.
				 */
				int rv;
				char buf[2048];
				struct sockaddr_storage z;

				len = sizeof(z);
				rv = recvfrom(s, buf, sizeof(buf), MSG_PEEK,
				    (struct sockaddr *)&z, &len);
				if (rv == -1)
					err(1, "recvfrom");

				rv = connect(s, (struct sockaddr *)&z, len);
				if (rv == -1)
					err(1, "connect");

				if (vflag)
					report_sock("Connection received",
					    (struct sockaddr *)&z, len,
					    family == AF_UNIX ? host : NULL);

#ifdef HAVE_TLS
				readwrite(s, NULL);
			} else {
				struct tls *tls_cctx = NULL;
#else
				readwrite(s);
			} else {
#endif
				int connfd;

				len = sizeof(cliaddr);
				connfd = accept4(s, (struct sockaddr *)&cliaddr,
				    &len, SOCK_NONBLOCK);
				if (connfd == -1) {
					/* For now, all errnos are fatal */
					err(1, "accept");
				}
				if (vflag)
					report_sock("Connection received",
					    (struct sockaddr *)&cliaddr, len,
					    family == AF_UNIX ? host : NULL);
#ifdef HAVE_TLS
				if ((usetls) &&
				    (tls_cctx = tls_setup_server(tls_ctx, connfd, host)))
					readwrite(connfd, tls_cctx);
				if (!usetls)
					readwrite(connfd, NULL);
				if (tls_cctx)
					timeout_tls(s, tls_cctx, tls_close);
				close(connfd);
				tls_free(tls_cctx);
#else
				readwrite(connfd);
				close(connfd);
#endif
			}
			if (family == AF_UNIX && uflag) {
				if (connect(s, NULL, 0) == -1)
					err(1, "connect");
			}

			if (!kflag) {
				if (s != -1)
					close(s);
				break;
			}
		}
	} else if (family == AF_UNIX) {
		ret = 0;

		if ((s = unix_connect(host)) > 0) {
			if (!zflag)
#ifdef HAVE_TLS
				readwrite(s, NULL);
#else
				readwrite(s);
#endif
			close(s);
		} else {
			warn("%s", host);
			ret = 1;
		}

		if (uflag && !sflag) {
			unlink(unix_dg_tmp_socket);
			char *nam = strrchr(unix_dg_tmp_socket, '/');
			if (nam != NULL) {
				nam[0] = '\0';
				rmdir(unix_dg_tmp_socket);
			}
		}
		return ret;
	} else {
		int i = 0;

		/* Construct the portlist[] array. */
		build_ports(uport);

		/* Cycle through portlist, connecting to each port. */
		for (s = -1, i = 0; portlist[i] != NULL; i++) {
			if (s != -1)
				close(s);
#ifdef HAVE_TLS
			tls_free(tls_ctx);
			tls_ctx = NULL;

			if (usetls) {
				if ((tls_ctx = tls_client()) == NULL)
					errx(1, "tls client creation failed");
				if (tls_configure(tls_ctx, tls_cfg) == -1)
					errx(1, "tls configuration failed (%s)",
					    tls_error(tls_ctx));
			}
#endif
			if (xflag)
				s = socks_connect(host, portlist[i], hints,
				    proxy, proxyport, proxyhints, socksv,
				    Pflag);
			else
				s = remote_connect(host, portlist[i], hints,
				    ipaddr);

			if (s == -1)
				continue;

			ret = 0;
			if (vflag || zflag) {
				int print_info = 1;

				/* For UDP, make sure we are connected. */
				if (uflag) {
					/* No info on failed or skipped test. */
					if ((print_info = udptest(s)) == -1) {
						ret = 1;
						continue;
					}
				}
				if (print_info == 1) {
					char *proto = proto_name(uflag, dccpflag);
					connection_info(host, portlist[i],
					    proto, ipaddr);
				}
			}
			if (Fflag)
				fdpass(s);
#ifdef HAVE_TLS
			else {
				if (usetls)
					tls_setup_client(tls_ctx, s, host);
				if (!zflag)
					readwrite(s, tls_ctx);
				if (tls_ctx)
					timeout_tls(s, tls_ctx, tls_close);
			}
#else
			else if (!zflag)
				readwrite(s);
#endif
		}
	}

	if (s != -1)
		close(s);
#ifdef HAVE_TLS
	tls_free(tls_ctx);
	tls_config_free(tls_cfg);
#endif

	return ret;
}

int
unix_setup_sockaddr(char *path, struct sockaddr_un *s_un, int *addrlen)
{
	int sun_path_len;

	*addrlen = offsetof(struct sockaddr_un, sun_path);
	memset(s_un, 0, *addrlen);
	s_un->sun_family = AF_UNIX;

	if (path[0] == '\0') {
		/* Always reject the empty path, aka NUL abstract socket on
		 * Linux (OTOH the *empty* abstract socket is supported and
		 * specified as @""). */
		errno = EINVAL;
		return -1;
	}
#ifdef __linux__
	/* If the unix domain socket path starts with '@',
	 * treat it as a Linux abstract name. */
	else if (path[0] == '@') {
		if ((sun_path_len = strlen(path)) <= sizeof(s_un->sun_path)) {
			s_un->sun_path[0] = '\0';
			strncpy(s_un->sun_path+1, path+1, sun_path_len-1);
			*addrlen += sun_path_len;
		} else {
			errno = ENAMETOOLONG;
			return -1;
		}
	}
#endif
	else if ((sun_path_len = strlcpy(s_un->sun_path, path, sizeof(s_un->sun_path))) <
	    sizeof(s_un->sun_path))
		*addrlen += sun_path_len + 1; /* account for trailing '\0' */
	else {
		errno = ENAMETOOLONG;
		return -1;
	}
	return 0;
}

/*
 * unix_bind()
 * Returns a unix socket bound to the given path
 */
int
unix_bind(char *path, int flags)
{
	struct sockaddr_un s_un;
	int s, save_errno, addrlen;

	if (unix_setup_sockaddr(path, &s_un, &addrlen) == -1)
		return -1;

	/* Create unix domain socket. */
	if ((s = socket(AF_UNIX, flags | (uflag ? SOCK_DGRAM : SOCK_STREAM),
	    0)) == -1)
		return -1;

#ifdef __linux__
	if (path[0] != '@')
#endif
	unlink(path);

	if (bind(s, (struct sockaddr *)&s_un, addrlen) == -1) {
		save_errno = errno;
		close(s);
		errno = save_errno;
		return -1;
	}
	if (vflag)
		report_sock("Bound", NULL, 0, path);

	return s;
}

#ifdef HAVE_TLS
int
timeout_tls(int s, struct tls *tls_ctx, int (*func)(struct tls *))
{
	struct pollfd pfd;
	int ret;

	while ((ret = (*func)(tls_ctx)) != 0) {
		if (ret == TLS_WANT_POLLIN)
			pfd.events = POLLIN;
		else if (ret == TLS_WANT_POLLOUT)
			pfd.events = POLLOUT;
		else
			break;
		pfd.fd = s;
		if ((ret = poll(&pfd, 1, timeout)) == 1)
			continue;
		else if (ret == 0) {
			errno = ETIMEDOUT;
			ret = -1;
			break;
		} else
			err(1, "poll failed");
	}

	return ret;
}

void
tls_setup_client(struct tls *tls_ctx, int s, char *host)
{
	const char *errstr;

	if (tls_connect_socket(tls_ctx, s,
	    tls_expectname ? tls_expectname : host) == -1) {
		errx(1, "tls connection failed (%s)",
		    tls_error(tls_ctx));
	}
	if (timeout_tls(s, tls_ctx, tls_handshake) == -1) {
		if ((errstr = tls_error(tls_ctx)) == NULL)
			errstr = strerror(errno);
		errx(1, "tls handshake failed (%s)", errstr);
	}
	if (vflag)
		report_tls(tls_ctx, host);
	if (tls_expecthash && (tls_peer_cert_hash(tls_ctx) == NULL ||
	    strcmp(tls_expecthash, tls_peer_cert_hash(tls_ctx)) != 0))
		errx(1, "peer certificate is not %s", tls_expecthash);
	if (Zflag) {
		save_peer_cert(tls_ctx, Zflag);
		if (Zflag != stderr && (fclose(Zflag) != 0))
			err(1, "fclose failed saving peer cert");
	}
}

struct tls *
tls_setup_server(struct tls *tls_ctx, int connfd, char *host)
{
	struct tls *tls_cctx;
	const char *errstr;

	if (tls_accept_socket(tls_ctx, &tls_cctx, connfd) == -1) {
		warnx("tls accept failed (%s)", tls_error(tls_ctx));
	} else if (timeout_tls(connfd, tls_cctx, tls_handshake) == -1) {
		if ((errstr = tls_error(tls_cctx)) == NULL)
			errstr = strerror(errno);
		warnx("tls handshake failed (%s)", errstr);
	} else {
		int gotcert = tls_peer_cert_provided(tls_cctx);

		if (vflag && gotcert)
			report_tls(tls_cctx, host);
		if ((TLSopt & TLS_CCERT) && !gotcert)
			warnx("No client certificate provided");
		else if (gotcert && tls_expecthash &&
		    (tls_peer_cert_hash(tls_cctx) == NULL ||
		    strcmp(tls_expecthash, tls_peer_cert_hash(tls_cctx)) != 0))
			warnx("peer certificate is not %s", tls_expecthash);
		else if (gotcert && tls_expectname &&
		    (!tls_peer_cert_contains_name(tls_cctx, tls_expectname)))
			warnx("name (%s) not found in client cert",
			    tls_expectname);
		else {
			return tls_cctx;
		}
	}
	return NULL;
}
#endif

/*
 * unix_connect()
 * Returns a socket connected to a local unix socket. Returns -1 on failure.
 */
int
unix_connect(char *path)
{
	struct sockaddr_un s_un;
	int s, save_errno, addrlen;

	if (unix_setup_sockaddr(path, &s_un, &addrlen) == -1)
		return -1;

	if (uflag) {
		if ((s = unix_bind(unix_dg_tmp_socket, SOCK_CLOEXEC)) == -1)
			err(1, "%s", unix_dg_tmp_socket);
	} else {
		if ((s = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) == -1)
			return -1;
	}

	if (connect(s, (struct sockaddr *)&s_un, addrlen) == -1) {
		save_errno = errno;
		close(s);
		errno = save_errno;
		return -1;
	}
	return s;
}

/*
 * unix_listen()
 * Create a unix domain socket, and listen on it.
 */
int
unix_listen(char *path)
{
	int s;

	if ((s = unix_bind(path, 0)) == -1)
		return -1;
	if (listen(s, 5) == -1) {
		close(s);
		return -1;
	}
	if (vflag)
		report_sock("Listening", NULL, 0, path);

	return s;
}

char *proto_name(int uflag, int dccpflag) {
	char *proto = NULL;

	if (uflag)
		proto = "udp";
#if defined(IPPROTO_DCCP) && defined(SOCK_DCCP)
	else if (dccpflag)
		proto = "dccp";
#endif
	else
		proto = "tcp";

	return proto;
}

/*
 * remote_connect()
 * Returns a socket connected to a remote host. Properly binds to a local
 * port or source address if needed. Returns -1 on failure.
 */
int
remote_connect(const char *host, const char *port, struct addrinfo hints,
    char *ipaddr)
{
	struct addrinfo *res, *res0;
	int s = -1, error, herr, on = 1, save_errno;
        (void)on;

	if ((error = getaddrinfo(host, port, &hints, &res0)))
		errx(1, "getaddrinfo for host \"%s\" port %s: %s", host,
		    port, gai_strerror(error));

	for (res = res0; res; res = res->ai_next) {
		if ((s = socket(res->ai_family, res->ai_socktype |
		    SOCK_NONBLOCK, res->ai_protocol)) == -1)
			continue;

		/* Bind to a local port or source address if specified. */
		if (sflag || pflag) {
			struct addrinfo ahints, *ares;

#ifdef SO_BINDANY
			/* try SO_BINDANY, but don't insist */
			setsockopt(s, SOL_SOCKET, SO_BINDANY, &on, sizeof(on));
#endif
			memset(&ahints, 0, sizeof(struct addrinfo));
			ahints.ai_family = res->ai_family;
			if (uflag) {
				ahints.ai_socktype = SOCK_DGRAM;
				ahints.ai_protocol = IPPROTO_UDP;
			}
#if defined(IPPROTO_DCCP) && defined(SOCK_DCCP)
			else if (dccpflag) {
				hints.ai_socktype = SOCK_DCCP;
				hints.ai_protocol = IPPROTO_DCCP;
			}
#endif
			else {
				ahints.ai_socktype = SOCK_STREAM;
				ahints.ai_protocol = IPPROTO_TCP;
			}
			ahints.ai_flags = AI_PASSIVE;
			if ((error = getaddrinfo(sflag, pflag, &ahints, &ares)))
				errx(1, "getaddrinfo: %s", gai_strerror(error));

			if (bind(s, (struct sockaddr *)ares->ai_addr,
			    ares->ai_addrlen) == -1)
				err(1, "bind failed");
			freeaddrinfo(ares);
		}

		set_common_sockopts(s, res->ai_addr);

		if (ipaddr != NULL) {
			herr = getnameinfo(res->ai_addr, res->ai_addrlen,
			    ipaddr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			switch (herr) {
			case 0:
				break;
			case EAI_SYSTEM:
				err(1, "getnameinfo");
			default:
				errx(1, "getnameinfo: %s", gai_strerror(herr));
			}
		}

		if ((error = connect_with_timeout(s, res->ai_addr, res->ai_addrlen,
		    timeout)) == CONNECTION_SUCCESS)
			break;

		char *proto = proto_name(uflag, dccpflag);

		if (vflag) {
			/* only print IP if there is something to report */
			if (nflag || ipaddr == NULL ||
			    (strncmp(host, ipaddr, NI_MAXHOST) == 0))
				warn("connect to %s port %s (%s) %s", host,
				    port, proto,
				    error == CONNECTION_TIMEOUT ? "timed out" : "failed");
			else
				warn("connect to %s (%s) port %s (%s) %s",
				    host, ipaddr, port, proto,
				    error == CONNECTION_TIMEOUT ? "timed out" : "failed");
		}

		save_errno = errno;
		close(s);
		errno = save_errno;
		s = -1;
	}

	freeaddrinfo(res0);

	return s;
}

int
timeout_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
	struct pollfd pfd;
	socklen_t optlen;
	int optval;
	int ret;

	if ((ret = connect(s, name, namelen)) != 0 && errno == EINPROGRESS) {
		pfd.fd = s;
		pfd.events = POLLOUT;
		if ((ret = poll(&pfd, 1, timeout)) == 1) {
			optlen = sizeof(optval);
			if ((ret = getsockopt(s, SOL_SOCKET, SO_ERROR,
			    &optval, &optlen)) == 0) {
				errno = optval;
				ret = optval == 0 ? 0 : -1;
			}
		} else if (ret == 0) {
			errno = ETIMEDOUT;
			ret = -1;
		} else
			err(1, "poll failed");
	}

	return ret;
}

static int connect_with_timeout(int fd, const struct sockaddr *sa,
    socklen_t salen, int ctimeout)
{
	int err;
	struct timeval tv, *tvp = NULL;
	fd_set connect_fdset;
	socklen_t len;
	int orig_flags;

	orig_flags = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, orig_flags | O_NONBLOCK) < 0 ) {
		warn("can't set O_NONBLOCK - timeout not available");
		if (connect(fd, sa, salen) == 0)
			return CONNECTION_SUCCESS;
		else
			return CONNECTION_FAILED;
	}

	/* set connect timeout */
	if (ctimeout > 0) {
		tv.tv_sec = (time_t)ctimeout/1000;
		tv.tv_usec = 0;
		tvp = &tv;
	}

	/* attempt the connection */
	err = connect(fd, sa, salen);
	if (err != 0 && errno == EINPROGRESS) {
		/* connection is proceeding
		 * it is complete (or failed) when select returns */

		/* initialize connect_fdset */
		FD_ZERO(&connect_fdset);
		FD_SET(fd, &connect_fdset);

		/* call select */
		do {
			err = select(fd + 1, NULL, &connect_fdset, NULL, tvp);
		} while (err < 0 && errno == EINTR);

		/* select error */
		if (err < 0)
			errx(1,"select error: %s", strerror(errno));
		/* we have reached a timeout */
		if (err == 0)
			return CONNECTION_TIMEOUT;
		/* select returned successfully, but we must test socket
		 * error for result */
		len = sizeof(err);
		if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
			errx(1, "getsockopt error: %s", strerror(errno));
		/* setup errno according to the result returned by
		 * getsockopt */
		if (err != 0)
			errno = err;
	}

	/* return aborted if an error occured, and valid otherwise */
	fcntl(fd, F_SETFL, orig_flags);
	return (err != 0)? CONNECTION_FAILED : CONNECTION_SUCCESS;
}

/*
 * local_listen()
 * Returns a socket listening on a local port, binds to specified source
 * address. Returns -1 on failure.
 */
int
local_listen(const char *host, const char *port, struct addrinfo hints)
{
	struct addrinfo *res, *res0;
	int s = -1, ret, x = 1, save_errno;
	int error;

	/* Allow nodename to be null. */
	hints.ai_flags |= AI_PASSIVE;

	/*
	 * In the case of binding to a wildcard address
	 * default to binding to an ipv4 address.
	 */
	if (host == NULL && hints.ai_family == AF_UNSPEC)
		hints.ai_family = AF_INET;

	if ((error = getaddrinfo(host, port, &hints, &res0)))
		errx(1, "getaddrinfo: %s", gai_strerror(error));

	for (res = res0; res; res = res->ai_next) {
		if ((s = socket(res->ai_family, res->ai_socktype,
		    res->ai_protocol)) == -1)
			continue;

		ret = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x));
		if (ret == -1)
			warn("Couldn't set SO_REUSEADDR");

#ifdef SO_REUSEPORT
		ret = setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &x, sizeof(x));
		if (ret == -1 && !IsWindows())
			warn("Couldn't set SO_REUSEPORT");
#endif

		set_common_sockopts(s, res->ai_addr);

		if (bind(s, (struct sockaddr *)res->ai_addr,
		    res->ai_addrlen) == 0)
			break;

		save_errno = errno;
		close(s);
		errno = save_errno;
		s = -1;
	}

	if (!uflag && s != -1) {
		if (listen(s, 1) == -1)
			err(1, "listen");
	}
	if (vflag && s != -1) {
		struct sockaddr_storage ss;
		socklen_t len;

		len = sizeof(ss);
		if (getsockname(s, (struct sockaddr *)&ss, &len) == -1)
			err(1, "getsockname");
		report_sock(uflag ? "Bound" : "Listening",
		    (struct sockaddr *)&ss, len, NULL);
	}

	freeaddrinfo(res0);

	return s;
}

/*
 * readwrite()
 * Loop that polls on the network file descriptor and stdin.
 */
void
#ifdef HAVE_TLS
readwrite(int net_fd, struct tls *tls_ctx)
#else
readwrite(int net_fd)
#endif
{
	struct pollfd pfd[4];
	int stdin_fd = STDIN_FILENO;
	int stdout_fd = STDOUT_FILENO;
	unsigned char netinbuf[BUFSIZE];
	size_t netinbufpos = 0;
	unsigned char stdinbuf[BUFSIZE];
	size_t stdinbufpos = 0;
	int n, num_fds;
	ssize_t ret;

	/* don't read from stdin if requested */
	if (dflag)
		stdin_fd = -1;

	/* stdin */
	pfd[POLL_STDIN].fd = stdin_fd;
	pfd[POLL_STDIN].events = POLLIN;

	/* network out */
	pfd[POLL_NETOUT].fd = net_fd;
	pfd[POLL_NETOUT].events = 0;

	/* network in */
	pfd[POLL_NETIN].fd = net_fd;
	pfd[POLL_NETIN].events = POLLIN;

	/* stdout */
	pfd[POLL_STDOUT].fd = stdout_fd;
	pfd[POLL_STDOUT].events = 0;

	while (1) {
		/* both inputs are gone, buffers are empty, we are done */
		if (pfd[POLL_STDIN].fd == -1 && pfd[POLL_NETIN].fd == -1 &&
		    stdinbufpos == 0 && netinbufpos == 0) {
			if (qflag <= 0)
				return;
			goto delay_exit;
		}
		/* both outputs are gone, we can't continue */
		if (pfd[POLL_NETOUT].fd == -1 && pfd[POLL_STDOUT].fd == -1) {
			if (qflag <= 0)
				return;
			goto delay_exit;
		}
		/* listen and net in gone, queues empty, done */
		if (lflag && pfd[POLL_NETIN].fd == -1 &&
		    stdinbufpos == 0 && netinbufpos == 0) {
			if (qflag <= 0)
				return;
delay_exit:
			close(net_fd);
			signal(SIGALRM, quit);
			alarm(qflag);
		}

		/* poll */
		num_fds = poll(pfd, 4, timeout);

		/* treat poll errors */
		if (num_fds == -1)
			err(1, "polling error");

		/* timeout happened */
		if (num_fds == 0)
			return;

		/* treat socket error conditions */
		for (n = 0; n < 4; n++) {
			if (pfd[n].revents & (POLLERR|POLLNVAL)) {
				pfd[n].fd = -1;
			}
		}
		/* reading is possible after HUP */
		if (pfd[POLL_STDIN].events & POLLIN &&
		    pfd[POLL_STDIN].revents & POLLHUP &&
		    !(pfd[POLL_STDIN].revents & POLLIN))
			pfd[POLL_STDIN].fd = -1;

		if (pfd[POLL_NETIN].events & POLLIN &&
		    pfd[POLL_NETIN].revents & POLLHUP &&
		    !(pfd[POLL_NETIN].revents & POLLIN))
			pfd[POLL_NETIN].fd = -1;

		if (pfd[POLL_NETOUT].revents & POLLHUP) {
			if (pfd[POLL_NETOUT].fd != -1 && Nflag)
				shutdown(pfd[POLL_NETOUT].fd, SHUT_WR);
			pfd[POLL_NETOUT].fd = -1;
		}
		/* if HUP, stop watching stdout */
		if (pfd[POLL_STDOUT].revents & POLLHUP)
			pfd[POLL_STDOUT].fd = -1;
		/* if no net out, stop watching stdin */
		if (pfd[POLL_NETOUT].fd == -1)
			pfd[POLL_STDIN].fd = -1;
		/* if no stdout, stop watching net in */
		if (pfd[POLL_STDOUT].fd == -1) {
			if (pfd[POLL_NETIN].fd != -1)
				shutdown(pfd[POLL_NETIN].fd, SHUT_RD);
			pfd[POLL_NETIN].fd = -1;
		}

		/* try to read from stdin */
		if (pfd[POLL_STDIN].revents & POLLIN && stdinbufpos < BUFSIZE) {
			ret = fillbuf(pfd[POLL_STDIN].fd, stdinbuf,
#ifdef HAVE_TLS
			    &stdinbufpos, NULL);
			if (ret == TLS_WANT_POLLIN)
				pfd[POLL_STDIN].events = POLLIN;
			else if (ret == TLS_WANT_POLLOUT)
				pfd[POLL_STDIN].events = POLLOUT;
			else
#else
			    &stdinbufpos);
#endif
			if (ret == 0 || ret == -1)
				pfd[POLL_STDIN].fd = -1;
			/* read something - poll net out */
			if (stdinbufpos > 0)
				pfd[POLL_NETOUT].events = POLLOUT;
			/* filled buffer - remove self from polling */
			if (stdinbufpos == BUFSIZE)
				pfd[POLL_STDIN].events = 0;
		}
		/* try to write to network */
		if (pfd[POLL_NETOUT].revents & POLLOUT && stdinbufpos > 0) {
			ret = drainbuf(pfd[POLL_NETOUT].fd, stdinbuf,
#ifdef HAVE_TLS
			    &stdinbufpos, tls_ctx);
			if (ret == TLS_WANT_POLLIN)
				pfd[POLL_NETOUT].events = POLLIN;
			else if (ret == TLS_WANT_POLLOUT)
				pfd[POLL_NETOUT].events = POLLOUT;
			else
#else
			    &stdinbufpos, (iflag || Cflag) ? 1 : 0);
#endif
			if (ret == -1)
				pfd[POLL_NETOUT].fd = -1;
			/* buffer empty - remove self from polling */
			if (stdinbufpos == 0)
				pfd[POLL_NETOUT].events = 0;
			/* buffer no longer full - poll stdin again */
			if (stdinbufpos < BUFSIZE)
				pfd[POLL_STDIN].events = POLLIN;
		}
		/* try to read from network */
		if (pfd[POLL_NETIN].revents & POLLIN && netinbufpos < BUFSIZE) {
			ret = fillbuf(pfd[POLL_NETIN].fd, netinbuf,
#ifdef HAVE_TLS
			    &netinbufpos, tls_ctx);
			if (ret == TLS_WANT_POLLIN)
				pfd[POLL_NETIN].events = POLLIN;
			else if (ret == TLS_WANT_POLLOUT)
				pfd[POLL_NETIN].events = POLLOUT;
			else
#else
			    &netinbufpos);
#endif
			if (ret == -1)
				pfd[POLL_NETIN].fd = -1;
			/* eof on net in - remove from pfd */
			if (ret == 0) {
				shutdown(pfd[POLL_NETIN].fd, SHUT_RD);
				pfd[POLL_NETIN].fd = -1;
			}
			if (recvlimit > 0 && ++recvcount >= recvlimit) {
				if (pfd[POLL_NETIN].fd != -1)
					shutdown(pfd[POLL_NETIN].fd, SHUT_RD);
				pfd[POLL_NETIN].fd = -1;
				pfd[POLL_STDIN].fd = -1;
			}
			/* read something - poll stdout */
			if (netinbufpos > 0)
				pfd[POLL_STDOUT].events = POLLOUT;
			/* filled buffer - remove self from polling */
			if (netinbufpos == BUFSIZE)
				pfd[POLL_NETIN].events = 0;
			/* handle telnet */
			if (pfd[POLL_NETIN].fd != -1 && tflag)
				atelnet(pfd[POLL_NETIN].fd, netinbuf,
				    netinbufpos);
		}
		/* try to write to stdout */
		if (pfd[POLL_STDOUT].revents & POLLOUT && netinbufpos > 0) {
			ret = drainbuf(pfd[POLL_STDOUT].fd, netinbuf,
#ifdef HAVE_TLS
			    &netinbufpos, NULL);
			if (ret == TLS_WANT_POLLIN)
				pfd[POLL_STDOUT].events = POLLIN;
			else if (ret == TLS_WANT_POLLOUT)
				pfd[POLL_STDOUT].events = POLLOUT;
			else
#else
			    &netinbufpos, 0);
#endif
			if (ret == -1)
				pfd[POLL_STDOUT].fd = -1;
			/* buffer empty - remove self from polling */
			if (netinbufpos == 0)
				pfd[POLL_STDOUT].events = 0;
			/* buffer no longer full - poll net in again */
			if (netinbufpos < BUFSIZE)
				pfd[POLL_NETIN].events = POLLIN;
		}

		/* stdin gone and queue empty? */
		if (pfd[POLL_STDIN].fd == -1 && stdinbufpos == 0) {
			if (pfd[POLL_NETOUT].fd != -1 && Nflag)
				shutdown(pfd[POLL_NETOUT].fd, SHUT_WR);
			pfd[POLL_NETOUT].fd = -1;
			/* #817050: handle UDP sockets and kflag */
			if ((lflag || uflag) && pfd[POLL_NETIN].fd != -1 &&
			    qflag >= 0 && netinbufpos == 0) {
				shutdown(pfd[POLL_NETIN].fd, SHUT_RD);
				pfd[POLL_NETIN].fd = -1;
				kflag = 0;
			}
		}
		/* net in gone and queue empty? */
		if (pfd[POLL_NETIN].fd == -1 && netinbufpos == 0) {
			pfd[POLL_STDOUT].fd = -1;
		}
	}
}

ssize_t
drainbuf(int fd, unsigned char *buf, size_t *bufpos, int oneline)
{
	ssize_t n, r;
	ssize_t adjust;
	unsigned char *lf = NULL;

	if (fd == -1)
		return -1;

	if (oneline)
		lf = memchr(buf, '\n', *bufpos);
	if (lf == NULL) {
		n = *bufpos;
		oneline = 0;
	}
	else if (Cflag && (lf == buf || buf[lf - buf - 1] != '\r')) {
		n = lf - buf;
		oneline = 2;
	}
	else
		n = lf - buf + 1;
	if (n > 0)
		n = write(fd, buf, n);

	/* don't treat EAGAIN, EINTR as error */
	if (n == -1 && (errno == EAGAIN || errno == EINTR))
		n = -2;
	if (oneline == 2 && n >= 0)
		n++;
	if (n <= 0)
		return n;

	if (oneline == 2 && (r = atomicio(vwrite, fd, "\r\n", 2)) != 2)
		err(1, "write failed (%zu/2)", r);
	if (oneline > 0 && iflag)
		sleep(iflag);

	/* adjust buffer */
	adjust = *bufpos - n;
	if (adjust > 0)
		memmove(buf, buf + n, adjust);
	*bufpos -= n;
	return n;
}

ssize_t
#ifdef HAVE_TLS
fillbuf(int fd, unsigned char *buf, size_t *bufpos, struct tls *tls)
#else
fillbuf(int fd, unsigned char *buf, size_t *bufpos)
#endif
{
	size_t num = BUFSIZE - *bufpos;
	ssize_t n;

	if (fd == -1)
		return -1;

#ifdef HAVE_TLS
	if (tls) {
		n = tls_read(tls, buf + *bufpos, num);
		if (n == -1)
			errx(1, "tls read failed (%s)", tls_error(tls));
	} else {
#endif
		n = read(fd, buf + *bufpos, num);
		/* don't treat EAGAIN, EINTR as error */
		if (n == -1 && (errno == EAGAIN || errno == EINTR))
#ifdef HAVE_TLS
			n = TLS_WANT_POLLIN;
	}
#else
			n = -2;
#endif
	if (n <= 0)
		return n;
	*bufpos += n;
	return n;
}

/*
 * fdpass()
 * Pass the connected file descriptor to stdout and exit.
 */
void
fdpass(int nfd)
{
	struct msghdr mh;
	union {
		struct cmsghdr hdr;
		char buf[CMSG_SPACE(sizeof(int))];
	} cmsgbuf;
	struct cmsghdr *cmsg;
	struct iovec iov;
	char c = '\0';
	ssize_t r;
	struct pollfd pfd;

	/* Avoid obvious stupidity */
	if (isatty(STDOUT_FILENO))
		errx(1, "Cannot pass file descriptor to tty");

	memset(&mh, 0, sizeof(mh));
	memset(&cmsgbuf, 0, sizeof(cmsgbuf));
	memset(&iov, 0, sizeof(iov));

	mh.msg_control = (caddr_t)&cmsgbuf.buf;
	mh.msg_controllen = sizeof(cmsgbuf.buf);
	cmsg = CMSG_FIRSTHDR(&mh);
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int *)CMSG_DATA(cmsg) = nfd;

	iov.iov_base = &c;
	iov.iov_len = 1;
	mh.msg_iov = &iov;
	mh.msg_iovlen = 1;

	memset(&pfd, 0, sizeof(pfd));
	pfd.fd = STDOUT_FILENO;
	pfd.events = POLLOUT;
	for (;;) {
		r = sendmsg(STDOUT_FILENO, &mh, 0);
		if (r == -1) {
			if (errno == EAGAIN || errno == EINTR) {
				if (poll(&pfd, 1, -1) == -1)
					err(1, "poll");
				continue;
			}
			err(1, "sendmsg");
		} else if (r != 1)
			errx(1, "sendmsg: unexpected return value %zd", r);
		else
			break;
	}
	exit(0);
}

/* Deal with RFC 854 WILL/WONT DO/DONT negotiation. */
void
atelnet(int nfd, unsigned char *buf, unsigned int size)
{
	unsigned char *p, *end;
	unsigned char obuf[4];

	if (size < 3)
		return;
	end = buf + size - 2;

	for (p = buf; p < end; p++) {
		if (*p != IAC)
			continue;

		obuf[0] = IAC;
		p++;
		if ((*p == WILL) || (*p == WONT))
			obuf[1] = DONT;
		else if ((*p == DO) || (*p == DONT))
			obuf[1] = WONT;
		else
			continue;

		p++;
		obuf[2] = *p;
		if (atomicio(vwrite, nfd, obuf, 3) != 3)
			warn("Write Error!");
	}
}

int
strtoport(char *portstr, int udp)
{
	struct servent *entry;
	const char *errstr;
	char *proto;
	int port = -1;

	proto = udp ? "udp" : "tcp";

	port = strtonum(portstr, 1, PORT_MAX, &errstr);
	if (errstr == NULL)
		return port;
	if (errno != EINVAL)
		errx(1, "port number %s: %s", errstr, portstr);
	if ((entry = getservbyname(portstr, proto)) == NULL)
		errx(1, "service \"%s\" unknown", portstr);
	return ntohs(entry->s_port);
}

/*
 * build_ports()
 * Build an array of ports in portlist[], listing each port
 * that we should try to connect to.
 */
void
build_ports(char **p)
{
	struct servent *sv;
	char *n;
	int hi, lo, cp;
	int x = 0;
	int i;

	char *proto = proto_name(uflag, dccpflag);
	for (i = 0; p[i] != NULL; i++) {
		sv = getservbyname(p[i], proto);
		if (sv) {
			if (asprintf(&portlist[x], "%d", ntohs(sv->s_port)) < 0)
				err(1, "asprintf");
			x++;
		} else if (isdigit((unsigned char)*p[i]) && (n = strchr(p[i], '-')) != NULL) {
			*n = '\0';
			n++;

			/* Make sure the ports are in order: lowest->highest. */
			hi = strtoport(n, uflag);
			lo = strtoport(p[i], uflag);
			if (lo > hi) {
				cp = hi;
				hi = lo;
				lo = cp;
			}

			/* Load ports sequentially. */
			for (cp = lo; cp <= hi; cp++) {
				if (asprintf(&portlist[x], "%d", cp) == -1)
					err(1, "asprintf");
				x++;
			}
		} else {
			hi = strtoport(p[i], uflag);
			if (asprintf(&portlist[x], "%d", hi) < 0)
				err(1, "asprintf");
			x++;
		}
	}

	/*
	 * Initialize portlist with a random permutation using
	 * Fisher–Yates shuffle.
	 */
	if (rflag) {
		for (i = x-1; i > 0; i--) {
			cp = arc4random_uniform(i+1);
			if (cp != i) {
				n = portlist[i];
				portlist[i] = portlist[cp];
				portlist[cp] = n;
			}
		}
	}
}

/*
 * udptest()
 * Do a few writes to see if the UDP port is there.
 * Fails once PF state table is full.
 */
int
udptest(int s)
{
	int i, t;

	/* Only write to the socket in scan mode or interactive mode. */
	if (!zflag && !isatty(STDIN_FILENO))
		return 0;

	if ((write(s, "X", 1) != 1) ||
	    ((write(s, "X", 1) != 1) && (errno == ECONNREFUSED)))
		return -1;

	/* Give the remote host some time to reply. */
	for (i = 0, t = (timeout == -1) ? UDP_SCAN_TIMEOUT : (timeout / 1000);
	     i < t; i++) {
		sleep(1);
		if ((write(s, "X", 1) != 1) && (errno == ECONNREFUSED))
			return -1;
	}
	return 1;
}

void
connection_info(const char *host, const char *port, const char *proto,
    const char *ipaddr)
{
	struct servent *sv;
	char *service = "*";

	/* Look up service name unless -n. */
	if (!nflag) {
		sv = getservbyport(ntohs(atoi(port)), proto);
		if (sv != NULL)
			service = sv->s_name;
	}

	fprintf(stderr, "Connection to %s", host);

	/*
	 * if we aren't connecting thru a proxy and
	 * there is something to report, print IP
	 */
	if (!nflag && !xflag && strcmp(host, ipaddr) != 0)
		fprintf(stderr, " (%s)", ipaddr);

	fprintf(stderr, " %s port [%s/%s] succeeded!\n", port, proto, service);
}

void
set_common_sockopts(int s, const struct sockaddr* sa)
{
	int x = 1;
	int af = sa->sa_family;

#ifdef SO_BROADCAST
	if (bflag) {
		/* allow datagram sockets to send packets to a broadcast address
		 * (this option has no effect on stream-oriented sockets) */
		if (setsockopt(s, SOL_SOCKET, SO_BROADCAST,
			&x, sizeof(x)) == -1)
			err(1, NULL);
	}
#endif
#if defined(TCP_MD5SIG_EXT) && defined(TCP_MD5SIG_MAXKEYLEN)
	if (Sflag) {
		struct tcp_md5sig sig;
		memset(&sig, 0, sizeof(sig));
		memcpy(&sig.tcpm_addr, sa, sizeof(struct sockaddr_storage));
		sig.tcpm_keylen = TCP_MD5SIG_MAXKEYLEN < strlen(Sflag_password)
		    ? TCP_MD5SIG_MAXKEYLEN
		    : strlen(Sflag_password);
		memcpy(sig.tcpm_key, Sflag_password, sig.tcpm_keylen);
		sig.tcpm_flags = TCP_MD5SIG_FLAG_PREFIX;
		if (setsockopt(s, IPPROTO_TCP, TCP_MD5SIG_EXT,
		    &sig, sizeof(sig)) == -1)
			err(1, NULL);
	}
#endif
	if (Dflag) {
		if (setsockopt(s, SOL_SOCKET, SO_DEBUG,
		    &x, sizeof(x)) == -1)
			err(1, NULL);
	}
	if (Tflag != -1) {
		if (af == AF_INET && setsockopt(s, IPPROTO_IP,
		    IP_TOS, &Tflag, sizeof(Tflag)) == -1)
			err(1, "set IP ToS");

#ifdef IPV6_TCLASS
		else if (af == AF_INET6 && setsockopt(s, IPPROTO_IPV6,
		    IPV6_TCLASS, &Tflag, sizeof(Tflag)) == -1)
			err(1, "set IPv6 traffic class");
#else
		else if (af == AF_INET6)
			errx(1, "can't set IPv6 traffic class (unavailable)");
#endif
	}
	if (Iflag) {
		if (setsockopt(s, SOL_SOCKET, SO_RCVBUF,
		    &Iflag, sizeof(Iflag)) == -1)
			err(1, "set TCP receive buffer size");
	}
	if (Oflag) {
		if (setsockopt(s, SOL_SOCKET, SO_SNDBUF,
		    &Oflag, sizeof(Oflag)) == -1)
			err(1, "set TCP send buffer size");
	}

	if (ttl != -1) {
		if (af == AF_INET && setsockopt(s, IPPROTO_IP,
		    IP_TTL, &ttl, sizeof(ttl)))
			err(1, "set IP TTL");

#ifdef IPV6_UNICAST_HOPS
		else if (af == AF_INET6 && setsockopt(s, IPPROTO_IPV6,
		    IPV6_UNICAST_HOPS, &ttl, sizeof(ttl)))
			err(1, "set IPv6 unicast hops");
#else
		else if (af == AF_INET6)
			errx(1, "can't set IPv6 unicast hops (unavailable)");
#endif
	}

	if (minttl != -1) {
#ifdef IP_MINTTL
		if (af == AF_INET && setsockopt(s, IPPROTO_IP,
		    IP_MINTTL, &minttl, sizeof(minttl)))
			err(1, "set IP min TTL");
#else
		if (af == AF_INET)
			errx(1, "can't set IP min TTL (unavailable)");
#endif

#ifdef IPV6_MINHOPCOUNT
		else if (af == AF_INET6 && setsockopt(s, IPPROTO_IPV6,
		    IPV6_MINHOPCOUNT, &minttl, sizeof(minttl)))
			err(1, "set IPv6 min hop count");
#else
		else if (af == AF_INET6)
			errx(1, "can't set IPv6 min hop count (unavailable)");
#endif
	}
}

int
process_tos_opt(char *s, int *val)
{
	/* DiffServ Codepoints and other TOS mappings */
	const struct toskeywords {
		const char	*keyword;
		int		 val;
	} *t, toskeywords[] = {
		{ "af11",		IPTOS_DSCP_AF11 },
		{ "af12",		IPTOS_DSCP_AF12 },
		{ "af13",		IPTOS_DSCP_AF13 },
		{ "af21",		IPTOS_DSCP_AF21 },
		{ "af22",		IPTOS_DSCP_AF22 },
		{ "af23",		IPTOS_DSCP_AF23 },
		{ "af31",		IPTOS_DSCP_AF31 },
		{ "af32",		IPTOS_DSCP_AF32 },
		{ "af33",		IPTOS_DSCP_AF33 },
		{ "af41",		IPTOS_DSCP_AF41 },
		{ "af42",		IPTOS_DSCP_AF42 },
		{ "af43",		IPTOS_DSCP_AF43 },
		{ "critical",		IPTOS_PREC_CRITIC_ECP },
		{ "cs0",		IPTOS_DSCP_CS0 },
		{ "cs1",		IPTOS_DSCP_CS1 },
		{ "cs2",		IPTOS_DSCP_CS2 },
		{ "cs3",		IPTOS_DSCP_CS3 },
		{ "cs4",		IPTOS_DSCP_CS4 },
		{ "cs5",		IPTOS_DSCP_CS5 },
		{ "cs6",		IPTOS_DSCP_CS6 },
		{ "cs7",		IPTOS_DSCP_CS7 },
		{ "ef",			IPTOS_DSCP_EF },
		{ "inetcontrol",	IPTOS_PREC_INTERNETCONTROL },
		{ "lowcost",		IPTOS_LOWCOST },
		{ "lowdelay",		IPTOS_LOWDELAY },
		{ "netcontrol",		IPTOS_PREC_NETCONTROL },
		{ "reliability",	IPTOS_RELIABILITY },
		{ "throughput",		IPTOS_THROUGHPUT },
		{ NULL,			-1 },
	};

	for (t = toskeywords; t->keyword != NULL; t++) {
		if (strcmp(s, t->keyword) == 0) {
			*val = t->val;
			return 1;
		}
	}

	return 0;
}

#ifdef HAVE_TLS
int
process_tls_opt(char *s, int *flags)
{
	size_t len;
	char *v;

	const struct tlskeywords {
		const char	*keyword;
		int		 flag;
		char		**value;
	} *t, tlskeywords[] = {
		{ "ciphers",		-1,			&tls_ciphers },
		{ "clientcert",		TLS_CCERT,		NULL },
		{ "muststaple",		TLS_MUSTSTAPLE,		NULL },
		{ "noverify",		TLS_NOVERIFY,		NULL },
		{ "noname",		TLS_NONAME,		NULL },
		{ "protocols",		-1,			&tls_protocols },
		{ NULL,			-1,			NULL },
	};

	len = strlen(s);
	if ((v = strchr(s, '=')) != NULL) {
		len = v - s;
		v++;
	}

	for (t = tlskeywords; t->keyword != NULL; t++) {
		if (strlen(t->keyword) == len &&
		    strncmp(s, t->keyword, len) == 0) {
			if (t->value != NULL) {
				if (v == NULL)
					errx(1, "invalid tls value `%s'", s);
				*t->value = v;
			} else {
				*flags |= t->flag;
			}
			return 1;
		}
	}
	return 0;
}

void
save_peer_cert(struct tls *tls_ctx, FILE *fp)
{
	const char *pem;
	size_t plen;

	if ((pem = tls_peer_cert_chain_pem(tls_ctx, &plen)) == NULL)
		errx(1, "Can't get peer certificate");
	if (fprintf(fp, "%.*s", (int)plen, pem) < 0)
		err(1, "unable to save peer cert");
	if (fflush(fp) != 0)
		err(1, "unable to flush peer cert");
}

void
report_tls(struct tls *tls_ctx, char *host)
{
	time_t t;
	const char *ocsp_url;

	fprintf(stderr, "TLS handshake negotiated %s/%s with host %s\n",
	    tls_conn_version(tls_ctx), tls_conn_cipher(tls_ctx), host);
	fprintf(stderr, "Peer name: %s\n",
	    tls_expectname ? tls_expectname : host);
	if (tls_peer_cert_subject(tls_ctx))
		fprintf(stderr, "Subject: %s\n",
		    tls_peer_cert_subject(tls_ctx));
	if (tls_peer_cert_issuer(tls_ctx))
		fprintf(stderr, "Issuer: %s\n",
		    tls_peer_cert_issuer(tls_ctx));
	if ((t = tls_peer_cert_notbefore(tls_ctx)) != -1)
		fprintf(stderr, "Valid From: %s", ctime(&t));
	if ((t = tls_peer_cert_notafter(tls_ctx)) != -1)
		fprintf(stderr, "Valid Until: %s", ctime(&t));
	if (tls_peer_cert_hash(tls_ctx))
		fprintf(stderr, "Cert Hash: %s\n",
		    tls_peer_cert_hash(tls_ctx));
	ocsp_url = tls_peer_ocsp_url(tls_ctx);
	if (ocsp_url != NULL)
		fprintf(stderr, "OCSP URL: %s\n", ocsp_url);
	switch (tls_peer_ocsp_response_status(tls_ctx)) {
	case TLS_OCSP_RESPONSE_SUCCESSFUL:
		fprintf(stderr, "OCSP Stapling: %s\n",
		    tls_peer_ocsp_result(tls_ctx) == NULL ? "" :
		    tls_peer_ocsp_result(tls_ctx));
		fprintf(stderr,
		    "  response_status=%d cert_status=%d crl_reason=%d\n",
		    tls_peer_ocsp_response_status(tls_ctx),
		    tls_peer_ocsp_cert_status(tls_ctx),
		    tls_peer_ocsp_crl_reason(tls_ctx));
		t = tls_peer_ocsp_this_update(tls_ctx);
		fprintf(stderr, "  this update: %s",
		    t != -1 ? ctime(&t) : "\n");
		t = tls_peer_ocsp_next_update(tls_ctx);
		fprintf(stderr, "  next update: %s",
		    t != -1 ? ctime(&t) : "\n");
		t = tls_peer_ocsp_revocation_time(tls_ctx);
		fprintf(stderr, "  revocation: %s",
		    t != -1 ? ctime(&t) : "\n");
		break;
	case -1:
		break;
	default:
		fprintf(stderr,
		    "OCSP Stapling:  failure - response_status %d (%s)\n",
		    tls_peer_ocsp_response_status(tls_ctx),
		    tls_peer_ocsp_result(tls_ctx) == NULL ? "" :
		    tls_peer_ocsp_result(tls_ctx));
		break;
	}
}
#endif

void
report_sock(const char *msg, const struct sockaddr *sa, socklen_t salen,
    char *path)
{
	char host[NI_MAXHOST], port[NI_MAXSERV];
	int herr;
	int flags = NI_NUMERICSERV;

	if (path != NULL) {
		fprintf(stderr, "%s on %s\n", msg, path);
		return;
	}

	if (nflag)
		flags |= NI_NUMERICHOST;

	herr = getnameinfo(sa, salen, host, sizeof(host), port, sizeof(port),
	    flags);
	switch (herr) {
	case 0:
		break;
	case EAI_SYSTEM:
		warn("getnameinfo");
		return;
	default:
		warnx("getnameinfo: %s", gai_strerror(herr));
		return;
	}

	fprintf(stderr, "%s on %s %s\n", msg, host, port);
}

void
help(void)
{
#ifdef DEBIAN_VERSION
	fprintf(stderr, "OpenBSD netcat (Debian patchlevel " DEBIAN_VERSION ")\n");
#endif
	usage(0);
	fprintf(stderr, "\tCommand Summary:\n\
	\t-4		Use IPv4\n\
	\t-6		Use IPv6\n\
	\t-b		Allow broadcast\n\
	\t-C		Send CRLF as line-ending\n\
	\t-D		Enable the debug socket option\n\
	\t-d		Detach from stdin\n\
	\t-F		Pass socket fd\n\
	\t-h		This help text\n\
	\t-I length	TCP receive buffer length\n\
	\t-i interval	Delay interval for lines sent, ports scanned\n\
	\t-k		Keep inbound sockets open for multiple connects\n\
	\t-l		Listen mode, for inbound connects\n\
	\t-M ttl		Outgoing TTL / Hop Limit\n\
	\t-m minttl	Minimum incoming TTL / Hop Limit\n\
	\t-N		Shutdown the network socket after EOF on stdin\n\
	\t-n		Suppress name/port resolutions\n\
	\t-O length	TCP send buffer length\n\
	\t-P proxyuser\tUsername for proxy authentication\n\
	\t-p port\t	Specify local port for remote connects\n\
	\t-q secs\t	quit after EOF on stdin and delay of secs\n\
	\t-r		Randomize remote ports\n\
	\t-S		Enable the TCP MD5 signature option\n\
	\t-s sourceaddr	Local source address\n\
	\t-T keyword	TOS value\n\
	\t-t		Answer TELNET negotiation\n\
	\t-U		Use UNIX domain socket\n\
	\t-u		UDP mode\n\
	\t-V rtable	Specify alternate routing table\n\
	\t-v		Verbose\n\
	\t-W recvlimit	Terminate after receiving a number of packets\n\
	\t-w timeout	Timeout for connects and final net reads\n\
	\t-X proto	Proxy protocol: \"4\", \"5\" (SOCKS) or \"connect\"\n\
	\t-x addr[:port]\tSpecify proxy address and port\n\
	\t-Z		DCCP mode\n\
	\t-z		Zero-I/O mode [used for scanning]\n\
	Port numbers can be individual or ranges: lo-hi [inclusive]\n");
	exit(0);
}

void
usage(int ret)
{
	fprintf(stderr,
	    "usage: nc [-46CDdFhklNnrStUuvZz] [-I length] [-i interval] [-M ttl]\n"
	    "\t  [-m minttl] [-O length] [-P proxy_username] [-p source_port]\n"
	    "\t  [-q seconds] [-s sourceaddr] [-T keyword] [-V rtable] [-W recvlimit]\n"
	    "\t  [-w timeout] [-X proxy_protocol] [-x proxy_address[:port]]\n"
	    "\t  [destination] [port]\n");
	if (ret)
		exit(1);
}

/*
 * quit()
 * handler for a "-q" timeout (exit 0 instead of 1)
 */
static void quit(int sig)
{
	exit(0);
}
