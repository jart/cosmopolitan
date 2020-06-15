#define GetConsoleMode(...) __imp_GetConsoleMode(__VA_ARGS__)
#define SetConsoleCP(...) __imp_SetConsoleCP(__VA_ARGS__)
#define SetConsoleCtrlHandler(...) __imp_SetConsoleCtrlHandler(__VA_ARGS__)
#define SetConsoleMode(...) __imp_SetConsoleMode(__VA_ARGS__)
#define SetConsoleOutputCP(...) __imp_SetConsoleOutputCP(__VA_ARGS__)

extern typeof(GetConsoleMode) *const __imp_GetConsoleMode __msabi;
extern typeof(SetConsoleCP) *const __imp_SetConsoleCP __msabi;
extern typeof(SetConsoleCtrlHandler) *const __imp_SetConsoleCtrlHandler __msabi;
extern typeof(SetConsoleMode) *const __imp_SetConsoleMode __msabi;
extern typeof(SetConsoleOutputCP) *const __imp_SetConsoleOutputCP __msabi;
