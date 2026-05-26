set(everest_target "${TF_PSA_CRYPTO_TARGET_PREFIX}everest")
if (USE_STATIC_TF_PSA_CRYPTO_LIBRARY)
    set(everest_static_target ${everest_target})
endif()
set(target_libraries ${everest_target})
if(USE_STATIC_TF_PSA_CRYPTO_LIBRARY AND USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    string(APPEND everest_static_target "_static")
    list(APPEND target_libraries ${everest_static_target})
endif()

foreach (target IN LISTS target_libraries)
    add_library(${target} OBJECT
      library/x25519.c
      library/Hacl_Curve25519_joined.c)

    tf_psa_crypto_set_base_compile_options(${target})
    target_include_directories(${target}
      PUBLIC include
      PRIVATE # Add the build-tree include directory before the source-tree one
              # so that generated headers in the build tree take precedence.
              ${PROJECT_BINARY_DIR}/include
              ${PROJECT_SOURCE_DIR}/include
              ${TF_PSA_CRYPTO_DRIVERS_INCLUDE_DIRS}
              include/tf-psa-crypto/private/everest
              include/tf-psa-crypto/private/everest/kremlib
              ${PROJECT_SOURCE_DIR}/core)
    tf_psa_crypto_set_config_files_compile_definitions(${target})
    if(TF_PSA_CRYPTO_TEST_DRIVER)
        add_dependencies(${target} ${TF_PSA_CRYPTO_TEST_DRIVER_GENERATION_TARGETS})
    endif()
endforeach(target)

if(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)
    set_property(TARGET ${everest_target} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(USE_SHARED_TF_PSA_CRYPTO_LIBRARY)

if(INSTALL_TF_PSA_CRYPTO_HEADERS)

  install(DIRECTORY "include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.h"
  )

endif(INSTALL_TF_PSA_CRYPTO_HEADERS)
