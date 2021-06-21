.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WideCharToMultiByte,WideCharToMultiByte,1553

	.text.windows
WideCharToMultiByte:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WideCharToMultiByte(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WideCharToMultiByte,globl
	.previous
