#!/bin/sh
m=tinylinux
t=/tmp/pledge-test

if [ $# = 0 ]; then
  if ! [ $(id -u) = 0 ]; then
    make -j16 MODE=fastbuild \
         o/fastbuild/examples/ls \
         o/fastbuild/tool/curl/curl \
         o/fastbuild/examples/life \
         o/fastbuild/examples/hello \
         o/fastbuild/examples/printargs \
         o/fastbuild/tool/build/assimilate \
         o/fastbuild/tool/build/pledge || exit
    make -j16 MODE=$m \
         o/$m/examples/ls \
         o/$m/tool/curl/curl \
         o/$m/examples/life \
         o/$m/examples/hello \
         o/$m/examples/printargs \
         o/$m/tool/build/assimilate \
         o/$m/tool/build/pledge || exit
    test/tool/build/pledge_test.sh ape_binfmt_test_suite || exit
    test/tool/build/pledge_test.sh ape_loader_test_suite || exit
    test/tool/build/pledge_test.sh ape_assimilated_test_suite || exit
    test/tool/build/pledge_test.sh ape_native_test_suite || exit
    sudo test/tool/build/pledge_test.sh setuid_setup || exit
    test/tool/build/pledge_test.sh setuid_test_suite || exit
  else
    echo need to run as an unprivileged user with sudo access >&2
    exit 1
  fi
fi

check() {
  if [ $? = 0 ]; then
    printf '\e[32mok\e[0m\n'
  else
    echo failed >&2
    exit 1
  fi
}

startit() {
  printf 'testing %-30s ' "$*" >&2
}

checkem() {
  if [ $? = 0 ]; then
    printf '\e[1;32mOK\e[0m\n'
  else
    printf '\e[1;31mFAILED\e[0m\n'
    exit 1
  fi
}

if [ "$1" = setuid_setup ]; then

  rm -rf $t || exit
  mkdir -p $t || exit
  chmod 01777 $t || exit
  cp o/$m/tool/build/pledge $t || exit
  chmod 06755 $t/pledge || exit

elif [ "$1" = ape_binfmt_test_suite ]; then

  ape/apeinstall.sh >/dev/null 2>&1

  startit ape binfmt life
  o/fastbuild/tool/build/pledge -p 'stdio rpath prot_exec' o/fastbuild/examples/life
  [ $? = 42 ]
  checkem

  startit ape binfmt hello
  [ "$(o/fastbuild/tool/build/pledge -p 'stdio rpath prot_exec' o/fastbuild/examples/hello)" = "hello world" ]
  checkem

  startit ape binfmt curl
  [ "$(o/fastbuild/tool/build/pledge -p 'stdio inet dns rpath prot_exec' o/fastbuild/tool/curl/curl https://justine.lol/hello.txt)" = "hello world" ]
  checkem

elif [ "$1" = ape_loader_test_suite ]; then

  ape/apeuninstall.sh >/dev/null 2>&1

  startit ape loader life
  o/fastbuild/tool/build/pledge -p 'stdio rpath prot_exec' o/fastbuild/examples/life
  [ $? = 42 ]
  checkem

  startit ape loader hello
  [ "$(o/fastbuild/tool/build/pledge -p 'stdio rpath prot_exec' o/fastbuild/examples/hello)" = "hello world" ]
  checkem

  startit ape loader curl
  [ "$(o/fastbuild/tool/build/pledge -p 'stdio inet dns rpath prot_exec' o/fastbuild/tool/curl/curl https://justine.lol/hello.txt)" = "hello world" ]
  checkem

  ape/apeinstall.sh >/dev/null 2>&1

elif [ "$1" = ape_assimilated_test_suite ]; then

  mkdir -p $t/assimilated

  startit ape assimilated life
  cp o/fastbuild/examples/life $t/assimilated
  o/fastbuild/tool/build/assimilate $t/assimilated/life
  o/$m/tool/build/pledge -p 'stdio' $t/assimilated/life
  [ $? = 42 ]
  checkem

  startit ape assimilated hello
  cp o/fastbuild/examples/hello $t/assimilated
  o/fastbuild/tool/build/assimilate $t/assimilated/hello
  [ "$(o/$m/tool/build/pledge -p 'stdio' $t/assimilated/hello)" = "hello world" ]
  checkem

  startit ape assimilated curl
  cp o/fastbuild/tool/curl/curl $t/assimilated
  o/fastbuild/tool/build/assimilate $t/assimilated/curl
  [ "$(o/$m/tool/build/pledge -p 'stdio rpath inet dns' $t/assimilated/curl https://justine.lol/hello.txt)" = "hello world" ]
  checkem

elif [ "$1" = ape_native_test_suite ]; then

  startit ape native life
  o/$m/tool/build/pledge -p 'stdio' o/$m/examples/life
  [ $? = 42 ]
  checkem

  startit ape native hello
  [ "$(o/$m/tool/build/pledge -p 'stdio' o/$m/examples/hello)" = "hello world" ]
  checkem

  startit ape native curl
  [ "$(o/$m/tool/build/pledge -p 'stdio rpath inet dns' o/$m/tool/curl/curl https://justine.lol/hello.txt)" = "hello world" ]
  checkem

elif [ "$1" = setuid_test_suite ]; then

  startit setuid life
  $t/pledge -p 'stdio' o/$m/examples/life
  [ $? = 42 ]
  checkem

  startit setuid hello
  [ "$($t/pledge -p 'stdio' o/$m/examples/hello)" = "hello world" ]
  checkem

  startit setuid curl
  [ "$($t/pledge -p 'stdio rpath inet dns' o/$m/tool/curl/curl https://justine.lol/hello.txt)" = "hello world" ]
  checkem

  startit setuid getuid
  [ "$($t/pledge -p 'stdio rpath proc tty' o/$m/examples/printargs 2>&1 | grep getuid | grep -o [[:digit:]]*)" = "$(id -u)" ]
  checkem

  startit setuid geteuid
  [ "$($t/pledge -p 'stdio rpath proc tty' o/$m/examples/printargs 2>&1 | grep geteuid | grep -o [[:digit:]]*)" = "$(id -u)" ]
  checkem

  startit setuid no capabilities
  [ "$($t/pledge -p 'stdio rpath proc tty' o/$m/examples/printargs 2>&1 | grep CAP_ | wc -l)" = 0 ]
  checkem

  startit setuid maximum nice
  $t/pledge -np 'stdio rpath proc tty' o/$m/examples/printargs 2>&1 | grep SCHED_IDLE >/dev/null
  checkem

  startit setuid chroot
  mkdir $t/jail &&
  touch $t/jail/hi &&
  cp o/$m/examples/ls $t/jail &&
  $t/pledge -v / -c $t/jail -p 'stdio rpath' /ls / | grep 'DT_REG     /hi' >/dev/null
  checkem

fi
