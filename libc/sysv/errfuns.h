#ifndef COSMOPOLITAN_LIBC_SYSV_ERRFUNS_H_
#define COSMOPOLITAN_LIBC_SYSV_ERRFUNS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifdef __x86_64__

/**
 * @fileoverview Optimized error return paths.
 *
 * Saying this:
 *
 *     return einval();
 *
 * Instead of this:
 *
 *     errno = EINVAL;
 *     return -1;
 *
 * Allows the compiler to generate 11 fewer bytes of code each time.
 *
 * @return always -1
 * @see libc/sysv/errfuns.inc (for implementation)
 */

intptr_t einval(void) relegated;
intptr_t eperm(void) relegated;
intptr_t enoent(void) relegated;
intptr_t esrch(void) relegated;
intptr_t eintr(void) relegated;
intptr_t eio(void) relegated;
intptr_t enxio(void) relegated;
intptr_t e2big(void) relegated;
intptr_t enoexec(void) relegated;
intptr_t ebadf(void) relegated;
intptr_t echild(void) relegated;
intptr_t eagain(void) relegated;
intptr_t enomem(void) relegated;
intptr_t eacces(void) relegated;
intptr_t efault(void) relegated;
intptr_t enotblk(void) relegated;
intptr_t ebusy(void) relegated;
intptr_t eexist(void) relegated;
intptr_t exdev(void) relegated;
intptr_t enodev(void) relegated;
intptr_t enotdir(void) relegated;
intptr_t eisdir(void) relegated;
intptr_t enfile(void) relegated;
intptr_t emfile(void) relegated;
intptr_t enotty(void) relegated;
intptr_t enotsup(void) relegated;
intptr_t etxtbsy(void) relegated;
intptr_t efbig(void) relegated;
intptr_t enospc(void) relegated;
intptr_t espipe(void) relegated;
intptr_t erofs(void) relegated;
intptr_t emlink(void) relegated;
intptr_t epipe(void) relegated;
intptr_t edom(void) relegated;
intptr_t erange(void) relegated;
intptr_t edeadlk(void) relegated;
intptr_t enametoolong(void) relegated;
intptr_t enolck(void) relegated;
intptr_t enosys(void) relegated;
intptr_t enotempty(void) relegated;
intptr_t eloop(void) relegated;
intptr_t enomsg(void) relegated;
intptr_t eidrm(void) relegated;
intptr_t echrng(void) relegated;
intptr_t el2nsync(void) relegated;
intptr_t el3hlt(void) relegated;
intptr_t el3rst(void) relegated;
intptr_t elnrng(void) relegated;
intptr_t eunatch(void) relegated;
intptr_t enocsi(void) relegated;
intptr_t el2hlt(void) relegated;
intptr_t ebade(void) relegated;
intptr_t ebadr(void) relegated;
intptr_t exfull(void) relegated;
intptr_t enoano(void) relegated;
intptr_t ebadrqc(void) relegated;
intptr_t ebadslt(void) relegated;
intptr_t enostr(void) relegated;
intptr_t enodata(void) relegated;
intptr_t etime(void) relegated;
intptr_t enosr(void) relegated;
intptr_t enonet(void) relegated;
intptr_t enopkg(void) relegated;
intptr_t eremote(void) relegated;
intptr_t enolink(void) relegated;
intptr_t eadv(void) relegated;
intptr_t esrmnt(void) relegated;
intptr_t ecomm(void) relegated;
intptr_t eproto(void) relegated;
intptr_t emultihop(void) relegated;
intptr_t edotdot(void) relegated;
intptr_t ebadmsg(void) relegated;
intptr_t eoverflow(void) relegated;
intptr_t enotuniq(void) relegated;
intptr_t ebadfd(void) relegated;
intptr_t eremchg(void) relegated;
intptr_t elibacc(void) relegated;
intptr_t elibbad(void) relegated;
intptr_t elibscn(void) relegated;
intptr_t elibmax(void) relegated;
intptr_t elibexec(void) relegated;
intptr_t eilseq(void) relegated;
intptr_t erestart(void) relegated;
intptr_t estrpipe(void) relegated;
intptr_t eusers(void) relegated;
intptr_t enotsock(void) relegated;
intptr_t edestaddrreq(void) relegated;
intptr_t emsgsize(void) relegated;
intptr_t eprototype(void) relegated;
intptr_t enoprotoopt(void) relegated;
intptr_t eprotonosupport(void) relegated;
intptr_t esocktnosupport(void) relegated;
intptr_t eopnotsupp(void) relegated;
intptr_t epfnosupport(void) relegated;
intptr_t eafnosupport(void) relegated;
intptr_t eaddrinuse(void) relegated;
intptr_t eaddrnotavail(void) relegated;
intptr_t enetdown(void) relegated;
intptr_t enetunreach(void) relegated;
intptr_t enetreset(void) relegated;
intptr_t econnaborted(void) relegated;
intptr_t econnreset(void) relegated;
intptr_t enobufs(void) relegated;
intptr_t eisconn(void) relegated;
intptr_t enotconn(void) relegated;
intptr_t eshutdown(void) relegated;
intptr_t etoomanyrefs(void) relegated;
intptr_t etimedout(void) relegated;
intptr_t econnrefused(void) relegated;
intptr_t ehostdown(void) relegated;
intptr_t ehostunreach(void) relegated;
intptr_t ealready(void) relegated;
intptr_t einprogress(void) relegated;
intptr_t estale(void) relegated;
intptr_t euclean(void) relegated;
intptr_t enotnam(void) relegated;
intptr_t enavail(void) relegated;
intptr_t eisnam(void) relegated;
intptr_t eremoteio(void) relegated;
intptr_t edquot(void) relegated;
intptr_t enomedium(void) relegated;
intptr_t emediumtype(void) relegated;
intptr_t ecanceled(void) relegated;
intptr_t enokey(void) relegated;
intptr_t ekeyexpired(void) relegated;
intptr_t ekeyrevoked(void) relegated;
intptr_t ekeyrejected(void) relegated;
intptr_t eownerdead(void) relegated;
intptr_t enotrecoverable(void) relegated;
intptr_t erfkill(void) relegated;
intptr_t ehwpoison(void) relegated;

