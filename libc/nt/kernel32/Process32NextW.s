.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_Process32NextW,Process32NextW,0

	.text.windows
Process32Next:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_Process32NextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	Process32Next,globl
	.previous
