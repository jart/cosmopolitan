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
#include "libc/bsdstdlib.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/ctype.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdckdint.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/time.h"
#include "libc/x/x.h"
#include "net/http/apesig.h"
#include "net/http/escape.h"
#include "net/http/ssh.h"
#include "net/https/fetch.h"
#include "third_party/getopt/getopt.internal.h"

static const char *const help = "\
Copyright 2025 Justine Alexandra Roberts Tunney - ISC license\n\
Actually Portable Executable Code Signing Database Tool 1.0. Usage:\n\
apetrust [FLAGS...] add|list [@GITHUB-USER | PATH...]\n\
  -f PATH  path of ape trust file [defaults to ~/.ape.trust]\n\
  -e TIME  expiration time delta (e.g. 1y, 1w, 1d)\n\
  -C TEXT  sets comment on inserted entry\n\
  -h       show this help information\n\
\n";

struct Strings {
  size_t n, c;
  const char **p;
};

struct ApeTrustsFile {
  size_t size;
  uint8_t *data;
  struct ApeTrusts db;
};

struct AuthorizedKey {
  uint8_t pubkey[32];
  char *comment;
};

struct AuthorizedKeys {
  size_t n, c;
  struct AuthorizedKey *p;
};

int64_t now;
const char *prog;
const char *command;
const char *my_comment;
const char *trust_path;
struct ApeTrustsFile trusts;
long expires_delta;

[[noreturn]] void Die(const char *thing, const char *reason) {
  fputs(thing, stderr);
  fputs(": fatal error: ", stderr);
  fputs(reason, stderr);
  fputs("\n", stderr);
  exit(1);
}

[[noreturn]] void DieSys(const char *thing) {
  perror(thing);
  exit(1);
}

[[noreturn]] void DieOom(void) {
  Die(prog, "out of memory");
}

[[noreturn]] void DieApeSig(const char *thing, int err) {
  char message[64];
  DescribeApeSigError(message, sizeof(message), err);
  Die(thing, message);
}

bool IsEmpty(const char *s) {
  return !s || !*s;
}

void *Malloc(size_t n) {
  void *p;
  if (!(p = malloc(n)))
    DieOom();
  return p;
}

char *StrDup(const char *s) {
  char *p;
  if (!(p = strdup(s)))
    DieOom();
  return p;
}

void *Realloc(void *p, size_t n) {
  if (!(p = realloc(p, n)))
    DieOom();
  return p;
}

char *StrCat(const char *a, const char *b) {
  char *p;
  size_t n, m;
  n = strlen(a);
  m = strlen(b);
  p = Malloc(n + m + 1);
  if (n)
    memcpy(p, a, n);
  memcpy(p + n, b, m + 1);
  return p;
}

void Appendw(char **b, uint64_t w) {
  if (appendw(b, w) == -1)
    DieOom();
}

void Appends(char **b, const char *s) {
  if (appends(b, s) == -1)
    DieOom();
}

void Appendd(char **b, const void *p, size_t n) {
  if (appendd(b, p, n) == -1)
    DieOom();
}

long ParseTimeDelta(const char *s) {
  char *e;
  long x = strtol(s, &e, 10);
  while (isspace(*e))
    ++e;
  if (!e[0])
    Die(prog, "time delta missing suffix");
  if (e[1])
    Die(prog, "time delta suffix too long");
  long m;
  switch (e[0]) {
    case 'd':
    case 'D':
      m = 24ll * 60 * 60;
      break;
    case 'm':
    case 'M':
      m = 30ll * 24 * 60 * 60;
      break;
    case 'y':
    case 'Y':
      m = 365ll * 24 * 60 * 60;
      break;
    default:
      Die(prog, "invalid time delta suffix");
  }
  if (ckd_mul(&x, x, m))
    Die(prog, "time delta overflow");
  return x;
}

bool FileExists(const char *path) {
  struct stat st;
  return !stat(path, &st);
}

const char *GetDefaultTrustPath(void) {
  if (!getuid())
    if (FileExists("/etc/ape.trust"))
      return "/etc/ape.trust";
  const char *home;
  if ((home = getenv("HOME")))
    return StrCat(home, "/.ape.trust");
  return 0;
}

