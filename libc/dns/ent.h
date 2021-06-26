#ifndef COSMOPOLITAN_LIBC_DNS_ENT_H_
#define COSMOPOLITAN_LIBC_DNS_ENT_H_
#include "libc/dns/dns.h"

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern int h_errno;

struct hostent {
  char *h_name;       /* official name of host */
  char **h_aliases;   /* alias list */
  int h_addrtype;     /* host address type */
  int h_length;       /* length of address */
  char **h_addr_list; /* list of addresses */
};
#define h_addr h_addr_list[0]

struct hostent *gethostent(void);
struct hostent *gethostbyname(const char *);
struct hostent *gethostbyaddr(const void *, socklen_t, int);
void sethostent(int);
void endhostent(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_ENT_H_ */
