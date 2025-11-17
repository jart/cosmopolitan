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
#include "net/http/apesig.h"
#include "libc/elf/def.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/leb128.h"
#include "libc/intrin/likely.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/stdckdint.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"

// Actually Portable Executable Code Signature File Format
//
// When an APE binary is code signed we store a file named ".sig" in the
// PKZIP container at the end of the executable. It holds binary data we
// can serialize and deserialize using the functions in this module. You
// can see we're using a wire format similar to protocol buffers, that's
// like an OpenSSH private key file, except instead of 32-bit big endian
// prefixes, integers have a variable width LEB encoding. This gives the
// best balance between tiny and fast which is important for APE loading
//
// Parsing the sig file takes 70ns and serializing it takes about 600ns.
// Your sig file contains the output of SerializeApeSigs() which holds a
// struct ApeSig record. When subrecords are encoded, the encoded record
// is prefixed by its size, so that fields can be appended in the future

#define FREE(p) \
  free(p);      \
  p = 0

#define CHECK_ERROR(x)       \
  if (UNLIKELY((err = (x)))) \
  goto OnError

// consumes uint64_t
// variable-length leb encoded word
// safely narrowed to type `w` specifies
#define PARSE_ULEBL(e, p, n, w)               \
  do {                                        \
    uint64_t x;                               \
    int r = unuleb64((const char *)p, n, &x); \
    if (UNLIKELY(r < 0))                      \
      return e;                               \
    if (UNLIKELY(ckd_add(&w, x, 0)))          \
      return e;                               \
    p += r;                                   \
    n -= r;                                   \
  } while (0)

// consumes char*
// 1. search for nul-terminator
// 2. set 's' pointer to beginning borrowed
#define PARSE_CHARS(e, p, n, s)           \
  do {                                    \
    char *t;                              \
    if (UNLIKELY(!(t = memchr(p, 0, n)))) \
      return e;                           \
    size_t m = t - (const char *)p + 1;   \
    s = (const char *)p;                  \
    p += m;                               \
    n -= m;                               \
  } while (0)

// consumes struct ApeBytes
// 1. consume uleb64 byte length
// 2. sets `d.data` to those bytes borrowed
// 3. set `d.size` to byte length
#define PARSE_SLICE(e, p, n, d) \
  do {                          \
    size_t m;                   \
    PARSE_ULEBL(e, p, n, m);    \
    if (UNLIKELY(m > n))        \
      return e;                 \
    d.data = (uint8_t *)p;      \
    d.size = m;                 \
    p += m;                     \
    n -= m;                     \
  } while (0)

void DestroyApeSigSig(struct ApeSigSig *obj) {
}

void DestroyApeSigKey(struct ApeSigKey *obj) {
}

void DestroyApeSigElfPhdr(struct ApeSigElfPhdr *obj) {
  DestroyApeSigSig(&obj->sig);
}

void DestroyApeSigElf(struct ApeSigElf *obj) {
  for (int i = 0; i < obj->phdrs_count; ++i)
    DestroyApeSigElfPhdr(&obj->phdrs[i]);
  free(obj->phdrs);
}

void DestroyApeSig(struct ApeSig *obj) {
  DestroyApeSigKey(&obj->key);
  for (int i = 0; i < obj->elfs_count; ++i)
    DestroyApeSigElf(&obj->elfs[i]);
  free(obj->elfs);
}

void DestroyApeSigs(struct ApeSigs *obj) {
  for (int i = 0; i < obj->sigs_count; ++i)
    DestroyApeSig(&obj->sigs[i]);
  free(obj->sigs);
}

void DestroyApeTrust(struct ApeTrust *obj) {
  DestroyApeSigKey(&obj->key);
}

void DestroyApeTrusts(struct ApeTrusts *obj) {
  for (int i = 0; i < obj->trusts_count; ++i)
    DestroyApeTrust(&obj->trusts[i]);
  free(obj->trusts);
}

int ParseApeSigSig(const uint8_t *p, size_t n, struct ApeSigSig *obj) {
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(140, p, n, obj->algorithm);
  PARSE_SLICE(141, p, n, obj->sig);
  return 0;
}

int ParseApeSigKey(const uint8_t *p, size_t n, struct ApeSigKey *obj) {
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(170, p, n, obj->algorithm);
  PARSE_SLICE(171, p, n, obj->pub);
  return 0;
}

int ParseApeSigElfPhdr(const uint8_t *p, size_t n, struct ApeSigElfPhdr *obj) {
  int err;
  struct ApeBytes subrecord;
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(130, p, n, obj->type);
  PARSE_ULEBL(131, p, n, obj->flags);
  PARSE_SLICE(132, p, n, subrecord);
  if ((err = ParseApeSigSig(subrecord.data, subrecord.size, &obj->sig)))
    return err;
  return 0;
}