void SetDefaultArgs(void) {
  trust_path = GetDefaultTrustPath();
}

void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "he:f:C:")) != -1) {
    switch (opt) {
      case 'f':
        trust_path = optarg;
        break;
      case 'e':
        expires_delta = ParseTimeDelta(optarg);
        break;
      case 'C':
        my_comment = optarg;
        break;
      case 'h':
        fputs(help, stdout);
        exit(0);
      default:
        fputs(help, stderr);
        exit(1);
    }
  }
  if (!trust_path)
    Die(prog, "trust file path not specified");
  if (optind == argc)
    Die(prog, "missing command");
  command = argv[optind++];
}

void GetProg(char **argv) {
  prog = argv[0];
  if (!prog)
    prog = "apetrust";
}

bool HasString(struct Strings *l, const char *s) {
  for (size_t i = 0; i < l->n; ++i)
    if (!strcmp(l->p[i], s))
      return true;
  return false;
}

void AddString(struct Strings *l, const char *s) {
  if (l->n == l->c) {
    l->c += 1;
    l->c += l->c >> 1;
    l->p = Realloc(l->p, l->c * sizeof(*l->p));
  }
  l->p[l->n++] = s;
}

void AddAuthorizedKey(struct AuthorizedKeys *db, const uint8_t pubkey[32],
                      const char *comment) {
  if (db->n == db->c) {
    db->c += 1;
    db->c += db->c >> 1;
    db->p = Realloc(db->p, db->c * sizeof(*db->p));
  }
  memcpy(db->p[db->n].pubkey, pubkey, 32);
  db->p[db->n].comment = strdup(comment ? comment : "");
  db->n++;
}

int LoadAuthorizedKeysLine(struct AuthorizedKeys *db, const char *line,
                           size_t length) {
  if (length < 16)
    return EINVAL;
  if (memcmp(line, "ssh-ed25519 ", 12))
    return EINVAL;
  struct OpensshPublicKey pk;
  int err = ParseOpensshPublicKey(line, length, &pk);
  if (err) {
    DestroyOpensshPublicKey(&pk);
    return err;
  }
  if (pk.size == 32 && !strcmp(pk.algo, "ssh-ed25519"))
    AddAuthorizedKey(db, pk.data, pk.comment);
  DestroyOpensshPublicKey(&pk);
  return 0;
}

void LoadAuthorizedKeysData(struct AuthorizedKeys *db, const char *data,
                            size_t size) {
  while (size) {
    const char *eol;
    if ((eol = memchr(data, '\n', size))) {
      const char *eol2 = memchr(data, '#', eol - data);
      if (!eol2)
        eol2 = eol;
      LoadAuthorizedKeysLine(db, data, eol - data);
      ++eol;
      size -= eol - data;
      data = eol;
    } else {
      LoadAuthorizedKeysLine(db, data, size);
      break;
    }
  }
}

void LoadAuthorizedKeysFile(struct AuthorizedKeys *db, const char *path) {
  char *data;
  size_t size;
  if (!(data = xslurp(path, &size)))
    DieSys(path);
  LoadAuthorizedKeysData(db, data, size);
  free(data);
}

void LoadApeTrustsFile(const char *path) {
  int err;
  if (!(trusts.data = xslurp(path, &trusts.size)))
    DieSys(path);
  if ((err = ParseApeTrusts(trusts.data, trusts.size, &trusts.db)))
    DieApeSig(path, err);
}

// a github account is trusted if a keyless entry references it
bool HaveGithub(const char *github) {
  if (IsEmpty(github))
    return false;
  for (size_t i = 0; i < trusts.db.trusts_count; ++i)
    if (!trusts.db.trusts[i].key.algorithm)
      if (!strcmp(trusts.db.trusts[i].github, github))
        return true;
  return false;
}

bool HavePublicKey(const uint8_t pubkey[32]) {
  for (size_t i = 0; i < trusts.db.trusts_count; ++i)
    if (trusts.db.trusts[i].key.algorithm == APESIG_ALGORITHM_ED25519 &&
        trusts.db.trusts[i].key.pub.size == 32 &&
        !memcmp(trusts.db.trusts[i].key.pub.data, pubkey, 32))
      return true;
  return false;
}

