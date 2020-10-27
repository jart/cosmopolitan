#ifndef COSMOPOLITAN_LIBC_LOG_COLOR_H_
#define COSMOPOLITAN_LIBC_LOG_COLOR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define CLS    (cancolor() ? "\r\e[J" : "")
#define RED    (cancolor() ? "\e[30;101m" : "")
#define GREEN  (cancolor() ? "\e[32m" : "")
#define UNBOLD (cancolor() ? "\e[22m" : "")
#define RED2   (cancolor() ? "\e[91;1m" : "")
#define BLUE1  (cancolor() ? "\e[94;49m" : "")
#define BLUE2  (cancolor() ? "\e[34m" : "")
#define RESET  (cancolor() ? "\e[0m" : "")
#define SUBTLE (cancolor() ? "\e[35m" : "")

bool cancolor(void) nothrow nocallback nosideeffect;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_COLOR_H_ */
