#define CreateFileMappingNuma(...) __imp_CreateFileMappingNumaW(__VA_ARGS__)
#define MapViewOfFileExNuma(...)   __imp_MapViewOfFileExNuma(__VA_ARGS__)
#define FlushViewOfFile(...)       __imp_FlushViewOfFile(__VA_ARGS__)
#define UnmapViewOfFile(...)       __imp_UnmapViewOfFile(__VA_ARGS__)

extern typeof(UnmapViewOfFile) *const __imp_UnmapViewOfFile __msabi;
extern typeof(FlushViewOfFile) *const __imp_FlushViewOfFile __msabi;
extern typeof(MapViewOfFileExNuma) *const __imp_MapViewOfFileExNuma __msabi;
extern typeof(CreateFileMappingNuma) *const
    __imp_CreateFileMappingNumaW __msabi;
