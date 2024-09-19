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

#define MA_DEBUG_OUTPUT
#define MA_DR_MP3_NO_STDIO
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_ENGINE
#define MA_NO_GENERATION
#define MA_NO_NODE_GRAPH
#define MA_NO_RESOURCE_MANAGER
#define MA_STATIC

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

struct CosmoAudio {
  enum CosmoAudioDeviceType deviceType;
  ma_uint32 outputBufferFrames;
  ma_uint32 inputBufferFrames;
  int sampleRate;
  int channels;
  int isLeft;
  ma_context context;
  ma_device device;
  ma_pcm_rb output;
  ma_pcm_rb input;
  ma_event event;
  ma_log log;
};

static int read_ring_buffer(ma_log* log, ma_pcm_rb* rb, float* pOutput,
                            ma_uint32 frameCount, ma_uint32 channels) {
  ma_result result;
  ma_uint32 framesRead;
  ma_uint32 framesToRead;
  for (framesRead = 0; framesRead < frameCount; framesRead += framesToRead) {
    framesToRead = frameCount - framesRead;
    void* pMappedBuffer;
    result = ma_pcm_rb_acquire_read(rb, &framesToRead, &pMappedBuffer);
    if (result != MA_SUCCESS) {
      ma_log_postf(log, MA_LOG_LEVEL_WARNING,
                   "ma_pcm_rb_acquire_read failed: %s\n",
                   ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
    if (!framesToRead)
      break;
    memcpy(pOutput + framesRead * channels, pMappedBuffer,
           framesToRead * channels * sizeof(float));
    result = ma_pcm_rb_commit_read(rb, framesToRead);
    if (result != MA_SUCCESS) {
      if (result == MA_AT_END) {
        framesRead += framesToRead;
        break;
      }
      ma_log_postf(log, MA_LOG_LEVEL_WARNING,
                   "ma_pcm_rb_commit_read failed: %s\n",
                   ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
  }
  return framesRead;
}

static int write_ring_buffer(ma_log* log, ma_pcm_rb* rb, const float* pInput,
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
      ma_log_postf(log, MA_LOG_LEVEL_WARNING,
                   "ma_pcm_rb_acquire_write failed: %s\n",
                   ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
    if (!framesToWrite)
      break;
    memcpy(pMappedBuffer, pInput + framesWritten * channels,
           framesToWrite * channels * sizeof(float));
    result = ma_pcm_rb_commit_write(rb, framesToWrite);
    if (result != MA_SUCCESS) {
      if (result == MA_AT_END) {
        framesWritten += framesToWrite;
        break;
      }
      ma_log_postf(log, MA_LOG_LEVEL_WARNING,
                   "ma_pcm_rb_commit_write failed: %s\n",
                   ma_result_description(result));
      return COSMOAUDIO_ERROR;
    }
  }
  return framesWritten;
}

static void data_callback_f32(ma_device* pDevice, float* pOutput,
                              const float* pInput, ma_uint32 frameCount) {
  struct CosmoAudio* ca = (struct CosmoAudio*)pDevice->pUserData;
  if (ca->deviceType & kCosmoAudioDeviceTypePlayback) {
    //
    // "By default, miniaudio will pre-silence the data callback's
    //  output buffer. If you know that you will always write valid data
    //  to the output buffer you can disable pre-silencing by setting
    //  the noPreSilence config option in the device config to true."
    //
    //          —Quoth miniaudio documentation § 16.1. Low Level API
    //
    if (ca->isLeft) {
      int framesCopied = read_ring_buffer(&ca->log, &ca->output, pOutput,
                                          frameCount, ca->channels);
      if (framesCopied < (int)frameCount)
        ca->isLeft = 0;
    } else {
      // TODO(jart): Maybe we should stretch the audio too short?
      int frameOffset;
      int availableFrames = ma_pcm_rb_available_read(&ca->output);
      if (availableFrames >= (int)frameCount) {
        frameOffset = 0;
      } else {
        frameOffset = frameCount - availableFrames;
        frameCount = availableFrames;
      }
      read_ring_buffer(&ca->log, &ca->output,
                       pOutput + frameOffset * ca->channels, frameCount,
                       ca->channels);
      ca->isLeft = 1;
    }
  }
  if (ca->deviceType & kCosmoAudioDeviceTypeCapture)
    write_ring_buffer(&ca->log, &ca->input, pInput, frameCount, ca->channels);
  ma_event_signal(&ca->event);
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
  if (options->bufferFrames < 0)
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
  ca = (struct CosmoAudio*)calloc(1, sizeof(struct CosmoAudio));
  if (!ca)
    return COSMOAUDIO_ERROR;
  ca->channels = options->channels;
  ca->sampleRate = options->sampleRate;
  ca->deviceType = options->deviceType;

  // Create win32-style condition variable.
  if (ma_event_init(&ca->event) != MA_SUCCESS) {
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  // Create audio log.
  if (ma_log_init(NULL, &ca->log) != MA_SUCCESS) {
    ma_event_uninit(&ca->event);
    free(ca);
    return COSMOAUDIO_ERROR;
  }
  if (!options->debugLog)
    ca->log.callbackCount = 0;

  // Create audio context.
  ma_context_config contextConfig = ma_context_config_init();
  contextConfig.pLog = &ca->log;
  if (ma_context_init(NULL, 0, &contextConfig, &ca->context) != MA_SUCCESS) {
    ma_event_uninit(&ca->event);
    ma_log_uninit(&ca->log);
    free(ca);
    return COSMOAUDIO_ERROR;
  }

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
  result = ma_device_init(&ca->context, &deviceConfig, &ca->device);
  if (result != MA_SUCCESS) {
    ma_context_uninit(&ca->context);
    ma_event_uninit(&ca->event);
    ma_log_uninit(&ca->log);
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  // Initialize the speaker ring buffer.
  int period = ca->device.playback.internalPeriodSizeInFrames;
  if (!options->bufferFrames) {
    ca->outputBufferFrames = period * 10;
  } else if (options->bufferFrames < period * 2) {
    ca->outputBufferFrames = period * 2;
  } else {
    ca->outputBufferFrames = options->bufferFrames;
  }
  if (ca->deviceType & kCosmoAudioDeviceTypePlayback) {
    result = ma_pcm_rb_init(ma_format_f32, ca->channels, ca->outputBufferFrames,
                            NULL, NULL, &ca->output);
    if (result != MA_SUCCESS) {
      ma_device_uninit(&ca->device);
      ma_context_uninit(&ca->context);
      ma_event_uninit(&ca->event);
      ma_log_uninit(&ca->log);
      free(ca);
      return COSMOAUDIO_ERROR;
    }
    ma_pcm_rb_set_sample_rate(&ca->output, ca->sampleRate);
  }

  // Initialize the microphone ring buffer.
  period = ca->device.capture.internalPeriodSizeInFrames;
  if (!options->bufferFrames) {
    ca->inputBufferFrames = period * 10;
  } else if (options->bufferFrames < period * 2) {
    ca->inputBufferFrames = period * 2;
  } else {
    ca->inputBufferFrames = options->bufferFrames;
  }
  if (ca->deviceType & kCosmoAudioDeviceTypeCapture) {
    result = ma_pcm_rb_init(ma_format_f32, ca->channels, ca->inputBufferFrames,
                            NULL, NULL, &ca->input);
    if (result != MA_SUCCESS) {
      ma_device_uninit(&ca->device);
      if (ca->deviceType & kCosmoAudioDeviceTypePlayback)
        ma_pcm_rb_uninit(&ca->output);
      ma_context_uninit(&ca->context);
      ma_event_uninit(&ca->event);
      ma_log_uninit(&ca->log);
      free(ca);
      return COSMOAUDIO_ERROR;
    }
    ma_pcm_rb_set_sample_rate(&ca->output, ca->sampleRate);
  }

  // Start audio playback.
  if (ma_device_start(&ca->device) != MA_SUCCESS) {
    ma_device_uninit(&ca->device);
    if (ca->deviceType & kCosmoAudioDeviceTypePlayback)
      ma_pcm_rb_uninit(&ca->output);
    if (ca->deviceType & kCosmoAudioDeviceTypeCapture)
      ma_pcm_rb_uninit(&ca->input);
    ma_context_uninit(&ca->context);
    ma_event_uninit(&ca->event);
    ma_log_uninit(&ca->log);
    free(ca);
    return COSMOAUDIO_ERROR;
  }

  *out_ca = ca;
  return COSMOAUDIO_SUCCESS;
}

/**
 * Closes audio device and frees all associated resources.
 *
 * This function is non-blocking and will drop buffered audio. In
 * playback mode, you need to call cosmoaudio_flush() to ensure data
 * supplied by cosmoaudio_write() gets played on your speaker.
 *
 * Calling this function twice on the same object will result in
 * undefined behavior. Even if this function fails, the `ca` will be
 * freed to the greatest extent possible.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @return 0 on success, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_close(struct CosmoAudio* ca) {
  if (!ca)
    return COSMOAUDIO_EINVAL;
  ma_device_uninit(&ca->device);  // do this first
  if (ca->deviceType & kCosmoAudioDeviceTypePlayback)
    ma_pcm_rb_uninit(&ca->output);
  if (ca->deviceType & kCosmoAudioDeviceTypeCapture)
    ma_pcm_rb_uninit(&ca->input);
  ma_context_uninit(&ca->context);
  ma_event_uninit(&ca->event);
  ma_log_uninit(&ca->log);
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
 * This function never blocks. Programs that don't have their own timer
 * can use cosmoaudio_poll() to wait until audio may be written.
 *
 * For any given CosmoAudio object, it's assumed that only a single
 * thread will call this function.
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
  if (!(ca->deviceType & kCosmoAudioDeviceTypePlayback))
    return COSMOAUDIO_EINVAL;
  if (1u + frames > ca->outputBufferFrames)
    return COSMOAUDIO_ENOBUF;
  if (!frames)
    return 0;
  if (!data)
    return COSMOAUDIO_EINVAL;
  return write_ring_buffer(&ca->log, &ca->output, data, frames, ca->channels);
}

/**
 * Reads raw audio data from microphone.
 *
 * The data is read from a ring buffer in real-time, which is then
 * played back on the audio device. This has tolerence for a certain
 * amount of buffering (based on the `bufferFrames` parameter passed to
 * cosmoaudio_open(), which by default assumes this function will be
 * called at at a regular time interval.
 *
 * This function never blocks. Programs that don't have their own timer
 * can use cosmoaudio_poll() to wait until audio may be read.
 *
 * For any given CosmoAudio object, it's assumed that only a single
 * thread will call this function.
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
  if (!(ca->deviceType & kCosmoAudioDeviceTypeCapture))
    return COSMOAUDIO_EINVAL;
  if (!frames)
    return 0;
  if (!data)
    return COSMOAUDIO_EINVAL;
  return read_ring_buffer(&ca->log, &ca->input, data, frames, ca->channels);
}

/**
 * Waits until it's possible to read/write audio.
 *
 * This function is uninterruptible. All signals are masked throughout
 * the duration of time this function may block, including cancelation
 * signals, because this is not a cancelation point. Cosmopolitan Libc
 * applies this masking in its dlopen wrapper.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @param in_out_readFrames if non-NULL specifies how many frames of
 *     capture data be immediately readable by cosmoaudio_read() before
 *     this can return; it must not exceed the buffer size; on return
 *     this will be set to the actual number of frames in the buffer;
 *     if the caller supplies a zero then this call is a non-blocking
 *     way to query buffer sizes
 * @param in_out_writeFrames if non-NULL specifies how many frames of
 *     capture data be immediately writable by cosmoaudio_write() before
 *     this can return; it must not exceed the buffer size; on return
 *     this will be set to the actual number of frames in the buffer;
 *     if the caller supplies a zero then this call is a non-blocking
 *     way to query buffer sizes
 * @return 0 on success, or negative error code on error
 */
COSMOAUDIO_ABI int cosmoaudio_poll(struct CosmoAudio* ca,
                                   int* in_out_readFrames,
                                   int* in_out_writeFrames) {
  if (!ca)
    return COSMOAUDIO_EINVAL;
  if (!in_out_readFrames && !in_out_writeFrames)
    return COSMOAUDIO_EINVAL;
  if (in_out_readFrames && !(ca->deviceType & kCosmoAudioDeviceTypeCapture))
    return COSMOAUDIO_EINVAL;
  if (in_out_writeFrames && !(ca->deviceType & kCosmoAudioDeviceTypePlayback))
    return COSMOAUDIO_EINVAL;
  if (in_out_readFrames && 1u + *in_out_readFrames > ca->inputBufferFrames)
    return COSMOAUDIO_ENOBUF;
  if (in_out_writeFrames && 1u + *in_out_writeFrames > ca->outputBufferFrames)
    return COSMOAUDIO_ENOBUF;
  for (;;) {
    int done = 1;
    ma_uint32 readable = 0;
    ma_uint32 writable = 0;
    if (in_out_readFrames) {
      readable = ma_pcm_rb_available_read(&ca->input);
      done &= readable >= (ma_uint32)*in_out_readFrames;
    }
    if (in_out_writeFrames) {
      writable = ma_pcm_rb_available_write(&ca->output);
      done &= writable >= (ma_uint32)*in_out_writeFrames;
    }
    if (done) {
      if (in_out_readFrames)
        *in_out_readFrames = readable;
      if (in_out_writeFrames)
        *in_out_writeFrames = writable;
      return COSMOAUDIO_SUCCESS;
    }
    if (ma_event_wait(&ca->event) != MA_SUCCESS)
      return COSMOAUDIO_ERROR;
  }
}

/**
 * Waits for written samples to be sent to device.
 *
 * This function is only valid to call in playback or duplex mode.
 *
 * This function is uninterruptible. All signals are masked throughout
 * the duration of time this function may block, including cancelation
 * signals, because this is not a cancelation point. Cosmopolitan Libc
 * applies this masking in its dlopen wrapper.
 *
 * @param ca is CosmoAudio object returned earlier by cosmoaudio_open()
 * @return 0 on success, or negative error code on failure
 */
COSMOAUDIO_ABI int cosmoaudio_flush(struct CosmoAudio* ca) {
  if (!ca)
    return COSMOAUDIO_EINVAL;
  if (!(ca->deviceType & kCosmoAudioDeviceTypePlayback))
    return COSMOAUDIO_EINVAL;
  for (;;) {
    if (!ma_pcm_rb_available_read(&ca->output))
      return COSMOAUDIO_SUCCESS;
    if (ma_event_wait(&ca->event) != MA_SUCCESS)
      return COSMOAUDIO_ERROR;
  }
}
