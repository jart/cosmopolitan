#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateFileMappingW,CreateFileMappingW,0

#ifdef __x86_64__
	.text.windows
__CreateFileMapping:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileMappingW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__CreateFileMapping,globl
	.previous
#endif
