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
#include "net/http/ssh.h"
#include "libc/ctype.h"
#include "libc/fmt/itoa.h"
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "net/http/escape.h"

#define OPENSSH_KEY_V1            "openssh-key-v1"
#define BEGIN_OPENSSH_PRIVATE_KEY "-----BEGIN OPENSSH PRIVATE KEY-----\n"
#define END_OPENSSH_PRIVATE_KEY   "-----END OPENSSH PRIVATE KEY-----"

#define SSH_PARSE_U32BE(e, p, n, m) \
  if (n < 4)                        \
    return e;                       \
  m = READ32BE(p);                  \
  p += 4;                           \
  n -= 4

#define SSH_PARSE_BYTES(e, p, n, m, a) \
  if (m > n)                           \
    return e;                          \
  if (!(a = malloc(m + 1)))            \
    return SSH_ERR_OUT_OF_MEMORY;      \
  if (m)                               \
    memcpy(a, p, m);                   \
  a[m] = 0;                            \
  p += m;                              \
  n -= m

void DestroyOpensshPublicKey(struct OpensshPublicKey *key) {
  free(key->algo);
  free(key->data);
  free(key->comment);
}

static void DestroyOpensshPrivateKey(struct OpensshPrivateKey *key) {
  DestroyOpensshPublicKey(&key->pub);
  explicit_bzero(key->data, key->size);
  free(key->data);
  free(key->comment);
}

static void DestroyOpensshKey(struct OpensshKey *key) {
  DestroyOpensshPublicKey(&key->pub);
  DestroyOpensshPrivateKey(&key->key);
}

void DestroyOpensshKeyV1(struct OpensshKeyV1 *key) {
  free(key->cipher);
  free(key->kdfname);
  free(key->kdfparams);
  for (size_t i = 0; i < key->keycount; ++i)
    DestroyOpensshKey(&key->keys[i]);
  free(key->keys);
}

void DestroyOpensshKnownHost(struct OpensshKnownHost *obj) {
  free(obj->host);
  DestroyOpensshPublicKey(&obj->pub);
}

void DestroyOpensshKnownHosts(struct OpensshKnownHosts *obj) {
  for (size_t i = 0; i < obj->n; ++i)
    DestroyOpensshKnownHost(&obj->p[i]);
  free(obj->p);
}

static int ParseOpensshPublicKeyBinary(const char *p, size_t n,
                                       struct OpensshPublicKey *key) {
  size_t m;
  memset(key, 0, sizeof(*key));
  SSH_PARSE_U32BE(100, p, n, m);
  SSH_PARSE_BYTES(101, p, n, m, key->algo);
  SSH_PARSE_U32BE(102, p, n, m);
  SSH_PARSE_BYTES(103, p, n, m, key->data);
  key->size = m;
  return 0;
}

static int ParseOpensshPrivateKey(const char *p, size_t n,
                                  struct OpensshPrivateKey *key) {
  size_t m;
  memset(key, 0, sizeof(*key));
  SSH_PARSE_U32BE(110, p, n, key->rng1);
  SSH_PARSE_U32BE(111, p, n, key->rng2);
  if (key->rng1 != key->rng2)
    return SSH_ERR_KEY_CORRUPTED;
  SSH_PARSE_U32BE(112, p, n, m);
  SSH_PARSE_BYTES(113, p, n, m, key->pub.algo);
  SSH_PARSE_U32BE(114, p, n, m);
  SSH_PARSE_BYTES(115, p, n, m, key->pub.data);
  key->pub.size = m;
  SSH_PARSE_U32BE(116, p, n, m);
  SSH_PARSE_BYTES(117, p, n, m, key->data);
  key->size = m;
  SSH_PARSE_U32BE(118, p, n, m);
  SSH_PARSE_BYTES(119, p, n, m, key->comment);
  return 0;
}

