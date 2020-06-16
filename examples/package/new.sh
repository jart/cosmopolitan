#!/bin/sh
#
# SYNOPSIS
#
#   Creates new package in repository.
#
# EXAMPLE
#
#   examples/package/new.sh com/github/user/project

DIR=${1:?need directory arg}
VAR=$(echo "$DIR" | tr a-z A-Z | tr / _)
DIRNAME=${DIR%/*}
BASENAME=${DIR##*/}
FILENAME="$DIR/$BASENAME"
MAKEFILE="$DIR/$BASENAME.mk"

if [ -d "$DIR" ]; then
  echo "already exists: $DIR" >&2
  exit 1
fi

mkdir -p "$DIR" &&
  cp -R examples/package/* "$DIR" &&
  rm -f "$DIR/new.sh" &&
  find "$DIR" -type f |
    xargs sed -i -e "
        s~EXAMPLES_PACKAGE~$VAR~g
        s~examples/package/package~$FILENAME~g
        s~examples/package~$DIR~g
        s~%AUTHOR%~$(git config user.name) <$(git config user.email)>~g
      " &&
  sed -i -e "
      s~include $DIR/build.mk~# XXX: include $DIR/build.mk~
      s~include $DIR/lib/build.mk~# XXX: include $DIR/lib/build.mk~
      /#-φ-examples\/package\/new\.sh/i\
include $DIR/lib/build.mk
      /#-φ-examples\/package\/new\.sh/i\
include $DIR/build.mk
    " Makefile
