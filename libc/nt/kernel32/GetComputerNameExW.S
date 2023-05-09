#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetComputerNameExW,GetComputerNameExW,0

	.text.windows
GetComputerNameEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetComputerNameExW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetComputerNameEx,globl
	.previous
