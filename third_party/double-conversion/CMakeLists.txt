cmake_minimum_required(VERSION 3.0)
project(double-conversion VERSION 3.2.0)

if(BUILD_SHARED_LIBS AND MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(headers
    double-conversion/bignum.h
    double-conversion/cached-powers.h
    double-conversion/diy-fp.h
    double-conversion/double-conversion.h
    double-conversion/double-to-string.h
    double-conversion/fast-dtoa.h
    double-conversion/fixed-dtoa.h
    double-conversion/ieee.h
    double-conversion/string-to-double.h
    double-conversion/strtod.h
    double-conversion/utils.h)

add_library(double-conversion
            double-conversion/bignum.cc
            double-conversion/bignum-dtoa.cc
            double-conversion/cached-powers.cc
            double-conversion/double-to-string.cc
            double-conversion/fast-dtoa.cc
            double-conversion/fixed-dtoa.cc
            double-conversion/string-to-double.cc
            double-conversion/strtod.cc
            ${headers})
target_include_directories(
    double-conversion PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# pick a version #
set_target_properties(double-conversion PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 3)

# set up testing if requested
option(BUILD_TESTING "Build test programs" OFF)
if(BUILD_TESTING)
  enable_testing()
  include(CTest)
  add_subdirectory(test)
endif()

####
# Installation (https://github.com/forexample/package-example)

include(GNUInstallDirs)

# Layout. This works for all platforms:
#   * <prefix>/lib/cmake/<PROJECT-NAME>
#   * <prefix>/lib/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with function 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)

# Targets:
#   * <prefix>/lib/libdouble-conversion.a
#   * header location after install: <prefix>/include/double-conversion/*.h
#   * headers can be included by C++ code `#include <double-conversion/*.h>`
install(
    TARGETS double-conversion
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Headers:
#   * double-conversion/*.h -> <prefix>/include/double-conversion/*.h
install(
    FILES ${headers}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/double-conversion"
)

# Config
#   * <prefix>/lib/cmake/double-conversion/double-conversionConfig.cmake
#   * <prefix>/lib/cmake/double-conversion/double-conversionConfigVersion.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/double-conversion/double-conversionTargets.cmake
install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
