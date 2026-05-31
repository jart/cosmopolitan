file(GLOB platform_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
set(tf_psa_crypto_objlib "platform")
include(${PROJECT_SOURCE_DIR}/objlib.cmake)
