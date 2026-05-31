set(libs
    ${mbedx509_target}
)

set(executables
    cert_app
    cert_req
    cert_write
    crl_app
    load_roots
    req_app
)
add_dependencies(${programs_target} ${executables})

foreach(exe IN LISTS executables)
    add_executable(${exe} ${exe}.c $<TARGET_OBJECTS:mbedtls_test>)
    set_base_compile_options(${exe})
    target_link_libraries(${exe} ${libs} ${CMAKE_THREAD_LIBS_INIT})
    target_include_directories(${exe} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../framework/tests/include)
endforeach()

target_link_libraries(cert_app ${mbedtls_target})
# For mbedtls_timing_get_timer()
target_link_libraries(load_roots ${mbedtls_target})

install(TARGETS ${executables}
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
