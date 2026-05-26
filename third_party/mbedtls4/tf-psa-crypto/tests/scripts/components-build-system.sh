# components-build-system.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later

# This file contains test components that are executed by all.sh

################################################################
#### Build System Testing
################################################################

component_test_tf_psa_crypto_shared () {
    msg "build/test: shared libraries" # ~ 2min
    # We're not building in the OUT_OF_SOURCE_DIR directory in this case
    # because we want "tfpsacrypto_dlopen" program to be accessible as
    # "<tf-psa-crypto-root>/programs/test/tfpsacrypto_dlopen"
    # in dlopen_demo.sh below.
    cmake -DUSE_SHARED_TF_PSA_CRYPTO_LIBRARY=ON "$TF_PSA_CRYPTO_ROOT_DIR"
    make
    ldd programs/test/benchmark | grep libtfpsacrypto
    make test
    $FRAMEWORK/tests/programs/dlopen_demo.sh
}

component_test_tf_psa_crypto_out_of_source () {
    msg "build: cmake tf-psa-crypto 'out-of-source' build"
    cd $OUT_OF_SOURCE_DIR
    # Note: Explicitly generate files as these are turned off in releases
    cmake -D CMAKE_BUILD_TYPE:String=Check -D GEN_FILES=ON "$TF_PSA_CRYPTO_ROOT_DIR"
    make
    msg "test: cmake tf-psa-crypto 'out-of-source' build"
    make test
}

component_test_tf_psa_crypto_as_subdirectory () {
    msg "build: cmake 'as-subdirectory' build"
    cd programs/test/cmake_subproject
    # Note: Explicitly generate files as these are turned off in releases
    cmake -D GEN_FILES=ON .
    make
    ./cmake_subproject
}

component_test_tf_psa_crypto_as_package () {
    msg "build: cmake 'as-package' build"
    root_dir="$(pwd)"
    cd programs/test/cmake_package
    build_variant_dir="$(pwd)"
    cmake .
    make
    ./cmake_package
    if [[ "$OSTYPE" == linux* ]]; then
        PKG_CONFIG_PATH="${build_variant_dir}/tf-psa-crypto/pkgconfig" \
        ${root_dir}/framework/scripts/pkgconfig.sh \
        tfpsacrypto
        # This is the EXPECTED package name. Renaming it could break consumers
        # of pkg-config, consider carefully.
    fi
}

component_test_tf_psa_crypto_cmake_as_package_install () {
    msg "build: cmake 'as-installed-package' build"
    cd programs/test/cmake_package_install
    # Note: Explicitly generate files as these are turned off in releases
    cmake .
    make
    ./cmake_package_install
}