#if defined(__MNO_RED_ZONE__) && defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define __ERRFUN(FUNC)               \
  ({                                 \
    intptr_t NegOne;                 \
    asm volatile("call\t" FUNC       \
                 : "=a"(NegOne)      \
                 : /* no outputs */  \
                 : "rcx", "memory"); \
    NegOne;                          \
  })
#define einval()          __ERRFUN("einval")
#define eperm()           __ERRFUN("eperm")
#define enoent()          __ERRFUN("enoent")
#define esrch()           __ERRFUN("esrch")
#define eintr()           __ERRFUN("eintr")
#define eio()             __ERRFUN("eio")
#define enxio()           __ERRFUN("enxio")
#define e2big()           __ERRFUN("e2big")
#define enoexec()         __ERRFUN("enoexec")
#define ebadf()           __ERRFUN("ebadf")
#define echild()          __ERRFUN("echild")
#define eagain()          __ERRFUN("eagain")
#define enomem()          __ERRFUN("enomem")
#define eacces()          __ERRFUN("eacces")
#define efault()          __ERRFUN("efault")
#define enotblk()         __ERRFUN("enotblk")
#define ebusy()           __ERRFUN("ebusy")
#define eexist()          __ERRFUN("eexist")
#define exdev()           __ERRFUN("exdev")
#define enodev()          __ERRFUN("enodev")
#define enotdir()         __ERRFUN("enotdir")
#define eisdir()          __ERRFUN("eisdir")
#define enfile()          __ERRFUN("enfile")
#define emfile()          __ERRFUN("emfile")
#define enotty()          __ERRFUN("enotty")
#define enotsup()         __ERRFUN("enotsup")
#define etxtbsy()         __ERRFUN("etxtbsy")
#define efbig()           __ERRFUN("efbig")
#define enospc()          __ERRFUN("enospc")
#define espipe()          __ERRFUN("espipe")
#define erofs()           __ERRFUN("erofs")
#define emlink()          __ERRFUN("emlink")
#define epipe()           __ERRFUN("epipe")
#define edom()            __ERRFUN("edom")
#define erange()          __ERRFUN("erange")
#define edeadlk()         __ERRFUN("edeadlk")
#define enametoolong()    __ERRFUN("enametoolong")
#define enolck()          __ERRFUN("enolck")
#define enosys()          __ERRFUN("enosys")
#define enotempty()       __ERRFUN("enotempty")
#define eloop()           __ERRFUN("eloop")
#define enomsg()          __ERRFUN("enomsg")
#define eidrm()           __ERRFUN("eidrm")
#define echrng()          __ERRFUN("echrng")
#define el2nsync()        __ERRFUN("el2nsync")
#define el3hlt()          __ERRFUN("el3hlt")
#define el3rst()          __ERRFUN("el3rst")
#define elnrng()          __ERRFUN("elnrng")
#define eunatch()         __ERRFUN("eunatch")
#define enocsi()          __ERRFUN("enocsi")
#define el2hlt()          __ERRFUN("el2hlt")
#define ebade()           __ERRFUN("ebade")
#define ebadr()           __ERRFUN("ebadr")
#define exfull()          __ERRFUN("exfull")
#define enoano()          __ERRFUN("enoano")
#define ebadrqc()         __ERRFUN("ebadrqc")
#define ebadslt()         __ERRFUN("ebadslt")
#define enostr()          __ERRFUN("enostr")
#define enodata()         __ERRFUN("enodata")
#define etime()           __ERRFUN("etime")
#define enosr()           __ERRFUN("enosr")
#define enonet()          __ERRFUN("enonet")
#define enopkg()          __ERRFUN("enopkg")
#define eremote()         __ERRFUN("eremote")
#define enolink()         __ERRFUN("enolink")
#define eadv()            __ERRFUN("eadv")
#define esrmnt()          __ERRFUN("esrmnt")
#define ecomm()           __ERRFUN("ecomm")
#define eproto()          __ERRFUN("eproto")
#define emultihop()       __ERRFUN("emultihop")
#define edotdot()         __ERRFUN("edotdot")
#define ebadmsg()         __ERRFUN("ebadmsg")
#define eoverflow()       __ERRFUN("eoverflow")
#define enotuniq()        __ERRFUN("enotuniq")
#define ebadfd()          __ERRFUN("ebadfd")
#define eremchg()         __ERRFUN("eremchg")
#define elibacc()         __ERRFUN("elibacc")
#define elibbad()         __ERRFUN("elibbad")
#define elibscn()         __ERRFUN("elibscn")
#define elibmax()         __ERRFUN("elibmax")
#define elibexec()        __ERRFUN("elibexec")
#define eilseq()          __ERRFUN("eilseq")
#define erestart()        __ERRFUN("erestart")
#define estrpipe()        __ERRFUN("estrpipe")
#define eusers()          __ERRFUN("eusers")
#define enotsock()        __ERRFUN("enotsock")
#define edestaddrreq()    __ERRFUN("edestaddrreq")
#define emsgsize()        __ERRFUN("emsgsize")
#define eprototype()      __ERRFUN("eprototype")
#define enoprotoopt()     __ERRFUN("enoprotoopt")
#define eprotonosupport() __ERRFUN("eprotonosupport")
#define esocktnosupport() __ERRFUN("esocktnosupport")
#define eopnotsupp()      __ERRFUN("eopnotsupp")
#define epfnosupport()    __ERRFUN("epfnosupport")
#define eafnosupport()    __ERRFUN("eafnosupport")
#define eaddrinuse()      __ERRFUN("eaddrinuse")
#define eaddrnotavail()   __ERRFUN("eaddrnotavail")
#define enetdown()        __ERRFUN("enetdown")
#define enetunreach()     __ERRFUN("enetunreach")
#define enetreset()       __ERRFUN("enetreset")
#define econnaborted()    __ERRFUN("econnaborted")
#define econnreset()      __ERRFUN("econnreset")
#define enobufs()         __ERRFUN("enobufs")
#define eisconn()         __ERRFUN("eisconn")
#define enotconn()        __ERRFUN("enotconn")
#define eshutdown()       __ERRFUN("eshutdown")
#define etoomanyrefs()    __ERRFUN("etoomanyrefs")
#define etimedout()       __ERRFUN("etimedout")
#define econnrefused()    __ERRFUN("econnrefused")
#define ehostdown()       __ERRFUN("ehostdown")
#define ehostunreach()    __ERRFUN("ehostunreach")
#define ealready()        __ERRFUN("ealready")
#define einprogress()     __ERRFUN("einprogress")
#define estale()          __ERRFUN("estale")
#define euclean()         __ERRFUN("euclean")
#define enotnam()         __ERRFUN("enotnam")
#define enavail()         __ERRFUN("enavail")
#define eisnam()          __ERRFUN("eisnam")
#define eremoteio()       __ERRFUN("eremoteio")
#define edquot()          __ERRFUN("edquot")
#define enomedium()       __ERRFUN("enomedium")
#define emediumtype()     __ERRFUN("emediumtype")
#define ecanceled()       __ERRFUN("ecanceled")
#define enokey()          __ERRFUN("enokey")
#define ekeyexpired()     __ERRFUN("ekeyexpired")
#define ekeyrevoked()     __ERRFUN("ekeyrevoked")
#define ekeyrejected()    __ERRFUN("ekeyrejected")
#define eownerdead()      __ERRFUN("eownerdead")
#define enotrecoverable() __ERRFUN("enotrecoverable")
#define erfkill()         __ERRFUN("erfkill")
#define ehwpoison()       __ERRFUN("ehwpoison")
#endif

