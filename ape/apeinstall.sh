#!/bin/sh

PROG=${0##*/}
MODE=${MODE:-$m}
TMPDIR=${TMPDIR:-/tmp}
COSMO=${COSMO:-/opt/cosmo}
COSMOS=${COSMOS:-/opt/cosmos}

if [ ! -f ape/loader.c ]; then
  cd "$COSMO" || exit
fi

if [ -x build/bootstrap/make.com ]; then
  MAKE=build/bootstrap/make.com
else
  MAKE=make
fi

if [ "$(id -u)" -eq 0 ]; then
  SUDO=
elif command -v sudo >/dev/null 2>&1; then
  SUDO=sudo
elif command -v doas >/dev/null 2>&1; then
  SUDO=doas
else
  echo "need root or sudo" >&2
  exit
fi

if command -v install >/dev/null 2>&1; then
  if [ x"$(uname -s)" = xLinux ]; then
    INSTALL="install -o root -g root -m 755"
  else
    INSTALL="install -o root -g wheel -m 755"
  fi
else
  INSTALL="cp -f"
fi

echo "Actually Portable Executable (APE) Installer" >&2
echo "Author:  Justine Tunney <jtunney@gmail.com>"  >&2

# special installation process for apple silicon
if [ x"$(uname -s)" = xDarwin ] && [ x"$(uname -m)" = xarm64 ]; then
  echo "cc -O -o $TMPDIR/ape.$$ ape/ape-m1.c" >&2
  cc -O -o "$TMPDIR/ape.$$" ape/ape-m1.c || exit
  trap 'rm "$TMPDIR/ape.$$"' EXIT
  if [ ! -d /usr/local/bin ]; then
    echo "$SUDO mkdir -p /usr/local/bin" >&2
    $SUDO mkdir -p /usr/local/bin || exit
  fi
  echo "$SUDO $INSTALL $TMPDIR/ape.$$ /usr/local/bin/ape" >&2
  $SUDO $INSTALL "$TMPDIR/ape.$$" /usr/local/bin/ape || exit
  exit
fi

if [ x"$(uname -m)" = xarm64 ] || [ x"$(uname -m)" = xaarch64 ]; then
  MODE=aarch64
  EXT=elf
  BEXT=aarch64
elif [ x"$(uname -m)" = xx86_64 ]; then
  MODE=
  if [ x"$(uname -s)" = xDarwin ]; then
    EXT=macho
  else
    EXT=elf
  fi
  BEXT=$EXT
else
  echo "unsupported architecture $(uname -m)" >&2
  exit
fi

################################################################################
# INSTALL APE LOADER SYSTEMWIDE

if [ -f o/$MODE/depend ] && $MAKE -j8 o/$MODE/ape; then
  echo "successfully recompiled ape loader" >&2
elif [ -x o/$MODE/ape/ape.$EXT ]; then
  echo "using ape loader you compiled earlier" >&2
elif [ -d build/bootstrap ]; then
  # if make isn't being used then it's unlikely the user changed the sources
  # in that case the prebuilt binaries should be completely up-to-date
  echo "using prebuilt ape loader from cosmo repo" >&2
  mkdir -p o/$MODE/ape || exit
  cp -af build/bootstrap/ape.$BEXT o/$MODE/ape/ape.$EXT || exit
else
  echo "no cosmopolitan libc repository here" >&2
  echo "fetching ape loader from justine.lol" >&2
  mkdir -p o/$MODE/ape || exit
  if command -v wget >/dev/null 2>&1; then
    wget -qO o/$MODE/ape/ape.$EXT https://justine.lol/ape.$BEXT || exit
  else
    curl -Rso o/$MODE/ape/ape.$EXT https://justine.lol/ape.$BEXT || exit
  fi
  chmod +x o/$MODE/ape/ape.$EXT || exit
fi

if ! [ /usr/bin/ape -nt o/$MODE/ape/ape.$EXT ]; then
  echo >&2
  echo "installing o/$MODE/ape/ape.$EXT to /usr/bin/ape" >&2
  echo "$SUDO $INSTALL o/$MODE/ape/ape.$EXT /usr/bin/ape" >&2
  $SUDO $INSTALL o/$MODE/ape/ape.$EXT /usr/bin/ape || exit
  echo "done" >&2
fi

################################################################################
# REGISTER APE LOADER WITH BINFMT_MISC TOO (LINUX-ONLY)

if [ x"$(uname -s)" = xLinux ]; then

  if [ -e /proc/sys/fs/binfmt_misc/APE ]; then
    echo >&2
    echo it looks like APE is already registered with binfmt_misc >&2
    echo To reinstall please run ape/apeuninstall.sh first >&2
    echo please check that it is mapped to ape not /bin/sh >&2
    echo cat /proc/sys/fs/binfmt_misc/APE >&2
    cat /proc/sys/fs/binfmt_misc/APE >&2
    exit
  fi

  if ! [ -e /proc/sys/fs/binfmt_misc ]; then
    echo >&2
    echo loading binfmt_misc into your kernel >&2
    echo you may need to edit configs to persist across reboot >&2
    echo $SUDO modprobe binfmt_misc >&2
    $SUDO modprobe binfmt_misc || exit
    echo done >&2
  fi

  if ! [ -e /proc/sys/fs/binfmt_misc/register ]; then
    echo >&2
    echo mounting binfmt_misc into your kernel >&2
    echo you may need to edit configs to persist across reboot >&2
    echo $SUDO mount -t binfmt_misc none /proc/sys/fs/binfmt_misc >&2
    $SUDO mount -t binfmt_misc none /proc/sys/fs/binfmt_misc || exit
    echo done >&2
  fi

  echo >&2
  echo registering APE with binfmt_misc >&2
  echo you may need to edit configs to persist across reboot >&2
  echo '$SUDO sh -c "echo '"'"':APE:M::MZqFpD::/usr/bin/ape:'"'"' >/proc/sys/fs/binfmt_misc/register"' >&2
  $SUDO sh -c "echo ':APE:M::MZqFpD::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register" || exit
  echo '$SUDO sh -c "echo '"'"':APE-jart:M::jartsr::/usr/bin/ape:'"'"' >/proc/sys/fs/binfmt_misc/register"' >&2
  $SUDO sh -c "echo ':APE-jart:M::jartsr::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register" || exit
  echo done >&2

  if [ x"$(cat /proc/sys/fs/binfmt_misc/status)" = xdisabled ]; then
    echo >&2
    echo enabling binfmt_misc >&2
    echo you may need to edit configs to persist across reboot >&2
    echo $SUDO sh -c 'echo 1 >/proc/sys/fs/binfmt_misc/status' >&2
    $SUDO sh -c 'echo 1 >/proc/sys/fs/binfmt_misc/status' || exit
    echo done >&2
  fi

fi

################################################################################

{
  echo
  echo "------------------------------------------------------------------"
  echo
  echo "APE INSTALL COMPLETE"
  echo
  echo "If you decide to uninstall APE later on"
  echo "you may do so using ape/apeuninstall.sh"
  echo
  echo "Enjoy your APE loader (>'.')>"
  echo
} >&2
