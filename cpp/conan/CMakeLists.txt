cmake_minimum_required(VERSION 3.27)
project(EIVA_WORKSHOP CXX)

find_package(ddsbus CONFIG REQUIRED)

add_subdirectory(idls)
add_subdirectory(solution)
