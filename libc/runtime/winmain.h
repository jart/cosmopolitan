#ifndef COSMOPOLITAN_LIBC_RUNTIME_WINMAIN_H_
#define COSMOPOLITAN_LIBC_RUNTIME_WINMAIN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct WinMain {
  int nCmdShow;
  int64_t hInstance;
  int64_t hPrevInstance;
};

extern struct WinMain g_winmain;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_WINMAIN_H_ */
