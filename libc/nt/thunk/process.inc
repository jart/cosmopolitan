#define GetEnvironmentVariable(...) __imp_GetEnvironmentVariableW(__VA_ARGS__)
extern typeof(GetEnvironmentVariable) *const __imp_GetEnvironmentVariableW
    __msabi;

#define SetEnvironmentVariable(...) __imp_SetEnvironmentVariableW(__VA_ARGS__)
extern typeof(SetEnvironmentVariable) *const __imp_SetEnvironmentVariableW
    __msabi;

#define GetCurrentProcessId(...) __imp_GetCurrentProcessId(__VA_ARGS__)
extern typeof(GetCurrentProcessId) *const __imp_GetCurrentProcessId __msabi;

extern typeof(FormatMessage) *const __imp_FormatMessageW __msabi;
extern typeof(SetLastError) *const __imp_SetLastError __msabi;
extern typeof(FormatMessage) *const __imp_FormatMessage __msabi;
