.text
.globl sha256_update_x86_64
sha256_update_x86_64:
  push %r15
  push %r14
  push %r13
  push %r12
  push %rsi
  push %rdi
  push %rbp
  push %rbx
  movdqu 0(%rdi), %xmm1
  movdqu 16(%rdi), %xmm2
  mov $289644378169868803, %rax
  pinsrq $0, %rax, %xmm7
  mov $868365760874482187, %rax
  pinsrq $1, %rax, %xmm7
  pshufd $27, %xmm1, %xmm0
  pshufd $177, %xmm1, %xmm1
  pshufd $27, %xmm2, %xmm2
  movdqu %xmm7, %xmm8
  palignr $8, %xmm2, %xmm1
  shufpd $0, %xmm0, %xmm2
  jmp L1
.balign 16
L0:
  movdqu 0(%rsi), %xmm3
  movdqu 16(%rsi), %xmm4
  movdqu 32(%rsi), %xmm5
  pshufb %xmm7, %xmm3
  movdqu 48(%rsi), %xmm6
  movdqu 0(%rcx), %xmm0
  paddd %xmm3, %xmm0
  pshufb %xmm7, %xmm4
  movdqu %xmm2, %xmm10
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm1, %xmm9
  sha256rnds2 %xmm2, %xmm1
  movdqu 16(%rcx), %xmm0
  paddd %xmm4, %xmm0
  pshufb %xmm7, %xmm5
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  add $64, %rsi
  sha256msg1 %xmm4, %xmm3
  sha256rnds2 %xmm2, %xmm1
  movdqu 32(%rcx), %xmm0
  paddd %xmm5, %xmm0
  pshufb %xmm7, %xmm6
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm6, %xmm7
  palignr $4, %xmm5, %xmm7
  paddd %xmm7, %xmm3
  sha256msg1 %xmm5, %xmm4
  sha256rnds2 %xmm2, %xmm1
  movdqu 48(%rcx), %xmm0
  paddd %xmm6, %xmm0
  sha256msg2 %xmm6, %xmm3
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm3, %xmm7
  palignr $4, %xmm6, %xmm7
  paddd %xmm7, %xmm4
  sha256msg1 %xmm6, %xmm5
  sha256rnds2 %xmm2, %xmm1
  movdqu 64(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 80(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 96(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 112(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 128(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 144(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 160(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 176(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 192(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  paddd %xmm7, %xmm5
  sha256msg1 %xmm3, %xmm6
  sha256rnds2 %xmm2, %xmm1
  movdqu %xmm3, %xmm7
  movdqu %xmm6, %xmm0
  movdqu %xmm4, %xmm3
  movdqu %xmm5, %xmm4
  movdqu %xmm7, %xmm6
  movdqu %xmm0, %xmm5
  movdqu 208(%rcx), %xmm0
  paddd %xmm3, %xmm0
  sha256msg2 %xmm3, %xmm4
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  movdqu %xmm4, %xmm7
  palignr $4, %xmm3, %xmm7
  sha256rnds2 %xmm2, %xmm1
  paddd %xmm7, %xmm5
  movdqu 224(%rcx), %xmm0
  paddd %xmm4, %xmm0
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  sha256msg2 %xmm4, %xmm5
  movdqu %xmm8, %xmm7
  sha256rnds2 %xmm2, %xmm1
  movdqu 240(%rcx), %xmm0
  paddd %xmm5, %xmm0
  sha256rnds2 %xmm1, %xmm2
  pshufd $14, %xmm0, %xmm0
  sub $1, %rdx
  sha256rnds2 %xmm2, %xmm1
  paddd %xmm10, %xmm2
  paddd %xmm9, %xmm1
.balign 16
L1:
  cmp $0, %rdx
  ja L0
  pshufd $177, %xmm2, %xmm2
  pshufd $27, %xmm1, %xmm7
  pshufd $177, %xmm1, %xmm1
  shufpd $3, %xmm2, %xmm1
  palignr $8, %xmm7, %xmm2
  movdqu %xmm1, 0(%rdi)
  movdqu %xmm2, 16(%rdi)
  pop %rbx
  pop %rbp
  pop %rdi
  pop %rsi
  pop %r12
  pop %r13
  pop %r14
  pop %r15
  ret
