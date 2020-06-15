.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_VirtualProtect,VirtualProtect,1764

	.text.windows
VirtualProtect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualProtect(%rip),%rax
	jmp	__sysv2nt
	.endfn	VirtualProtect,globl
	.previous
