cmake_minimum_required(VERSION 3.22)
project(CHIP8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SFML 3 COMPONENTS Graphics REQUIRED)

add_library(
    chip8_core STATIC
    src/Machine.cpp
)
target_include_directories(chip8_core PUBLIC
    include
)

target_compile_options(chip8_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>    
)

add_executable(CHIP8 src/main.cpp)
target_link_libraries(CHIP8 PRIVATE chip8_core SFML::Graphics)
target_compile_options(CHIP8 PRIVATE 
    $<$<CXX_COMPILER_ID:GNU,Clang>: -Wall -Wextra -Wpedantic>
)

if(WIN32)
    add_custom_command(TARGET CHIP8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:CHIP8>
            $<TARGET_FILE_DIR:CHIP8>
        COMMAND_EXPAND_LISTS
    )
endif()
