file(GLOB builtin_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)
set(tf_psa_crypto_driver "builtin")
include(${PROJECT_SOURCE_DIR}/drivers/driver.cmake)
