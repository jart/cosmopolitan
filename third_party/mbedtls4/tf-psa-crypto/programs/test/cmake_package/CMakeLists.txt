cmake_minimum_required(VERSION 2.8.12)

#
# Simulate configuring and building TF-PSA-Crypto as the user might do it.
# We'll skip installing it, and use the build directory directly instead.
#

set(TF-PSA-Crypto_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(TF-PSA-Crypto_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/tf-psa-crypto")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        "-H${TF-PSA-Crypto_SOURCE_DIR}"
        "-B${TF-PSA-Crypto_BINARY_DIR}"
        "-DENABLE_PROGRAMS=NO"
        "-DENABLE_TESTING=NO")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        --build "${TF-PSA-Crypto_BINARY_DIR}")

#
# Locate the package.
#

set(TF-PSA-Crypto_DIR "${TF-PSA-Crypto_BINARY_DIR}/cmake")
find_package(TF-PSA-Crypto REQUIRED)

#
# At this point, the TF-PSA-Crypto targets should have been imported, and we
# can now link to them from our own program.
#

add_executable(cmake_package cmake_package.c)
target_link_libraries(cmake_package TF-PSA-Crypto::tfpsacrypto)