#else
#include "libc/errno.h"

static inline intptr_t einval(void) {
  errno = EINVAL;
  return -1;
}

static inline intptr_t eperm(void) {
  errno = EPERM;
  return -1;
}

static inline intptr_t enoent(void) {
  errno = ENOENT;
  return -1;
}

static inline intptr_t esrch(void) {
  errno = ESRCH;
  return -1;
}

static inline intptr_t eintr(void) {
  errno = EINTR;
  return -1;
}

static inline intptr_t eio(void) {
  errno = EIO;
  return -1;
}

static inline intptr_t enxio(void) {
  errno = ENXIO;
  return -1;
}

static inline intptr_t e2big(void) {
  errno = E2BIG;
  return -1;
}

static inline intptr_t enoexec(void) {
  errno = ENOEXEC;
  return -1;
}

static inline intptr_t ebadf(void) {
  errno = EBADF;
  return -1;
}

static inline intptr_t echild(void) {
  errno = ECHILD;
  return -1;
}

static inline intptr_t eagain(void) {
  errno = EAGAIN;
  return -1;
}

static inline intptr_t enomem(void) {
  errno = ENOMEM;
  return -1;
}

static inline intptr_t eacces(void) {
  errno = EACCES;
  return -1;
}

static inline intptr_t efault(void) {
  errno = EFAULT;
  return -1;
}

