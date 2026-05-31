set(drivers
    builtin
    everest
    p256-m
    pqcp
)

# Drivers currently depend on each other through the indirect inclusion of
# `crypto_driver_contexts_*.h` headers, so all driver include paths must be
# visible when building each driver. TF_PSA_CRYPTO_DRIVERS_INCLUDE_DIRS
# provides these include paths.
set(TF_PSA_CRYPTO_DRIVERS_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/builtin/include
        ${CMAKE_CURRENT_SOURCE_DIR}/everest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/p256-m
        ${CMAKE_CURRENT_SOURCE_DIR}/pqcp/include
)

if(CMAKE_COMPILER_IS_MSVC)
    if(MSVC_STATIC_RUNTIME)
        foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS_CHECK)
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endforeach(flag_var)
    endif()
endif()

if(TF_PSA_CRYPTO_TEST_DRIVER)
    # Use `libtestdriver1` as the driver name to align with the name used so
    # far for driver dispatch testing in Mbed TLS. This allows to reuse most
    # of the test driver code as it is.
    list(APPEND drivers libtestdriver1)
    list(APPEND TF_PSA_CRYPTO_DRIVERS_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/libtestdriver1/include)

    # TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS:
    #   The list of targets involved in generating the test drivers. Test code
    #   includes driver headers, and drivers indirectly include headers from
    #   other drivers, so driver and test object targets must not start building
    #   until test driver generation has completed. The variable
    #   TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS provides the list of generation
    #   targets that driver and test targets should depend on.
    set(TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS "libtestdriver1_generation")
    set(TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS
        "${TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS}" PARENT_SCOPE)

    # In the build tree, create a `drivers/libtestdriver1` directory, copy
    # `tests/scripts/libtestdriver1.cmake` to it as `CMakeLists.txt`, and add
    # that directory as a subdirectory. This causes the test driver tree
    # defined in `libtestdriver1.cmake` to be configured and built as part of
    # the main build. The libtestdriver1 code is generated and compiled in the
    # build tree, similarly to an in-source-tree build.
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libtestdriver1)
    configure_file(
        ${PROJECT_SOURCE_DIR}/tests/scripts/libtestdriver1.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libtestdriver1/CMakeLists.txt
        COPYONLY
    )
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/libtestdriver1
                     ${CMAKE_CURRENT_BINARY_DIR}/libtestdriver1)
endif()

# Lists of driver targets.
#
# TF_PSA_CRYPTO_DRIVER_STATIC_TARGETS:
#   The list of driver targets to use when building the static library
#   (defined only if USE_STATIC_TF_PSA_CRYPTO_LIBRARY is enabled).
#
# TF_PSA_CRYPTO_DRIVER_TARGETS:
#   The list of driver targets to use when building the shared library.
#   If shared-library support is disabled, this list is identical to
#   TF_PSA_CRYPTO_DRIVER_STATIC_TARGETS.
#
# Notes:
# - TF_PSA_CRYPTO_DRIVER_TARGETS is not limited to shared builds; it may be
#   used whenever the static/shared distinction does not matter (e.g. when
#   collecting include directories from driver targets).
#
set(driver_targets "")
foreach(driver ${drivers})
    list(APPEND driver_targets "${TF_PSA_CRYPTO_TARGET_PREFIX}${driver}")
endforeach()

if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    if(NOT USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
        set(driver_static_targets ${driver_targets})
    else()
        set(driver_static_targets "")
        foreach(target ${driver_targets})
            list(APPEND driver_static_targets "${target}_static")
        endforeach()
    endif()
endif()

set(TF_PSA_CRYPTO_DRIVERS "${drivers}" PARENT_SCOPE)
set(TF_PSA_CRYPTO_DRIVER_STATIC_TARGETS "${driver_static_targets}" PARENT_SCOPE)
set(TF_PSA_CRYPTO_DRIVER_TARGETS "${driver_targets}" PARENT_SCOPE)

add_subdirectory(everest)
add_subdirectory(p256-m)
add_subdirectory(builtin)
add_subdirectory(pqcp)

#
# Add to the list of directories with internal headers the driver ones that
# must be visible by core and tests code.
# Done after adding the driver sub-directories as drivers do not need to see
# each other internal headers.
#
list(APPEND TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/builtin/src
        ${CMAKE_CURRENT_SOURCE_DIR}/pqcp/src
)
set(TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS
        "${TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS}" PARENT_SCOPE)
