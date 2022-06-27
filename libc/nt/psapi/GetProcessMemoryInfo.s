.include "o/libc/nt/codegen.inc"
.imp	psapi,__imp_GetProcessMemoryInfo,GetProcessMemoryInfo,0

	.text.windows
GetProcessMemoryInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessMemoryInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessMemoryInfo,globl
	.previous
