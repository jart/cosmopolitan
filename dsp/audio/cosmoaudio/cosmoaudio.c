#define COSMOAUDIO_BUILD
#include "cosmoaudio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MA_STATIC
#define MA_NO_DECODING
#define MA_NO_ENCODING
#ifdef NDEBUG
#define MA_DR_MP3_NO_STDIO
#endif
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#ifndef NDEBUG
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

struct CosmoAudio {
  ma_device device;
  ma_pcm_rb input;
  ma_pcm_rb output;
  ma_uint32 sampleRate;
  ma_uint32 channels;
};

static int read_ring_buffer(ma_pcm_rb* rb, float* pOutput, ma_uint32 frameCount,
                            ma_uint32 channels) {
  ma_result result;
  ma_uint32 framesRead;
  ma_uint32 framesToRead;
  for (framesRead = 0; framesRead < frameCount; framesRead += framesToRead) {
    framesToRead = frameCount - framesRead;
    void* pMappedBuffer;
    result = ma_pcm_rb_acquire_read(rb, &framesToRead, &pMappedBuffer);
    if (result != MA_SUCCESS) {
      LOG("ma_pcm_rb_acquire_read failed: %s\n", ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
    if (!framesToRead)
      break;
    memcpy(pOutput + framesRead * channels, pMappedBuffer,
           framesToRead * channels * sizeof(float));
    result = ma_pcm_rb_commit_read(rb, framesToRead);
    if (result != MA_SUCCESS) {
      if (result == MA_AT_END)
        break;
      LOG("ma_pcm_rb_commit_read failed: %s\n", ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
  }
  return framesRead;
}

static int write_ring_buffer(ma_pcm_rb* rb, const float* pInput,
                             ma_uint32 frameCount, ma_uint32 channels) {
  ma_result result;
  ma_uint32 framesWritten;
  ma_uint32 framesToWrite;
  for (framesWritten = 0; framesWritten < frameCount;
       framesWritten += framesToWrite) {
    framesToWrite = frameCount - framesWritten;
    void* pMappedBuffer;
    result = ma_pcm_rb_acquire_write(rb, &framesToWrite, &pMappedBuffer);
    if (result != MA_SUCCESS) {
      LOG("ma_pcm_rb_acquire_write failed: %s\n",
          ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
    if (!framesToWrite)
      break;
    memcpy(pMappedBuffer, pInput + framesWritten * channels,
           framesToWrite * channels * sizeof(float));
    result = ma_pcm_rb_commit_write(rb, framesToWrite);
    if (result != MA_SUCCESS) {
      if (result == MA_AT_END)
        break;
      LOG("ma_pcm_rb_commit_write failed: %s\n", ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
  }
  return framesWritten;
}

static void data_callback_f32(ma_device* pDevice, float* pOutput,
                              const float* pInput, ma_uint32 frameCount) {
  struct CosmoAudio* ca = (struct CosmoAudio*)pDevice->pUserData;
  read_ring_buffer(&ca->output, pOutput, frameCount, ca->channels);
  write_ring_buffer(&ca->input, pInput, frameCount, ca->channels);
}

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput,
                          ma_uint32 frameCount) {
  data_callback_f32(pDevice, (float*)pOutput, (const float*)pInput, frameCount);
}

/**
 * Opens access to speaker and microphone.
 *
 * @param cap will receive pointer to allocated CosmoAudio object on success,
 *     which must be freed by caller with cosmoaudio_close()
 * @param sampleRate is sample rate in Hz, e.g. 44100
 * @param channels is number of channels (1 for mono, 2 for stereo)
 * @return 0 on success, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_open(struct CosmoAudio** cap, int sampleRate,
                                   int channels) {

  // Allocate cosmo audio object.
  struct CosmoAudio* ca;
  if (!(ca = (struct CosmoAudio*)malloc(sizeof(struct CosmoAudio))))
    return COSMOAUDIO_ERROR;
  ca->channels = channels;
  ca->sampleRate = sampleRate;

  // Initialize device.
  ma_result result;
  ma_device_config deviceConfig = ma_device_config_init(ma_device_type_duplex);
  deviceConfig.sampleRate = sampleRate;
  deviceConfig.capture.channels = channels;
  deviceConfig.capture.format = ma_format_f32;
  deviceConfig.capture.shareMode = ma_share_mode_shared;
  deviceConfig.playback.channels = channels;
  deviceConfig.playback.format = ma_format_f32;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = ca;
  result = ma_device_init(NULL, &deviceConfig, &ca->device);
  if (result != MA_SUCCESS) {
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  // Initialize the speaker ring buffer.
  result = ma_pcm_rb_init(ma_format_f32, channels,
                          ca->device.playback.internalPeriodSizeInFrames * 10,
                          NULL, NULL, &ca->output);
  if (result != MA_SUCCESS) {
    ma_device_uninit(&ca->device);
    free(ca);
    return COSMOAUDIO_ERROR;
  }
  ma_pcm_rb_set_sample_rate(&ca->output, sampleRate);

  // Initialize the microphone ring buffer.
  result = ma_pcm_rb_init(ma_format_f32, channels,
                          ca->device.capture.internalPeriodSizeInFrames * 10,
                          NULL, NULL, &ca->input);
  if (result != MA_SUCCESS) {
    ma_pcm_rb_uninit(&ca->output);
    ma_device_uninit(&ca->device);
    free(ca);
    return COSMOAUDIO_ERROR;
  }
  ma_pcm_rb_set_sample_rate(&ca->output, sampleRate);

  // Start audio playback.
  if (ma_device_start(&ca->device) != MA_SUCCESS) {
    ma_pcm_rb_uninit(&ca->input);
    ma_pcm_rb_uninit(&ca->output);
    ma_device_uninit(&ca->device);
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  *cap = ca;
  return COSMOAUDIO_SUCCESS;
}

/**
 * Closes audio device and frees all associated resources.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @return 0 on success, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_close(struct CosmoAudio* ca) {
  ma_device_uninit(&ca->device);
  ma_pcm_rb_uninit(&ca->output);
  free(ca);
  return COSMOAUDIO_SUCCESS;
}

/**
 * Writes raw audio data to speaker.
 *
 * The data is written to a ring buffer in real-time, which is then
 * played back very soon on the audio device. This has tolerence for
 * a certain amount of buffering, but expects that this function is
 * repeatedly called at a regular time interval. The caller should
 * have its own sleep loop for this purpose.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @param data is pointer to raw audio samples, expected to be in the range
 *     -1.0 to 1.0, where channels are interleaved
 * @param frames is the number of frames (i.e. number of samples divided by
 *     number of channels) from `data` to write to audio device
 * @return number of frames written, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_write(struct CosmoAudio* ca, const float* data,
                                    int frames) {
  return write_ring_buffer(&ca->output, data, frames, ca->channels);
}

/**
 * Reads raw audio data from microphone.
 *
 * The data is read from a ring buffer in real-time, which is then
 * played back very soon on the audio device. This has tolerence for
 * a certain amount of buffering, but expects that this function is
 * repeatedly called at a regular time interval. The caller should
 * have its own sleep loop for this purpose.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @param data is pointer to raw audio samples, expected to be in the range
 *     -1.0 to 1.0, where channels are interleaved
 * @param frames is the number of frames (i.e. number of samples divided by
 *     number of channels) from `data` to read from microphone
 * @return number of frames read, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_read(struct CosmoAudio* ca, float* data,
                                   int frames) {
  int read;
  for (int i = 0; i < frames; i += read) {
    int remaining = frames - i;
    read = read_ring_buffer(&ca->input, data + i * ca->channels, remaining,
                            ca->channels);
    if (read < 0)
      return read;
  }
  return frames;
}

#ifdef _MSC_VER
#include <Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
  (void)hModule;
  (void)lpReserved;
  (void)ul_reason_for_call;
}
#endif
