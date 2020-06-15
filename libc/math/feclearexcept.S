.global feclearexcept
.type feclearexcept,@function
feclearexcept:
	# maintain exceptions in the sse mxcsr, clear x87 exceptions
	mov	%edi,%ecx
	and	$0x3f,%ecx
	fnstsw	%ax
	test	%eax,%ecx
	jz	1f
	fnclex
1:	stmxcsr -8(%rsp)
	and	$0x3f,%eax
	or	%eax,-8(%rsp)
	test	%ecx,-8(%rsp)
	jz	1f
	not	%ecx
	and	%ecx,-8(%rsp)
	ldmxcsr	-8(%rsp)
1:	xor	%eax,%eax
	ret
