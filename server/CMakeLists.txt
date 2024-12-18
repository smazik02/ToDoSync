cmake_minimum_required(VERSION 3.30)
project(ToDoSync)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SOURCE_DIR src)

add_compile_options(-Wall)

include_directories(include)

add_executable(ToDoSync src/main.cpp
        src/server/Server.cpp
        src/server/Server.hpp
        src/types.hpp
        src/repository/Repository.cpp
        src/repository/Repository.hpp)
