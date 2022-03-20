#define FlushViewOfFile(...) __imp_FlushViewOfFile(__VA_ARGS__)
#define UnmapViewOfFile(...) __imp_UnmapViewOfFile(__VA_ARGS__)

extern typeof(LocalFree) *const __imp_LocalFree __msabi;
extern typeof(UnmapViewOfFile) *const __imp_UnmapViewOfFile __msabi;
extern typeof(FlushViewOfFile) *const __imp_FlushViewOfFile __msabi;