static int ParseOpensshKeyV1Impl(const char *p, size_t n,
                                 struct OpensshKeyV1 *key) {
  size_t m, keycount;
  memset(key, 0, sizeof(*key));
  m = sizeof(OPENSSH_KEY_V1);
  if (m > n)
    return 21;
  if (memcmp(p, OPENSSH_KEY_V1, m))
    return 22;
  p += m;
  n -= m;
  SSH_PARSE_U32BE(120, p, n, m);
  SSH_PARSE_BYTES(121, p, n, m, key->cipher);
  SSH_PARSE_U32BE(122, p, n, m);
  SSH_PARSE_BYTES(123, p, n, m, key->kdfname);
  SSH_PARSE_U32BE(124, p, n, m);
  SSH_PARSE_BYTES(125, p, n, m, key->kdfparams);
  key->kdfsize = m;
  if (strcmp(key->cipher, "none"))
    return SSH_ERR_KEY_IS_ENCRYPTED;
  SSH_PARSE_U32BE(126, p, n, keycount);
  if (keycount > 100)
    return 127;
  if (!(key->keys = malloc(keycount * sizeof(struct OpensshKey))))
    return SSH_ERR_OUT_OF_MEMORY;
  for (size_t i = 0; i < keycount; ++i) {
    int e;
    SSH_PARSE_U32BE(128, p, n, m);
    if (m > n)
      return 129;
    if ((e = ParseOpensshPublicKeyBinary(p, m, &key->keys[i].pub)))
      return e;
    memset(&key->keys[i].key, 0, sizeof(key->keys[i].key));
    p += m;
    n -= m;
    SSH_PARSE_U32BE(130, p, n, m);
    if (m > n)
      return 131;
    if ((e = ParseOpensshPrivateKey(p, m, &key->keys[i].key)))
      return e;
    ++key->keycount;
  }
  return 0;
}

int ParseOpensshPublicKey(const char *p, size_t n,
                          struct OpensshPublicKey *key) {
  if (n > 0 && !*p)
    return ParseOpensshPublicKeyBinary(p, n, key);
  if (n == -1)
    n = strlen(p);
  const char *sp1 = memchr(p, ' ', n);
  if (!sp1)
    return 70;
  size_t m = sp1 + 1 - p;
  p += m;
  n -= m;
  const char *sp2 = memchr(p, ' ', n);
  if (sp2) {
    m = sp2 - p;
  } else {
    m = n;
  }
  char *data;
  size_t size;
  if (!(data = DecodeBase64(p, m, &size)))
    return SSH_ERR_OUT_OF_MEMORY;
  int err = ParseOpensshPublicKeyBinary(data, size, key);
  free(data);
  if (err)
    return err;
  p += m;
  n -= m;
  if (n && *p == ' ')
    ++p, --n;
  if ((key->comment = malloc(n + 1))) {
    if (n)
      memcpy(key->comment, p, n);
    key->comment[n] = 0;
  }
  return 0;
}

// parse file like `~/.ssh/id_ed25519` (run `ssh-keygen -t ed25519`)
int ParseOpensshKeyV1(const char *s, size_t n, struct OpensshKeyV1 *key) {
  if (n == -1)
    n = strlen(s);
  if (!ParseOpensshKeyV1Impl(s, n, key))
    return 0;
  DestroyOpensshKeyV1(key);
  char *begin, *end;
  if ((begin = strstr(s, BEGIN_OPENSSH_PRIVATE_KEY))) {
    begin += strlen(BEGIN_OPENSSH_PRIVATE_KEY);
    if ((end = strstr(begin, END_OPENSSH_PRIVATE_KEY))) {
      char *data;
      if (!(data = DecodeBase64(begin, end - begin, &n)))
        return SSH_ERR_OUT_OF_MEMORY;
      int err = ParseOpensshKeyV1Impl(data, n, key);
      free(data);
      return err;
    }
  }
  return SSH_ERR_NOT_OPENSSH_KEY_V1;
}

int ExtractEd25519PrivateKey(const struct OpensshKeyV1 *key,
                             uint8_t private_key[32], char **opt_out_comment) {
  int err = SSH_ERR_KEY_ISNT_ED25519;
  for (size_t i = 0; i < key->keycount; ++i) {
    if (strcmp(key->keys[i].pub.algo, "ssh-ed25519"))
      continue;
    if (key->keys[i].key.size != 64) {
      err = 40;
      continue;
    }
    if (key->keys[i].pub.size != 32) {
      err = 41;
      continue;
    }
    if (key->keys[i].key.pub.size != 32) {
      err = 42;
      continue;
    }
    if (memcmp(key->keys[i].pub.data, key->keys[i].key.pub.data, 32)) {
      err = 43;
      continue;
    }
    if (memcmp(key->keys[i].pub.data, key->keys[i].key.data + 32, 32)) {
      err = 44;
      continue;
    }
    memcpy(private_key, key->keys[i].key.data, 32);
    if (opt_out_comment)
      *opt_out_comment =
          key->keys[i].key.comment ? strdup(key->keys[i].key.comment) : 0;
    return 0;
  }
  return err;
}

