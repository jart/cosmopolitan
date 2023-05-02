#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegFlushKey,RegFlushKey,0

#ifdef __x86_64__
	.text.windows
RegFlushKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegFlushKey(%rip)
	leave
	ret
	.endfn	RegFlushKey,globl
	.previous
#endif
