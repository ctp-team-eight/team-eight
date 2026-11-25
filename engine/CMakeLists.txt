cmake_minimum_required(VERSION 3.20)
project(medaid_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(MEDAID_BUILD_TESTS "Build the C++ unit tests" OFF)

add_library(medaid_core STATIC src/series.cpp src/integrate.cpp src/interpolate.cpp)
target_include_directories(medaid_core PUBLIC include)
target_compile_options(medaid_core PRIVATE -Wall -Wextra -Wpedantic)

# SKBUILD is set when pip builds the package through scikit-build-core.
# A plain CMake build skips this, so the C++ tests do not need Python.
if(SKBUILD)
  find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development.Module)
  find_package(nanobind CONFIG REQUIRED)

  nanobind_add_module(_core NB_STATIC bindings/module.cpp)
  target_link_libraries(_core PRIVATE medaid_core)
  install(TARGETS _core LIBRARY DESTINATION medaid_engine)
endif()

if(MEDAID_BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
  )
  FetchContent_MakeAvailable(Catch2)

  enable_testing()
  add_executable(engine_tests tests/cpp/test_engine.cpp)
  target_link_libraries(engine_tests PRIVATE medaid_core Catch2::Catch2WithMain)
  add_test(NAME engine_tests COMMAND engine_tests)
endif()
