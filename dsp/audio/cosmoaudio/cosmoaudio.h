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
#define COSMOAUDIO_ABI __attribute__((__ms_abi__, __visibility__("default")))
#else
#define COSMOAUDIO_ABI __attribute__((__visibility__("default")))
#endif
#endif

#define COSMOAUDIO_SUCCESS -0  // no error or nothing written
#define COSMOAUDIO_ERROR   -1  // unspecified error
#define COSMOAUDIO_EINVAL  -2  // invalid parameters passed to api
#define COSMOAUDIO_ELINK   -3  // loading cosmoaudio dso failed
#define COSMOAUDIO_ENOBUF  -4  // invalid buffering parameters

#ifdef __cplusplus
extern "C" {
#endif

struct CosmoAudio;

enum CosmoAudioDeviceType {
  kCosmoAudioDeviceTypePlayback = 1,
  kCosmoAudioDeviceTypeCapture = 2,
  kCosmoAudioDeviceTypeDuplex =
      kCosmoAudioDeviceTypePlayback | kCosmoAudioDeviceTypeCapture,
};

struct CosmoAudioOpenOptions {

  // This field must be set to sizeof(struct CosmoAudioOpenOptions) or
  // cosmoaudio_open() will return COSMOAUDIO_EINVAL.
  int sizeofThis;

  // Whether you want this object to open the speaker or microphone.
  // Please note that asking for microphone access may cause some OSes
  // like MacOS to show a popup asking the user for permission.
  enum CosmoAudioDeviceType deviceType;

  // The sample rate can be 44100 for CD quality, 8000 for telephone
  // quality, etc. Values below 8000 are currently not supported.
  int sampleRate;

  // The number of audio channels in each interleaved frame. Should be 1
  // for mono or 2 for stereo.
  int channels;

  // Number of frames in each ring buffer. A frame consists of a PCM
  // sample for each channel. Set to 0 for default. If this is less than
  // the device period size times two, it'll be increased to that value.
  int bufferFrames;

  // Enables debug logging if non-zero.
  int debugLog;
};

COSMOAUDIO_API int cosmoaudio_version(void) COSMOAUDIO_ABI;

COSMOAUDIO_API int cosmoaudio_open(              //
    struct CosmoAudio **out_ca,                  //
    const struct CosmoAudioOpenOptions *options  //
    ) COSMOAUDIO_ABI;

COSMOAUDIO_API int cosmoaudio_close(  //
    struct CosmoAudio *ca             //
    ) COSMOAUDIO_ABI;

COSMOAUDIO_API int cosmoaudio_write(  //
    struct CosmoAudio *ca,            //
    const float *samples,             //
    int frameCount                    //
    ) COSMOAUDIO_ABI;

COSMOAUDIO_API int cosmoaudio_flush(  //
    struct CosmoAudio *ca             //
    ) COSMOAUDIO_ABI;

COSMOAUDIO_API int cosmoaudio_read(  //
    struct CosmoAudio *ca,           //
    float *out_samples,              //
    int frameCount                   //
    ) COSMOAUDIO_ABI;

COSMOAUDIO_API int cosmoaudio_poll(  //
    struct CosmoAudio *ca,           //
    int *in_out_readFrames,          //
    int *in_out_writeFrames          //
    ) COSMOAUDIO_ABI;

#ifdef __cplusplus
}
#endif

#endif /* COSMOAUDIO_H_ */
