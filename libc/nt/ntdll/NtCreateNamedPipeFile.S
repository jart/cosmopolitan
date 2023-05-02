#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateNamedPipeFile,NtCreateNamedPipeFile

#ifdef __x86_64__
	.text.windows
NtCreateNamedPipeFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateNamedPipeFile(%rip),%rax
	jmp	__sysv2nt14
	.endfn	NtCreateNamedPipeFile,globl
	.previous
#endif
