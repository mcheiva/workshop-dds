cmake_minimum_required(VERSION 3.27)
project(EIVA_WORKSHOP CXX)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ddsbus CONFIG REQUIRED)

add_subdirectory(idls)
add_subdirectory(solution)
