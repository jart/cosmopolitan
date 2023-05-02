#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetComputerNameExW,GetComputerNameExW,0

#ifdef __x86_64__
	.text.windows
GetComputerNameEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetComputerNameExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetComputerNameEx,globl
	.previous
#endif
