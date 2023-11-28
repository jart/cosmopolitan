#ifndef COSMOPOLITAN_LIBC_LOG_COLOR_H_
#define COSMOPOLITAN_LIBC_LOG_COLOR_H_
#include "libc/log/internal.h"

#define CLS    (!__nocolor ? "\r\e[J" : "")
#define RED    (!__nocolor ? "\e[30;101m" : "")
#define GREEN  (!__nocolor ? "\e[32m" : "")
#define UNBOLD (!__nocolor ? "\e[22m" : "")
#define RED2   (!__nocolor ? "\e[91;1m" : "")
#define BLUE1  (!__nocolor ? "\e[94;49m" : "")
#define BLUE2  (!__nocolor ? "\e[34m" : "")
#define RESET  (!__nocolor ? "\e[0m" : "")
#define SUBTLE (!__nocolor ? "\e[35m" : "")

#endif /* COSMOPOLITAN_LIBC_LOG_COLOR_H_ */
