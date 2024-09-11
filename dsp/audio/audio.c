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
#include "dsp/audio/cosmoaudio/cosmoaudio.h"
#include "dsp/audio/describe.h"
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/dlopen/dlfcn.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/temp.h"
#include "libc/thread/thread.h"

#define COSMOAUDIO_MINIMUM_VERISON 1

#define COSMOAUDIO_DSO_NAME "cosmoaudio." STRINGIFY(COSMOAUDIO_MINIMUM_VERISON)

__static_yoink("dsp/audio/cosmoaudio/miniaudio.h");
__static_yoink("dsp/audio/cosmoaudio/cosmoaudio.h");
__static_yoink("dsp/audio/cosmoaudio/cosmoaudio.c");
__static_yoink("dsp/audio/cosmoaudio/cosmoaudio.dll");

static const struct Source {
  const char *zip;
  const char *name;
} srcs[] = {
    {"/zip/dsp/audio/cosmoaudio/miniaudio.h", "miniaudio.h"},
    {"/zip/dsp/audio/cosmoaudio/cosmoaudio.h", "cosmoaudio.h"},
    {"/zip/dsp/audio/cosmoaudio/cosmoaudio.c", "cosmoaudio.c"},  // must last
};

static struct {
  pthread_once_t once;
  typeof(cosmoaudio_open) *open;
  typeof(cosmoaudio_close) *close;
  typeof(cosmoaudio_write) *write;
  typeof(cosmoaudio_flush) *flush;
  typeof(cosmoaudio_read) *read;
  typeof(cosmoaudio_poll) *poll;
} g_audio;

static const char *cosmoaudio_tmp_dir(void) {
  const char *tmpdir;
  if (!(tmpdir = getenv("TMPDIR")) || !*tmpdir)
    if (!(tmpdir = getenv("HOME")) || !*tmpdir)
      tmpdir = ".";
  return tmpdir;
}

static bool cosmoaudio_app_dir(char *path, size_t size) {
  strlcpy(path, cosmoaudio_tmp_dir(), size);
  strlcat(path, "/.cosmo/", size);
  if (makedirs(path, 0755))
    return false;
  return true;
}

static bool cosmoaudio_dso_path(char *path, size_t size) {
  if (!cosmoaudio_app_dir(path, size))
    return false;
  strlcat(path, COSMOAUDIO_DSO_NAME, size);
  if (IsWindows()) {
    strlcat(path, ".dll", size);
  } else if (IsXnu()) {
    strlcat(path, ".dylib", size);
  } else {
    strlcat(path, ".so", size);
  }
  return true;
}

static bool cosmoaudio_extract(const char *zip, const char *to) {
  int fdin, fdout;
  char stage[PATH_MAX];
  strlcpy(stage, to, sizeof(stage));
  if (strlcat(stage, ".XXXXXX", sizeof(stage)) >= sizeof(stage)) {
    errno = ENAMETOOLONG;
    return false;
  }
  if ((fdout = mkostemp(stage, O_CLOEXEC)) == -1)
    return false;
  if ((fdin = open(zip, O_RDONLY | O_CLOEXEC)) == -1) {
    close(fdout);
    unlink(stage);
    return false;
  }
  if (copyfd(fdin, fdout, -1) == -1) {
    close(fdin);
    close(fdout);
    unlink(stage);
    return false;
  }
  if (close(fdout)) {
    close(fdin);
    unlink(stage);
    return false;
  }
  if (close(fdin)) {
    unlink(stage);
    return false;
  }
  if (rename(stage, to)) {
    unlink(stage);
    return false;
  }
  return true;
}

