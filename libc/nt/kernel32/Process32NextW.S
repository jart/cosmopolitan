#include "libc/nt/codegen.h"
.imp	kernel32,__imp_Process32NextW,Process32NextW,0

#ifdef __x86_64__
	.text.windows
Process32Next:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_Process32NextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	Process32Next,globl
	.previous
#endif