// 1. we accept openssh private key as binary
// 2. we accept openssh private key with ascii armoring
// 3. we accept binary if parse fails and file contains exactly 32 bytes
int ParseEd25519PrivateKey(const char *p, size_t n, uint8_t private_key[32],
                           char **opt_out_comment) {
  int err;
  struct OpensshKeyV1 key;
  if (!(err = ParseOpensshKeyV1(p, n, &key)))
    err = ExtractEd25519PrivateKey(&key, private_key, opt_out_comment);
  DestroyOpensshKeyV1(&key);
  if (err && n == 32) {
    memcpy(private_key, p, 32);
    if (opt_out_comment)
      *opt_out_comment = 0;
    err = 0;
  }
  return err;
}

int ParseOpensshKnownHost(const char *p, size_t n,
                          struct OpensshKnownHost *obj) {
  int err;
  size_t m;
  const char *e;
  memset(obj, 0, sizeof(*obj));
  n = n == -1 ? strlen(p) : n;
  if (n > 3 && p[0] == '|' && p[1] == '1' && p[2] == '|') {
    // new format is `|1|b64salt|b64hash ssh-ed25519 b64pub`
    // so you say `hmac_sha1 (b64salt, hostname) == b64hash`
    p += 3;
    n -= 3;
    if (!(e = memchr(p, '|', n)))
      return 80;
    char *b;
    if (!(b = DecodeBase64(p, e - p, &m)))
      return SSH_ERR_OUT_OF_MEMORY;
    if (m != 20) {
      free(b);
      return 81;
    }
    memcpy(obj->salt, b, 20);
    free(b);
    n -= e - p + 1;
    p = e + 1;
    if (!(e = memchr(p, ' ', n)))
      return 82;
    if (!(b = DecodeBase64(p, e - p, &m)))
      return SSH_ERR_OUT_OF_MEMORY;
    if (m != 20) {
      free(b);
      return 83;
    }
    memcpy(obj->hash, b, 20);
    free(b);
  } else {
    // old format is `hostname ssh-ed25519 b64pub`
    if (!(e = memchr(p, ' ', n)))
      return SSH_ERR_OUT_OF_MEMORY;
    if (!(obj->host = strndup(p, e - p)))
      return SSH_ERR_OUT_OF_MEMORY;
  }
  n -= e - p + 1;
  p = e + 1;
  if (n < 12)
    return 84;
  if (memcmp(p, "ssh-ed25519 ", 12))
    return SSH_ERR_ALGORITHM_UNSUPPORTED;
  if ((err = ParseOpensshPublicKey(p, n, &obj->pub)))
    return err;
  return 0;
}

int ParseOpensshKnownHosts(const char *p, size_t n,
                           struct OpensshKnownHosts *obj) {
  memset(obj, 0, sizeof(*obj));
  n = n == -1 ? strlen(p) : n;
  while (n) {

    // parse line
    size_t ln;
    const char *e;
    const char *lp;
    if ((e = memchr(p, '\n', n))) {
      lp = p;
      ln = e - p;
      n -= e - p + 1;
      p = e + 1;
    } else {
      lp = p;
      ln = n;
      n = 0;
    }

    // strip comment
    if ((e = memchr(lp, '#', ln)))
      ln = e - p;

    // chomp whitespace
    while (ln && isspace(*lp)) {
      ++lp;
      --ln;
    }

    // skip empty lines
    if (!ln)
      continue;

    // extend array
    if (obj->n == obj->c) {
      struct OpensshKnownHost *p2;
      obj->c += 1;
      obj->c += obj->c >> 1;
      if (!(p2 = realloc(obj->p, obj->c * sizeof(*obj->p))))
        return SSH_ERR_OUT_OF_MEMORY;
      obj->p = p2;
    }

    // parse line
    int err;
    if ((err = ParseOpensshKnownHost(lp, ln, &obj->p[obj->n]))) {
      if (err == SSH_ERR_ALGORITHM_UNSUPPORTED) {
        DestroyOpensshKnownHost(&obj->p[obj->n]);
        continue;
      }
      return err;
    }
    ++obj->n;
  }
  return 0;
}

void DescribeOpensshParseError(char *buf, size_t size, int err) {
  char tmp[64];
  const char *res;
  switch (err) {
    case SSH_ERR_OUT_OF_MEMORY:
      res = "out of memory";
      break;
    case SSH_ERR_NOT_OPENSSH_KEY_V1:
      res = "not an openssh-key-v1 file";
      break;
    case SSH_ERR_KEY_IS_ENCRYPTED:
      res = "ssh key is encrypted";
      break;
    case SSH_ERR_KEY_ISNT_ED25519:
      res = "key isn't ssh-ed25519";
      break;
    case SSH_ERR_KEY_CORRUPTED:
      res = "ssh key corrupted";
      break;
    default:
      FormatInt32(stpcpy(tmp, "ssh key parse error no. "), err);
      res = tmp;
      break;
  }
  strlcpy(buf, res, size);
}
