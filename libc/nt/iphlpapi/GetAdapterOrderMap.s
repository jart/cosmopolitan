.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_GetAdapterOrderMap,GetAdapterOrderMap,0

	.text.windows
GetAdapterOrderMap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetAdapterOrderMap(%rip)
	leave
	ret
	.endfn	GetAdapterOrderMap,globl
	.previous
