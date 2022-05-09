.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_GetAddrInfoExW,GetAddrInfoExW,31

	.text.windows
GetAddrInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAddrInfoExW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	GetAddrInfoEx,globl
	.previous
