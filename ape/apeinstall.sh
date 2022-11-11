#!/bin/sh

if [ "$(id -u)" -eq 0 ]; then
  SUDO=
else
  SUDO=sudo
fi

echo "Actually Portable Executable (APE) Installer" >&2
echo "Author:  Justine Tunney <jtunney@gmail.com>"  >&2

################################################################################
# INSTALL APE LOADER SYSTEMWIDE

if [ -f o/depend ]; then
  # mkdeps.com build was successfully run so assume we can build
  echo >&2
  echo "recompiling ape loader" >&2
  echo "running: make -j8 o//ape" >&2
  make -j8 o//ape || exit
  echo "done" >&2
elif [ -d build/bootstrap ]; then
  # if make isn't being used then it's unlikely the user changed the sources
  # in that case the prebuilt binaries should be completely up-to-date
  echo "using prebuilt ape loader from cosmo repo" >&2
  mkdir -p o//ape || exit
  cp -af build/bootstrap/ape.elf o//ape/ape.elf || exit
  cp -af build/bootstrap/ape.macho o//ape/ape.macho || exit
else
  echo "no cosmopolitan libc repository here" >&2
  echo "fetching ape loader from justine.lol"  >&2
  mkdir -p o//ape || exit
  if command -v wget >/dev/null 2>&1; then
    wget -qO o//ape/ape.elf https://justine.lol/ape.elf || exit
    wget -qO o//ape/ape.macho https://justine.lol/ape.macho || exit
  else
    curl -Rso o//ape/ape.elf https://justine.lol/ape.elf || exit
    curl -Rso o//ape/ape.macho https://justine.lol/ape.macho || exit
  fi
  chmod +x o//ape/ape.elf || exit
  chmod +x o//ape/ape.macho || exit
fi

if [ "$(uname -s)" = "Darwin" ]; then
  if ! [ /usr/bin/ape -nt o//ape/ape.macho ]; then
    echo >&2
    echo "installing o//ape/ape.elf to /usr/bin/ape" >&2
    echo "$SUDO mv -f o//ape/ape.elf /usr/bin/ape" >&2
    $SUDO cp -f o//ape/ape.macho /usr/bin/ape || exit
    echo "done" >&2
  fi
else
  if ! [ /usr/bin/ape -nt o//ape/ape.elf ]; then
    echo >&2
    echo "installing o//ape/ape.elf to /usr/bin/ape" >&2
    echo "$SUDO mv -f o//ape/ape.elf /usr/bin/ape" >&2
    $SUDO cp -f o//ape/ape.elf /usr/bin/ape || exit
    echo "done" >&2
  fi
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
