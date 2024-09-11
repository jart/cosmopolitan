/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/audio/describe.h"
#include "dsp/audio/cosmoaudio/cosmoaudio.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.h"

#define append(...) o += ksnprintf(buf + o, n - o, __VA_ARGS__)

const char *cosmoaudio_describe_status(char *buf, int n, int status) {
  switch (status) {
    case COSMOAUDIO_SUCCESS:
      return "COSMOAUDIO_SUCCESS";
    case COSMOAUDIO_ERROR:
      return "COSMOAUDIO_ERROR";
    case COSMOAUDIO_EINVAL:
      return "COSMOAUDIO_EINVAL";
    case COSMOAUDIO_ELINK:
      return "COSMOAUDIO_ELINK";
    case COSMOAUDIO_ENOBUF:
      return "COSMOAUDIO_ENOBUF";
    default:
      ksnprintf(buf, n, "%d", status);
      return buf;
  }
}

const char *cosmoaudio_describe_open_options(
    char *buf, int n, const struct CosmoAudioOpenOptions *options) {
  int o = 0;
  char b128[128];
  bool gotsome = false;
  if (!options)
    return "NULL";
  if (kisdangerous(options)) {
    ksnprintf(buf, n, "%p", options);
    return buf;
  }
  append("{");

  if (options->sampleRate) {
    if (gotsome)
      append(", ");
    append(".sampleRate=%d", options->sampleRate);
    gotsome = true;
  }

  if (options->channels) {
    if (gotsome)
      append(", ");
    append(".channels=%d", options->channels);
    gotsome = true;
  }

  if (options->deviceType) {
    if (gotsome)
      append(", ");
    static struct DescribeFlags kDeviceType[] = {
        {kCosmoAudioDeviceTypeDuplex, "Duplex"},      //
        {kCosmoAudioDeviceTypeCapture, "Capture"},    //
        {kCosmoAudioDeviceTypePlayback, "Playback"},  //
    };
    append(".deviceType=%s",
           _DescribeFlags(b128, 128, kDeviceType, ARRAYLEN(kDeviceType),
                          "kCosmoAudioDeviceType", options->deviceType));
    gotsome = true;
  }

  if (options->bufferFrames) {
    if (gotsome)
      append(", ");
    append(".bufferFrames=%d", options->bufferFrames);
    gotsome = true;
  }

  if (options->debugLog) {
    if (gotsome)
      append(", ");
    append(".debugLog=%d", options->debugLog);
    gotsome = true;
  }

  if (options->sizeofThis) {
    if (gotsome)
      append(", ");
    append(".sizeofThis=%d", options->sizeofThis);
    gotsome = true;
  }

  append("}");
  return buf;
}

const char *cosmoaudio_describe_poll_frames(char *buf, int n,
                                            int *in_out_frames) {
  if (!in_out_frames)
    return "NULL";
  if (kisdangerous(in_out_frames)) {
    ksnprintf(buf, n, "%p", in_out_frames);
    return buf;
  }
  ksnprintf(buf, n, "[%d]", *in_out_frames);
  return buf;
}
