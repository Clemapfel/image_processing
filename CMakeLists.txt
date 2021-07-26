cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -lstdc++fs -pthread -lpthread")
set(CMAKE_CXX_STANDARD 20)

project(image_processing)

# sfml
include_directories("./SFML/include")
find_package(SFML COMPONENTS graphics REQUIRED)

# lua
include_directories("./sol2/include")
include_directories("/usr/include/lua5.3")
find_package(Lua REQUIRED)

# eigen
include_directories("./eigen/Eigen")

# common
add_library(common STATIC
        include/image.hpp
        include/color.hpp
        .src/color.inl
        .src/color.cpp
        include/vector.hpp
        .src/vector.inl)

set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(common sfml-graphics ${LUA_LIBRARIES})

# include directories for convenience
include_directories("../image_processing") # for .inl files
include_directories("include")
include_directories(".src") # for .inl files

# executables
add_executable(TEST .test/main.cpp)
target_link_libraries(TEST common)
