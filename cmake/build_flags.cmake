set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -stdlib=libc++ -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wodr -Wnon-virtual-dtor -Woverloaded-virtual -fno-operator-names")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align -Wpointer-arith -Wundef -Wcast-qual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self -Wmissing-include-dirs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls -Wswitch-default")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreturn-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb3 -gdwarf -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fno-inline -fno-inline-functions")

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ggdb3")

# Common sanitizer flags
set(SANITIZER_FLAGS "-O3 -g -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_ASAN} ${SANITIZER_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS_UBSAN "${CMAKE_CXX_FLAGS_UBSAN} ${SANITIZER_FLAGS} -fsanitize=vptr,shift,integer-divide-by-zero,unreachable,vla-bound,null,return,signed-integer-overflow,bounds,alignment,object-size,float-divide-by-zero,float-cast-overflow,nonnull-attribute,returns-nonnull-attribute,bool,enum")
set(CMAKE_CXX_FLAGS_MSAN "${CMAKE_CXX_FLAGS_MSAN} ${SANITIZER_FLAGS} -fsanitize=memory")
