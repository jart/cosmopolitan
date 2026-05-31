set(tf_psa_crypto_driver "pqcp")

# Point the C compiler only to files under include and src, written by us.
# The actual code from mldsa-native is included through #include directives.
file(GLOB pqcp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)

include(${PROJECT_SOURCE_DIR}/drivers/driver.cmake)

foreach (target IN LISTS target_libraries)
    target_include_directories(${target}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
        PRIVATE mldsa-native/mldsa
    )
endforeach(target)
