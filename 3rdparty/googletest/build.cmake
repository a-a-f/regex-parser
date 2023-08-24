include(ExternalProject)

set(GoogleTest_TARGET GoogleTest)

ExternalProject_Add(${GoogleTest_TARGET}
	URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/GoogleTest
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_CXX_FLAGS="-stdlib=libc++"
	BUILD_COMMAND ${MAKE})

ExternalProject_Get_Property(${GoogleTest_TARGET} install_dir)

add_library(libGoogleTest STATIC IMPORTED)
set_property(TARGET libGoogleTest PROPERTY IMPORTED LOCATION ${install_dir}/lib)
add_dependencies(libGoogleTest ${GoogleTest_TARGET})

set(GoogleTest_LIBRARIES
	${install_dir}/lib/libgmock_main.a
	${install_dir}/lib/libgmock.a
	${install_dir}/lib/libgtest.a)

set(GoogleTest_INCLUDE_DIRS "${install_dir}/include")
