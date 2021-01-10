asm(".section .start,\"ax\",@progbits\n\t"
    ".globl\t_start\n"
    "_start:\n\t"
    "jmp\t1f\n1:\t"
    "call\tmain\n\t"
    "nop\n\t"
    ".previous");
