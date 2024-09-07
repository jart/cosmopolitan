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

#ifdef __cplusplus
extern "C" {
#endif

struct CosmoAudio;

typedef void cosmoaudio_data_callback_f(  //
    struct CosmoAudio *ca,                //
    float *outputSamples,                 //
    const float *inputSamples,            //
    int frameCount,                       //
    int channels,                         //
    void *argument);

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

  // Number of periods in ring buffer. Set to 0 for default. Higher
  // numbers (e.g. 20) means more buffering. Lower numbers (e.g. 2)
  // means less buffering. This is ignored if callback is specified.
  int periods;

  // If callback is NULL, then cosmoaudio_write() and cosmoaudio_read()
  // should be used, which ring buffer audio to the default internal
  // routine. Setting this callback to non-NULL puts CosmoAudio in
  // manual mode, where the callback is responsible for copying PCM
  // samples each time the device calls this.
  cosmoaudio_data_callback_f *dataCallback;

  // This is an arbitrary value passed to the callback.
  void *argument;
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

COSMOAUDIO_API int cosmoaudio_read(  //
    struct CosmoAudio *ca,           //
    float *out_samples,              //
    int frameCount                   //
    ) COSMOAUDIO_ABI;

#ifdef __cplusplus
}
#endif

#endif /* COSMOAUDIO_H_ */
