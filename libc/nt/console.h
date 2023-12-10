#ifndef COSMOPOLITAN_LIBC_NT_CONSOLE_H_
#define COSMOPOLITAN_LIBC_NT_CONSOLE_H_
#include "libc/nt/struct/charinfo.h"
#include "libc/nt/struct/consolecursorinfo.h"
#include "libc/nt/struct/consolescreenbufferinfo.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/nt/struct/consoleselectioninfo.h"
#include "libc/nt/struct/coord.h"
#include "libc/nt/struct/inputrecord.h"
#include "libc/nt/struct/smallrect.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/typedef/handlerroutine.h"
/*                            ░░░░
                       ▒▒▒░░░▒▒▒▒▒▒▒▓▓▓░
                      ▒▒▒▒░░░▒▒▒▒▒▒▓▓▓▓▓▓░
                     ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓░
                     ▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   ▓▓▓▓▓▓▒        ▒▒▒▓▓█
                    ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                   ░▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   █▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                   ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓░  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                  ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒
                  ▒▒▒▒▓▓      ▓▒▒▓▓▓▓   ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                                   ▒▓  ▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓
                  ░░░░░░░░░░░▒▒▒▒      ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                ▒▒░░░░░░░░░░▒▒▒▒▒▓▓▓     ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
               ░▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓░      ░▓███▓
               ▒▒░░░░░░░░░░▒▒▒▒▒▓▓░  ▒▓▓▓▒▒▒         ░▒▒▒▓   ████████████
              ▒▒░░░░░░░░░░░▒▒▒▒▒▓▓  ▒▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒░           ░███
              ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓            ███
             ▒▒░░░░░░░░░░▒▒▒▒▒▒▓▓  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒            ▓██
             ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒▓           ▓██
            ▒▒░░░▒▒▒░░░▒▒░▒▒▒▓▓▒  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒           ███
                            ░▒▓  ░▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓          ▓██
╔────────────────────────────────────────────────────────────────▀▀▀─────────│─╗
│ cosmopolitan § new technology » console                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kNtAttachParentProcess -1u

COSMOPOLITAN_C_START_

bool32 WriteConsoleOutput(int64_t hConsoleOutput,
                          const struct NtCharInfo *lpBuffer,
                          struct NtCoord dwBufferSize,
                          struct NtCoord dwBufferCoord,
                          struct NtSmallRect *lpWriteRegion);
bool32 ReadConsoleInput(int64_t hConsoleInput, struct NtInputRecord *lpBuffer,
                        uint32_t nLength, uint32_t *lpNumberOfEventsRead);
bool32 PeekConsoleInput(int64_t hConsoleInput, struct NtInputRecord *lpBuffer,
                        uint32_t nLength, uint32_t *lpNumberOfEventsRead);
bool32 GetNumberOfConsoleInputEvents(int64_t hConsoleInput,
                                     uint32_t *lpNumberOfEvents);
bool32 ReadConsoleOutput(int64_t hConsoleOutput, struct NtCharInfo *lpBuffer,
                         struct NtCoord dwBufferSize,
                         struct NtCoord dwBufferCoord,
                         struct NtSmallRect *lpReadRegion);
bool32 WriteConsoleInput(int64_t hConsoleInput,
                         const struct NtInputRecord *lpBuffer, uint32_t nLength,
                         uint32_t *lpNumberOfEventsWritten);
bool32 FlushConsoleInputBuffer(int64_t hConsoleInput);

int64_t GetConsoleWindow(void);
bool32 GetConsoleMode(int64_t hConsoleHandle, uint32_t *lpMode);
bool32 SetConsoleMode(int64_t hConsoleHandle, uint32_t dwMode);
int32_t AllocConsole(void);
int32_t FreeConsole(void);
int32_t AttachConsole(uint32_t dwProcessId);
uint32_t GetConsoleTitle(char16_t *lpConsoleTitle, uint32_t nSize);
int32_t SetConsoleTitle(const char16_t *lpConsoleTitle);
bool32 GetConsoleScreenBufferInfo(
    int64_t hConsoleOutput,
    struct NtConsoleScreenBufferInfo *out_lpConsoleScreenBufferInfo);
bool32 GetConsoleScreenBufferInfoEx(
    int64_t hConsoleOutput,
    struct NtConsoleScreenBufferInfoEx *in_out_lpConsoleScreenBufferInfo);
bool32 SetConsoleScreenBufferInfoEx(
    int64_t hConsoleOutput,
    const struct NtConsoleScreenBufferInfoEx *lpConsoleScreenBufferInfo);
bool32 SetConsoleScreenBufferSize(int64_t lpConsoleOutput,
                                  struct NtCoord dwSize);
struct NtCoord GetLargestConsoleWindowSize(int64_t hConsoleHandle);
int32_t ReadConsole(int64_t hConsoleInput, void *lpBuffer,
                    uint32_t nNumberOfCharsToRead,
                    uint32_t *lpNumberOfCharsRead, void *lpReserved);
int32_t WriteConsole(int64_t hConsoleOutput, const void *lpBuffer,
                     uint32_t nNumberOfCharsToWrite,
                     uint32_t *lpNumberOfCharsWritten, void *lpReserved)
    paramsnonnull((2, 4));
bool32 GetNumberOfConsoleMouseButtons(uint32_t *out_lpNumberOfMouseButtons)
    paramsnonnull();
bool32 GetConsoleSelectionInfo(
    struct NtConsoleSelectionInfo *out_lpConsoleSelectionInfo);
uint32_t WaitForInputIdle(int64_t hProcess, uint32_t dwMilliseconds);
uint32_t GetConsoleCP(void);
bool32 SetConsoleCP(uint32_t wCodePageID);
bool32 SetConsoleOutputCP(uint32_t wCodePageID);
uint32_t GetConsoleOutputCP(void);
bool32 SetConsoleCtrlHandler(NtHandlerRoutine opt_HandlerRoutine, bool32 Add);
bool32 GenerateConsoleCtrlEvent(uint32_t dwCtrlEvent,
                                uint32_t dwProcessGroupId);

bool32 GetConsoleCursorInfo(
    int64_t hConsoleOutput,
    struct NtConsoleCursorInfo *out_lpConsoleCursorInfo);
bool32 SetConsoleCursorInfo(
    int64_t hConsoleOutput,
    const struct NtConsoleCursorInfo *lpConsoleCursorInfo);

long CreatePseudoConsole(struct NtCoord size, int64_t hInput, int64_t hOutput,
                         uint32_t dwFlags, int64_t *out_phPC);
long ResizePseudoConsole(int64_t hPC, struct NtCoord size);
void ClosePseudoConsole(int64_t hPC);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/console.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_CONSOLE_H_ */
