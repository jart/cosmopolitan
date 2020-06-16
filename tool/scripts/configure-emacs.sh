#!/bin/sh

if ! [ -e ape/ape.S ]; then
  echo please cd to cosmopolitan root directory >&2
  exit 1
fi

COSMOPOLITAN="$PWD"
EMACSCONFIGS="$HOME/.emacs.d/init.el"
[ -e "$EMACSCONFIGS" ] || EMACSCONFIGS="$HOME/.emacs"

cat >>"$EMACSCONFIGS" <<EOF # idempotent
(let ((path "$COSMOPOLITAN/tool/emacs"))
  (when (file-directory-p path)
    (add-to-list 'load-path path)
    (require 'cosmo)))
EOF
