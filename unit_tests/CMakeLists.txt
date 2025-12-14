# https://google.github.io/googletest/quickstart-cmake.html
cmake_minimum_required(VERSION 3.14)
project(Flexheeple_Unit_Tests)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # Release v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

if (NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_C_FLAGS_DEBUG "-ggdb3")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
else
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall /Wextra /std:clatest")
    set(CMAKE_C_FLAGS_DEBUG "/ggdb3")
    set(CMAKE_C_FLAGS_RELEASE "/O3")
endif(NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
else
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} /Wall /Wextra /std:c++latest")
    set(CMAKE_CXX_FLAGS_DEBUG "/ggdb3")
    set(CMAKE_CXX_FLAGS_RELEASE "/O3")
endif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

add_executable(fsv_unit_test fsv_unit_test.cpp)
target_link_libraries(
    fsv_unit_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(fsv_unit_test)
