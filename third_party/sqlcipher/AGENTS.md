# cherrybean

Redbean variant with SQLCipher instead of vanilla SQLite. Encrypted DB
files plus `sqlcipher_export()`.

## Layout (git parallel, Cosmo build hierarchical)

| Path | GitHub | Role |
|---|---|---|
| `/home/don/q/pr/cherrybean` | `dwomick/cherrybean` | Product notes, `make test`, this file |
| `/home/don/q/pr/cosmopolitan/cherrybean` | `dwomick/cosmopolitan` `@cherrybean` | Cosmo fork: binary + vendored SQLCipher |
| `/home/don/q/pr/sqlcipher/cherrybean` | `dwomick/sqlcipher` `@cherrybean` | Clean SQLCipher 4.18.0 (no Cosmo VFS in git) |

Pins: `deps.lock`. Cosmo-only reproduce: clone `dwomick/cosmopolitan` branch `cherrybean`, `make -j8 o//tool/net/cherrybean`, run `third_party/sqlcipher/encrypt.lua`.

Vanilla `redbean` in the Cosmo tree is unchanged (`third_party/sqlite3` 3.40.0).

## Build / test

```sh
make -j8          # builds ../cosmopolitan/cherrybean o//tool/net/cherrybean
make test         # cherrybean -i tests/encrypt.lua
```

Do not re-download cosmocc or regenerate `sqlite3.c` unless SQLCipher
upstream moved. Toolchain: `cosmopolitan/cherrybean/.cosmocc/3.9.2`.

## Upgrade policy

- Keep SQLCipher **current**. Do not freeze SQLite at Cosmo’s 3.40.
- Cosmo VFS intent lives only in
  `cosmopolitan/cherrybean/third_party/sqlcipher/patches/cosmo-vfs.patch`.
  `upgrade.sh` archives SQLCipher HEAD, applies the patch, amalgamates.
  SQLCipher git stays rebaseable.
- `cherrybean.c` and `lsqlite3-sqlcipher.c` stay one-line wrappers.
  Local redbean surface is `CHERRYBEAN_SQLCIPHER` ifdefs in `lsqlite3.c`.
- After SQLCipher bump: merge that repo, run `upgrade.sh`, fix the
  patch if `patch -p1` fails.
- After Cosmo redbean bump: merge `tool/net/redbean.c` and `lsqlite3.c`.

## Do not

- Edit `sqlite3.c` by hand
- Enable `SQLITE_ENABLE_BATCH_ATOMIC_WRITE` without `IsLinux()` ioctl
  guards (would F2FS-ioctl on Windows)
- Graft SQLCipher codec onto Cosmo’s 3.40 tree
- Add SQLCipher as a nested git submodule of the product repo

Full rationale: `DECISIONS.md`.