int ParseApeSigElf(const uint8_t *p, size_t n, struct ApeSigElf *obj) {
  int err;
  uint8_t phdrs_count;
  struct ApeBytes subrecord;
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(120, p, n, obj->machine);
  PARSE_SLICE(121, p, n, subrecord);
  if ((err = ParseApeSigSig(subrecord.data, subrecord.size, &obj->sig)))
    return err;
  PARSE_ULEBL(122, p, n, phdrs_count);
  if (!(obj->phdrs = malloc(phdrs_count * sizeof(*obj->phdrs))))
    return APESIG_ERR_OOM;
  for (int i = 0; i < phdrs_count; ++i) {
    PARSE_SLICE(123, p, n, subrecord);
    if ((err = ParseApeSigElfPhdr(subrecord.data, subrecord.size,
                                  &obj->phdrs[i]))) {
      DestroyApeSigElfPhdr(&obj->phdrs[i]);
      return err;
    }
    if (ckd_add(&obj->phdrs_count, obj->phdrs_count, 1))
      return 124;
  }
  return 0;
}

int ParseApeSig(const uint8_t *p, size_t n, struct ApeSig *obj) {
  int err;
  uint8_t elfs_count;
  struct ApeBytes subrecord;
  memset(obj, 0, sizeof(*obj));
  PARSE_CHARS(103, p, n, obj->path);
  PARSE_ULEBL(104, p, n, obj->issued);
  PARSE_ULEBL(105, p, n, obj->expires);
  PARSE_CHARS(106, p, n, obj->identity);
  PARSE_CHARS(107, p, n, obj->github);
  PARSE_SLICE(108, p, n, subrecord);
  if ((err = ParseApeSigKey(subrecord.data, subrecord.size, &obj->key)))
    return err;
  PARSE_ULEBL(109, p, n, elfs_count);
  if (!(obj->elfs = malloc(elfs_count * sizeof(*obj->elfs))))
    return APESIG_ERR_OOM;
  for (int i = 0; i < elfs_count; ++i) {
    PARSE_SLICE(116, p, n, subrecord);
    if ((err = ParseApeSigElf(subrecord.data, subrecord.size, &obj->elfs[i]))) {
      DestroyApeSigElf(&obj->elfs[i]);
      return err;
    }
    if (ckd_add(&obj->elfs_count, obj->elfs_count, 1))
      return 115;
  }
  return 0;
}

int ParseApeSigs(const uint8_t *p, size_t n, struct ApeSigs *obj) {
  int err;
  uint16_t sigs_count;
  struct ApeBytes subrecord;
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(110, p, n, obj->magic);
  if (obj->magic != APESIG_MAGIC)
    return APESIG_ERR_MAGIC;
  PARSE_ULEBL(112, p, n, sigs_count);
  if (!(obj->sigs = malloc(sigs_count * sizeof(*obj->sigs))))
    return APESIG_ERR_OOM;
  for (int i = 0; i < sigs_count; ++i) {
    PARSE_SLICE(113, p, n, subrecord);
    if ((err = ParseApeSig(subrecord.data, subrecord.size, &obj->sigs[i]))) {
      DestroyApeSig(&obj->sigs[i]);
      return err;
    }
    if (ckd_add(&obj->sigs_count, obj->sigs_count, 1))
      return 114;
  }
  return 0;
}

int ParseApeTrust(const uint8_t *p, size_t n, struct ApeTrust *obj) {
  int err;
  struct ApeBytes subrecord;
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(163, p, n, obj->issued);
  PARSE_ULEBL(164, p, n, obj->expires);
  PARSE_CHARS(165, p, n, obj->comment);
  PARSE_CHARS(166, p, n, obj->github);
  PARSE_SLICE(167, p, n, subrecord);
  if ((err = ParseApeSigKey(subrecord.data, subrecord.size, &obj->key)))
    return err;
  return 0;
}

int ParseApeTrusts(const uint8_t *p, size_t n, struct ApeTrusts *obj) {
  int e;
  uint32_t trusts_count;
  struct ApeBytes subrecord;
  memset(obj, 0, sizeof(*obj));
  PARSE_ULEBL(150, p, n, obj->magic);
  if (obj->magic != APETRUST_MAGIC)
    return APESIG_ERR_MAGIC;
  PARSE_ULEBL(152, p, n, trusts_count);
  if (!(obj->trusts = malloc(trusts_count * sizeof(*obj->trusts))))
    return APESIG_ERR_OOM;
  for (long i = 0; i < trusts_count; ++i) {
    PARSE_SLICE(154, p, n, subrecord);
    if ((e = ParseApeTrust(subrecord.data, subrecord.size, &obj->trusts[i]))) {
      DestroyApeTrust(&obj->trusts[i]);
      return e;
    }
    if (ckd_add(&obj->trusts_count, obj->trusts_count, 1))
      return 155;
  }
  return 0;
}

