#ifndef COSMOPOLITAN_LIBC_NT_PAINT_H_
#define COSMOPOLITAN_LIBC_NT_PAINT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § new technology » cpu graphics                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

struct NtSize {
  int32_t cx;
  int32_t cy;
};

struct NtRectangle {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

struct NtDrawTextParams {
  uint32_t cbSize;
  int32_t iTabLength;
  int32_t iLeftMargin;
  int32_t iRightMargin;
  uint32_t uiLengthDrawn;
};

struct NtPaintStruct {
  int64_t hdc;
  int32_t fErase;
  struct NtRectangle rcPaint;
  int32_t fRestore;
  int32_t fIncUpdate;
  unsigned char rgbReserved[32];
};

int64_t BeginPaint(int64_t hWnd, struct NtPaintStruct *lpPaint);
int32_t EndPaint(int64_t hWnd, const struct NtPaintStruct *lpPaint);
int32_t BitBlt(int64_t hdc, int x, int y, int cx, int cy, int64_t hdcSrc,
               int x1, int y1, uint32_t rop);
int32_t GetClientRect(int64_t hWnd, struct NtRectangle *lpRect);
int32_t GetWindowRect(int64_t hWnd, struct NtRectangle *lpRect);
int32_t SetBkMode(int64_t hdc, int mode);
uint32_t SetTextColor(int64_t hdc, uint32_t color);
uint32_t SetTextAlign(int64_t hdc, uint32_t align);
int32_t SetTextJustification(int64_t hdc, int extra, int count);
int32_t DrawText(int64_t hdc, const char16_t *lpchText, int cchText,
                 struct NtRectangle *lprc, uint32_t format);
int32_t DrawTextEx(int64_t hdc, char16_t *lpchText, int cchText,
                   struct NtRectangle *lprc, uint32_t format,
                   struct NtDrawTextParams *lpdtp);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_PAINT_H_ */
