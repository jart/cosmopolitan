#ifndef COSMOPOLITAN_LIBC_DNS_ENT_H_
#define COSMOPOLITAN_LIBC_DNS_ENT_H_
#include "libc/dns/dns.h"

#define HOST_NOT_FOUND 1
#define TRY_AGAIN      2
#define NO_RECOVERY    3
#define NO_DATA        4

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct netent {
  char *n_name;     /* official network name */
  char **n_aliases; /* alias list */
  int n_addrtype;   /* net address type */
  uint32_t n_net;   /* network number */
};

struct protoent {
  char *p_name;     /* official protocol name */
  char **p_aliases; /* alias list */
  int p_proto;      /* protocol number */
};

struct hostent {
  char *h_name;       /* official name of host */
  char **h_aliases;   /* alias list */
  int h_addrtype;     /* host address type */
  int h_length;       /* length of address */
  char **h_addr_list; /* list of addresses */
};
#define h_addr h_addr_list[0]

struct servent {
  char *s_name;     /* official service name */
  char **s_aliases; /* alias list */
  int s_port;       /* port number (in network byte order) */
  char *s_proto;    /* protocol to use */
};

extern int h_errno;
void herror(const char *);
const char *hstrerror(int);

struct netent *getnetent(void);
struct netent *getnetbyname(const char *);
struct netent *getnetbyaddr(uint32_t, int);
void setnetent(int);
void endnetent(void);

struct protoent *getprotoent(void);
struct protoent *getprotobyname(const char *);
struct protoent *getprotobynumber(int);
void setprotoent(int);
void endprotoent(void);

struct hostent *gethostent(void);
struct hostent *gethostbyname(const char *);
struct hostent *gethostbyaddr(const void *, socklen_t, int);
void sethostent(int);
void endhostent(void);

struct servent *getservent(void);
struct servent *getservbyname(const char *, const char *);
struct servent *getservbyport(int, const char *);
void setservent(int);
void endservent(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_ENT_H_ */
