#define GetSystemInfo(...) __imp_GetSystemInfo(__VA_ARGS__)
#define GetTempPath(...) __imp_GetTempPathW(__VA_ARGS__)

extern typeof(GetSystemInfo) *const __imp_GetSystemInfo __msabi;
extern typeof(GetTempPath) *const __imp_GetTempPathW __msabi;
