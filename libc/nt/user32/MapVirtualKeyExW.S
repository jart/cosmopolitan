#include "libc/nt/codegen.h"
.imp	user32,__imp_MapVirtualKeyExW,MapVirtualKeyExW,2155

#ifdef __x86_64__
	.text.windows
MapVirtualKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapVirtualKeyExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MapVirtualKeyEx,globl
	.previous
#endif
