file(GLOB src_crypto "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# The generated file `psa_crypto_driver_wrappers_no_static.c` may exist in the
# source tree and therefore be picked up by the glob above. Remove it from
# `src_crypto` if present, then append it using a relative path. This ensures
# that the build system prefers the file from the build tree over the one in
# the source tree.
list(FIND src_crypto
     "${CMAKE_CURRENT_SOURCE_DIR}/psa_crypto_driver_wrappers_no_static.c"
     _idx)
if(NOT _idx EQUAL -1)
    list(REMOVE_AT src_crypto ${_idx})
endif()
list(APPEND src_crypto psa_crypto_driver_wrappers_no_static.c)

if(GEN_FILES)
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/psa_crypto_driver_wrappers.h
            ${CMAKE_CURRENT_BINARY_DIR}/psa_crypto_driver_wrappers_no_static.c
        COMMAND
            ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/scripts/generate_driver_wrappers.py
                ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            ${PROJECT_SOURCE_DIR}/scripts/generate_driver_wrappers.py
            ${PROJECT_SOURCE_DIR}/scripts/data_files/driver_templates/psa_crypto_driver_wrappers.h.jinja
            ${PROJECT_SOURCE_DIR}/scripts/data_files/driver_templates/psa_crypto_driver_wrappers_no_static.c.jinja
    )

    execute_process(
        COMMAND
            ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/scripts/generate_config_checks.py
            --list-for-cmake "${CMAKE_CURRENT_BINARY_DIR}"
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE
            TF_PSA_CRYPTO_GENERATED_CONFIG_CHECKS_HEADERS)

    add_custom_command(
        OUTPUT ${TF_PSA_CRYPTO_GENERATED_CONFIG_CHECKS_HEADERS}
        COMMAND
            ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/scripts/generate_config_checks.py
                ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            ${PROJECT_SOURCE_DIR}/scripts/generate_config_checks.py
            ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/scripts/mbedtls_framework/config_checks_generator.py
    )

    add_custom_target(
            ${TF_PSA_CRYPTO_TARGET_PREFIX}libtfpsacrypto_generated_files_target
        DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/psa_crypto_driver_wrappers.h"
            "${CMAKE_CURRENT_BINARY_DIR}/psa_crypto_driver_wrappers_no_static.c"
            ${TF_PSA_CRYPTO_GENERATED_CONFIG_CHECKS_HEADERS}
        )

    # List generated headers as sources explicitly. Normally CMake finds
    # headers by tracing include directives, but if that happens before the
    # generated headers are generated, this process doesn't find them.
    list(APPEND src_crypto
        "${CMAKE_CURRENT_BINARY_DIR}/psa_crypto_driver_wrappers.h"
        ${TF_PSA_CRYPTO_GENERATED_CONFIG_CHECKS_HEADERS}
    )
endif(GEN_FILES)

