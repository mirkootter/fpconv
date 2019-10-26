cmake_minimum_required(VERSION 3.8)

project("FPConv")

add_subdirectory("src/lib")
add_subdirectory("src/app")

add_executable(gui "src/dummy.cpp")
target_link_libraries(gui app)
