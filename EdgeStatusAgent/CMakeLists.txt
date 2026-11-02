cmake_minimum_required(VERSION 3.16)
project(EdgeStatusAgent LANGUAGES CXX)

add_executable(EdgeStatusAgent src/main.cpp)
target_compile_features(EdgeStatusAgent PRIVATE cxx_std_17)

install(TARGETS EdgeStatusAgent RUNTIME DESTINATION bin)
