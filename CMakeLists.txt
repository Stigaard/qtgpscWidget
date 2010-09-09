project (qtgpsc)
cmake_minimum_required (VERSION 2.6)

# Add an uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Initialize project version
file (READ VERSION AU_PROJECT_VERSION)
add_definitions (-DPROJECT_VERSION="${AU_PROJECT_VERSION}")

add_subdirectory (src)
