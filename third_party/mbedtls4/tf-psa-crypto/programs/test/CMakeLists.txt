# Create an empty list of executables which will be filled by all the targets
# that will be built below. This is used at the bottom for the install() command.
set(executables)

function(tfpsacrypto_build_program_common   exe)
    add_executable(${exe} ${ARGN})
    tf_psa_crypto_set_base_compile_options(${exe})
    add_dependencies(${programs_target} ${exe})
    list(APPEND executables exe)
endfunction()

# benchmark
tfpsacrypto_build_program_common(benchmark benchmark.c)
target_sources(benchmark PRIVATE $<TARGET_OBJECTS:tf_psa_crypto_test>)
target_link_libraries(benchmark PRIVATE ${tfpsacrypto_target} ${CMAKE_THREAD_LIBS_INIT})

# which_aes
tfpsacrypto_build_program_common(which_aes which_aes.c)
target_link_libraries(which_aes PRIVATE ${tfpsacrypto_target})

# dlopen
if(USE_SHARED_TF_PSA_CRYPTO_LIBRARY AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "[Ww][Ii][Nn]")
    tfpsacrypto_build_program_common(tfpsacrypto_dlopen tfpsacrypto_dlopen.c)
    target_include_directories(tfpsacrypto_dlopen PRIVATE ${PROJECT_SOURCE_DIR}/drivers/builtin/include/)
    target_link_libraries(tfpsacrypto_dlopen PRIVATE ${CMAKE_DL_LIBS} ${tfpsacrypto_target})
endif()

# zeroize
tfpsacrypto_build_program_common(tf_psa_crypto_zeroize ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/tests/programs/zeroize.c)
target_link_libraries(tf_psa_crypto_zeroize ${tfpsacrypto_target})
target_include_directories(tf_psa_crypto_zeroize PRIVATE ${PROJECT_SOURCE_DIR}/tests/include/test)

install(TARGETS ${executable_programs}
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
