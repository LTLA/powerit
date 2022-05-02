cmake_minimum_required(VERSION 3.14)

project(powerit
    VERSION 1.0.0
    DESCRIPTION "C++ implementation of power iterations"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(powerit INTERFACE)

add_subdirectory(extern)

target_link_libraries(powerit INTERFACE aarand)

target_include_directories(powerit INTERFACE include/)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()
