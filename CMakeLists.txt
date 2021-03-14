cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_TOOLCHAIN_FILE "D:/Programming/Utilities/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(C2MEngine VERSION 0.1.0)

add_compile_options(/std:c++latest)

include(CTest)
enable_testing()

find_package(SFML COMPONENTS system window graphics REQUIRED)
find_package(GLEW REQUIRED)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory("./src")

target_link_libraries(C2MEngine PUBLIC sfml-system sfml-window sfml-graphics)
target_link_libraries(C2MEngine PUBLIC GLEW::GLEW)