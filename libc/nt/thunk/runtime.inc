#define FreeEnvironmentStrings(...) __imp_FreeEnvironmentStringsW(__VA_ARGS__)
extern typeof(FreeEnvironmentStrings) *const __imp_FreeEnvironmentStringsW
    __msabi;

#define GetCommandLine(...) __imp_GetCommandLineW(__VA_ARGS__)
extern typeof(GetCommandLine) *const __imp_GetCommandLineW __msabi;

#define GetEnvironmentStrings(...) __imp_GetEnvironmentStringsW(__VA_ARGS__)
extern typeof(GetEnvironmentStrings) *const __imp_GetEnvironmentStringsW
    __msabi;

#define GetStdHandle(...) __imp_GetStdHandle(__VA_ARGS__)
extern typeof(GetStdHandle) *const __imp_GetStdHandle __msabi;

#define SetStdHandle(...) __imp_SetStdHandle(__VA_ARGS__)
extern typeof(SetStdHandle) *const __imp_SetStdHandle __msabi;

#define ReadFile(...) __imp_ReadFile(__VA_ARGS__)
extern typeof(ReadFile) *const __imp_ReadFile __msabi;

#define WriteFile(...) __imp_WriteFile(__VA_ARGS__)
extern typeof(WriteFile) *const __imp_WriteFile __msabi;

#define SetDefaultDllDirectories(...) \
  __imp_SetDefaultDllDirectories(__VA_ARGS__)
extern typeof(SetDefaultDllDirectories) *const __imp_SetDefaultDllDirectories
    __msabi;

#define GetModuleFileName(...) __imp_GetModuleFileNameW(__VA_ARGS__)
extern typeof(GetModuleFileName) *const __imp_GetModuleFileNameW __msabi;

extern typeof(GetLastError) *const __imp_GetLastError __msabi;
extern typeof(ExitProcess) *const __imp_ExitProcess __msabi;
