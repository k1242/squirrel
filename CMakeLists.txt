cmake_minimum_required(VERSION 4.0)
project(squirrel CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(engine)

add_executable(squirrel
    main.cpp
    engine/Move.cpp
)