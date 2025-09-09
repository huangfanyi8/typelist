cmake_minimum_required(VERSION 3.28)
project(meta
            LANGUAGES CXX
            DESCRIPTION "CXX17 implement TMP base on type list"
            HOMEPAGE_URL "https://github.com/huangfanyi8/typelist")

set(CMAKE_CXX_STANDARD 17)
add_library(tmp_14 INTERFACE include/meta.hpp)
add_library(tmp_17 INTERFACE include/meta_17.hpp)
set(root_directory ${CMAKE_CURRENT_SOURCE_DIR})
add_library(meta INTERFACE meta/meta.h)
target_compile_features(meta INTERFACE cxx_std_17)

add_executable(meta_test  meta/meta.cpp)
