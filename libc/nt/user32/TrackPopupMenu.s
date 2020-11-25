.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_TrackPopupMenu,TrackPopupMenu,2443

	.text.windows
TrackPopupMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TrackPopupMenu(%rip),%rax
	jmp	__sysv2nt8
	.endfn	TrackPopupMenu,globl
	.previous
