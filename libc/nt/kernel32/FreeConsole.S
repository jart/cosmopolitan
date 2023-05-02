#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FreeConsole,FreeConsole,0

#ifdef __x86_64__
	.text.windows
FreeConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_FreeConsole(%rip)
	leave
	ret
	.endfn	FreeConsole,globl
	.previous
#endif
