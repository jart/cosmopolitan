#
# CMake build system design considerations:
#
# - Include directories:
#   + Do not define include directories globally using the include_directories
#     command but rather at the target level using the
#     target_include_directories command. That way, it is easier to guarantee
#     that targets are built using the proper list of include directories.
#   + Use the PUBLIC and PRIVATE keywords to specify the scope of include
#     directories. That way, a target linking to a library (using the
#     target_link_libraries command) inherits from the library PUBLIC include
#     directories and not from the PRIVATE ones.
# - TF_PSA_CRYPTO_TARGET_PREFIX: CMake targets are designed to be alterable by
#   calling CMake in order to avoid target name clashes, via the use of
#   TF_PSA_CRYPTO_TARGET_PREFIX. The value of this variable is prefixed to the
#   tfpsacrypto and tfpsacrypto-apidoc targets.
#

# We specify a minimum requirement of 3.10.2, but for now use 3.5.1 here
# until our infrastructure catches up.
cmake_minimum_required(VERSION 3.5.1)

include(CMakePackageConfigHelpers)

# Include convenience functions for printing properties and variables, like
# cmake_print_properties(), cmake_print_variables().
include(CMakePrintHelpers)

# https://cmake.org/cmake/help/latest/policy/CMP0011.html
# Setting this policy is required in CMake >= 3.18.0, otherwise a warning is generated. The OLD
# policy setting is deprecated, and will be removed in future versions.
cmake_policy(SET CMP0011 NEW)
# https://cmake.org/cmake/help/latest/policy/CMP0012.html
# Setting the CMP0012 policy to NEW is required for FindPython3 to work with CMake 3.18.2
# (there is a bug in this particular version), otherwise, setting the CMP0012 policy is required
# for CMake versions >= 3.18.3 otherwise a deprecated warning is generated. The OLD policy setting
# is deprecated and will be removed in future versions.
cmake_policy(SET CMP0012 NEW)

set(TF_PSA_CRYPTO_VERSION 1.1.0)
set(TF_PSA_CRYPTO_SOVERSION 2)

if(TEST_CPP)
    project("TF-PSA-Crypto"
        LANGUAGES C CXX
        VERSION ${TF_PSA_CRYPTO_VERSION}
    )
else()
    project("TF-PSA-Crypto"
        LANGUAGES C
        VERSION ${TF_PSA_CRYPTO_VERSION}
    )
endif()

include(GNUInstallDirs)

# Determine if TF-PSA-Crypto is being built as a subproject using add_subdirectory()
if(NOT DEFINED TF_PSA_CRYPTO_AS_SUBPROJECT)
  set(TF_PSA_CRYPTO_AS_SUBPROJECT ON)
  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(TF_PSA_CRYPTO_AS_SUBPROJECT OFF)
  endif()
endif()

# Set the project, Mbed TLS and framework root directory.
set(TF_PSA_CRYPTO_FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/framework)

# Put the version numbers into relevant files
set(version_number_files
        doxygen/input/doc_mainpage.h
        doxygen/tfpsacrypto.doxyfile)
foreach(file ${version_number_files})
    configure_file(${file}.in
                   ${PROJECT_SOURCE_DIR}/${file})
endforeach(file)

ADD_CUSTOM_TARGET(${TF_PSA_CRYPTO_TARGET_PREFIX}tfpsacrypto-apidoc
    COMMAND doxygen tfpsacrypto.doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)

option(ENABLE_PROGRAMS "Build TF-PSA-Crypto programs." ON)

option(TF_PSA_CRYPTO_FATAL_WARNINGS "Compiler warnings treated as errors" ON)
if(CMAKE_HOST_WIN32)
    # N.B. The comment on the next line is significant! If you change it,
    # edit the sed command in prepare_release.sh that modifies
    # CMakeLists.txt.
    option(GEN_FILES "Generate the auto-generated files as needed" OFF) # off in development
else()
    option(GEN_FILES "Generate the auto-generated files as needed" OFF)
endif()

