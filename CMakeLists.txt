cmake_minimum_required(VERSION 3.20)

project(chess_engine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chess_engine_lib
	src/board.cpp
	src/loader.cpp
	src/display.cpp
)

target_include_directories(chess_engine_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_executable(chess_engine
    app/main.cpp
)

target_link_libraries(chess_engine
    PRIVATE
        chess_engine_lib
)