cmake_minimum_required(VERSION 3.5.1)

# Test the target renaming support by adding a prefix to the targets built
set(TF_PSA_CRYPTO_TARGET_PREFIX subproject_test_)

# We use the parent TF-PSA-Crypto directory as the TF_PSA_CRYPTO_DIR for this
# test. Other projects that use TF-PSA-Crypto as a subproject are likely to
# add by their own relative paths.
set(TF_PSA_CRYPTO_DIR ../../../)

# Add TF-PSA-Crypto as a subdirectory.
add_subdirectory(${TF_PSA_CRYPTO_DIR} build)

# Link against all the TF-PSA-Crypto library. Verifies that the target has been
# created using the specified prefix

add_executable(cmake_subproject cmake_subproject.c)
target_link_libraries(cmake_subproject subproject_test_tfpsacrypto ${CMAKE_THREAD_LIBS_INIT})
