#!/bin/sh
# clean.sh — remove all upstream mbedTLS 4 source files from this directory,
# leaving only the Cosmopolitan-specific files:
#
#   BUILD.mk
#   README.cosmo (this file's sibling)
#   clean.sh     (this script)
#   update.sh    (this file's sibling)
#   include/mbedtls/mbedtls_config.h   (Cosmo-owned config)
#   include/mbedtls/platform.h         (Cosmo-owned shim)
#   include/mbedtls/private_access.h   (Cosmo-owned shim)
#
# After running this script, run update.sh to restore the upstream source.
# Do not run make directly after clean.sh without first running update.sh.

set -e
cd "$(dirname "$0")"

# Generated wrappers — safe to remove, recreated by build/mkmbedtls4wrappers.py
rm -rf generated

# Upstream directories — safe to remove entirely.
rm -rf \
    3rdparty \
    ChangeLog.d \
    cmake \
    configs \
    docs \
    doxygen \
    framework \
    library \
    pkgconfig \
    programs \
    scripts \
    tests \
    tf-psa-crypto

# include/ contains mostly upstream headers but three files are Cosmo-owned:
#   mbedtls_config.h, platform.h, private_access.h
# Remove only the upstream subdirectory content, then restore the Cosmo files.
if [ -d include ]; then
    find include -type f \
        ! -name 'mbedtls_config.h' \
        ! -name 'platform.h' \
        ! -name 'private_access.h' \
        -delete
    find include -type d -empty -delete
fi

# Upstream top-level files
rm -f \
    BRANCHES.md \
    BUGS.md \
    ChangeLog \
    CMakeLists.txt \
    CONTRIBUTING.md \
    DartConfiguration.tcl \
    dco.txt \
    LICENSE \
    README.md \
    SECURITY.md \
    SUPPORT.md


echo "Done. Upstream mbedTLS 4 source removed."
echo "Cosmopolitan files retained: BUILD.mk, README.cosmo, clean.sh, update.sh"
echo "  include/mbedtls/{mbedtls_config.h,platform.h,private_access.h} preserved."
echo "Run 'update.sh <version>' to restore upstream source."