static int AppendUlebl(char **b, uint64_t x) {
  char t[10];
  if (UNLIKELY(appendd(b, t, uleb64(t, x) - t) == -1))
    return APESIG_ERR_OOM;
  return 0;
}

static int AppendBytes(char **b, const struct ApeBytes d) {
  int err;
  if (UNLIKELY((err = AppendUlebl(b, d.size))))
    return err;
  if (UNLIKELY(appendd(b, d.data, d.size) == -1))
    return APESIG_ERR_OOM;
  return 0;
}

static int AppendChars(char **b, const char *s) {
  size_t n;
  if (!s)
    s = "";
  n = strlen(s);
  if (UNLIKELY(appendd(b, s, n + 1) == -1))
    return APESIG_ERR_OOM;
  return 0;
}

int SerializeApeSigSig(const struct ApeSigSig *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  CHECK_ERROR(AppendUlebl(&b, obj->algorithm));
  CHECK_ERROR(AppendBytes(&b, obj->sig));
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(b);
  return err;
}

int SerializeApeSigKey(const struct ApeSigKey *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  CHECK_ERROR(AppendUlebl(&b, obj->algorithm));
  CHECK_ERROR(AppendBytes(&b, obj->pub));
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(b);
  return err;
}

int SerializeApeSigElfPhdr(const struct ApeSigElfPhdr *obj,
                           struct ApeBytes *out) {
  int err;
  char *b = 0;
  struct ApeBytes subrecord = {0};
  CHECK_ERROR(AppendUlebl(&b, obj->type));
  CHECK_ERROR(AppendUlebl(&b, obj->flags));
  CHECK_ERROR(SerializeApeSigSig(&obj->sig, &subrecord));
  CHECK_ERROR(AppendBytes(&b, subrecord));
  FREE(subrecord.data);
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(subrecord.data);
  free(b);
  return err;
}

int SerializeApeSigElf(const struct ApeSigElf *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  struct ApeBytes subrecord = {0};
  CHECK_ERROR(AppendUlebl(&b, obj->machine));
  CHECK_ERROR(SerializeApeSigSig(&obj->sig, &subrecord));
  CHECK_ERROR(AppendBytes(&b, subrecord));
  FREE(subrecord.data);
  CHECK_ERROR(AppendUlebl(&b, obj->phdrs_count));
  for (size_t i = 0; i < obj->phdrs_count; ++i) {
    CHECK_ERROR(SerializeApeSigElfPhdr(&obj->phdrs[i], &subrecord));
    CHECK_ERROR(AppendBytes(&b, subrecord));
    FREE(subrecord.data);
  }
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(subrecord.data);
  free(b);
  return err;
}

int SerializeApeSig(const struct ApeSig *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  struct ApeBytes subrecord = {0};
  CHECK_ERROR(AppendChars(&b, obj->path));
  CHECK_ERROR(AppendUlebl(&b, obj->issued));
  CHECK_ERROR(AppendUlebl(&b, obj->expires));
  CHECK_ERROR(AppendChars(&b, obj->identity));
  CHECK_ERROR(AppendChars(&b, obj->github));
  CHECK_ERROR(SerializeApeSigKey(&obj->key, &subrecord));
  CHECK_ERROR(AppendBytes(&b, subrecord));
  FREE(subrecord.data);
  CHECK_ERROR(AppendUlebl(&b, obj->elfs_count));
  for (size_t i = 0; i < obj->elfs_count; ++i) {
    CHECK_ERROR(SerializeApeSigElf(&obj->elfs[i], &subrecord));
    CHECK_ERROR(AppendBytes(&b, subrecord));
    FREE(subrecord.data);
  }
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(subrecord.data);
  free(b);
  return err;
}

int SerializeApeSigs(const struct ApeSigs *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  struct ApeBytes subrecord = {0};
  CHECK_ERROR(AppendUlebl(&b, obj->magic));
  CHECK_ERROR(AppendUlebl(&b, obj->sigs_count));
  for (int i = 0; i < obj->sigs_count; ++i) {
    CHECK_ERROR(SerializeApeSig(&obj->sigs[i], &subrecord));
    CHECK_ERROR(AppendBytes(&b, subrecord));
    FREE(subrecord.data);
  }
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(subrecord.data);
  free(b);
  return err;
}

