set(libs
    ${tfpsacrypto_target}
    ${CMAKE_THREAD_LIBS_INIT}
    ${mbedtls_target}
)

find_library(FUZZINGENGINE_LIB FuzzingEngine)
if(FUZZINGENGINE_LIB)
    project(fuzz CXX)
endif()

set(executables_no_common_c
    fuzz_pubkey
)
add_dependencies(${programs_target} ${executables_no_common_c})

set(executables_with_common_c
    fuzz_privkey
)
add_dependencies(${programs_target} ${executables_with_common_c})

foreach(exe IN LISTS executables_no_common_c executables_with_common_c)

    set(exe_sources ${exe}.c)
    if(NOT FUZZINGENGINE_LIB)
        list(APPEND exe_sources fuzz_onefile.c)
    endif()

    # This emulates "if ( ... IN_LIST ... )" which becomes available in CMake 3.3
    list(FIND executables_with_common_c ${exe} exe_index)
    if(${exe_index} GREATER -1)
        list(APPEND exe_sources fuzz_common.c)
    endif()

    add_executable(${exe} ${exe_sources} $<TARGET_OBJECTS:tf_psa_crypto_test>)
    tf_psa_crypto_set_base_compile_options(${exe})
    target_include_directories(${exe} PRIVATE ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/tests/include)

    if (NOT FUZZINGENGINE_LIB)
        target_link_libraries(${exe} ${libs})
    else()
        target_link_libraries(${exe} ${libs} FuzzingEngine)
        SET_TARGET_PROPERTIES(${exe} PROPERTIES LINKER_LANGUAGE CXX)
    endif()

endforeach()
