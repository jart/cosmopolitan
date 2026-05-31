option(INSTALL_TF_PSA_CRYPTO_HEADERS "Install TF PSA Crypto headers." ON)

# The handling of TF_PSA_CRYPTO_CONFIG_NAME assumes that the default
# configuration file 'include/psa/crypto_config.h' exists in the source tree.
# That file is copied into the build tree and then modified with 'config.py'
# according to the configuration name provided via TF_PSA_CRYPTO_CONFIG_NAME.

if(NOT "${TF_PSA_CRYPTO_CONFIG_NAME}" STREQUAL "")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/psa)
    file(COPY ${PROJECT_SOURCE_DIR}/include/psa/crypto_config.h
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/psa)

    message(STATUS "Setting ${TF_PSA_CRYPTO_CONFIG_NAME} configuration")
    execute_process(
        COMMAND
            ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/scripts/config.py
            -f ${CMAKE_CURRENT_BINARY_DIR}/psa/crypto_config.h
            ${TF_PSA_CRYPTO_CONFIG_NAME}
        RESULT_VARIABLE result
    )
    if(result)
        message(FATAL_ERROR "Setting ${TF_PSA_CRYPTO_CONFIG_NAME} configuration failed")
    endif()
endif(NOT "${TF_PSA_CRYPTO_CONFIG_NAME}" STREQUAL "")

if(INSTALL_TF_PSA_CRYPTO_HEADERS)
    install(DIRECTORY "."
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
        PATTERN "crypto_config.h" EXCLUDE
    )

    if("${TF_PSA_CRYPTO_CONFIG_NAME}" STREQUAL "")
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/psa/crypto_config.h"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/psa)
    else()
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/psa/crypto_config.h"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/psa)
    endif("${TF_PSA_CRYPTO_CONFIG_NAME}" STREQUAL "")
endif(INSTALL_TF_PSA_CRYPTO_HEADERS)
