#ifndef COSMOPOLITAN_LIBC_SYSV_ERRFUNS_H_
#define COSMOPOLITAN_LIBC_SYSV_ERRFUNS_H_

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

COSMOPOLITAN_C_START_

libcesque intptr_t einval(void) relegated;
libcesque intptr_t eperm(void) relegated;
libcesque intptr_t enoent(void) relegated;
libcesque intptr_t esrch(void) relegated;
libcesque intptr_t eintr(void) relegated;
libcesque intptr_t eio(void) relegated;
libcesque intptr_t enxio(void) relegated;
libcesque intptr_t e2big(void) relegated;
libcesque intptr_t enoexec(void) relegated;
libcesque intptr_t ebadf(void) relegated;
libcesque intptr_t echild(void) relegated;
libcesque intptr_t eagain(void) relegated;
libcesque intptr_t enomem(void) relegated;
libcesque intptr_t eacces(void) relegated;
libcesque intptr_t efault(void) relegated;
libcesque intptr_t enotblk(void) relegated;
libcesque intptr_t ebusy(void) relegated;
libcesque intptr_t eexist(void) relegated;
libcesque intptr_t exdev(void) relegated;
libcesque intptr_t enodev(void) relegated;
libcesque intptr_t enotdir(void) relegated;
libcesque intptr_t eisdir(void) relegated;
libcesque intptr_t enfile(void) relegated;
libcesque intptr_t emfile(void) relegated;
libcesque intptr_t enotty(void) relegated;
libcesque intptr_t enotsup(void) relegated;
libcesque intptr_t etxtbsy(void) relegated;
libcesque intptr_t efbig(void) relegated;
libcesque intptr_t enospc(void) relegated;
libcesque intptr_t espipe(void) relegated;
libcesque intptr_t erofs(void) relegated;
libcesque intptr_t emlink(void) relegated;
libcesque intptr_t epipe(void) relegated;
libcesque intptr_t edom(void) relegated;
libcesque intptr_t erange(void) relegated;
libcesque intptr_t edeadlk(void) relegated;
libcesque intptr_t enametoolong(void) relegated;
libcesque intptr_t enolck(void) relegated;
libcesque intptr_t enosys(void) relegated;
libcesque intptr_t enotempty(void) relegated;
libcesque intptr_t eloop(void) relegated;
libcesque intptr_t enomsg(void) relegated;
libcesque intptr_t eidrm(void) relegated;
libcesque intptr_t echrng(void) relegated;
libcesque intptr_t el2nsync(void) relegated;
libcesque intptr_t el3hlt(void) relegated;
libcesque intptr_t el3rst(void) relegated;
libcesque intptr_t elnrng(void) relegated;
libcesque intptr_t eunatch(void) relegated;
libcesque intptr_t enocsi(void) relegated;
libcesque intptr_t el2hlt(void) relegated;
libcesque intptr_t ebade(void) relegated;
libcesque intptr_t ebadr(void) relegated;
libcesque intptr_t exfull(void) relegated;
libcesque intptr_t enoano(void) relegated;
libcesque intptr_t ebadrqc(void) relegated;
libcesque intptr_t ebadslt(void) relegated;
libcesque intptr_t enostr(void) relegated;
libcesque intptr_t enodata(void) relegated;
libcesque intptr_t etime(void) relegated;
libcesque intptr_t enosr(void) relegated;
libcesque intptr_t enonet(void) relegated;
libcesque intptr_t enopkg(void) relegated;
libcesque intptr_t eremote(void) relegated;
libcesque intptr_t enolink(void) relegated;
libcesque intptr_t eadv(void) relegated;
libcesque intptr_t esrmnt(void) relegated;
libcesque intptr_t ecomm(void) relegated;
libcesque intptr_t eproto(void) relegated;
libcesque intptr_t emultihop(void) relegated;
libcesque intptr_t edotdot(void) relegated;
libcesque intptr_t ebadmsg(void) relegated;
libcesque intptr_t eoverflow(void) relegated;
libcesque intptr_t enotuniq(void) relegated;
libcesque intptr_t ebadfd(void) relegated;
libcesque intptr_t eremchg(void) relegated;
libcesque intptr_t elibacc(void) relegated;
libcesque intptr_t elibbad(void) relegated;
libcesque intptr_t elibscn(void) relegated;
libcesque intptr_t elibmax(void) relegated;
libcesque intptr_t elibexec(void) relegated;
libcesque intptr_t eilseq(void) relegated;
libcesque intptr_t erestart(void) relegated;
libcesque intptr_t estrpipe(void) relegated;
libcesque intptr_t eusers(void) relegated;
libcesque intptr_t enotsock(void) relegated;
libcesque intptr_t edestaddrreq(void) relegated;
libcesque intptr_t emsgsize(void) relegated;
libcesque intptr_t eprototype(void) relegated;
libcesque intptr_t enoprotoopt(void) relegated;
libcesque intptr_t eprotonosupport(void) relegated;
libcesque intptr_t esocktnosupport(void) relegated;
libcesque intptr_t eopnotsupp(void) relegated;
libcesque intptr_t epfnosupport(void) relegated;
libcesque intptr_t eafnosupport(void) relegated;
libcesque intptr_t eaddrinuse(void) relegated;
libcesque intptr_t eaddrnotavail(void) relegated;
libcesque intptr_t enetdown(void) relegated;
libcesque intptr_t enetunreach(void) relegated;
libcesque intptr_t enetreset(void) relegated;
libcesque intptr_t econnaborted(void) relegated;
libcesque intptr_t econnreset(void) relegated;
libcesque intptr_t enobufs(void) relegated;
libcesque intptr_t eisconn(void) relegated;
libcesque intptr_t enotconn(void) relegated;
libcesque intptr_t eshutdown(void) relegated;
libcesque intptr_t etoomanyrefs(void) relegated;
libcesque intptr_t etimedout(void) relegated;
libcesque intptr_t econnrefused(void) relegated;
libcesque intptr_t ehostdown(void) relegated;
libcesque intptr_t ehostunreach(void) relegated;
libcesque intptr_t ealready(void) relegated;
libcesque intptr_t einprogress(void) relegated;
libcesque intptr_t estale(void) relegated;
libcesque intptr_t euclean(void) relegated;
libcesque intptr_t enotnam(void) relegated;
libcesque intptr_t enavail(void) relegated;
libcesque intptr_t eisnam(void) relegated;
libcesque intptr_t eremoteio(void) relegated;
libcesque intptr_t edquot(void) relegated;
libcesque intptr_t enomedium(void) relegated;
libcesque intptr_t emediumtype(void) relegated;
libcesque intptr_t ecanceled(void) relegated;
libcesque intptr_t enokey(void) relegated;
libcesque intptr_t ekeyexpired(void) relegated;
libcesque intptr_t ekeyrevoked(void) relegated;
libcesque intptr_t ekeyrejected(void) relegated;
libcesque intptr_t eownerdead(void) relegated;
libcesque intptr_t enotrecoverable(void) relegated;
libcesque intptr_t erfkill(void) relegated;
libcesque intptr_t ehwpoison(void) relegated;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_ERRFUNS_H_ */
