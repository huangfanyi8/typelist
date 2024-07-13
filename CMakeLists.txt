cmake_minimum_required(VERSION 3.28)
project(meta
            LANGUAGES CXX
            DESCRIPTION "CXX17 implement TMP base on  type list"
            HOMEPAGE_URL "https://github.com/huangfanyi8/typelist")

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)