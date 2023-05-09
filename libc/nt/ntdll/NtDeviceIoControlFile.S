#include "libc/nt/ntdllimport.h"
.ntimp	NtDeviceIoControlFile,NtDeviceIoControlFile

	.text.windows
NtDeviceIoControlFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDeviceIoControlFile(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtDeviceIoControlFile,globl
	.previous
