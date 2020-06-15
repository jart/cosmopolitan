.global fegetenv
.type fegetenv,@function
fegetenv:
	xor %eax,%eax
	fnstenv (%rdi)
	stmxcsr 28(%rdi)
	ret
