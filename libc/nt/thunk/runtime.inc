#define ExitProcess(...)            __imp_ExitProcess(__VA_ARGS__)
#define FreeEnvironmentStrings(...) __imp_FreeEnvironmentStringsW(__VA_ARGS__)
#define GetCommandLine(...)         __imp_GetCommandLineW(__VA_ARGS__)
#define GetEnvironmentStrings(...)  __imp_GetEnvironmentStringsW(__VA_ARGS__)
#define GetStdHandle(...)           __imp_GetStdHandle(__VA_ARGS__)
#define SetStdHandle(...)           __imp_SetStdHandle(__VA_ARGS__)
#define ReadFile(...)               __imp_ReadFile(__VA_ARGS__)
#define WriteFile(...)              __imp_WriteFile(__VA_ARGS__)
#define SetDefaultDllDirectories(...) \
  __imp_SetDefaultDllDirectories(__VA_ARGS__)

extern typeof(ExitProcess) *const __imp_ExitProcess __msabi;
extern typeof(FreeEnvironmentStrings) *const
    __imp_FreeEnvironmentStringsW __msabi;
extern typeof(GetCommandLine) *const __imp_GetCommandLineW __msabi;
extern typeof(GetEnvironmentStrings) *const
    __imp_GetEnvironmentStringsW __msabi;
extern typeof(GetStdHandle) *const __imp_GetStdHandle __msabi;
extern typeof(SetStdHandle) *const __imp_SetStdHandle __msabi;
extern typeof(ReadFile) *const __imp_ReadFile __msabi;
extern typeof(WriteFile) *const __imp_WriteFile __msabi;
extern typeof(SetDefaultDllDirectories) *const
    __imp_SetDefaultDllDirectories __msabi;