# Support for package config and install to be added later.
option(DISABLE_PACKAGE_CONFIG_AND_INSTALL "Disable package configuration, target export and installation" ${TF_PSA_CRYPTO_AS_SUBPROJECT})

if (CMAKE_C_SIMULATE_ID)
    set(COMPILER_ID ${CMAKE_C_SIMULATE_ID})
else()
    set(COMPILER_ID ${CMAKE_C_COMPILER_ID})
endif(CMAKE_C_SIMULATE_ID)

string(REGEX MATCH "Clang" CMAKE_COMPILER_IS_CLANG "${COMPILER_ID}")
string(REGEX MATCH "GNU" CMAKE_COMPILER_IS_GNU "${COMPILER_ID}")
string(REGEX MATCH "IAR" CMAKE_COMPILER_IS_IAR "${COMPILER_ID}")
string(REGEX MATCH "MSVC" CMAKE_COMPILER_IS_MSVC "${COMPILER_ID}")

# the test suites currently have compile errors with MSVC
if(CMAKE_COMPILER_IS_MSVC)
    option(ENABLE_TESTING "Build TF-PSA-Crypto tests." OFF)
else()
    option(ENABLE_TESTING "Build TF-PSA-Crypto tests." ON)
endif()
option(TF_PSA_CRYPTO_TEST_DRIVER "Build TF-PSA-Crypto test driver for driver dispatch testing" OFF)

option(USE_STATIC_TF_PSA_CRYPTO_LIBRARY "Build TF-PSA-Crypto static library." ON)
option(USE_SHARED_TF_PSA_CRYPTO_LIBRARY "Build TF-PSA-Crypto shared library." OFF)
option(LINK_WITH_PTHREAD "Explicitly link Mbed TLS library to pthread." OFF)
option(LINK_WITH_TRUSTED_STORAGE "Explicitly link Mbed TLS library to trusted_storage." OFF)
option(MSVC_STATIC_RUNTIME "Build the libraries with /MT compiler flag" OFF)

set(tfpsacrypto_target "${TF_PSA_CRYPTO_TARGET_PREFIX}tfpsacrypto")
if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    set(tfpsacrypto_static_target ${tfpsacrypto_target})
endif()
if(USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    string(APPEND tfpsacrypto_static_target "_static")
endif()

# Python 3 is only needed here to check for configuration warnings.
if(NOT CMAKE_VERSION VERSION_LESS 3.15.0)
    set(Python3_FIND_STRATEGY LOCATION)
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        set(TF_PSA_CRYPTO_PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    endif()
else()
    find_package(PythonInterp 3)
    if(PYTHONINTERP_FOUND)
        set(TF_PSA_CRYPTO_PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})
    endif()
endif()

# We now potentially need to link all executables against PThreads, if available
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

# If this is the root project add longer list of available CMAKE_BUILD_TYPE values
if(NOT TF_PSA_CRYPTO_AS_SUBPROJECT)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
        CACHE STRING "Choose the type of build: None Debug Release Coverage ASan ASanDbg MemSan MemSanDbg Check CheckFull TSan TSanDbg"
        FORCE)
endif()

# Make TF_PSA_CRYPTO_CONFIG_FILE and TF_PSA_CRYPTO_USER_CONFIG_FILE into PATHs
set(TF_PSA_CRYPTO_CONFIG_FILE "" CACHE FILEPATH "TF-PSA-Crypto config file (overrides default).")
set(TF_PSA_CRYPTO_USER_CONFIG_FILE "" CACHE FILEPATH "TF-PSA-Crypto user config file (appended to default).")
set(TF_PSA_CRYPTO_CONFIG_NAME "" CACHE STRING "Name of the configuration to use (see config.py documentation)")

if(NOT "${TF_PSA_CRYPTO_CONFIG_NAME}" STREQUAL "")
    if(NOT "${TF_PSA_CRYPTO_CONFIG_FILE}" STREQUAL "")
        message(FATAL_ERROR "TF_PSA_CRYPTO_CONFIG_NAME and TF_PSA_CRYPTO_CONFIG_FILE are incompatible.")
    endif()
