cmake_minimum_required(VERSION 3.5.0)
project(ToDoSync)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SOURCE_DIR src)

add_compile_options(-Wall)

include_directories(include)

add_executable(ToDoSync src/main.cpp
        src/Server.cpp
        src/Parser.cpp
        src/Repository.cpp
        src/OperationService.cpp
        src/Validator.cpp)
