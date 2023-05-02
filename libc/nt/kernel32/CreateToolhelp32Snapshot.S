#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateToolhelp32Snapshot,CreateToolhelp32Snapshot,0

#ifdef __x86_64__
	.text.windows
CreateToolhelp32Snapshot:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateToolhelp32Snapshot(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateToolhelp32Snapshot,globl
	.previous
#endif
