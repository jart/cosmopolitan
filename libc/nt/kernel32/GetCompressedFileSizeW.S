#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCompressedFileSizeW,GetCompressedFileSizeW,0

#ifdef __x86_64__
	.text.windows
GetCompressedFileSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCompressedFileSizeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCompressedFileSize,globl
	.previous
#endif
