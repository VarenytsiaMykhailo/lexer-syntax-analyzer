cmake_minimum_required(VERSION 3.17)
project(syntax_analyzer)

set(CMAKE_CXX_STANDARD 20)

add_executable(syntax_analyzer main.cpp testFragment.cpp token.hpp)