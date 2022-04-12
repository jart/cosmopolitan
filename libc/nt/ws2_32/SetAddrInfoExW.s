.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_SetAddrInfoExW,SetAddrInfoExW,38

	.text.windows
SetAddrInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetAddrInfoExW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	SetAddrInfoEx,globl
	.previous
