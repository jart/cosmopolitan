#define GetCurrentThread(...)  __imp_GetCurrentThread(__VA_ARGS__)
#define GetThreadPriority(...) __imp_GetThreadPriority(__VA_ARGS__)
#define SetThreadPriority(...) __imp_SetThreadPriority(__VA_ARGS__)

extern typeof(GetCurrentThread) *const __imp_GetCurrentThread __msabi;
extern typeof(GetThreadPriority) *const __imp_GetThreadPriority __msabi;
extern typeof(SetThreadPriority) *const __imp_SetThreadPriority __msabi;
