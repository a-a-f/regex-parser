add_subdirectory(parser)

if (BUILD_TESTING)
    include_directories(SYSTEM ${GoogleTest_INCLUDE_DIRS})

    macro (collect_tests_sources ROOT_DIR DEST)
        file(GLOB_RECURSE "${DEST}" CONFIGURE_DEPENDS RELATIVE "${ROOT_DIR}" "*/tests/*.cpp")
    endmacro()

    collect_tests_sources("${CMAKE_SOURCE_DIR}/src" tests_SRC)

    add_executable(tests ${tests_SRC})
    add_dependencies(tests ${GoogleTest_TARGET})
    target_link_libraries(tests parser_lib ${GoogleTest_LIBRARIES} Threads::Threads)

    add_test(tests tests)
    target_code_coverage(tests EXCLUDE ${tests_SRC})
endif()
