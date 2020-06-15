.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_MapViewOfFileExNuma,MapViewOfFileExNuma,1006

	.text.windows
MapViewOfFileExNuma:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapViewOfFileExNuma(%rip),%rax
	jmp	__sysv2nt8
	.endfn	MapViewOfFileExNuma,globl
	.previous
