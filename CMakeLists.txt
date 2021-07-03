cmake_minimum_required(VERSION 3.19)
project(oop_final_game)

set(CMAKE_CXX_STANDARD 17)

# SDL2
set(SDL_SHARED OFF)
add_subdirectory(3rd-party/SDL2)

# SDL2_gfx
file(GLOB SDL2_GFX_SOURCE_FILES 3rd-party/SDL2_gfx/*.c)
add_library(SDL2-gfx STATIC ${SDL2_GFX_SOURCE_FILES})
target_link_libraries(SDL2-gfx SDL2-static)
target_include_directories(SDL2-gfx PUBLIC 3rd-party/SDL2_gfx)

# liquidfun
cmake_policy(SET CMP0077 NEW)
set(BOX2D_BUILD_EXAMPLES OFF)
set(BOX2D_BUILD_UNITTESTS OFF)
add_subdirectory(3rd-party/liquidfun)
target_include_directories(Box2D PUBLIC 3rd-party/liquidfun)

# CGAL
find_package(Boost REQUIRED)

add_executable(oop_final_game
        main.cpp
        window.cpp
        level.cpp
        engine.cpp
        game.cpp
        generate_polygon.cpp)
target_link_libraries(oop_final_game Box2D SDL2-static SDL2-gfx Boost::headers)