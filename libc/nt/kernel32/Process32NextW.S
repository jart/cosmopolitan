#include "libc/nt/codegen.h"
.imp	kernel32,__imp_Process32NextW,Process32NextW,0

	.text.windows
Process32Next:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_Process32NextW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	Process32Next,globl
	.previous
