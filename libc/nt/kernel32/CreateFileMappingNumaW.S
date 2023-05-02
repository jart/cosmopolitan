#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateFileMappingNumaW,CreateFileMappingNumaW,0

#ifdef __x86_64__
	.text.windows
__CreateFileMappingNuma:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileMappingNumaW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	__CreateFileMappingNuma,globl
	.previous
#endif
