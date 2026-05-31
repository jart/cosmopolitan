# CMake module for building the object libraries that are linked to the
# TF-PSA-Crypto library but the driver object libraries.
# This file is meant to be included from the `CMakeLists.txt` of dispatch,
# extras, platform and utilities directories, e.g. in `extras/CMakeLists.txt`.
#
# The following variables must be defined by the caller before including this
# module:
# - tf_psa_crypto_objlib: the object library name
# - ${tf_psa_crypto_objlib}_src_files: the list of source files for the
#   object library.

set(${tf_psa_crypto_objlib}_target ${TF_PSA_CRYPTO_TARGET_PREFIX}${tf_psa_crypto_objlib})
if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    set(${tf_psa_crypto_objlib}_static_target ${${tf_psa_crypto_objlib}_target})
endif()
set(target_libraries ${${tf_psa_crypto_objlib}_target})
if(USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    string(APPEND ${tf_psa_crypto_objlib}_static_target "_static")
    list(APPEND target_libraries ${${tf_psa_crypto_objlib}_static_target})
endif()

foreach (target IN LISTS target_libraries)
    add_library(${target} OBJECT ${${tf_psa_crypto_objlib}_src_files})
    tf_psa_crypto_set_base_compile_options(${target})
    tf_psa_crypto_set_extra_compile_options(${target})

    target_include_directories(${target}
      PRIVATE # Add the build-tree include directory before the source-tree one
              # so that generated headers in the build tree take precedence.
              ${PROJECT_BINARY_DIR}/include
              ${PROJECT_SOURCE_DIR}/include
              ${TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS})
    foreach(driver_target ${TF_PSA_CRYPTO_DRIVER_TARGETS})
        get_target_property(public_includes ${driver_target} INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir IN LISTS public_includes)
            target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${dir}>)
        endforeach()
    endforeach()
    tf_psa_crypto_set_config_files_compile_definitions(${target})
    if(TF_PSA_CRYPTO_TEST_DRIVER)
        add_dependencies(${target} ${TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS})
    endif()
endforeach(target)

if(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    set_property(TARGET ${${tf_psa_crypto_objlib}_target} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)

# Lists of object library targets.
#
# TF_PSA_CRYPTO_OBJLIB_STATIC_TARGETS:
#   The list of object library targets to use when building the static library
#   (defined only if USE_STATIC_TF_PSA_CRYPTO_LIBRARY is enabled).
#
# TF_PSA_CRYPTO_OBJLIB_TARGETS:
#   The list of driver targets to use when building the shared library.
#   If shared-library support is disabled, this list is identical to
#   TF_PSA_CRYPTO_DRIVER_STATIC_TARGETS.
#
# Notes:
# - TF_PSA_CRYPTO_OBJLIB_TARGETS is not limited to shared builds; it may be
#   used whenever the static/shared distinction does not matter (e.g. when
#   collecting include directories from object library targets).
#
list(APPEND TF_PSA_CRYPTO_OBJLIB_TARGETS ${${tf_psa_crypto_objlib}_target})
if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    list(APPEND TF_PSA_CRYPTO_OBJLIB_STATIC_TARGETS ${${tf_psa_crypto_objlib}_static_target})
endif()

set(TF_PSA_CRYPTO_OBJLIB_TARGETS "${TF_PSA_CRYPTO_OBJLIB_TARGETS}" PARENT_SCOPE)
set(TF_PSA_CRYPTO_OBJLIB_STATIC_TARGETS "${TF_PSA_CRYPTO_OBJLIB_STATIC_TARGETS}" PARENT_SCOPE)
