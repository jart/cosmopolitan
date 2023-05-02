#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetLargestConsoleWindowSize,GetLargestConsoleWindowSize,0

#ifdef __x86_64__
	.text.windows
GetLargestConsoleWindowSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetLargestConsoleWindowSize(%rip)
	leave
	ret
	.endfn	GetLargestConsoleWindowSize,globl
	.previous
#endif
