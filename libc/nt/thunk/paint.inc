#define SetPixel(...) __imp_SetPixel(__VA_ARGS__)
#define GetPixel(...) __imp_GetPixel(__VA_ARGS__)

extern typeof(SetPixel) *const __imp_SetPixel __msabi;
extern typeof(GetPixel) *const __imp_GetPixel __msabi;