static bool cosmoaudio_build(const char *dso) {

  // extract sauce
  char src[PATH_MAX];
  for (int i = 0; i < sizeof(srcs) / sizeof(*srcs); ++i) {
    if (!cosmoaudio_app_dir(src, PATH_MAX))
      return false;
    strlcat(src, srcs[i].name, sizeof(src));
    if (!cosmoaudio_extract(srcs[i].zip, src))
      return false;
  }

  // create temporary name for compiled dso
  // it'll ensure build operation is atomic
  int fd;
  char tmpdso[PATH_MAX];
  strlcpy(tmpdso, dso, sizeof(tmpdso));
  strlcat(tmpdso, ".XXXXXX", sizeof(tmpdso));
  if ((fd = mkostemp(tmpdso, O_CLOEXEC)) != -1) {
    close(fd);
  } else {
    return false;
  }

  // build cosmoaudio with host c compiler
  char *args[] = {
      "cc",                                       //
      "-w",                                       //
      "-I.",                                      //
      "-O2",                                      //
      "-fPIC",                                    //
      "-shared",                                  //
      "-pthread",                                 //
      "-DNDEBUG",                                 //
      IsAarch64() ? "-ffixed-x28" : "-DIGNORE1",  //
      src,                                        //
      "-o",                                       //
      tmpdso,                                     //
      "-lm",                                      //
      IsNetbsd() ? 0 : "-ldl",                    //
      NULL,
  };
  int pid, ws;
  errno_t err = posix_spawnp(&pid, args[0], NULL, NULL, args, environ);
  if (err)
    return false;
  while (waitpid(pid, &ws, 0) == -1)
    if (errno != EINTR)
      return false;
  if (ws)
    return false;

  // move dso to its final destination
  if (rename(tmpdso, dso))
    return false;

  return true;
}

static void *cosmoaudio_dlopen(const char *name) {
  void *handle;
  if ((handle = cosmo_dlopen(name, RTLD_NOW))) {
    typeof(cosmoaudio_version) *version;
    if ((version = cosmo_dlsym(handle, "cosmoaudio_version")))
      if (version() >= COSMOAUDIO_MINIMUM_VERISON)
        return handle;
    cosmo_dlclose(handle);
  }
  return 0;
}

static void cosmoaudio_setup_impl(void) {
  void *handle;
  if (IsOpenbsd())
    return;  // no dlopen support yet
  if (IsXnu() && !IsXnuSilicon())
    return;  // no dlopen support yet
  if (!(handle = cosmoaudio_dlopen(COSMOAUDIO_DSO_NAME ".so")) &&
      !(handle = cosmoaudio_dlopen("lib" COSMOAUDIO_DSO_NAME ".so")) &&
      !(handle = cosmoaudio_dlopen("cosmoaudio.so")) &&
      !(handle = cosmoaudio_dlopen("libcosmoaudio.so"))) {
    char dso[PATH_MAX];
    if (!cosmoaudio_dso_path(dso, sizeof(dso)))
      return;
    if ((handle = cosmoaudio_dlopen(dso)))
      goto WeAreGood;
    if (IsWindows()) {
      if (cosmoaudio_extract("/zip/dsp/audio/cosmoaudio/cosmoaudio.dll", dso)) {
        if ((handle = cosmoaudio_dlopen(dso))) {
          goto WeAreGood;
        } else {
          return;
        }
      }
    }
    if (!cosmoaudio_build(dso))
      return;
    if (!(handle = cosmoaudio_dlopen(dso)))
      return;
  }
WeAreGood:
  g_audio.open = cosmo_dlsym(handle, "cosmoaudio_open");
  g_audio.close = cosmo_dlsym(handle, "cosmoaudio_close");
  g_audio.write = cosmo_dlsym(handle, "cosmoaudio_write");
  g_audio.flush = cosmo_dlsym(handle, "cosmoaudio_flush");
  g_audio.read = cosmo_dlsym(handle, "cosmoaudio_read");
  g_audio.poll = cosmo_dlsym(handle, "cosmoaudio_poll");
}

static void cosmoaudio_setup(void) {
  BLOCK_CANCELATION;
  cosmoaudio_setup_impl();
  ALLOW_CANCELATION;
}

static void cosmoaudio_init(void) {
  pthread_once(&g_audio.once, cosmoaudio_setup);
}

