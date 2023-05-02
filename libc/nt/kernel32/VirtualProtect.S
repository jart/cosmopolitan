#include "libc/nt/codegen.h"
.imp	kernel32,__imp_VirtualProtect,VirtualProtect,0

#ifdef __x86_64__
	.text.windows
__VirtualProtect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualProtect(%rip),%rax
	jmp	__sysv2nt
	.endfn	__VirtualProtect,globl
	.previous
#endif
