#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AllocConsole,AllocConsole,0

	.text.windows
AllocConsole:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_AllocConsole(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	AllocConsole,globl
	.previous
