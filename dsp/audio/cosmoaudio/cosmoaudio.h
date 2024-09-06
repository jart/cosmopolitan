#ifndef COSMOAUDIO_H_
#define COSMOAUDIO_H_

#ifdef _MSC_VER
#define COSMOAUDIO_ABI
#ifdef COSMOAUDIO_BUILD
#define COSMOAUDIO_API __declspec(dllexport)
#else
#define COSMOAUDIO_API __declspec(dllimport)
#endif
#else
#define COSMOAUDIO_API
#ifdef __x86_64__
#define COSMOAUDIO_ABI __attribute__((__ms_abi__))
#else
#define COSMOAUDIO_ABI
#endif
#endif

#define COSMOAUDIO_SUCCESS 0
#define COSMOAUDIO_ERROR   -1

#ifdef __cplusplus
extern "C" {
#endif

struct CosmoAudio;

COSMOAUDIO_API int cosmoaudio_open(struct CosmoAudio **, int,
                                   int) COSMOAUDIO_ABI;
COSMOAUDIO_API int cosmoaudio_close(struct CosmoAudio *) COSMOAUDIO_ABI;
COSMOAUDIO_API int cosmoaudio_write(struct CosmoAudio *, const float *,
                                    int) COSMOAUDIO_ABI;
COSMOAUDIO_API int cosmoaudio_read(struct CosmoAudio *, float *,
                                   int) COSMOAUDIO_ABI;

#ifdef __cplusplus
}
#endif

#endif /* COSMOAUDIO_H_ */
