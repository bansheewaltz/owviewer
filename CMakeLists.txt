cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER gcc)

project(3dviewer LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/updateSubmodules.cmake)

option(ENABLE_DOCS "Build documenation" ON)
option(ENABLE_TESTING "Enable testing" ON)
option(ENABLE_COVERAGE "Enable code coverage" ON)

add_subdirectory(extern/gifimage)
add_subdirectory(src/lib)
add_subdirectory(src/app)
target_include_directories(${PROJECT_NAME} PRIVATE extern)

# Target uninstall
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake)
endif()

# Target dist
include(InstallRequiredSystemLibraries)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-v1.0-Source")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v1.0-${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
