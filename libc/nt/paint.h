#ifndef COSMOPOLITAN_LIBC_NT_PAINT_H_
#define COSMOPOLITAN_LIBC_NT_PAINT_H_
#include "libc/nt/struct/drawtextparams.h"
#include "libc/nt/struct/paintstruct.h"
#include "libc/nt/struct/rect.h"
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § new technology » cpu graphics                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int64_t BeginPaint(int64_t hWnd, struct NtPaintStruct *lpPaint);
int32_t EndPaint(int64_t hWnd, const struct NtPaintStruct *lpPaint);
int32_t BitBlt(int64_t hdc, int x, int y, int cx, int cy, int64_t hdcSrc,
               int x1, int y1, uint32_t rop);
int32_t GetClientRect(int64_t hWnd, struct NtRect *lpRect);
int32_t GetWindowRect(int64_t hWnd, struct NtRect *lpRect);
int32_t SetBkMode(int64_t hdc, int mode);
uint32_t SetTextColor(int64_t hdc, uint32_t color);
uint32_t SetTextAlign(int64_t hdc, uint32_t align);
int32_t SetTextJustification(int64_t hdc, int extra, int count);
int32_t DrawText(int64_t hdc, const char16_t *lpchText, int cchText,
                 struct NtRect *lprc, uint32_t format);
int32_t DrawTextEx(int64_t hdc, char16_t *lpchText, int cchText,
                   struct NtRect *lprc, uint32_t format,
                   struct NtDrawTextParams *lpdtp);
int32_t FillRect(int64_t hDC, const struct NtRect *lpRC, int64_t hBrush);
uint32_t GetPixel(int64_t hdc, int x, int y);
uint32_t SetPixel(int64_t hdc, int x, int y, uint32_t color);
bool32 RedrawWindow(int64_t hWnd, const struct NtRect *opt_lprcUpdate,
                    int64_t opt_hrgnUpdate, uint32_t rdwFlags);
int64_t CreateCompatibleDC(int64_t hdc);
int64_t CreateCompatibleBitmap(int64_t hdc, int cx, int cy);
int64_t SelectObject(int64_t hdc, int64_t h);
bool32 DeleteObject(int64_t ho);
bool32 DeleteDC(int64_t hdc);
int SaveDC(int64_t hdc);
bool32 RestoreDC(int64_t hdc, int nSavedDC);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/paint.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_PAINT_H_ */
