#!/bin/sh
m=tinylinux
t=/tmp/pledge-test

if [ $# = 0 ]; then
  if ! [ $(id -u) = 0 ]; then
    make -j16 MODE=fastbuild \
         o/fastbuild/examples/ls.com \
         o/fastbuild/tool/curl/curl.com \
         o/fastbuild/examples/life.com \
         o/fastbuild/examples/hello.com \
         o/fastbuild/examples/printargs.com \
         o/fastbuild/tool/build/assimilate.com \
         o/fastbuild/tool/build/pledge.com || exit
    make -j16 MODE=$m \
         o/$m/examples/ls.com \
         o/$m/tool/curl/curl.com \
         o/$m/examples/life.com \
         o/$m/examples/hello.com \
         o/$m/examples/printargs.com \
         o/$m/tool/build/assimilate.com \
         o/$m/tool/build/pledge.com || exit
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
  cp o/$m/tool/build/pledge.com $t || exit
  chmod 06755 $t/pledge.com || exit

elif [ "$1" = ape_binfmt_test_suite ]; then

  ape/apeinstall.sh >/dev/null 2>&1

  startit ape binfmt life.com
  o/fastbuild/tool/build/pledge.com -p 'stdio rpath prot_exec' o/fastbuild/examples/life.com
  [ $? = 42 ]
  checkem

  startit ape binfmt hello.com
  [ "$(o/fastbuild/tool/build/pledge.com -p 'stdio rpath prot_exec' o/fastbuild/examples/hello.com)" = "hello world" ]
  checkem

  startit ape binfmt curl.com
  [ "$(o/fastbuild/tool/build/pledge.com -p 'stdio inet dns rpath prot_exec' o/fastbuild/tool/curl/curl.com https://justine.lol/hello.txt)" = "hello world" ]
  checkem

elif [ "$1" = ape_loader_test_suite ]; then

  ape/apeuninstall.sh >/dev/null 2>&1

  startit ape loader life.com
  o/fastbuild/tool/build/pledge.com -p 'stdio rpath prot_exec' o/fastbuild/examples/life.com
  [ $? = 42 ]
  checkem

  startit ape loader hello.com
  [ "$(o/fastbuild/tool/build/pledge.com -p 'stdio rpath prot_exec' o/fastbuild/examples/hello.com)" = "hello world" ]
  checkem

  startit ape loader curl.com
  [ "$(o/fastbuild/tool/build/pledge.com -p 'stdio inet dns rpath prot_exec' o/fastbuild/tool/curl/curl.com https://justine.lol/hello.txt)" = "hello world" ]
  checkem

  ape/apeinstall.sh >/dev/null 2>&1

elif [ "$1" = ape_assimilated_test_suite ]; then

  mkdir -p $t/assimilated

  startit ape assimilated life.com
  cp o/fastbuild/examples/life.com $t/assimilated
  o/fastbuild/tool/build/assimilate.com $t/assimilated/life.com
  o/$m/tool/build/pledge.com -p 'stdio' $t/assimilated/life.com
  [ $? = 42 ]
  checkem

  startit ape assimilated hello.com
  cp o/fastbuild/examples/hello.com $t/assimilated
  o/fastbuild/tool/build/assimilate.com $t/assimilated/hello.com
  [ "$(o/$m/tool/build/pledge.com -p 'stdio' $t/assimilated/hello.com)" = "hello world" ]
  checkem

  startit ape assimilated curl.com
  cp o/fastbuild/tool/curl/curl.com $t/assimilated
  o/fastbuild/tool/build/assimilate.com $t/assimilated/curl.com
  [ "$(o/$m/tool/build/pledge.com -p 'stdio rpath inet dns' $t/assimilated/curl.com https://justine.lol/hello.txt)" = "hello world" ]
  checkem

elif [ "$1" = ape_native_test_suite ]; then

  startit ape native life.com
  o/$m/tool/build/pledge.com -p 'stdio' o/$m/examples/life.com
  [ $? = 42 ]
  checkem

  startit ape native hello.com
  [ "$(o/$m/tool/build/pledge.com -p 'stdio' o/$m/examples/hello.com)" = "hello world" ]
  checkem

  startit ape native curl.com
  [ "$(o/$m/tool/build/pledge.com -p 'stdio rpath inet dns' o/$m/tool/curl/curl.com https://justine.lol/hello.txt)" = "hello world" ]
  checkem

elif [ "$1" = setuid_test_suite ]; then

  startit setuid life.com
  $t/pledge.com -p 'stdio' o/$m/examples/life.com
  [ $? = 42 ]
  checkem

  startit setuid hello.com
  [ "$($t/pledge.com -p 'stdio' o/$m/examples/hello.com)" = "hello world" ]
  checkem

  startit setuid curl.com
  [ "$($t/pledge.com -p 'stdio rpath inet dns' o/$m/tool/curl/curl.com https://justine.lol/hello.txt)" = "hello world" ]
  checkem

  startit setuid getuid
  [ "$($t/pledge.com -p 'stdio rpath proc tty' o/$m/examples/printargs.com 2>&1 | grep getuid | grep -o [[:digit:]]*)" = "$(id -u)" ]
  checkem

  startit setuid geteuid
  [ "$($t/pledge.com -p 'stdio rpath proc tty' o/$m/examples/printargs.com 2>&1 | grep geteuid | grep -o [[:digit:]]*)" = "$(id -u)" ]
  checkem

  startit setuid no capabilities
  [ "$($t/pledge.com -p 'stdio rpath proc tty' o/$m/examples/printargs.com 2>&1 | grep CAP_ | wc -l)" = 0 ]
  checkem

  startit setuid maximum nice
  $t/pledge.com -np 'stdio rpath proc tty' o/$m/examples/printargs.com 2>&1 | grep SCHED_IDLE >/dev/null
  checkem

  startit setuid chroot
  mkdir $t/jail &&
  touch $t/jail/hi &&
  cp o/$m/examples/ls.com $t/jail &&
  $t/pledge.com -v / -c $t/jail -p 'stdio rpath' /ls.com / | grep 'DT_REG     /hi' >/dev/null
  checkem

fi
