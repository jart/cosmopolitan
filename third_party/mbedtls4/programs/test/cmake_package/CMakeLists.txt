cmake_minimum_required(VERSION 3.5.1)

#
# Simulate configuring and building Mbed TLS as the user might do it. We'll
# skip installing it, and use the build directory directly instead.
#

set(MbedTLS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(MbedTLS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbedtls")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        "-H${MbedTLS_SOURCE_DIR}"
        "-B${MbedTLS_BINARY_DIR}"
        "-DENABLE_PROGRAMS=NO"
        "-DENABLE_TESTING=NO"
        # Turn on generated files explicitly in case this is a release
        "-DGEN_FILES=ON")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        --build "${MbedTLS_BINARY_DIR}")

#
# Locate the package.
#

set(MbedTLS_DIR "${MbedTLS_BINARY_DIR}/cmake")
find_package(MbedTLS REQUIRED)

#
# At this point, the Mbed TLS targets should have been imported, and we can now
# link to them from our own program.
#

add_executable(cmake_package cmake_package.c)
target_link_libraries(cmake_package
    MbedTLS::mbedtls MbedTLS::mbedx509 MbedTLS::tfpsacrypto)