char *GetTimestampZulu(int64_t t) {
  int x;
  struct tm tm;
  static char s[20];
  gmtime_r(&t, &tm);
  x = tm.tm_year + 1900;
  s[0] = '0' + x / 1000;
  s[1] = '0' + x / 100 % 10;
  s[2] = '0' + x / 10 % 10;
  s[3] = '0' + x % 10;
  s[4] = '-';
  x = tm.tm_mon + 1;
  s[5] = '0' + x / 10;
  s[6] = '0' + x % 10;
  s[7] = '-';
  x = tm.tm_mday;
  s[8] = '0' + x / 10;
  s[9] = '0' + x % 10;
  s[10] = 'T';
  x = tm.tm_hour;
  s[11] = '0' + x / 10;
  s[12] = '0' + x % 10;
  s[13] = ':';
  x = tm.tm_min;
  s[14] = '0' + x / 10;
  s[15] = '0' + x % 10;
  s[16] = ':';
  x = tm.tm_sec;
  s[17] = '0' + x / 10;
  s[18] = '0' + x % 10;
  s[19] = 'Z';
  return s;
}

void ListTrust(const struct ApeTrust *trust) {
  if (trust->key.algorithm) {
    char *c = 0;
    if (!IsEmpty(trust->comment)) {
      Appends(&c, " ");
      Appends(&c, trust->comment);
    }
    if (!IsEmpty(trust->github)) {
      Appends(&c, " github:");
      Appends(&c, trust->github);
    }
    if (trust->issued) {
      Appends(&c, " issued:");
      Appends(&c, GetTimestampZulu(trust->issued));
    }
    if (trust->expires) {
      Appends(&c, " expires:");
      Appends(&c, GetTimestampZulu(trust->expires));
    }
    if (trust->key.algorithm == APESIG_ALGORITHM_ED25519 &&
        trust->key.pub.size == 32) {
      char binary[51] = "\0\0\0\13"    // 11 in big endian
                        "ssh-ed25519"  // algorithm string
                        "\0\0\0\40";   // 32 in big endian
      memcpy(binary + 4 + 11 + 4, trust->key.pub.data, 32);
      fputs("ssh-ed25519 ", stdout);
      char *s;
      if (!(s = EncodeBase64(binary, 51, 0)))
        DieOom();
      fputs(s, stdout);
      free(s);
      if (c) {
        fputs(c, stdout);
        free(c);
      }
      fputs("\n", stdout);
    } else {
      Die(trust_path, "ape trust db uses unsupported algorithm");
    }
  } else if (!IsEmpty(trust->github)) {
    fputs("github ", stdout);
    fputs(trust->github, stdout);
    if (!IsEmpty(trust->comment)) {
      fputs(" ", stdout);
      fputs(trust->comment, stdout);
    }
    if (trust->issued) {
      fputs(" issued:", stdout);
      fputs(GetTimestampZulu(trust->issued), stdout);
    }
    if (trust->expires) {
      fputs(" expires:", stdout);
      fputs(GetTimestampZulu(trust->expires), stdout);
    }
    fputs("\n", stdout);
  }
}

void List(void) {
  LoadApeTrustsFile(trust_path);
  for (long i = 0; i < trusts.db.trusts_count; ++i)
    ListTrust(&trusts.db.trusts[i]);
}

struct ApeTrust *AppendTrust(void) {
  if (ckd_add(&trusts.db.trusts_count, trusts.db.trusts_count, 1))
    Die(prog, "trust database has too many entries");
  trusts.db.trusts = Realloc(
      trusts.db.trusts, trusts.db.trusts_count * sizeof(*trusts.db.trusts));
  struct ApeTrust *trust = &trusts.db.trusts[trusts.db.trusts_count - 1];
  memset(trust, 0, sizeof(*trust));
  trust->issued = now;
  if (expires_delta)
    if (ckd_add(&trust->expires, now, expires_delta))
      Die(prog, "expiration delta overflow");
  return trust;
}