int SerializeApeTrust(const struct ApeTrust *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  struct ApeBytes subrecord = {0};
  CHECK_ERROR(AppendUlebl(&b, obj->issued));
  CHECK_ERROR(AppendUlebl(&b, obj->expires));
  CHECK_ERROR(AppendChars(&b, obj->comment));
  CHECK_ERROR(AppendChars(&b, obj->github));
  CHECK_ERROR(SerializeApeSigKey(&obj->key, &subrecord));
  CHECK_ERROR(AppendBytes(&b, subrecord));
  FREE(subrecord.data);
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(subrecord.data);
  free(b);
  return err;
}

int SerializeApeTrusts(const struct ApeTrusts *obj, struct ApeBytes *out) {
  int err;
  char *b = 0;
  struct ApeBytes subrecord = {0};
  CHECK_ERROR(AppendUlebl(&b, obj->magic));
  CHECK_ERROR(AppendUlebl(&b, obj->trusts_count));
  for (long i = 0; i < obj->trusts_count; ++i) {
    CHECK_ERROR(SerializeApeTrust(&obj->trusts[i], &subrecord));
    CHECK_ERROR(AppendBytes(&b, subrecord));
    FREE(subrecord.data);
  }
  out->size = appendz(b).i;
  out->data = (uint8_t *)b;
  return 0;
OnError:
  free(subrecord.data);
  free(b);
  return err;
}

static bool IsLegalProtection(int elfprot) {
  switch (elfprot) {
    case PF_R:
    case PF_X:
    case PF_R | PF_X:
    case PF_R | PF_W:
    case PF_R | PF_W | PF_X:
      return true;
    default:
      return false;
  }
}

static bool IsLegalMachine(int machine) {
  switch (machine) {
    case EM_NEXGEN32E:
    case EM_AARCH64:
    case EM_PPC64:
    case EM_RISCV:
    case EM_S390:
      return true;
    default:
      return false;
  }
}

int CheckApeSigSig(const struct ApeSigSig *obj) {
  if (obj->algorithm != APESIG_ALGORITHM_ED25519)
    return APESIG_ERR_ALGORITHM;
  if (obj->sig.size != 64)
    return 230;
  return 0;
}

int CheckApeSigKey(const struct ApeSigKey *obj) {
  if (obj->algorithm != APESIG_ALGORITHM_ED25519)
    return APESIG_ERR_ALGORITHM;
  if (obj->pub.size != 32)
    return 230;
  return 0;
}

int CheckApeSigElfPhdr(const struct ApeSigElfPhdr *obj) {
  int err;
  if (obj->type != PT_LOAD)
    return 240;
  if (!IsLegalProtection(obj->flags & (PF_R | PF_W | PF_X)))
    return 241;
  if ((err = CheckApeSigSig(&obj->sig)))
    return err;
  return 0;
}

int CheckApeSigElf(const struct ApeSigElf *obj) {
  int err;
  if (!IsLegalMachine(obj->machine))
    return 220;
  if ((err = CheckApeSigSig(&obj->sig)))
    return err;
  for (int i = 0; i < obj->phdrs_count; ++i)
    if ((err = CheckApeSigElfPhdr(&obj->phdrs[i])))
      return err;
  return 0;
}

int CheckApeSig(const struct ApeSig *obj) {
  int err;
  if (obj->key.algorithm != APESIG_ALGORITHM_ED25519)
    return 212;
  if (obj->key.pub.size != 32)
    return 213;
  for (int i = 0; i < obj->elfs_count; ++i)
    if ((err = CheckApeSigElf(&obj->elfs[i])))
      return err;
  return 0;
}

int CheckApeSigs(const struct ApeSigs *obj) {
  int err;
  if (obj->magic != APESIG_MAGIC)
    return APESIG_ERR_MAGIC;
  for (int i = 0; i < obj->sigs_count; ++i)
    if ((err = CheckApeSig(&obj->sigs[i])))
      return err;
  return 0;
}

void DescribeApeSigError(char *buf, size_t size, int err) {
  char tmp[32];
  const char *res;
  switch (err) {
    case APESIG_ERR_OOM:
      res = "out of memory";
      break;
    case APESIG_ERR_MAGIC:
      res = "not an ape signature";
      break;
    case APESIG_ERR_ALGORITHM:
      res = "ape signature uses unsupported algorithm";
      break;
    default:
      FormatInt32(stpcpy(tmp, "ape sig error no. "), err);
      res = tmp;
      break;
  }
  strlcpy(buf, res, size);
}
