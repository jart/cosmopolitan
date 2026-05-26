file(GLOB extras_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)
set(tf_psa_crypto_objlib "extras")
include(${PROJECT_SOURCE_DIR}/objlib.cmake)