endif(NOT "${TF_PSA_CRYPTO_CONFIG_NAME}" STREQUAL "")

# Create a symbolic link from ${base_name} in the binary directory
# to the corresponding path in the source directory.
# Note: Copies the file(s) on Windows.
function(tf_psa_crypto_link_to_source base_name)
    set(link "${CMAKE_CURRENT_BINARY_DIR}/${base_name}")
    set(target "${CMAKE_CURRENT_SOURCE_DIR}/${base_name}")

    # Linking to non-existent file is not desirable. At best you will have a
    # dangling link, but when building in tree, this can create a symbolic link
    # to itself.
    if (EXISTS ${target} AND NOT EXISTS ${link})
        if (CMAKE_HOST_UNIX)
            execute_process(COMMAND ln -s ${target} ${link}
                RESULT_VARIABLE result
                ERROR_VARIABLE output)

            if (NOT ${result} EQUAL 0)
                message(FATAL_ERROR "Could not create symbolic link for: ${target} --> ${output}")
            endif()
        else()
            if (IS_DIRECTORY ${target})
                file(GLOB_RECURSE files FOLLOW_SYMLINKS LIST_DIRECTORIES false RELATIVE ${target} "${target}/*")
                foreach(file IN LISTS files)
                    configure_file("${target}/${file}" "${link}/${file}" COPYONLY)
                endforeach(file)
            else()
                configure_file(${target} ${link} COPYONLY)
            endif()
        endif()
    endif()
endfunction(tf_psa_crypto_link_to_source)

# Get the filename without the final extension (i.e. convert "a.b.c" to "a.b")
function(tf_psa_crypto_get_name_without_last_ext dest_var full_name)
    # Split into a list on '.' (but a cmake list is just a ';'-separated string)
    string(REPLACE "." ";" ext_parts "${full_name}")
    # Remove the last item if there are more than one
    list(LENGTH ext_parts ext_parts_len)
    if (${ext_parts_len} GREATER "1")
        math(EXPR ext_parts_last_item "${ext_parts_len} - 1")
        list(REMOVE_AT ext_parts ${ext_parts_last_item})
    endif()
    # Convert back to a string by replacing separators with '.'
    string(REPLACE ";" "." no_ext_name "${ext_parts}")
    # Copy into the desired variable
    set(${dest_var} ${no_ext_name} PARENT_SCOPE)
endfunction(tf_psa_crypto_get_name_without_last_ext)

include(CheckCCompilerFlag)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)

# Set base compile options to be used for the compilation of all C files of
# all targets.
function(tf_psa_crypto_set_base_compile_options target)
    if(CMAKE_COMPILER_IS_GNU)
        tf_psa_crypto_set_gnu_base_compile_options(${target})
    elseif(CMAKE_COMPILER_IS_CLANG)
        tf_psa_crypto_set_clang_base_compile_options(${target})
    elseif(CMAKE_COMPILER_IS_IAR)
        tf_psa_crypto_set_iar_base_compile_options(${target})
    elseif(CMAKE_COMPILER_IS_MSVC)
        tf_psa_crypto_set_msvc_base_compile_options(${target})
    endif()
    if(TF_PSA_CRYPTO_TEST_DRIVER)
        target_compile_definitions(${target}
            PRIVATE "-DPSA_CRYPTO_DRIVER_TEST"
                    "-DMBEDTLS_TEST_LIBTESTDRIVER1"
                    "-DTF_PSA_CRYPTO_TEST_LIBTESTDRIVER1"
        )
    endif()
endfunction(tf_psa_crypto_set_base_compile_options)

