set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/tests/scripts/generate_test_driver.py
)

message(STATUS "Generating libtestdriver1-list-vars.cmake")
execute_process(
    COMMAND ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
            ./tests/scripts/generate_test_driver.py
            ${CMAKE_CURRENT_BINARY_DIR}
            --list-vars-for-cmake libtestdriver1-list-vars.cmake
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE result)
if(result)
    message(FATAL_ERROR "Generation of list-vars.cmake failed: ${result}")
endif()

# libtestdriver1-list-vars.cmake defines three list variables:
# - libtestdriver1_input_files: the list of generate_test_driver.py input files
# - libtestdriver1_files: the list of generate_test_driver.py output files
# - libtestdriver1_src_files: the list of generate_test_driver.py output C files
include(${CMAKE_CURRENT_BINARY_DIR}/libtestdriver1-list-vars.cmake)

set(input_files "")
foreach(relpath IN LISTS libtestdriver1_input_files)
    list(APPEND input_files ${PROJECT_SOURCE_DIR}/${relpath})
endforeach()

add_custom_command(
    OUTPUT ${libtestdriver1_files}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND
        ${TF_PSA_CRYPTO_PYTHON_EXECUTABLE}
            ./tests/scripts/generate_test_driver.py ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${PROJECT_SOURCE_DIR}/tests/scripts/generate_test_driver.py
            ${input_files}
    COMMENT "Generating test driver libtestdriver1 tree"
)

add_custom_target(libtestdriver1_generation
    DEPENDS ${libtestdriver1_files})

set(tf_psa_crypto_driver "libtestdriver1")
include(${PROJECT_SOURCE_DIR}/drivers/driver.cmake)
