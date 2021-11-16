cmake_minimum_required(VERSION 3.10)

project(orbitals)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(src)