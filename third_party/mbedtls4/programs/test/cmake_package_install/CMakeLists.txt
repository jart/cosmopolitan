cmake_minimum_required(VERSION 3.5.1)

#
# Simulate configuring and building Mbed TLS as the user might do it. We'll
# install into a directory inside our own build directory.
#

set(MbedTLS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(MbedTLS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbedtls")
set(MbedTLS_BINARY_DIR "${MbedTLS_INSTALL_DIR}${CMAKE_FILES_DIRECTORY}")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        "-H${MbedTLS_SOURCE_DIR}"
        "-B${MbedTLS_BINARY_DIR}"
        "-DENABLE_PROGRAMS=NO"
        "-DENABLE_TESTING=NO"
        # Turn on generated files explicitly in case this is a release
        "-DGEN_FILES=ON"
        "-DUSE_SHARED_MBEDTLS_LIBRARY=ON"
        "-DCMAKE_INSTALL_PREFIX=${MbedTLS_INSTALL_DIR}")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        --build "${MbedTLS_BINARY_DIR}"
        --target install)

#
# Locate the package.
#

list(INSERT CMAKE_PREFIX_PATH 0 "${MbedTLS_INSTALL_DIR}")
find_package(MbedTLS REQUIRED)

#
# At this point, the Mbed TLS targets should have been imported, and we can now
# link to them from our own program.
#

add_executable(cmake_package_install cmake_package_install.c)

string(REGEX MATCH "GNU" CMAKE_COMPILER_IS_GNU "${CMAKE_C_COMPILER_ID}")
if(CMAKE_COMPILER_IS_GNU)
    target_compile_options(cmake_package_install PRIVATE -Wall -Werror)
endif()

target_link_libraries(cmake_package_install
    MbedTLS::mbedtls MbedTLS::mbedx509 MbedTLS::tfpsacrypto)
