# CMake module for building drivers.
# This file is meant to be included from the `CMakeLists.txt` of drivers, e.g.
# in `builtin/CMakeLists.txt`.
#
# We do not use it for everest and p256-m for the time being, but it is one
# of the goals of https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/579 to fix
# that.
#
# The driver public headers should be located in the `drivers/*/include`
# directory.
#
# The following variables must be defined by the caller before including this
# module:
# - tf_psa_crypto_driver: the driver name (e.g. "builtin", "libtestdriver1")
# - ${tf_psa_crypto_driver}_src_files: the list of source files for the driver

set(${tf_psa_crypto_driver}_target ${TF_PSA_CRYPTO_TARGET_PREFIX}${tf_psa_crypto_driver})
if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    set(${tf_psa_crypto_driver}_static_target ${${tf_psa_crypto_driver}_target})
endif()
set(target_libraries ${${tf_psa_crypto_driver}_target})
if(USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    string(APPEND ${tf_psa_crypto_driver}_static_target "_static")
    list(APPEND target_libraries ${${tf_psa_crypto_driver}_static_target})
endif()

foreach (target IN LISTS target_libraries)
    add_library(${target} OBJECT ${${tf_psa_crypto_driver}_src_files})
    tf_psa_crypto_set_base_compile_options(${target})
    tf_psa_crypto_set_extra_compile_options(${target})

    target_include_directories(${target}
      PUBLIC include
      PRIVATE # Add the build-tree include directory before the source-tree one
              # so that generated headers in the build tree take precedence.
              ${PROJECT_BINARY_DIR}/include
              ${PROJECT_SOURCE_DIR}/include
              ${TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS}
              ${TF_PSA_CRYPTO_DRIVERS_INCLUDE_DIRS})
    tf_psa_crypto_set_config_files_compile_definitions(${target})
    if(TF_PSA_CRYPTO_TEST_DRIVER)
        add_dependencies(${target} ${TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS})
    endif()
endforeach(target)

if(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    set_property(TARGET ${${tf_psa_crypto_driver}_target} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)

if(INSTALL_TF_PSA_CRYPTO_HEADERS)
  install(DIRECTORY "include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h")
endif(INSTALL_TF_PSA_CRYPTO_HEADERS)