component_tf_psa_crypto_build_custom_config_file () {
    # Make a copy of config file
    cp "$CRYPTO_CONFIG_H" include/psa/crypto_config_default.h

    # Build once to get the generated files (for which need an intact config file)
    msg "build: cmake out-of-source with default config file"
    cd "$OUT_OF_SOURCE_DIR"
    cmake "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "build: cmake out-of-source with -DMBEDTLS_CONFIG_FILE"
    cd "$TF_PSA_CRYPTO_ROOT_DIR"
    cp "$CRYPTO_CONFIG_H" "$OUT_OF_SOURCE_DIR/crypto_config_custom.h"
    cd "$OUT_OF_SOURCE_DIR"
    # Force a build failure if the default crypto_config.h header file is included
    # in the build instead of the one we pass to CMake on the command line.
    echo '#error "cmake -DMBEDTLS_CONFIG_FILE is not working."' > "$TF_PSA_CRYPTO_ROOT_DIR/$CRYPTO_CONFIG_H"
    cmake -DGEN_FILES=OFF -DTF_PSA_CRYPTO_CONFIG_FILE=crypto_config_custom.h "$TF_PSA_CRYPTO_ROOT_DIR"
    make

    msg "build: cmake out-of-source with -DTF_PSA_CRYPTO_CONFIG_FILE + -DTF_PSA_CRYPTO_USER_CONFIG_FILE"
    # In the user config, disable one feature (for simplicity, pick a feature
    # that nothing else depends on).
    echo '#undef MBEDTLS_NIST_KW_C' >crypto_config_user.h
    # Before rebuilding the library let's verify that the feature was present in the previous build.
    grep -q mbedtls_nist_kw_wrap core/libtfpsacrypto.a
    cmake -DGEN_FILES=OFF -DTF_PSA_CRYPTO_CONFIG_FILE=crypto_config_custom.h \
            -DTF_PSA_CRYPTO_USER_CONFIG_FILE=crypto_config_user.h "$TF_PSA_CRYPTO_ROOT_DIR"
    make
    # Verify that MBEDTLS_NIST_KW_C was really disabled, i.e. crypto_config_user.h was included
    # correctly in the build.
    not grep -q mbedtls_nist_kw_wrap core/libtfpsacrypto.a

    cd "$TF_PSA_CRYPTO_ROOT_DIR"
    rm -rf "$OUT_OF_SOURCE_DIR"

    # Now repeat the test for an in-tree build:

    # Restore config for the in-tree test
    cp include/psa/crypto_config_default.h "$CRYPTO_CONFIG_H"

    # Build once to get the generated files (for which need an intact config)
    msg "build: cmake in-tree with default config file"
    cmake .
    make

    msg "build: cmake in-tree with -DTF_PSA_CRYPTO_CONFIG_FILE"
    cp include/psa/crypto_config.h crypto_config_custom.h

    echo '#error "cmake -DMBEDTLS_CONFIG_FILE is not working."' > "$TF_PSA_CRYPTO_ROOT_DIR/$CRYPTO_CONFIG_H"
    cmake -DGEN_FILES=OFF -DTF_PSA_CRYPTO_CONFIG_FILE=crypto_config_custom.h .
    make

    msg "build: cmake in-tree with -DTF_PSA_CRYPTO_CONFIG_FILE + -DTF_PSA_CRYPTO_USER_CONFIG_FILE"
    # In the user config, disable one feature (for simplicity, pick a feature
    # that nothing else depends on).
    echo '#undef MBEDTLS_NIST_KW_C' >crypto_config_user.h
    # Before rebuilding the library let's verify that the feature was present in the previous build.
    grep -q mbedtls_nist_kw_wrap core/libtfpsacrypto.a
    cmake -DGEN_FILES=OFF -DTF_PSA_CRYPTO_CONFIG_FILE=crypto_config_custom.h -DTF_PSA_CRYPTO_USER_CONFIG_FILE=crypto_config_user.h .
    make
    # Verify that MBEDTLS_NIST_KW_C was really disabled, i.e. crypto_config_user.h was included
    # correctly in the build.
    not grep -q mbedtls_nist_kw_wrap core/libtfpsacrypto.a

    # Restore default crypto config file and remove generated ones
    mv include/psa/crypto_config_default.h "$CRYPTO_CONFIG_H"
    rm -f crypto_config_custom.h crypto_config_user.h
}

component_tf_psa_crypto_build_config_name () {
    cp "$CRYPTO_CONFIG_H" include/psa/crypto_config_default.h
    scripts/config.py full
    cp "$CRYPTO_CONFIG_H" include/psa/crypto_config_full.h
    cp include/psa/crypto_config_default.h "$CRYPTO_CONFIG_H"

    msg "build: cmake out-of-source with full config"
    cd "$OUT_OF_SOURCE_DIR"
    cmake -DCMAKE_INSTALL_PREFIX="$OUT_OF_SOURCE_DIR/install_full" -DTF_PSA_CRYPTO_CONFIG_NAME=full "$TF_PSA_CRYPTO_ROOT_DIR"
    cd "$TF_PSA_CRYPTO_ROOT_DIR"
    echo '#error "cmake -DTF_PSA_CRYPTO_CONFIG_NAME=full does not work"' > "$CRYPTO_CONFIG_H"
    cmake --build "$OUT_OF_SOURCE_DIR" --target tfpsacrypto

    # Restore the default config file in the source tree as we need a sane
    # one to build some PSA test data.
    cp include/psa/crypto_config_default.h "$CRYPTO_CONFIG_H"
    cmake --build "$OUT_OF_SOURCE_DIR"
    cmake --build "$OUT_OF_SOURCE_DIR" --target install

    cmp -s $OUT_OF_SOURCE_DIR/include/psa/crypto_config.h include/psa/crypto_config_full.h
    cmp -s $OUT_OF_SOURCE_DIR/install_full/include/psa/crypto_config.h include/psa/crypto_config_full.h

    rm -rf "$OUT_OF_SOURCE_DIR"
    mv include/psa/crypto_config_default.h "$CRYPTO_CONFIG_H"
    rm -f include/psa/crypto_config_full.h
}

component_tf_psa_crypto_build_programs_no_testing () {
    # Verify that the type of builds performed by oss-fuzz don't get accidentally broken
    msg "build: cmake with -DENABLE_PROGRAMS=ON and -DENABLE_TESTING=OFF"
    cd "$OUT_OF_SOURCE_DIR"
    cmake -DENABLE_PROGRAMS=ON -DENABLE_TESTING=OFF "$TF_PSA_CRYPTO_ROOT_DIR"
    make
}
