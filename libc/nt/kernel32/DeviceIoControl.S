#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DeviceIoControl,DeviceIoControl,0

#ifdef __x86_64__
	.text.windows
__DeviceIoControl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DeviceIoControl(%rip),%rax
	jmp	__sysv2nt8
	.endfn	__DeviceIoControl,globl
	.previous
#endif