static inline intptr_t enotblk(void) {
  errno = ENOTBLK;
  return -1;
}

static inline intptr_t ebusy(void) {
  errno = EBUSY;
  return -1;
}

static inline intptr_t eexist(void) {
  errno = EEXIST;
  return -1;
}

static inline intptr_t exdev(void) {
  errno = EXDEV;
  return -1;
}

static inline intptr_t enodev(void) {
  errno = ENODEV;
  return -1;
}

static inline intptr_t enotdir(void) {
  errno = ENOTDIR;
  return -1;
}

static inline intptr_t eisdir(void) {
  errno = EISDIR;
  return -1;
}

static inline intptr_t enfile(void) {
  errno = ENFILE;
  return -1;
}

static inline intptr_t emfile(void) {
  errno = EMFILE;
  return -1;
}

static inline intptr_t enotty(void) {
  errno = ENOTTY;
  return -1;
}

static inline intptr_t enotsup(void) {
  errno = ENOTSUP;
  return -1;
}

static inline intptr_t etxtbsy(void) {
  errno = ETXTBSY;
  return -1;
}

static inline intptr_t efbig(void) {
  errno = EFBIG;
  return -1;
}

static inline intptr_t enospc(void) {
  errno = ENOSPC;
  return -1;
}

static inline intptr_t espipe(void) {
  errno = ESPIPE;
  return -1;
}

static inline intptr_t erofs(void) {
  errno = EROFS;
  return -1;
}

static inline intptr_t emlink(void) {
  errno = EMLINK;
  return -1;
}

static inline intptr_t epipe(void) {
  errno = EPIPE;
  return -1;
}

static inline intptr_t edom(void) {
  errno = EDOM;
  return -1;
}

static inline intptr_t erange(void) {
  errno = ERANGE;
  return -1;
}

static inline intptr_t edeadlk(void) {
  errno = EDEADLK;
  return -1;
}

static inline intptr_t enametoolong(void) {
  errno = ENAMETOOLONG;
  return -1;
}

static inline intptr_t enolck(void) {
  errno = ENOLCK;
  return -1;
}

static inline intptr_t enosys(void) {
  errno = ENOSYS;
  return -1;
}

static inline intptr_t enotempty(void) {
  errno = ENOTEMPTY;
  return -1;
}

static inline intptr_t eloop(void) {
  errno = ELOOP;
  return -1;
}

