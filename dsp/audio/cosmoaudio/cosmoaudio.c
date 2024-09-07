// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#define COSMOAUDIO_BUILD
#include "cosmoaudio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MA_STATIC
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_ENGINE
#define MA_NO_GENERATION
#define MA_NO_NODE_GRAPH
#define MA_NO_RESOURCE_MANAGER
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
  ma_uint32 periods;
  enum CosmoAudioDeviceType deviceType;
  cosmoaudio_data_callback_f* dataCallback;
  void* argument;
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
  if (ca->dataCallback) {
    ca->dataCallback(ca, pOutput, pInput, frameCount, ca->channels,
                     ca->argument);
  } else {
    if (ca->deviceType & kCosmoAudioDeviceTypePlayback)
      read_ring_buffer(&ca->output, pOutput, frameCount, ca->channels);
    if (ca->deviceType & kCosmoAudioDeviceTypeCapture)
      write_ring_buffer(&ca->input, pInput, frameCount, ca->channels);
  }
}

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput,
                          ma_uint32 frameCount) {
  data_callback_f32(pDevice, (float*)pOutput, (const float*)pInput, frameCount);
}

/**
 * Returns current version of cosmo audio library.
 */
COSMOAUDIO_ABI int cosmoaudio_version(void) {
  return 1;
}

/**
 * Opens access to speaker and microphone.
 *
 * @param out_ca will receive pointer to allocated CosmoAudio object,
 *     which must be freed by caller with cosmoaudio_close(); if this
 *     function fails, then this will receive a NULL pointer value so
 *     that cosmoaudio_close(), cosmoaudio_write() etc. can be called
 *     without crashing if no error checking is performed
 * @return 0 on success, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_open(  //
    struct CosmoAudio** out_ca,      //
    const struct CosmoAudioOpenOptions* options) {

  // Validate arguments.
  if (!out_ca)
    return COSMOAUDIO_EINVAL;
  *out_ca = NULL;
  if (!options)
    return COSMOAUDIO_EINVAL;
  if (options->sizeofThis < (int)sizeof(struct CosmoAudioOpenOptions))
    return COSMOAUDIO_EINVAL;
  if (options->periods < 0)
    return COSMOAUDIO_EINVAL;
  if (options->sampleRate < 8000)
    return COSMOAUDIO_EINVAL;
  if (options->channels < 1)
    return COSMOAUDIO_EINVAL;
  if (!options->deviceType)
    return COSMOAUDIO_EINVAL;
  if (options->deviceType &
      ~(kCosmoAudioDeviceTypePlayback | kCosmoAudioDeviceTypeCapture))
    return COSMOAUDIO_EINVAL;

  // Allocate cosmo audio object.
  struct CosmoAudio* ca;
  if (!(ca = (struct CosmoAudio*)calloc(1, sizeof(struct CosmoAudio))))
    return COSMOAUDIO_ERROR;
  ca->channels = options->channels;
  ca->sampleRate = options->sampleRate;
  ca->deviceType = options->deviceType;
  ca->periods = options->periods ? options->periods : 10;
  ca->dataCallback = options->dataCallback;
  ca->argument = options->argument;

  // Initialize device.
  ma_result result;
  ma_device_config deviceConfig;
  deviceConfig = ma_device_config_init(ca->deviceType);
  deviceConfig.sampleRate = ca->sampleRate;
  if (ca->deviceType & kCosmoAudioDeviceTypeCapture) {
    deviceConfig.capture.channels = ca->channels;
    deviceConfig.capture.format = ma_format_f32;
    deviceConfig.capture.shareMode = ma_share_mode_shared;
  }
  if (ca->deviceType & kCosmoAudioDeviceTypePlayback) {
    deviceConfig.playback.channels = ca->channels;
    deviceConfig.playback.format = ma_format_f32;
  }
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = ca;
  result = ma_device_init(NULL, &deviceConfig, &ca->device);
  if (result != MA_SUCCESS) {
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  // Initialize the speaker ring buffer.
  if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypePlayback)) {
    result = ma_pcm_rb_init(
        ma_format_f32, ca->channels,
        ca->device.playback.internalPeriodSizeInFrames * ca->periods, NULL,
        NULL, &ca->output);
    if (result != MA_SUCCESS) {
      ma_device_uninit(&ca->device);
      free(ca);
      return COSMOAUDIO_ERROR;
    }
    ma_pcm_rb_set_sample_rate(&ca->output, ca->sampleRate);
  }

  // Initialize the microphone ring buffer.
  if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypeCapture)) {
    result = ma_pcm_rb_init(
        ma_format_f32, ca->channels,
        ca->device.capture.internalPeriodSizeInFrames * ca->periods, NULL, NULL,
        &ca->input);
    if (result != MA_SUCCESS) {
      if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypePlayback))
        ma_pcm_rb_uninit(&ca->output);
      ma_device_uninit(&ca->device);
      free(ca);
      return COSMOAUDIO_ERROR;
    }
    ma_pcm_rb_set_sample_rate(&ca->output, ca->sampleRate);
  }

  // Start audio playback.
  if (ma_device_start(&ca->device) != MA_SUCCESS) {
    if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypeCapture))
      ma_pcm_rb_uninit(&ca->input);
    if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypePlayback))
      ma_pcm_rb_uninit(&ca->output);
    ma_device_uninit(&ca->device);
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  *out_ca = ca;
  return COSMOAUDIO_SUCCESS;
}

/**
 * Closes audio device and frees all associated resources.
 *
 * Calling this function twice on the same object will result in
 * undefined behavior.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @return 0 on success, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_close(struct CosmoAudio* ca) {
  if (!ca)
    return COSMOAUDIO_EINVAL;
  ma_device_uninit(&ca->device);
  if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypePlayback))
    ma_pcm_rb_uninit(&ca->output);
  if (!ca->dataCallback && (ca->deviceType & kCosmoAudioDeviceTypeCapture))
    ma_pcm_rb_uninit(&ca->input);
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
  if (!ca)
    return COSMOAUDIO_EINVAL;
  if (frames < 0)
    return COSMOAUDIO_EINVAL;
  if (ca->dataCallback)
    return COSMOAUDIO_EINVAL;
  if (!(ca->deviceType & kCosmoAudioDeviceTypePlayback))
    return COSMOAUDIO_EINVAL;
  if (!frames)
    return 0;
  if (!data)
    return COSMOAUDIO_EINVAL;
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
  if (!ca)
    return COSMOAUDIO_EINVAL;
  if (frames < 0)
    return COSMOAUDIO_EINVAL;
  if (ca->dataCallback)
    return COSMOAUDIO_EINVAL;
  if (!(ca->deviceType & kCosmoAudioDeviceTypeCapture))
    return COSMOAUDIO_EINVAL;
  if (!frames)
    return 0;
  if (!data)
    return COSMOAUDIO_EINVAL;
  return read_ring_buffer(&ca->input, data, frames, ca->channels);
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
