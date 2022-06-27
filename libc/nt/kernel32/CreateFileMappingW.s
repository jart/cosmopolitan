.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateFileMappingW,CreateFileMappingW,0

	.text.windows
__CreateFileMapping:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileMappingW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__CreateFileMapping,globl
	.previous
