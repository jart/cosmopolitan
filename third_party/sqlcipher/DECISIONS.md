# cherrybean decisions (2026-08-20)

Session: `01a01ec4-169d-79f3-949d-c32fcdfe69cc`
Log: `~/.grok/sessions/%2Fhome%2Fdon%2Fq%2Fpr%2Fcherrybean/01a01ec4-169d-79f3-949d-c32fcdfe69cc/`
(`updates.jsonl` is authoritative). Resume:
`grok --resume 01a01ec4-169d-79f3-949d-c32fcdfe69cc`

## Product

cherrybean = redbean APE with SQLCipher 4.18.0 (SQLite 3.53.4) instead of
Cosmo vanilla sqlite 3.40.0. Goal: create/open/export encrypted SQLite
files from Lua.

## Layout

Git identity **parallel** (one GitHub fork per upstream). On-disk checkouts
already use `.bare` + worktrees (`master` / `cherrybean`) per project.

Build is **hierarchical inside Cosmo** because redbean is a hermetic APE
Makefile target, not `CC=cosmocc` autotools. cosmocc is a compiler zip
(`.cosmocc/`), not a subproject.

Do not subtree Cosmo or SQLCipher into the empty product git. Optional later:
thin lockfile/workspace repo for CI pins only.

## SQLCipher integration

- New Cosmo package `third_party/sqlcipher/` (does not replace sqlite3).
- Amalgamation + `sqlite3_cosmo.c` wrapper + `crypto_mbedtls.c`
  (`SQLCIPHER_CRYPTO_CUSTOM=sqlcipher_mbedtls_setup`). AES-256-CBC via
  AES-ECB so `MODE=tiny` (no mbedtls CBC) still links. HMAC-SHA1/256/512,
  PBKDF2, `getrandom`.
- Binary: `tool/net/cherrybean.c` (`#define REDBEAN "cherrybean"`).
- Second Lua object `lsqlite3-sqlcipher.c`; exclude it from `net.pkg`.
- Lua: `require("lsqlite3")`, `db:key` / `db:rekey`. Zipfile vtab and
  SESSION APIs are **off** on the SQLCipher binding.
- Flags: `SQLITE_HAS_CODEC`, `SQLITE_TEMP_STORE=2`,
  `SQLITE_EXTRA_INIT=sqlcipher_extra_init`, no `HAVE_MREMAP` (Cosmo has
  `cosmo_mremap`, not POSIX `mremap`).
- `sqlcipher_export()` is the plaintext path; `rekey` does not decrypt
  in place.

## VFS / APE (upgrade-friendly hunks only)

`patches/cosmo-vfs.patch` applied at amalgamation time, not in SQLCipher git:

- `HAVE_PREAD`/`HAVE_PWRITE` when `__COSMOPOLITAN__`
- `msync` mapped pages in `unixSync` (Windows FlushViewOfFile; OpenBSD)
- errno save/restore in `unixGetTempname` / `unixAccess`
- `SQLITE_MAX_MMAP_SIZE` on `__COSMOPOLITAN__`

Omitted on purpose: F2FS batch-atomic + `IsLinux()`, sqlite-side
`F_FULLFSYNC` (libc already does it), Cosmo include rewrite.

SQLCipher never shipped SQLite 3.40 (closest 4.5.3 = 3.39.4). Do **not**
roll SQLCipher back to match Cosmo sqlite; Cosmo/redbean updates are
easier with current SQLCipher + small VFS patches.

## Limitations vs redbean

Shared: `SQLITE_OS_UNIX` on all hosts; Cosmo sqlite Windows locks are a
no-op; Mac is POSIX locks not AFP. Both inherit that.

cherrybean-only:

- Encrypt/export **tested on Linux x86_64 only**
- SQLite 3.53 vs redbean 3.40
- No zipfile vtab, no session module
- No `SQLITE_ENABLE_BATCH_ATOMIC_WRITE`
- `help.txt` still says redbean
- Default build here is x86_64 APE (~3.2MB); aarch64 unbuilt

Highest-value next check: run `tests/encrypt.lua` (and mmap_size) on
Windows and Mac.

## Caches already on disk (do not redo)

- cosmocc 3.9.2: `cosmopolitan/cherrybean/.cosmocc/`
- Amalgamation: `cosmopolitan/cherrybean/third_party/sqlcipher/sqlite3.c` (~9.3MB)
- Built APE: `cosmopolitan/cherrybean/o/tool/net/cherrybean`
- SQLCipher amalgamation scratch is disposable (`/tmp/sqlcipher-amalg` etc.)

## GitHub pins (this snapshot)

| Repo | Branch | SHA |
|---|---|---|
| github.com/dwomick/cosmopolitan | cherrybean | `429e50dc6dcaa21f96458ca3b6d4b805b667551b` |
| github.com/dwomick/sqlcipher | cherrybean | `63697beb0fafcb61faa7a3e6fd267036548ab11b` |
| cosmocc | 3.9.2 | sha256 `f4ff13af65fcd309f3f1cfd04275996fb7f72a4897726628a8c9cf732e850193` |

Also `deps.lock`. Local clones should have remotes `upstream` →
`jart/cosmopolitan` and `sqlcipher/sqlcipher`.
