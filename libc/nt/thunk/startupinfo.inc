#define GetStartupInfo(...) __imp_GetStartupInfoW(__VA_ARGS__)

extern typeof(GetStartupInfo) *const __imp_GetStartupInfoW __msabi;
