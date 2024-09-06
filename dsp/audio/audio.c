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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/dlopen/dlfcn.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/proc/posix_spawn.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/temp.h"
#include "libc/thread/thread.h"

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
  typeof(cosmoaudio_read) *read;
} g_audio;

static const char *get_tmp_dir(void) {
  const char *tmpdir;
  if (!(tmpdir = getenv("TMPDIR")) || !*tmpdir)
    if (!(tmpdir = getenv("HOME")) || !*tmpdir)
      tmpdir = ".";
  return tmpdir;
}

static bool get_app_dir(char *path, size_t size) {
  strlcpy(path, get_tmp_dir(), size);
  strlcat(path, "/.cosmo/", size);
  if (makedirs(path, 0755))
    return false;
  return true;
}

static bool get_dso_path(char *path, size_t size) {
  if (!get_app_dir(path, size))
    return false;
  strlcat(path, "cosmoaudio", size);
  if (IsWindows()) {
    strlcat(path, ".dll", size);
  } else if (IsXnu()) {
    strlcat(path, ".dylib", size);
  } else {
    strlcat(path, ".so", size);
  }
  return true;
}

static int is_file_newer_than_time(const char *path, const char *other) {
  struct stat st1, st2;
  if (stat(path, &st1))
    // PATH should always exist when calling this function
    return -1;
  if (stat(other, &st2)) {
    if (errno == ENOENT) {
      // PATH should replace OTHER because OTHER doesn't exist yet
      return true;
    } else {
      // some other error happened, so we can't do anything
      return -1;
    }
  }
  // PATH should replace OTHER if PATH was modified more recently
  return timespec_cmp(st1.st_mtim, st2.st_mtim) > 0;
}

static int is_file_newer_than_bytes(const char *path, const char *other) {
  int other_fd;
  if ((other_fd = open(other, O_RDONLY | O_CLOEXEC)) == -1) {
    if (errno == ENOENT) {
      return true;
    } else {
      return -1;
    }
  }
  int path_fd;
  if ((path_fd = open(path, O_RDONLY | O_CLOEXEC)) == -1) {
    close(other_fd);
    return -1;
  }
  int res;
  long i = 0;
  for (;;) {
    char path_buf[512];
    ssize_t path_rc = pread(path_fd, path_buf, sizeof(path_buf), i);
    if (path_rc == -1) {
      res = -1;
      break;
    }
    char other_buf[512];
    ssize_t other_rc = pread(other_fd, other_buf, sizeof(other_buf), i);
    if (other_rc == -1) {
      res = -1;
      break;
    }
    if (!path_rc || !other_rc) {
      if (!path_rc && !other_rc)
        res = false;
      else
        res = true;
      break;
    }
    size_t size = path_rc;
    if (other_rc < path_rc)
      size = other_rc;
    if (memcmp(path_buf, other_buf, size)) {
      res = true;
      break;
    }
    i += size;
  }
  if (close(path_fd))
    res = -1;
  if (close(other_fd))
    res = -1;
  return res;
}

static int is_file_newer_than(const char *path, const char *other) {
  if (startswith(path, "/zip/"))
    // to keep builds deterministic, embedded zip files always have
    // the same timestamp from back in 2022 when it was implemented
    return is_file_newer_than_bytes(path, other);
  else
    return is_file_newer_than_time(path, other);
}

