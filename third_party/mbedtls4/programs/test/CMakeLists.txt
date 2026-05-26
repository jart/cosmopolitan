set(libs
    ${mbedtls_target}
)

set(executables
    metatest
    query_compile_time_config
    query_included_headers
    selftest
    udp_proxy
    zeroize
)
add_dependencies(${programs_target} ${executables})
add_dependencies(${ssl_opt_target} udp_proxy)
add_dependencies(${ssl_opt_target} query_compile_time_config)

if(TEST_CPP)
    set(cpp_dummy_build_cpp "${CMAKE_CURRENT_BINARY_DIR}/cpp_dummy_build.cpp")
    set(generate_cpp_dummy_build "${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp_dummy_build.sh")
    add_custom_command(
        OUTPUT "${cpp_dummy_build_cpp}"
        COMMAND "${generate_cpp_dummy_build}" "${cpp_dummy_build_cpp}"
        DEPENDS "${generate_cpp_dummy_build}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    add_executable(cpp_dummy_build "${cpp_dummy_build_cpp}")
    set_base_compile_options(cpp_dummy_build)
    target_include_directories(cpp_dummy_build
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/drivers/builtin/include)
    target_link_libraries(cpp_dummy_build ${tfpsacrypto_target} ${CMAKE_THREAD_LIBS_INIT})
endif()

if(USE_SHARED_MBEDTLS_LIBRARY AND
   NOT ${CMAKE_SYSTEM_NAME} MATCHES "[Ww][Ii][Nn]")
    add_executable(dlopen "dlopen.c")
    set_base_compile_options(dlopen)
    target_include_directories(dlopen
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/drivers/builtin/include)
    target_link_libraries(dlopen ${CMAKE_DL_LIBS})
endif()

if(GEN_FILES)
    find_package(Perl REQUIRED)

    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/query_config.c
        COMMAND
            ${PERL}
                ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate_query_config.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/../../include/mbedtls/mbedtls_config.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/include/psa/crypto_config.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/data_files/query_config.fmt
                ${CMAKE_CURRENT_BINARY_DIR}/query_config.c
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/generate_query_config.pl
            ${CMAKE_CURRENT_SOURCE_DIR}/../../include/mbedtls/mbedtls_config.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/include/psa/crypto_config.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/data_files/query_config.fmt
    )
    # this file will also be used in another directory, so create a target, see
    # https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#how-can-i-add-a-dependency-to-a-source-file-which-is-generated-in-a-subdirectory
    add_custom_target(generate_query_config_c
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/query_config.c)
else()
    link_to_source(query_config.c)
endif()

foreach(exe IN LISTS executables)
    set(source ${exe}.c)
    set(extra_sources "")
    if(NOT EXISTS ${source} AND
       EXISTS ${MBEDTLS_FRAMEWORK_DIR}/tests/programs/${source})
         set(source ${MBEDTLS_FRAMEWORK_DIR}/tests/programs/${source})
    endif()

    if(exe STREQUAL "query_compile_time_config")
        list(APPEND extra_sources
            ${MBEDTLS_FRAMEWORK_DIR}/tests/programs/query_config.h
            ${CMAKE_CURRENT_BINARY_DIR}/query_config.c)
    endif()
    add_executable(${exe} ${source} $<TARGET_OBJECTS:mbedtls_test>
        ${extra_sources})
    set_base_compile_options(${exe})
    target_include_directories(${exe}
        PRIVATE ${MBEDTLS_FRAMEWORK_DIR}/tests/include
                ${MBEDTLS_FRAMEWORK_DIR}/tests/programs)
    target_include_directories(${exe}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../library
                ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/core)
    if(exe STREQUAL "query_compile_time_config")
        target_include_directories(${exe} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    # Request C11, required for memory poisoning
    set_target_properties(${exe} PROPERTIES C_STANDARD 11)
    target_link_libraries(${exe} ${libs} ${CMAKE_THREAD_LIBS_INIT})
endforeach()

target_include_directories(metatest
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/drivers/builtin/src
                ${CMAKE_CURRENT_SOURCE_DIR}/../../tf-psa-crypto/platform
)
install(TARGETS ${executables}
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
