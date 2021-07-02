cmake_minimum_required(VERSION 3.19)
project(oop_final_game)

set(CMAKE_CXX_STANDARD 17)

# SDL2
set(SDL_SHARED OFF)
add_subdirectory(3rd-party/SDL2)

# liquidfun
set(BOX2D_BUILD_EXAMPLES OFF)
set(BOX2D_BUILD_UNITTESTS OFF)
cmake_policy(SET CMP0077 NEW)
add_subdirectory(3rd-party/liquidfun)
target_include_directories(Box2D PUBLIC 3rd-party/liquidfun/Box2D)

add_executable(oop_final_game main.cpp window.cpp)
target_link_libraries(oop_final_game Box2D SDL2-static)