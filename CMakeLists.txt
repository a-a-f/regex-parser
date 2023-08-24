cmake_minimum_required(VERSION 3.11.0)
project(regex-parser)

option(PERFORM_STATIC_CHECKS "Run static checks on the source code" OFF)
option(CODE_COVERAGE "Add support for coverage analysis" OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17.0)
    message(FATAL_ERROR "Clang version >= 17.0 required!")
  endif()
else()
    message(FATAL_ERROR "Unsupported compiler!")
endif()

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release;Debug;ASan;UBSan;MSan" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "CMAKE_BUILD_TYPE not specified. Set to RELWITHDEBINFO")
	set(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	include(CTest)
	enable_testing()
	message(STATUS "Tests are enabled")

  if(CODE_COVERAGE)
    message(STATUS "Code coverage is enabled")
  else()
    message(STATUS "Code coverage is disabled")
  endif()
endif()

include(cmake/utils.cmake)
include(cmake/code_coverage.cmake)

if(BUILD_TESTING)
  set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
  set(THREADS_PREFER_PTHREAD TRUE)
  find_package(Threads REQUIRED)
  include(3rdparty/googletest/build.cmake)
endif()

if(PERFORM_STATIC_CHECKS)
  find_most_recent_program(CLANG_TIDY clang-tidy)
  function(perform_static_checks target)
    set_target_properties(
      ${target} PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY};-config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
  endfunction()
  message(STATUS "Static checks are enabled")
else()
  function(perform_static_checks target)
  endfunction()
  message(STATUS "Static checks are disabled")
endif()

find_most_recent_program(CLANG_FORMAT clang-format)

if(CODE_COVERAGE)
  add_custom_target(coverage
    COMMAND make ccov-tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

add_custom_target(format
  COMMAND ${SHELL}
  ${CMAKE_SOURCE_DIR}/scripts/format.sh
  ${CLANG_FORMAT}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

include(cmake/build_flags.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)
