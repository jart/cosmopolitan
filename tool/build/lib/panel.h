#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PANEL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PANEL_H_
#include "tool/build/lib/buffer.h"
COSMOPOLITAN_C_START_

struct Panel {
  int top;
  int bottom;
  int left;
  int right;
  struct Buffer *lines;
  size_t n;
};

ssize_t PrintPanels(int, long, struct Panel *, long, long);
void PrintMessageBox(int, const char *, long, long);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PANEL_H_ */