function(tf_psa_crypto_set_gnu_base_compile_options target)
    # some warnings we want are not available with old GCC versions
    # note: starting with CMake 2.8 we could use CMAKE_C_COMPILER_VERSION
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wwrite-strings -Wmissing-prototypes)
    if (GCC_VERSION VERSION_GREATER 3.0 OR GCC_VERSION VERSION_EQUAL 3.0)
        target_compile_options(${target} PRIVATE -Wformat=2 -Wno-format-nonliteral)
    endif()
    if (GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
        target_compile_options(${target} PRIVATE -Wvla)
    endif()
    if (GCC_VERSION VERSION_GREATER 4.5 OR GCC_VERSION VERSION_EQUAL 4.5)
        target_compile_options(${target} PRIVATE -Wlogical-op)
    endif()
    if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
        target_compile_options(${target} PRIVATE -Wshadow)
    endif()
    if (GCC_VERSION VERSION_GREATER 5.0)
        CHECK_C_COMPILER_FLAG("-Wformat-signedness" C_COMPILER_SUPPORTS_WFORMAT_SIGNEDNESS)
        if(C_COMPILER_SUPPORTS_WFORMAT_SIGNEDNESS)
            target_compile_options(${target} PRIVATE -Wformat-signedness)
        endif()
    endif()
    if (GCC_VERSION VERSION_GREATER 7.0 OR GCC_VERSION VERSION_EQUAL 7.0)
      target_compile_options(${target} PRIVATE -Wformat-overflow=2 -Wformat-truncation)
    endif()
    target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:-O2>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-O0 -g3>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Coverage>:-O0 -g3 --coverage>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_COVERAGE "--coverage")
    # Old GCC versions hit a performance problem with test_suite_pkwrite
    # "Private keey write check EC" tests when building with Asan+UBSan
    # and -O3: those tests take more than 100x time than normal, with
    # test_suite_pkwrite taking >3h on the CI. Observed with GCC 5.4 on
    # Ubuntu 16.04 x86_64 and GCC 6.5 on Ubuntu 18.04 x86_64.
    # GCC 7.5 and above on Ubuntu 18.04 appear fine.
    # To avoid the performance problem, we use -O2 when GCC version is lower than 7.0.
    # It doesn't slow down much even with modern compiler versions.
    target_compile_options(${target} PRIVATE $<$<CONFIG:ASan>:-fsanitize=address -fno-common -fsanitize=undefined -fno-sanitize-recover=all>)
    if (GCC_VERSION VERSION_LESS 7.0)
        target_compile_options(${target} PRIVATE $<$<CONFIG:ASan>:-O2>)
    else()
        target_compile_options(${target} PRIVATE $<$<CONFIG:ASan>:-O3>)
    endif()
    set_target_properties(${target} PROPERTIES LINK_FLAGS_ASAN "-fsanitize=address -fsanitize=undefined")
    target_compile_options(${target} PRIVATE $<$<CONFIG:ASanDbg>:-fsanitize=address -fno-common -fsanitize=undefined -fno-sanitize-recover=all -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_ASANDBG "-fsanitize=address -fsanitize=undefined")
    target_compile_options(${target} PRIVATE $<$<CONFIG:TSan>:-fsanitize=thread -O3>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_TSAN "-fsanitize=thread")
    target_compile_options(${target} PRIVATE $<$<CONFIG:TSanDbg>:-fsanitize=thread -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_TSANDBG "-fsanitize=thread")
    target_compile_options(${target} PRIVATE $<$<CONFIG:Check>:-Os>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:CheckFull>:-Os -Wcast-qual>)

    if(TF_PSA_CRYPTO_FATAL_WARNINGS)
        target_compile_options(${target} PRIVATE -Werror)
    endif(TF_PSA_CRYPTO_FATAL_WARNINGS)
endfunction(tf_psa_crypto_set_gnu_base_compile_options)

