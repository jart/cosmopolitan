#include "libc/nt/ntdllimport.h"
.ntimp	NtDeviceIoControlFile,NtDeviceIoControlFile

#ifdef __x86_64__
	.text.windows
NtDeviceIoControlFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtDeviceIoControlFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtDeviceIoControlFile,globl
	.previous
#endif