static bool extract(const char *zip, const char *to) {
  int fdin, fdout;
  char stage[PATH_MAX];
  strlcpy(stage, to, sizeof(stage));
  if (strlcat(stage, ".XXXXXX", sizeof(stage)) >= sizeof(stage)) {
    errno = ENAMETOOLONG;
    return false;
  }
  if ((fdout = mkostemp(stage, O_CLOEXEC)) == -1) {
    return false;
  }
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

static bool deploy(const char *dso) {
  switch (is_file_newer_than("/zip/dsp/audio/cosmoaudio/cosmoaudio.dll", dso)) {
    case 0:
      return true;
    case 1:
      return extract("/zip/dsp/audio/cosmoaudio/cosmoaudio.dll", dso);
    default:
      return false;
  }
}

static bool build(const char *dso) {

  // extract source code
  char src[PATH_MAX];
  bool needs_rebuild = false;
  for (int i = 0; i < sizeof(srcs) / sizeof(*srcs); ++i) {
    get_app_dir(src, PATH_MAX);
    strlcat(src, srcs[i].name, sizeof(src));
    switch (is_file_newer_than(srcs[i].zip, src)) {
      case -1:
        return false;
      case 0:
        break;
      case 1:
        needs_rebuild = true;
        if (!extract(srcs[i].zip, src))
          return false;
        break;
      default:
        __builtin_unreachable();
    }
  }

  // determine if we need to build
  if (!needs_rebuild) {
    switch (is_file_newer_than(src, dso)) {
      case -1:
        return false;
      case 0:
        break;
      case 1:
        needs_rebuild = true;
        break;
      default:
        __builtin_unreachable();
    }
  }

  // compile dynamic shared object
  if (needs_rebuild) {
    int fd;
    char tmpdso[PATH_MAX];
    strlcpy(tmpdso, dso, sizeof(tmpdso));
    strlcat(tmpdso, ".XXXXXX", sizeof(tmpdso));
    if ((fd = mkostemp(tmpdso, O_CLOEXEC)) != -1) {
      close(fd);
    } else {
      return false;
    }
    char *args[] = {
        "cc",                                       //
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
        "-ldl",                                     //
        "-lm",                                      //
        NULL,
    };
    int pid, ws;
    errno_t err = posix_spawnp(&pid, "cc", NULL, NULL, args, environ);
    if (err)
      return false;
    while (waitpid(pid, &ws, 0) == -1) {
      if (errno != EINTR)
        return false;
    }
    if (ws)
      return false;
    if (rename(tmpdso, dso))
      return false;
  }

  return true;
}

static void cosmoaudio_setup(void) {
  void *handle;
  if (!(handle = cosmo_dlopen("cosmoaudio.so", RTLD_LOCAL))) {
    if (issetugid())
      return;
    char dso[PATH_MAX];
    if (!get_dso_path(dso, sizeof(dso)))
      return;
    if (IsWindows())
      if (deploy(dso))
        if ((handle = cosmo_dlopen(dso, RTLD_LOCAL)))
          goto WeAreGood;
    if (!build(dso))
      return;
    if (!(handle = cosmo_dlopen(dso, RTLD_LOCAL)))
      return;
  }
WeAreGood:
  g_audio.open = cosmo_dlsym(handle, "cosmoaudio_open");
  g_audio.close = cosmo_dlsym(handle, "cosmoaudio_close");
  g_audio.write = cosmo_dlsym(handle, "cosmoaudio_write");
  g_audio.read = cosmo_dlsym(handle, "cosmoaudio_read");
}

static void cosmoaudio_init(void) {
  pthread_once(&g_audio.once, cosmoaudio_setup);
}

COSMOAUDIO_ABI int cosmoaudio_open(struct CosmoAudio **cap, int sampleRate,
                                   int channels) {
  cosmoaudio_init();
  if (!g_audio.open)
    return COSMOAUDIO_ERROR;
  return g_audio.open(cap, sampleRate, channels);
}

COSMOAUDIO_ABI int cosmoaudio_close(struct CosmoAudio *ca) {
  cosmoaudio_init();
  if (!g_audio.close)
    return COSMOAUDIO_ERROR;
  return g_audio.close(ca);
}

COSMOAUDIO_ABI int cosmoaudio_write(struct CosmoAudio *ca, const float *data,
                                    int frames) {
  cosmoaudio_init();
  if (!g_audio.write)
    return COSMOAUDIO_ERROR;
  return g_audio.write(ca, data, frames);
}

COSMOAUDIO_ABI int cosmoaudio_read(struct CosmoAudio *ca, float *data,
                                   int frames) {
  cosmoaudio_init();
  if (!g_audio.read)
    return COSMOAUDIO_ERROR;
  return g_audio.read(ca, data, frames);
}
