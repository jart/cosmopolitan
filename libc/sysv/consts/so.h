#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_

#define SO_DEBUG 1

COSMOPOLITAN_C_START_

extern const int SO_TYPE;
extern const int SO_ERROR;
extern const int SO_ACCEPTCONN;
extern const int SO_REUSEADDR;
extern const int SO_KEEPALIVE;
extern const int SO_DONTROUTE;
extern const int SO_BROADCAST;
extern const int SO_USELOOPBACK;
extern const int SO_LINGER;
extern const int SO_OOBINLINE;
extern const int SO_SNDBUF;
extern const int SO_RCVBUF;
extern const int SO_RCVTIMEO;
extern const int SO_SNDTIMEO;
extern const int SO_RCVLOWAT;
extern const int SO_SNDLOWAT;

#define SO_TYPE        SO_TYPE
#define SO_ERROR       SO_ERROR
#define SO_ACCEPTCONN  SO_ACCEPTCONN
#define SO_REUSEADDR   SO_REUSEADDR
#define SO_KEEPALIVE   SO_KEEPALIVE
#define SO_DONTROUTE   SO_DONTROUTE
#define SO_BROADCAST   SO_BROADCAST
#define SO_USELOOPBACK SO_USELOOPBACK
#define SO_LINGER      SO_LINGER
#define SO_OOBINLINE   SO_OOBINLINE
#define SO_SNDBUF      SO_SNDBUF
#define SO_RCVBUF      SO_RCVBUF
#define SO_RCVTIMEO    SO_RCVTIMEO
#define SO_SNDTIMEO    SO_SNDTIMEO
#define SO_RCVLOWAT    SO_RCVLOWAT
#define SO_SNDLOWAT    SO_SNDLOWAT

/*
 * this isn't available on windows, but it should be fine to use anyway,
 * setsockopt will return ENOPROTOOPT which is perfectly fine to ignore.
 */
extern const int SO_REUSEPORT;
#define SO_REUSEPORT SO_REUSEPORT

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SO_H_ */
