#ifndef COSMOPOLITAN_TOOL_BUILD_ELF2PE_H_
#define COSMOPOLITAN_TOOL_BUILD_ELF2PE_H_

#define __dll_import(DLL, RET, FUNC, ARGS)                      \
  extern RET(*const __attribute__((__ms_abi__, __weak__)) FUNC) \
      ARGS __asm__("\"dll$" DLL "$" #FUNC "\"")

#endif /* COSMOPOLITAN_TOOL_BUILD_ELF2PE_H_ */