static inline intptr_t enomsg(void) {
  errno = ENOMSG;
  return -1;
}

static inline intptr_t eidrm(void) {
  errno = EIDRM;
  return -1;
}

static inline intptr_t echrng(void) {
  errno = ECHRNG;
  return -1;
}

static inline intptr_t el2nsync(void) {
  errno = EL2NSYNC;
  return -1;
}

static inline intptr_t el3hlt(void) {
  errno = EL3HLT;
  return -1;
}

static inline intptr_t el3rst(void) {
  errno = EL3RST;
  return -1;
}

static inline intptr_t elnrng(void) {
  errno = ELNRNG;
  return -1;
}

static inline intptr_t eunatch(void) {
  errno = EUNATCH;
  return -1;
}

static inline intptr_t enocsi(void) {
  errno = ENOCSI;
  return -1;
}

static inline intptr_t el2hlt(void) {
  errno = EL2HLT;
  return -1;
}

static inline intptr_t ebade(void) {
  errno = EBADE;
  return -1;
}

static inline intptr_t ebadr(void) {
  errno = EBADR;
  return -1;
}

static inline intptr_t exfull(void) {
  errno = EXFULL;
  return -1;
}

static inline intptr_t enoano(void) {
  errno = ENOANO;
  return -1;
}

static inline intptr_t ebadrqc(void) {
  errno = EBADRQC;
  return -1;
}

static inline intptr_t ebadslt(void) {
  errno = EBADSLT;
  return -1;
}

static inline intptr_t enostr(void) {
  errno = ENOSTR;
  return -1;
}

static inline intptr_t enodata(void) {
  errno = ENODATA;
  return -1;
}

static inline intptr_t etime(void) {
  errno = ETIME;
  return -1;
}

static inline intptr_t enosr(void) {
  errno = ENOSR;
  return -1;
}

static inline intptr_t enonet(void) {
  errno = ENONET;
  return -1;
}

static inline intptr_t enopkg(void) {
  errno = ENOPKG;
  return -1;
}

static inline intptr_t eremote(void) {
  errno = EREMOTE;
  return -1;
}

static inline intptr_t enolink(void) {
  errno = ENOLINK;
  return -1;
}

static inline intptr_t eadv(void) {
  errno = EADV;
  return -1;
}

static inline intptr_t esrmnt(void) {
  errno = ESRMNT;
  return -1;
}

static inline intptr_t ecomm(void) {
  errno = ECOMM;
  return -1;
}

static inline intptr_t eproto(void) {
  errno = EPROTO;
  return -1;
}

static inline intptr_t emultihop(void) {
  errno = EMULTIHOP;
  return -1;
}

static inline intptr_t edotdot(void) {
  errno = EDOTDOT;
  return -1;
}

static inline intptr_t ebadmsg(void) {
  errno = EBADMSG;
  return -1;
}

static inline intptr_t eoverflow(void) {
  errno = EOVERFLOW;
  return -1;
}

static inline intptr_t enotuniq(void) {
  errno = ENOTUNIQ;
  return -1;
}

static inline intptr_t ebadfd(void) {
  errno = EBADFD;
  return -1;
}

static inline intptr_t eremchg(void) {
  errno = EREMCHG;
  return -1;
}

static inline intptr_t elibacc(void) {
  errno = ELIBACC;
  return -1;
}

static inline intptr_t elibbad(void) {
  errno = ELIBBAD;
  return -1;
}

static inline intptr_t elibscn(void) {
  errno = ELIBSCN;
  return -1;
}

static inline intptr_t elibmax(void) {
  errno = ELIBMAX;
  return -1;
}

static inline intptr_t elibexec(void) {
  errno = ELIBEXEC;
  return -1;
}

static inline intptr_t eilseq(void) {
  errno = EILSEQ;
  return -1;
}

static inline intptr_t erestart(void) {
  errno = ERESTART;
  return -1;
}

static inline intptr_t estrpipe(void) {
  errno = ESTRPIPE;
  return -1;
}

static inline intptr_t eusers(void) {
  errno = EUSERS;
  return -1;
}

static inline intptr_t enotsock(void) {
  errno = ENOTSOCK;
  return -1;
}

static inline intptr_t edestaddrreq(void) {
  errno = EDESTADDRREQ;
  return -1;
}

static inline intptr_t emsgsize(void) {
  errno = EMSGSIZE;
  return -1;
}

