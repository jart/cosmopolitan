#!/bin/sh
# Rebuild sqlite3.c / sqlite3.h / sqlite3ext.h from a SQLCipher source tree,
# applying Cosmo APE VFS hunks from patches/cosmo-vfs.patch first.
#
# Usage: third_party/sqlcipher/upgrade.sh [/path/to/sqlcipher]
#
# SQLCipher git stays a clean (or nearly clean) upstream fork. Cosmo-specific
# VFS edits live only in patches/ so they can be rebased when either project
# moves. If patch(1) fails, refresh patches/cosmo-vfs.patch against the new
# src/os_unix.c and src/sqliteInt.h, then re-run.
set -eu
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
HERE=$(cd "$(dirname "$0")" && pwd)
SRC=${1:-$ROOT/../../sqlcipher/cherrybean}
PATCH=$HERE/patches/cosmo-vfs.patch
if [ ! -x "$SRC/configure" ]; then
	printf '%s\n' "$0: no SQLCipher tree at $SRC" >&2
	exit 1
fi
if [ ! -f "$PATCH" ]; then
	printf '%s\n' "$0: missing $PATCH" >&2
	exit 1
fi
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
WORK=$TMP/sqlcipher
mkdir -p "$WORK"
if [ -d "$SRC/.git" ] || [ -f "$SRC/.git" ]; then
	git -C "$SRC" archive HEAD | tar -x -C "$WORK"
else
	# linked worktree: .git is a file; archive still works via git -C
	if git -C "$SRC" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
		git -C "$SRC" archive HEAD | tar -x -C "$WORK"
	else
		cp -a "$SRC"/. "$WORK"/
		rm -rf "$WORK/.git" "$WORK/.bare"
	fi
fi
patch -d "$WORK" -p1 --forward --batch <"$PATCH"
cd "$WORK"
./configure --with-tempstore=yes --disable-tcl --disable-shared
make sqlite3.c
if ! grep -q cherrybean-cosmo-vfs sqlite3.c; then
	printf '%s\n' "$0: amalgamation missing cherrybean-cosmo-vfs markers" >&2
	exit 1
fi
cp sqlite3.c sqlite3.h sqlite3ext.h "$HERE/"
printf '%s\n' "updated $HERE from $SRC (cosmo-vfs.patch applied)"
