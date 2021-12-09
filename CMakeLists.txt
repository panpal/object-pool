cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(object-pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
        ${PROJECT_NAME}

        src/objectPool.cpp
)