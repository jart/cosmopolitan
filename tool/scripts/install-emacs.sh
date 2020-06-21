#!/bin/sh
# installs emacs version w/ working (replace-buffer-contents)
# it's sooooo worth it due to clang-format-10 working so well
set -ex
sudo apt build-dep emacs
cd "$HOME"
export CFLAGS="-O2"
rm -rf emacs-26.3
wget https://mirrors.kernel.org/gnu/emacs/emacs-26.3.tar.gz
tar xf emacs-26.3.tar.gz
cd emacs-26.3
./configure
make -j8
sudo make install
rm -f ../emacs-26.3.tar.gz
