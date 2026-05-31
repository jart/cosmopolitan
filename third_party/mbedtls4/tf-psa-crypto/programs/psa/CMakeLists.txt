set(executables
    aead_demo
    crypto_examples
    generate_random_uuid
    hmac_demo
    key_ladder_demo
    psa_constant_names
    psa_hash
)
add_dependencies(${programs_target} ${executables})

if(GEN_FILES)
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/psa_constant_names_generated.c
        COMMAND
            ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/scripts/generate_psa_constants.py
                ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
        DEPENDS
            ${PROJECT_SOURCE_DIR}/scripts/generate_psa_constants.py
            ${PROJECT_SOURCE_DIR}/include/psa/crypto_values.h
            ${PROJECT_SOURCE_DIR}/include/psa/crypto_extra.h
    )
else()
    tf_psa_crypto_link_to_source(psa_constant_names_generated.c)
endif()

foreach(exe IN LISTS executables)
    add_executable(${exe} ${exe}.c $<TARGET_OBJECTS:tf_psa_crypto_test>)
    tf_psa_crypto_set_base_compile_options(${exe})
    target_link_libraries(${exe} ${tfpsacrypto_target} ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(${exe} PRIVATE ${PROJECT_SOURCE_DIR}/framework/tests/include)
endforeach()

target_include_directories(psa_constant_names PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
if(GEN_FILES)
    add_custom_target(generate_psa_constant_names_generated_c
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/psa_constant_names_generated.c)
    add_dependencies(psa_constant_names generate_psa_constant_names_generated_c)
endif()

install(TARGETS ${executables}
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(PROGRAMS
            key_ladder_demo.sh
        DESTINATION "bin")
