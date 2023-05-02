#include "libc/nt/codegen.h"
.imp	user32,__imp_TrackPopupMenu,TrackPopupMenu,2443

#ifdef __x86_64__
	.text.windows
TrackPopupMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TrackPopupMenu(%rip),%rax
	jmp	__sysv2nt8
	.endfn	TrackPopupMenu,globl
	.previous
#endif
