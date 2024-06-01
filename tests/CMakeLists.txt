include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/afd902e992b720d1b3e106bc5e425a5768872265.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Avoid installing GoogleTest when installing this project.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

macro(create_test name)
    add_executable(
        ${name}
        src/powerit.cpp
    )

    target_link_libraries(
        ${name}
        gtest_main
        powerit
    )

    target_compile_options(libtest PRIVATE -Wall -Wpedantic -Werror -Wextra)

    set(CODE_COVERAGE OFF CACHE BOOL "Enable coverage testing for tests")
    if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${name} PRIVATE -O0 -g --coverage)
        target_link_options(${name} PRIVATE --coverage)
    endif()

    gtest_discover_tests(${name})
endmacro()

create_test(libtest)

find_package(OpenMP)
if(OpenMP_FOUND)
    create_test(omptest)
    target_link_libraries(omptest OpenMP::OpenMP_CXX)
endif()

create_test(partest)
target_compile_definitions(partest PRIVATE TEST_CUSTOM_PARALLEL=1)
