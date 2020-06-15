.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtCreateNamedPipeFile

	.text.windows
NtCreateNamedPipeFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateNamedPipeFile(%rip),%rax
	jmp	__sysv2nt14
	.endfn	NtCreateNamedPipeFile,globl
	.previous
