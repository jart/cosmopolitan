## Common shell functions used by demo scripts programs/*/*.sh.

## How to write a demo script
## ==========================
##
## Include this file near the top of each demo script:
##   . "${0%/*}/demo_common.sh"
##
## Start with a "msg" call that explains the purpose of the script.
## Then call the "depends_on" function to ensure that all config
## dependencies are met.
##
## As the last thing in the script, call the cleanup function.
##
## You can use the functions and variables described below.

set -e -u

# Check if the provided path ($1) can be a valid root for Mbed TLS or TF-PSA-Crypto.
# This is based on the fact that "scripts/project_name.txt" exists.
is_valid_root () {
  if ! [ -f "$1/scripts/project_name.txt" ]; then
    return 1;
  fi
  return 0;
}

## At the end of the while loop below $root_dir will point to the root directory
## of the Mbed TLS or TF-PSA-Crypto source tree.
root_dir="${0%/*}"
## Find a nice path to the root directory, avoiding unnecessary "../".
##
## The code supports demo scripts nested up to 4 levels deep.
##
## The code works no matter where the demo script is relative to the current
## directory, even if it is called with a relative path.
n=4
while true; do
  # If we went up too many folders, then give up and return a failure.
  if [ $n -eq 0 ]; then
    echo >&2 "This doesn't seem to be an Mbed TLS source tree."
    exit 125
  fi

  if is_valid_root "$root_dir"; then
    break;
  fi

  n=$((n - 1))
  case $root_dir in
    .) root_dir="..";;
    ..|?*/..) root_dir="$root_dir/..";;
    ?*/*) root_dir="${root_dir%/*}";;
    /*) root_dir="/";;
    *) root_dir=".";;
  esac
done

# Now that we have a root path we can source the "project_detection.sh" script.
. "$root_dir/framework/scripts/project_detection.sh"

## msg LINE...
## msg <TEXT_ORIGIN
## Display an informational message.
msg () {
  if [ $# -eq 0 ]; then
    sed 's/^/# /'
  else
    for x in "$@"; do
      echo "# $x"
    done
  fi
}

## run "Message" COMMAND ARGUMENT...
## Display the message, then run COMMAND with the specified arguments.
run () {
    echo
    echo "# $1"
    shift
    echo "+ $*"
    "$@"
}

## Like '!', but stop on failure with 'set -e'
not () {
  if "$@"; then false; fi
}

## run_bad "Message" COMMAND ARGUMENT...
## Like run, but the command is expected to fail.
run_bad () {
  echo
  echo "$1 This must fail."
  shift
  echo "+ ! $*"
  not "$@"
}

## This check is temporary and it's due to the fact that we currently build
## query_compile_time_config only in Mbed TLS repo and not in the TF-PSA-Crypto
## one. Once we'll have in both repos this check can be removed.
has_query_compile_time_config () {
  if ! [ -f "$1/programs/test/query_compile_time_config" ]; then
    return 1;
  fi
  return 0;
}

if has_query_compile_time_config "$root_dir"; then
  query_compile_time_config_dir="$root_dir/programs/test"
elif is_valid_root "$root_dir/.." && has_query_compile_time_config "$root_dir/.."; then
  query_compile_time_config_dir="$root_dir/../programs/test"
else
  query_compile_time_config_dir=""
fi

## config_has SYMBOL...
## Succeeds if the library configuration has all SYMBOLs set.
##
## Note: depending on the above check query_compile_time_config_dir might be
##       intentionally set to "". In this case the following function will fail.
config_has () {
  for x in "$@"; do
    # This function is commonly called in an if condition, where "set -e"
    # has no effect, so make sure to stop explicitly on error.
    "$query_compile_time_config_dir/query_compile_time_config" "$x" || return $?
  done
}

## depends_on SYMBOL...
## Exit if the library configuration does not have all SYMBOLs set.
depends_on () {
  if ! [ -f "$query_compile_time_config_dir/query_compile_time_config" ]; then
    echo "query_compile_time_config is missing"
    exit 127
  fi
  m=
  for x in "$@"; do
    if ! config_has "$x"; then
      m="$m $x"
    fi
  done
  if [ -n "$m" ]; then
    cat >&2 <<EOF
$0: this demo requires the following
configuration options to be enabled at compile time:
 $m
EOF
    # Exit with a success status so that this counts as a pass for run_demos.py.
    exit
  fi
}

## Add the names of files to clean up to this whitespace-separated variable.
## The file names must not contain whitespace characters.
files_to_clean=

## Call this function at the end of each script.
## It is called automatically if the script is killed by a signal.
cleanup () {
  rm -f -- $files_to_clean
}



################################################################
## End of the public interfaces. Code beyond this point is not
## meant to be called directly from a demo script.

trap 'cleanup; trap - HUP; kill -HUP $$' HUP
trap 'cleanup; trap - INT; kill -INT $$' INT
trap 'cleanup; trap - TERM; kill -TERM $$' TERM

if config_has MBEDTLS_ENTROPY_NV_SEED; then
  # Create a seedfile that's sufficiently long in all library configurations.
  # This is necessary for programs that use randomness.
  # Assume that the name of the seedfile is the default name.
  files_to_clean="$files_to_clean seedfile"
  dd if=/dev/urandom of=seedfile ibs=64 obs=64 count=1
fi
