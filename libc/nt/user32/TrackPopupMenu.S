#include "libc/nt/codegen.h"
.imp	user32,__imp_TrackPopupMenu,TrackPopupMenu,2443

	.text.windows
TrackPopupMenu:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TrackPopupMenu(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	TrackPopupMenu,globl
	.previous
