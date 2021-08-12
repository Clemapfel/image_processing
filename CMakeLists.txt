cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -lstdc++fs -pthread -lpthread")
set(CMAKE_CXX_STANDARD 20)

project(image_processing)

# sfml
# TODO make generic
include_directories("/home/clem/Workspace/rat_game/submodules/SFML-2.5.1/include")
set(SFML_DIR "/home/clem/Workspace/rat_game/submodules/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5.1 COMPONENTS graphics window system REQUIRED)

# lua
include_directories("./sol2/include")
include_directories("/usr/include/lua5.3")
find_package(Lua REQUIRED)

# eigen
include_directories("./eigen/Eigen")

# ranges
include_directories("./range-v3/include")

# common
add_library(common STATIC

        include/image.hpp
        .src/image.inl

        include/color.hpp
        .src/color.inl

        include/vector.hpp
        .src/vector.inl

        include/render_window.hpp
        .src/render_window.inl

        include/key_id.hpp
        .src/input_handler.inl

        include/image_handler.hpp
        .src/image_handler.inl

        .src/grayscale_image.inl
        include/grayscale_image.hpp

        include/binary_image.hpp
        .src/binary_image.inl

        include/sprite.hpp
        .src/sprite.inl

        include/spatial_filter.hpp
        .src/spatial_filter.inl

        include/intensity_transform.hpp
        .src/intensity_transform.inl

        include/morphological_transform.hpp
        .src/morphological_transform.inl

        include/image_segment.hpp .src/image_segment.inl include/noise_model.hpp .src/noise_generator.inl .src/noise_generator.inl include/detail/salt_and_pepper_distribution.hpp include/histogram.hpp docs/noise.cpp include/fourier_transform.hpp include/frequency_domain_filter.hpp .src/frequency_domain_filter.inl .src/fourier_transform.inl include/color_image.hpp .src/color_image.inl include/pseudocolor_mapping.hpp include/edge_detection.hpp .src/pseudocolor_mapping.inl include/detail/common.hpp)

#set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(common sfml-graphics ${LUA_LIBRARIES})

# include directories for convenience
include_directories("../image_processing") # for .inl files
include_directories("include")
include_directories(".src") # for .inl files

# executables
add_executable(TEST .test/main.cpp)
target_link_libraries(TEST common sfml-graphics sfml-window sfml-system sfml-audio sfml-network fftw3 fftw3l fftw3f ${LUA_LIBRARIES})