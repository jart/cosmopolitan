#include "libc/nt/codegen.h"
.imp	kernel32,__imp_TransmitCommChar,TransmitCommChar,0

#ifdef __x86_64__
	.text.windows
TransmitCommChar:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TransmitCommChar(%rip),%rax
	jmp	__sysv2nt
	.endfn	TransmitCommChar,globl
	.previous
#endif