function(tf_psa_crypto_set_clang_base_compile_options target)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wwrite-strings -Wmissing-prototypes -Wpointer-arith -Wimplicit-fallthrough -Wshadow -Wvla -Wformat=2 -Wno-format-nonliteral)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:-O2>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-O0 -g3>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Coverage>:-O0 -g3 --coverage>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_COVERAGE "--coverage")
    target_compile_options(${target} PRIVATE $<$<CONFIG:ASan>:-fsanitize=address -fno-common -fsanitize=undefined -fno-sanitize-recover=all -O3>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_ASAN "-fsanitize=address -fsanitize=undefined")
    target_compile_options(${target} PRIVATE $<$<CONFIG:ASanDbg>:-fsanitize=address -fno-common -fsanitize=undefined -fno-sanitize-recover=all -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_ASANDBG "-fsanitize=address -fsanitize=undefined")
    target_compile_options(${target} PRIVATE $<$<CONFIG:MemSan>:-fsanitize=memory>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_MEMSAN "-fsanitize=memory")
    target_compile_options(${target} PRIVATE $<$<CONFIG:MemSanDbg>:-fsanitize=memory -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_MEMSANDBG "-fsanitize=memory")
    target_compile_options(${target} PRIVATE $<$<CONFIG:TSan>:-fsanitize=thread -O3>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_TSAN "-fsanitize=thread")
    target_compile_options(${target} PRIVATE $<$<CONFIG:TSanDbg>:-fsanitize=thread -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls>)
    set_target_properties(${target} PROPERTIES LINK_FLAGS_TSANDBG "-fsanitize=thread")
    target_compile_options(${target} PRIVATE $<$<CONFIG:Check>:-Os>)

    if(TF_PSA_CRYPTO_FATAL_WARNINGS)
        target_compile_options(${target} PRIVATE -Werror)
    endif(TF_PSA_CRYPTO_FATAL_WARNINGS)
endfunction(tf_psa_crypto_set_clang_base_compile_options)

function(tf_psa_crypto_set_iar_base_compile_options target)
    target_compile_options(${target} PRIVATE --warn_about_c_style_casts)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:-Ohz>)
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:--debug -On>)

    if(TF_PSA_CRYPTO_FATAL_WARNINGS)
        target_compile_options(${target} PRIVATE --warnings_are_errors)
    endif(TF_PSA_CRYPTO_FATAL_WARNINGS)
endfunction(tf_psa_crypto_set_iar_base_compile_options)

function(tf_psa_crypto_set_msvc_base_compile_options target)
    # Strictest warnings, UTF-8 source and execution charset
    target_compile_options(${target} PRIVATE /W3 /utf-8)

    if(TF_PSA_CRYPTO_FATAL_WARNINGS)
        target_compile_options(${target} PRIVATE /WX)
    endif(TF_PSA_CRYPTO_FATAL_WARNINGS)

    if(MSVC_STATIC_RUNTIME)
        foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS_CHECK)
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endforeach(flag_var)
    endif()
endfunction(tf_psa_crypto_set_msvc_base_compile_options)


# Set extra compile options. These options are expected to be set for the
# compilation of the TF-PSA-Crypto library files. Not necessarily for the test
# or third party driver files.
function(tf_psa_crypto_set_extra_compile_options target)
    if(CMAKE_COMPILER_IS_GNU)
        tf_psa_crypto_set_gnu_extra_compile_options(${target})
    elseif(CMAKE_COMPILER_IS_CLANG)
        tf_psa_crypto_set_clang_extra_compile_options(${target})
    endif()
endfunction(tf_psa_crypto_set_extra_compile_options)

function(tf_psa_crypto_set_gnu_extra_compile_options target)
    target_compile_options(${target} PRIVATE -Wmissing-declarations)
endfunction(tf_psa_crypto_set_gnu_extra_compile_options)

function(tf_psa_crypto_set_clang_extra_compile_options target)
    target_compile_options(${target} PRIVATE
        -Wmissing-declarations
        -Wdocumentation
        -Wno-documentation-deprecated-sync
        -Wunreachable-code
    )
endfunction(tf_psa_crypto_set_clang_extra_compile_options)


function(tf_psa_crypto_set_config_files_compile_definitions target)
    if(TF_PSA_CRYPTO_CONFIG_FILE)
        target_compile_definitions(${target}
            PUBLIC TF_PSA_CRYPTO_CONFIG_FILE="${TF_PSA_CRYPTO_CONFIG_FILE}")
    endif()
    if(TF_PSA_CRYPTO_USER_CONFIG_FILE)
        target_compile_definitions(${target}
            PUBLIC TF_PSA_CRYPTO_USER_CONFIG_FILE="${TF_PSA_CRYPTO_USER_CONFIG_FILE}")
    endif()
