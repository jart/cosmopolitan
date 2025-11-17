/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "dsp/audio/cosmoaudio/cosmoaudio.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/miniaudio/miniaudio.h"

/**
 * @fileoverview audio file player
 *
 * This program provides a command line interface for playing .wav,
 * .mp3, .flac, and .ogg on the system speaker.
 */

volatile int gotsig;

void onsig(int sig) {
  gotsig = sig;
}

void usage(FILE *f) {
  fprintf(f, "usage: %s [-hrv] [-s 44100] [-c 2] FILE...\n",
          program_invocation_short_name);
}

int main(int argc, char *argv[]) {

  // parse flags
  int opt;
  int flag_repeat = 0;
  int flag_verbose = 0;
  int flag_channels = 2;
  int flag_samprate = 44100;
  while ((opt = getopt(argc, argv, "hrvc:s:")) != -1) {
    switch (opt) {
      case 'c':
        flag_channels = atoi(optarg);
        if (flag_channels < 1) {
          fprintf(stderr, "error: invalid channel count\n");
          exit(1);
        }
        break;
      case 's':
        flag_samprate = atoi(optarg);
        if (flag_samprate < 1) {
          fprintf(stderr, "error: invalid sampling rate\n");
          exit(1);
        }
        break;
      case 'v':
        ++flag_verbose;
        break;
      case 'r':
        ++flag_repeat;
        break;
      case 'h':
        usage(stdout);
        exit(0);
      default:
        usage(stderr);
        exit(1);
    }
  }

  // open speaker
  int status;
  struct CosmoAudio *ca;
  struct CosmoAudioOpenOptions cao = {0};
  cao.sizeofThis = sizeof(struct CosmoAudioOpenOptions);
  cao.deviceType = kCosmoAudioDeviceTypePlayback;
  cao.sampleRate = flag_samprate;
  cao.debugLog = flag_verbose > 1;
  cao.channels = flag_channels;
  status = cosmoaudio_open(&ca, &cao);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to open speaker: %d\n", status);
    exit(1);
  }

  // intercept ctrl-c
  signal(SIGINT, onsig);

  // play files
  int exit_code = 0;
  do {
    for (int arg = optind; arg < argc; ++arg) {
      const char *fname = argv[arg];

      if (flag_verbose)
        fprintf(stderr, "playing %s\n", fname);

      // create decoder
      ma_decoder_config decoderConfig =
          ma_decoder_config_init(ma_format_f32, flag_channels, flag_samprate);
      decoderConfig.resampling.algorithm = ma_resample_algorithm_linear;
      decoderConfig.resampling.linear.lpfOrder = 8;

      // open input file
      ma_decoder decoder;
      ma_result rc = ma_decoder_init_file(fname, &decoderConfig, &decoder);
      if (rc != MA_SUCCESS) {
        fprintf(stderr,
                "%s: failed to open audio file: %s (we support "
                ".wav, .mp3, .flac, and .ogg)\n",
                fname, ma_result_description(rc));
        exit_code = 1;
        goto exit;
      }

      // decode audio and transfer to speaker
      while (!gotsig) {
        float buf[512];
        ma_uint64 got = 0;
        ma_uint64 max = sizeof(buf) / sizeof(*buf) / flag_channels;
        ma_decoder_read_pcm_frames(&decoder, buf, max, &got);
        if (!got)
          break;
        int can = got;
        cosmoaudio_poll(ca, NULL, &can);
        cosmoaudio_write(ca, buf, got);
      }

      // cleanup
      ma_decoder_uninit(&decoder);
    }
  } while (!gotsig && flag_repeat);

  // we're done
exit:
  cosmoaudio_flush(ca);
  cosmoaudio_close(ca);
  exit(exit_code);
}
