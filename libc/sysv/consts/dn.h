#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_DN_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_DN_H_

#define DN_ACCESS    0x00000001 /* file accessed */
#define DN_MODIFY    0x00000002 /* file modified */
#define DN_CREATE    0x00000004 /* file created */
#define DN_DELETE    0x00000008 /* file removed */
#define DN_RENAME    0x00000010 /* file renamed */
#define DN_ATTRIB    0x00000020 /* file changed attibutes */
#define DN_MULTISHOT 0x80000000 /* don't remove notifier */

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_DN_H_ */