endfunction(tf_psa_crypto_set_config_files_compile_definitions)

if(CMAKE_BUILD_TYPE STREQUAL "Check" AND TEST_CPP)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    if(CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_GNU)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
    endif()
endif()

if (NOT EXISTS "${TF_PSA_CRYPTO_FRAMEWORK_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "${TF_PSA_CRYPTO_FRAMEWORK_DIR}/CMakeLists.txt not found. Run `git submodule update --init` from the source tree to fetch the submodule contents.")
endif()

#
# Directories with internal headers. The headers in that directories are not
# exposed to applications and thus not installed. These headers are currently
# consumed by Mbed TLS, eventually not so when we've finished cleaning up.
# Here we list all the directories containing such headers but the ones from
# drivers, that are appended in `drivers/CMakeLists.txt`.
#
set(TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/dispatch
        ${CMAKE_CURRENT_SOURCE_DIR}/extras
        ${CMAKE_CURRENT_SOURCE_DIR}/platform
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities
)

#
# `drivers` must be added before `core` because the build of the main library
# set up in `core` depends on the list of available drivers, which is
# constructed in `drivers/CMakeLists.txt`. This list may include a generated
# test driver.
#
add_subdirectory(framework)
add_subdirectory(include)
add_subdirectory(drivers)   # must precede core
add_subdirectory(extras)
add_subdirectory(platform)
add_subdirectory(utilities)
add_subdirectory(core)
add_subdirectory(pkgconfig)

#
# The C files in tests/src directory contain test code shared among test suites
# and programs. This shared test code is compiled and linked to test suites and
# programs objects as a set of compiled objects. The compiled objects are NOT
# built into a library that the test suite and program objects would link
# against as they link against the tfpsacrypto library. The reason is that such
# library is expected to have mutual dependencies with the aforementioned
# library and that there is as of today no portable way of handling such
# dependencies (only toolchain specific solutions).
#
# Thus the below definition of the `tf_psa_crypto_test` CMake library of
# objects target. This library of objects is used by tests and programs CMake
# files to define the test executables.
#
if(ENABLE_TESTING OR ENABLE_PROGRAMS)
    file(GLOB TF_PSA_CRYPTO_TEST_FILES
         ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/tests/src/*.c
         ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/tests/src/drivers/*.c
         tests/src/*.c)
    add_library(tf_psa_crypto_test OBJECT ${TF_PSA_CRYPTO_TEST_FILES})
    tf_psa_crypto_set_base_compile_options(tf_psa_crypto_test)
    if(GEN_FILES)
        add_custom_command(
            OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/tests/include/test/test_keys.h
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tests/include/test
            COMMAND
                "${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}"
                "${TF_PSA_CRYPTO_FRAMEWORK_DIR}/scripts/generate_test_keys.py"
                "--output"
                "${CMAKE_CURRENT_BINARY_DIR}/tests/include/test/test_keys.h"
            DEPENDS
                ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/scripts/generate_test_keys.py
        )
        add_custom_target(tf_psa_crypto_test_keys_header
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tests/include/test/test_keys.h)
        add_dependencies(tf_psa_crypto_test tf_psa_crypto_test_keys_header)
        if(TF_PSA_CRYPTO_TEST_DRIVER)
            add_dependencies(tf_psa_crypto_test ${TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS})
        endif()
    endif()

    file(WRITE
        ${CMAKE_CURRENT_BINARY_DIR}/tests/seedfile
        "This is a seedfile that contains 64 bytes ......................"
    )

    # Note: "tests/include/test" exists solely to provide access to
    #       "mbedtls/build_info.h". That header is an alias of
    #       "tf-psa-crypto/build_info.h".
    #
    #       It is included in framework headers and C modules because those
    #       components are also used in the Mbed TLS context.
    target_include_directories(tf_psa_crypto_test
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/tests/include
                ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/tests/include
                tests/include
                tests/include/test
                # Add the build-tree include directory before the source-tree one
                # so that generated headers in the build tree take precedence.
                ${CMAKE_CURRENT_BINARY_DIR}/include
                include
                ${TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS})

    foreach(driver_target ${TF_PSA_CRYPTO_DRIVER_TARGETS})
        get_target_property(public_includes ${driver_target} INTERFACE_INCLUDE_DIRECTORIES)
        target_include_directories(tf_psa_crypto_test PUBLIC ${public_includes})
    endforeach()

    # Request C11, needed for memory poisoning tests
    set_target_properties(tf_psa_crypto_test PROPERTIES C_STANDARD 11)
    tf_psa_crypto_set_config_files_compile_definitions(tf_psa_crypto_test)
endif()

if(ENABLE_PROGRAMS)
    add_subdirectory(programs)
endif()

if(ENABLE_TESTING)
    enable_testing()

    add_subdirectory(tests)

    # additional convenience targets for Unix only
    if(UNIX AND (NOT TF_PSA_CRYPTO_AS_SUBPROJECT))
        # For coverage testing:
        # 1. Build with:
        #         cmake -D CMAKE_BUILD_TYPE=Coverage /path/to/source && make
        # 2. Run the relevant tests for the part of the code you're interested in.
        #    For the reference coverage measurement, see
        #    tests/scripts/basic-build-test.sh
        # 3. Run framework/scripts/lcov.sh to generate an HTML report.
        ADD_CUSTOM_TARGET(lcov
            COMMAND framework/scripts/lcov.sh
        )

        ADD_CUSTOM_TARGET(memcheck
            COMMAND sed -i.bak s+/usr/bin/valgrind+`which valgrind`+ DartConfiguration.tcl
            COMMAND ctest -O memcheck.log -D ExperimentalMemCheck
            COMMAND tail -n1 memcheck.log | grep 'Memory checking results:' > /dev/null
            COMMAND rm -f memcheck.log
            COMMAND mv DartConfiguration.tcl.bak DartConfiguration.tcl
        )
    endif()

    # Make scripts needed for testing available in an out-of-source build.
    if (NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
        tf_psa_crypto_link_to_source(scripts)
        # Copy (don't link) DartConfiguration.tcl, needed for memcheck, to
        # keep things simple with the sed commands in the memcheck target.
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DartConfiguration.tcl
                    ${CMAKE_CURRENT_BINARY_DIR}/DartConfiguration.tcl COPYONLY)
    endif()
endif()

if(NOT DISABLE_PACKAGE_CONFIG_AND_INSTALL)
    configure_package_config_file(
        "cmake/TF-PSA-CryptoConfig.cmake.in"
        "cmake/TF-PSA-CryptoConfig.cmake"
            INSTALL_DESTINATION "cmake")

    write_basic_package_version_file(
        "cmake/TF-PSA-CryptoConfigVersion.cmake"
            COMPATIBILITY SameMajorVersion
            VERSION "${TF_PSA_CRYPTO_VERSION}")

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/TF-PSA-CryptoConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/cmake/TF-PSA-CryptoConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/TF-PSA-Crypto")

    export(
        EXPORT TF-PSA-CryptoTargets
        NAMESPACE TF-PSA-Crypto::
        FILE "cmake/TF-PSA-CryptoTargets.cmake")

    install(
        EXPORT TF-PSA-CryptoTargets
        NAMESPACE TF-PSA-Crypto::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/TF-PSA-Crypto"
        FILE "TF-PSA-CryptoTargets.cmake")

    if(CMAKE_VERSION VERSION_GREATER 3.15 OR CMAKE_VERSION VERSION_EQUAL 3.15)
        # Do not export the package by default
        cmake_policy(SET CMP0090 NEW)

        # Make this package visible to the system
        export(PACKAGE TF-PSA-Crypto)
    endif()
endif()
