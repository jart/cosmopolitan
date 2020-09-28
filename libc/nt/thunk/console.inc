#define GetConsoleMode(...) __imp_GetConsoleMode(__VA_ARGS__)
extern typeof(GetConsoleMode) *const __imp_GetConsoleMode __msabi;

#define SetConsoleCP(...) __imp_SetConsoleCP(__VA_ARGS__)
extern typeof(SetConsoleCP) *const __imp_SetConsoleCP __msabi;

#define GetConsoleCP(...) __imp_GetConsoleCP(__VA_ARGS__)
extern typeof(GetConsoleCP) *const __imp_GetConsoleCP __msabi;

#define SetConsoleCtrlHandler(...) __imp_SetConsoleCtrlHandler(__VA_ARGS__)
extern typeof(SetConsoleCtrlHandler) *const __imp_SetConsoleCtrlHandler __msabi;

#define SetConsoleMode(...) __imp_SetConsoleMode(__VA_ARGS__)
extern typeof(SetConsoleMode) *const __imp_SetConsoleMode __msabi;

#define SetConsoleOutputCP(...) __imp_SetConsoleOutputCP(__VA_ARGS__)
extern typeof(SetConsoleOutputCP) *const __imp_SetConsoleOutputCP __msabi;

#define GetConsoleOutputCP(...) __imp_GetConsoleOutputCP(__VA_ARGS__)
extern typeof(GetConsoleOutputCP) *const __imp_GetConsoleOutputCP __msabi;
