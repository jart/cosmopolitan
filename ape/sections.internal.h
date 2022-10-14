#ifndef COSMOPOLITAN_APE_SECTIONS_INTERNAL_H_
#define COSMOPOLITAN_APE_SECTIONS_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern unsigned char _base[];
extern unsigned char _ehead[];
extern unsigned char _etext[];
extern unsigned char _edata[];
extern unsigned char _ezip[];
extern unsigned char _end[];
extern unsigned char _ereal[];
extern unsigned char __privileged_start[];
extern unsigned char __privileged_addr[];
extern unsigned char __privileged_size[];
extern unsigned char __privileged_end[];
extern unsigned char __test_start[];
extern unsigned char __ro[];
extern unsigned char *__relo_start[];
extern unsigned char *__relo_end[];
extern uint8_t __zip_start[];
extern uint8_t __zip_end[];
extern uint8_t __data_start[];
extern uint8_t __data_end[];
extern uint8_t __bss_start[];
extern uint8_t __bss_end[];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_APE_SECTIONS_INTERNAL_H_ */