void RemoveGithub(const char *user) {
  long j = 0;
  for (long i = 0; i < trusts.db.trusts_count; ++i)
    if (!(!IsEmpty(trusts.db.trusts[i].github) &&
          !strcmp(trusts.db.trusts[i].github, user)))
      memmove(&trusts.db.trusts[j++], &trusts.db.trusts[i],
              sizeof(trusts.db.trusts[i]));
  trusts.db.trusts_count = j;
}

void AddAuthorizedKeys(struct AuthorizedKeys *db, const char *github) {
  for (size_t j = 0; j < db->n; ++j) {
    if (!HavePublicKey(db->p[j].pubkey)) {
      struct ApeTrust *trust = AppendTrust();
      trust->github = github;
      trust->comment = my_comment ? my_comment : db->p[j].comment;
      trust->key.algorithm = APESIG_ALGORITHM_ED25519;
      trust->key.pub.size = 32;
      trust->key.pub.data = db->p[j].pubkey;
    }
  }
}

void Save(void) {
  int err;
  struct ApeBytes bytes;
  if ((err = SerializeApeTrusts(&trusts.db, &bytes)))
    DieApeSig(prog, err);
  char ext[16] = ".";
  FormatUint32(ext + 1, arc4random());
  char *tmp = StrCat(trust_path, ext);
  if (xbarf(tmp, bytes.data, bytes.size))
    DieSys(trust_path);
  if (rename(tmp, trust_path))
    DieSys(trust_path);
  free(bytes.data);
  free(tmp);
}

void Add(int argc, char *argv[]) {
  if (!argc)
    Die(prog, "missing operand");
  if (FileExists(trust_path)) {
    LoadApeTrustsFile(trust_path);
  } else {
    trusts.db.magic = APETRUST_MAGIC;
  }
  for (int i = 0; i < argc; ++i) {
    struct AuthorizedKeys db = {0};
    if (argv[i][0] == '@') {
      const char *user = argv[i] + 1;
      char *data = 0;
      char *url = StrCat(StrCat("https://github.com/", user), ".keys");
      int status;
      if ((status = AppendFetch(&data, url)) == -1)
        DieSys(url);
      if (status != 200)
        Die(url, "non-200 http response received");
      LoadAuthorizedKeysData(&db, data, appendz(data).i);
      RemoveGithub(user);
      AppendTrust()->github = user;
      AddAuthorizedKeys(&db, user);
    } else {
      LoadAuthorizedKeysFile(&db, argv[i]);
      AddAuthorizedKeys(&db, 0);
    }
  }
  Save();
}

void Sync(void) {
  LoadApeTrustsFile(trust_path);
  struct Strings githubs = {0};
  for (long i = 0; i < trusts.db.trusts_count; ++i)
    if (!IsEmpty(trusts.db.trusts[i].github))
      if (!HasString(&githubs, trusts.db.trusts[i].github))
        AddString(&githubs, trusts.db.trusts[i].github);
  if (githubs.n) {
    for (long i = 0; i < githubs.n; ++i) {
      const char *user = githubs.p[i];
      char *data = 0;
      char *url = StrCat(StrCat("https://github.com/", user), ".keys");
      int status;
      if ((status = AppendFetch(&data, url)) == -1)
        DieSys(url);
      if (status != 200)
        Die(url, "non-200 http response received");
      struct AuthorizedKeys db = {0};
      LoadAuthorizedKeysData(&db, data, appendz(data).i);
      RemoveGithub(user);
      AppendTrust()->github = user;
      AddAuthorizedKeys(&db, user);
    }
    Save();
  }
  free(githubs.p);
}

int main(int argc, char *argv[]) {
  now = time(0);
  GetProg(argv);
  SetDefaultArgs();
  GetOpts(argc, argv);
  if (!strcmp(command, "list")) {
    List();
  } else if (!strcmp(command, "sync")) {
    Sync();
  } else if (!strcmp(command, "add")) {
    Add(argc - optind, argv + optind);
  } else {
    Die(prog, "invalid command");
  }
}
