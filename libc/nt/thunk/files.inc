#define CopyFile(...) __imp_CopyFileW(__VA_ARGS__)
extern typeof(CopyFile) *const __imp_CopyFileW __msabi;

#define GetFileType(...) __imp_GetFileType(__VA_ARGS__)
extern typeof(GetFileType) *const __imp_GetFileType __msabi;
