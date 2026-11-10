cmake_minimum_required(VERSION 3.21)

project(Lab5_Buoyancy_Variant16 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(Lab5_Buoyancy main.cpp)

target_link_libraries(Lab5_Buoyancy PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

if(MSVC)
    target_compile_options(Lab5_Buoyancy PRIVATE /W4)
endif()

set_property(TARGET Lab5_Buoyancy PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)
