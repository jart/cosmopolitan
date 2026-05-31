if(NOT DISABLE_PACKAGE_CONFIG_AND_INSTALL)
  include(JoinPaths.cmake)
  join_paths(PKGCONFIG_INCLUDEDIR "\${prefix}" "${CMAKE_INSTALL_INCLUDEDIR}")
  join_paths(PKGCONFIG_LIBDIR "\${prefix}" "${CMAKE_INSTALL_LIBDIR}")

  #define these manually since minimum CMAKE version is not 3.9 for DESCRIPTION and 3.12 for HOMEPAGE_URL usage in project() below.
  # Prefix with something that won't clash with newer versions of CMAKE.
  set(PKGCONFIG_PROJECT_DESCRIPTION "TF-PSA-Crypto is a C library that implements cryptographic primitives. Its small code footprint makes it suitable for embedded systems.")
  set(PKGCONFIG_PROJECT_HOMEPAGE_URL "https://www.trustedfirmware.org/projects/mbed-tls/")

  configure_file(tfpsacrypto.pc.in tfpsacrypto.pc @ONLY)
    install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tfpsacrypto.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