COSMOAUDIO_ABI int cosmoaudio_open(
    struct CosmoAudio **out_ca, const struct CosmoAudioOpenOptions *options) {
  int status;
  char sbuf[32];
  char dbuf[256];
  cosmoaudio_init();
  if (g_audio.open) {
    BLOCK_SIGNALS;
    status = g_audio.open(out_ca, options);
    ALLOW_SIGNALS;
  } else {
    status = COSMOAUDIO_ELINK;
  }
  STRACE("cosmoaudio_open([%p], %s) → %s",
         out_ca ? *out_ca : (struct CosmoAudio *)-1,
         cosmoaudio_describe_open_options(dbuf, sizeof(dbuf), options),
         cosmoaudio_describe_status(sbuf, sizeof(sbuf), status));
  return status;
}

COSMOAUDIO_ABI int cosmoaudio_close(struct CosmoAudio *ca) {
  int status;
  char sbuf[32];
  if (g_audio.close) {
    BLOCK_SIGNALS;
    status = g_audio.close(ca);
    ALLOW_SIGNALS;
  } else {
    status = COSMOAUDIO_ELINK;
  }
  STRACE("cosmoaudio_close(%p) → %s", ca,
         cosmoaudio_describe_status(sbuf, sizeof(sbuf), status));
  return status;
}

COSMOAUDIO_ABI int cosmoaudio_write(struct CosmoAudio *ca, const float *data,
                                    int frames) {
  int status;
  char sbuf[32];
  if (g_audio.write) {
    BLOCK_SIGNALS;
    status = g_audio.write(ca, data, frames);
    ALLOW_SIGNALS;
  } else {
    status = COSMOAUDIO_ELINK;
  }
  if (frames <= 0 || frames >= 160)
    DATATRACE("cosmoaudio_write(%p, %p, %d) → %s", ca, data, frames,
              cosmoaudio_describe_status(sbuf, sizeof(sbuf), status));
  return status;
}

COSMOAUDIO_ABI int cosmoaudio_read(struct CosmoAudio *ca, float *data,
                                   int frames) {
  int status;
  char sbuf[32];
  if (g_audio.read) {
    BLOCK_SIGNALS;
    status = g_audio.read(ca, data, frames);
    ALLOW_SIGNALS;
  } else {
    status = COSMOAUDIO_ELINK;
  }
  if (frames <= 0 || frames >= 160)
    DATATRACE("cosmoaudio_read(%p, %p, %d) → %s", ca, data, frames,
              cosmoaudio_describe_status(sbuf, sizeof(sbuf), status));
  return status;
}

COSMOAUDIO_ABI int cosmoaudio_flush(struct CosmoAudio *ca) {
  int status;
  char sbuf[32];
  if (g_audio.flush) {
    BLOCK_SIGNALS;
    status = g_audio.flush(ca);
    ALLOW_SIGNALS;
  } else {
    status = COSMOAUDIO_ELINK;
  }
  DATATRACE("cosmoaudio_flush(%p) → %s", ca,
            cosmoaudio_describe_status(sbuf, sizeof(sbuf), status));
  return status;
}

COSMOAUDIO_ABI int cosmoaudio_poll(struct CosmoAudio *ca,
                                   int *in_out_readFrames,
                                   int *in_out_writeFrames) {
  int status;
  char sbuf[32];
  char fbuf[2][20];
  if (g_audio.poll) {
    BLOCK_SIGNALS;
    status = g_audio.poll(ca, in_out_readFrames, in_out_writeFrames);
    ALLOW_SIGNALS;
  } else {
    status = COSMOAUDIO_ELINK;
  }
  DATATRACE("cosmoaudio_poll(%p, %s, %s) → %s", ca,
            cosmoaudio_describe_poll_frames(fbuf[0], sizeof(fbuf[0]),
                                            in_out_readFrames),
            cosmoaudio_describe_poll_frames(fbuf[1], sizeof(fbuf[1]),
                                            in_out_writeFrames),
            cosmoaudio_describe_status(sbuf, sizeof(sbuf), status));
  return status;
}
