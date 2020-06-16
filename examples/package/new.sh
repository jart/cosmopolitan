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
BASENAME=${DIR##*/}
FILENAME="$DIR/$BASENAME"
MAKEFILE="$DIR/$BASENAME.mk"

if [ -d "$DIR" ]; then
  echo "already exists: $DIR" >&2
  exit 1
fi

mkdir -p "$DIR" &&
  cp -R examples/package/new/* "$DIR" &&
  find "$DIR" -type f |
    xargs sed -i -e "
        s/EXAMPLES_PACKAGE/$VAR/g
        s/examples\/package\/package/$FILENAME/g
        s/examples\/package/$DIR/g
      " &&
  sed -i -e "
      /#-φ-examples\/package\/new\.sh/i\
include $MAKEFILE
    " Makefile
