cmake_minimum_required(VERSION 3.14...3.25)

project(powerit
    VERSION 1.0.0
    DESCRIPTION "C++ implementation of power iterations"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library
add_library(powerit INTERFACE)
add_library(ltla::powerit ALIAS powerit)

target_include_directories(powerit INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ltla>)
target_compile_features(powerit INTERFACE cxx_std_17)

# Dependencies
option(POWERIT_FETCH_EXTERN "Automatically fetch powerit's external dependencies." ON)

if(POWERIT_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(ltla_aarand CONFIG REQUIRED)
endif()

target_link_libraries(powerit INTERFACE ltla::aarand)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(POWERIT_TESTS "" ON)
else()
    option(POWERIT_TESTS "" OFF)
endif()
if(POWERIT_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

# Install
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ltla)

install(TARGETS powerit
    EXPORT poweritTargets)

install(EXPORT poweritTargets
    FILE ltla_poweritTargets.cmake
    NAMESPACE ltla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_powerit)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_poweritConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_powerit)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ltla_poweritConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_powerit)