if(CMAKE_C_COMPILER_ID MATCHES "AppleClang")
    set(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

if(WIN32)
    set(libs ${libs} ws2_32 bcrypt)
endif(WIN32)

if(LINK_WITH_PTHREAD)
    set(libs ${libs} ${CMAKE_THREAD_LIBS_INIT})
endif()

if(LINK_WITH_TRUSTED_STORAGE)
    set(libs ${libs} trusted_storage)
endif()

if (NOT USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND NOT USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    message(FATAL_ERROR "Need to choose static or shared TF-PSA-Crypto build!")
endif(NOT USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND NOT USE_SHARED_TF_PSA_CRYPTO_LIBRARY)

set(tfpsacrypto_target "${TF_PSA_CRYPTO_TARGET_PREFIX}tfpsacrypto")
if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    set(tfpsacrypto_static_target ${tfpsacrypto_target})
endif()

set(target_libraries ${tfpsacrypto_target})
if(USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    string(APPEND tfpsacrypto_static_target "_static")
    list(APPEND target_libraries ${tfpsacrypto_static_target})
endif()

if(USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    set(target_objects "")
    foreach(driver_target ${TF_PSA_CRYPTO_DRIVER_STATIC_TARGETS})
        list(APPEND target_objects "$<TARGET_OBJECTS:${driver_target}>")
    endforeach()
    foreach(objlib_target ${TF_PSA_CRYPTO_OBJLIB_STATIC_TARGETS})
        list(APPEND target_objects "$<TARGET_OBJECTS:${objlib_target}>")
    endforeach()
    add_library(${tfpsacrypto_static_target} STATIC
        ${src_crypto}
        ${target_objects})
    tf_psa_crypto_set_base_compile_options(${tfpsacrypto_static_target})
    tf_psa_crypto_set_extra_compile_options(${tfpsacrypto_static_target})
    set_target_properties(${tfpsacrypto_static_target} PROPERTIES OUTPUT_NAME tfpsacrypto)
    target_link_libraries(${tfpsacrypto_static_target} PUBLIC ${libs})

    if(GEN_FILES)
        add_dependencies(${tfpsacrypto_static_target}
            ${TF_PSA_CRYPTO_TARGET_PREFIX}libtfpsacrypto_generated_files_target)
    endif()
endif(USE_STATIC_TF_PSA_CRYPTO_LIBRARY)

if(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    set(CMAKE_LIBRARY_PATH ${CMAKE_CURRENT_BINARY_DIR})
    set(target_objects "")
    foreach(driver_target ${TF_PSA_CRYPTO_DRIVER_TARGETS})
        list(APPEND target_objects "$<TARGET_OBJECTS:${driver_target}>")
    endforeach()
    foreach(objlib_target ${TF_PSA_CRYPTO_OBJLIB_TARGETS})
        list(APPEND target_objects "$<TARGET_OBJECTS:${objlib_target}>")
    endforeach()
    add_library(${tfpsacrypto_target} SHARED
        ${src_crypto}
        ${target_objects})
    tf_psa_crypto_set_base_compile_options(${tfpsacrypto_target})
    tf_psa_crypto_set_extra_compile_options(${tfpsacrypto_target})
    set_target_properties(${tfpsacrypto_target} PROPERTIES VERSION ${TF_PSA_CRYPTO_VERSION} SOVERSION ${TF_PSA_CRYPTO_SOVERSION})
    target_link_libraries(${tfpsacrypto_target} PUBLIC ${libs})

    if(GEN_FILES)
        add_dependencies(${tfpsacrypto_target}
            ${TF_PSA_CRYPTO_TARGET_PREFIX}libtfpsacrypto_generated_files_target)
    endif()
endif(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)

foreach(target IN LISTS target_libraries)
    add_library(TF-PSA-Crypto::${target} ALIAS ${target})  # add_subdirectory support
    target_include_directories(${target}
        # Add the build-tree include directory before the source-tree one
        # so that generated headers in the build tree take precedence.
        PUBLIC $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>
               $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE ${TF_PSA_CRYPTO_PRIVATE_INCLUDE_DIRS}
                # Needed to include psa_crypto_driver_wrappers.h
                ${CMAKE_CURRENT_BINARY_DIR})
    foreach(driver_target ${TF_PSA_CRYPTO_DRIVER_TARGETS})
        get_target_property(public_includes ${driver_target} INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir IN LISTS public_includes)
            target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${dir}>)
        endforeach()
    endforeach()
    if(TF_PSA_CRYPTO_TEST_DRIVER)
        # Note: "tests/include/test" exists solely to provide access to
        #       "mbedtls/build_info.h". That header is an alias of
        #       "tf-psa-crypto/build_info.h".
        #
        #       It is included in framework headers and C modules because those
        #       components are also used in the Mbed TLS context.
        target_include_directories(${target}
            PRIVATE ${TF_PSA_CRYPTO_FRAMEWORK_DIR}/tests/include
                    ${PROJECT_SOURCE_DIR}/tests/include/test)
    endif()
    tf_psa_crypto_set_config_files_compile_definitions(${target})
    install(
        TARGETS ${target}
        EXPORT MbedTLSTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    install(
        TARGETS ${target}
        EXPORT TF-PSA-CryptoTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
endforeach(target)