static inline intptr_t eprototype(void) {
  errno = EPROTOTYPE;
  return -1;
}

static inline intptr_t enoprotoopt(void) {
  errno = ENOPROTOOPT;
  return -1;
}

static inline intptr_t eprotonosupport(void) {
  errno = EPROTONOSUPPORT;
  return -1;
}

static inline intptr_t esocktnosupport(void) {
  errno = ESOCKTNOSUPPORT;
  return -1;
}

static inline intptr_t eopnotsupp(void) {
  errno = EOPNOTSUPP;
  return -1;
}

static inline intptr_t epfnosupport(void) {
  errno = EPFNOSUPPORT;
  return -1;
}

static inline intptr_t eafnosupport(void) {
  errno = EAFNOSUPPORT;
  return -1;
}

static inline intptr_t eaddrinuse(void) {
  errno = EADDRINUSE;
  return -1;
}

static inline intptr_t eaddrnotavail(void) {
  errno = EADDRNOTAVAIL;
  return -1;
}

static inline intptr_t enetdown(void) {
  errno = ENETDOWN;
  return -1;
}

static inline intptr_t enetunreach(void) {
  errno = ENETUNREACH;
  return -1;
}

static inline intptr_t enetreset(void) {
  errno = ENETRESET;
  return -1;
}

static inline intptr_t econnaborted(void) {
  errno = ECONNABORTED;
  return -1;
}

static inline intptr_t econnreset(void) {
  errno = ECONNRESET;
  return -1;
}

static inline intptr_t enobufs(void) {
  errno = ENOBUFS;
  return -1;
}

static inline intptr_t eisconn(void) {
  errno = EISCONN;
  return -1;
}

static inline intptr_t enotconn(void) {
  errno = ENOTCONN;
  return -1;
}

static inline intptr_t eshutdown(void) {
  errno = ESHUTDOWN;
  return -1;
}

static inline intptr_t etoomanyrefs(void) {
  errno = ETOOMANYREFS;
  return -1;
}

static inline intptr_t etimedout(void) {
  errno = ETIMEDOUT;
  return -1;
}

static inline intptr_t econnrefused(void) {
  errno = ECONNREFUSED;
  return -1;
}

static inline intptr_t ehostdown(void) {
  errno = EHOSTDOWN;
  return -1;
}

static inline intptr_t ehostunreach(void) {
  errno = EHOSTUNREACH;
  return -1;
}

static inline intptr_t ealready(void) {
  errno = EALREADY;
  return -1;
}

static inline intptr_t einprogress(void) {
  errno = EINPROGRESS;
  return -1;
}

static inline intptr_t estale(void) {
  errno = ESTALE;
  return -1;
}

static inline intptr_t euclean(void) {
  errno = EUCLEAN;
  return -1;
}

static inline intptr_t enotnam(void) {
  errno = ENOTNAM;
  return -1;
}

static inline intptr_t enavail(void) {
  errno = ENAVAIL;
  return -1;
}

static inline intptr_t eisnam(void) {
  errno = EISNAM;
  return -1;
}

static inline intptr_t eremoteio(void) {
  errno = EREMOTEIO;
  return -1;
}

static inline intptr_t edquot(void) {
  errno = EDQUOT;
  return -1;
}

static inline intptr_t enomedium(void) {
  errno = ENOMEDIUM;
  return -1;
}

static inline intptr_t emediumtype(void) {
  errno = EMEDIUMTYPE;
  return -1;
}

static inline intptr_t ecanceled(void) {
  errno = ECANCELED;
  return -1;
}

static inline intptr_t enokey(void) {
  errno = ENOKEY;
  return -1;
}

static inline intptr_t ekeyexpired(void) {
  errno = EKEYEXPIRED;
  return -1;
}

static inline intptr_t ekeyrevoked(void) {
  errno = EKEYREVOKED;
  return -1;
}

static inline intptr_t ekeyrejected(void) {
  errno = EKEYREJECTED;
  return -1;
}

static inline intptr_t eownerdead(void) {
  errno = EOWNERDEAD;
  return -1;
}

static inline intptr_t enotrecoverable(void) {
  errno = ENOTRECOVERABLE;
  return -1;
}

static inline intptr_t erfkill(void) {
  errno = ERFKILL;
  return -1;
}

static inline intptr_t ehwpoison(void) {
  errno = EHWPOISON;
  return -1;
}

#endif /* __x86_64__ */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_ERRFUNS_H_ */
