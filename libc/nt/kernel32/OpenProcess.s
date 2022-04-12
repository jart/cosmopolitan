.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_OpenProcess,OpenProcess,0

	.text.windows
__OpenProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	__OpenProcess,globl
	.previous
