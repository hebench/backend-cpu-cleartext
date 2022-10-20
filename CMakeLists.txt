cmake_minimum_required(VERSION 3.12)
project(hebench_cleartext_backend LANGUAGES C CXX)

set(CMAKE_PROJECT_NAME ${PROJECT_NAME})

# General Setup
if(CMAKE_BUILD_TYPE)
  set(RELEASE_TYPES
      Debug
      Release
      RelWithDebInfo
      MinSizeRel)
  list(FIND RELEASE_TYPES ${CMAKE_BUILD_TYPE} INDEX_FOUND)
  if(${INDEX_FOUND} EQUAL -1)
    message(
      FATAL_ERROR
        "CMAKE_BUILD_TYPE must be one of Debug, Release, RelWithDebInfo, or MinSizeRel"
      )
  endif()
else()
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of Build" FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

###########################################################################################

# Benchmarks

## matrices
set(be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Matrix/include/mat_data_container.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Matrix/include/inl/mat_data_container.inl"
    )

## matmult
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Matrix/MatMult/include/bench_matmult.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Matrix/MatMult/latency/include/bench_matmul_l.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Matrix/MatMult/offline/include/bench_matmul_o.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Matrix/MatMult/include/inl/bench_matmult.inl"
    )

## vectors
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/include/vec_data_container.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/include/inl/vec_data_container.inl"
    )

## eltwiseadd
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseAdd/include/bench_eltadd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseAdd/latency/include/bench_eltadd_l.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseAdd/offline/include/bench_eltadd_o.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseAdd/include/inl/bench_eltadd.inl"
    )

## eltwisemult
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseMult/include/bench_eltmult.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseMult/latency/include/bench_eltmult_l.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseMult/offline/include/bench_eltmult_o.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/EltwiseMult/include/inl/bench_eltmult.inl"
    )

## dotproduct
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/DotProduct/include/bench_dotprod.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/DotProduct/latency/include/bench_dotprod_l.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/DotProduct/offline/include/bench_dotprod_o.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/DotProduct/include/inl/bench_dotprod.inl"
    )

## logisticregression
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/LogisticRegression/include/bench_logreg.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/LogisticRegression/latency/include/bench_logreg_l.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/LogisticRegression/offline/include/bench_logreg_o.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/LogisticRegression/include/inl/bench_logreg.inl"
    )

## logisticregression
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/SimpleSetIntersection/include/bench_simplesetint.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/SimpleSetIntersection/latency/include/bench_simplesetint_l.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/SimpleSetIntersection/offline/include/bench_simplesetint_o.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/Vector/SimpleSetIntersection/include/inl/bench_simplesetint.inl"
    )

## main
list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/clear_error.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/clear_engine.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/clear_version.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/data_container.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/clear_benchmark.h"
    )

list(APPEND be_cleartext_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/inl/data_container.inl"
    )

set(be_cleartext_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/clear_engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/clear_benchmark.cpp"
    )

add_library(${PROJECT_NAME} SHARED)# ${be_cleartext_SOURCES} ${be_cleartext_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

###########################################################################################

# third-party
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/third-party)

# Must Set 4 Variable per third-party, have a _COMPONENT_NAME.cmake file ready, then include the import util.
# Goal is for cmake to fail out if particular library (lib + includes) dont exist/aren't found
## _COMPONENT_NAME: The name used to generate lib-specific variables, printing, and filename resolution
## _COMPONENT_HEADER: A path to a known header file relative to the imported include directory
## _COMPONENT_LIB_NAME: The name of the library that will be used to link to it
## _HEADER_ONLY: Whether or not the library being imported is header-only

## API-BRIDGE
set(_COMPONENT_NAME "API_BRIDGE")
set(_COMPONENT_HEADER "hebench/api_bridge/api.h")
set(_COMPONENT_LIB_NAME "hebench_cpp")
set(_HEADER_ONLY FALSE)
include(cmake/utils/import-library.cmake)
include(cmake/third-party/${_COMPONENT_NAME}_check.cmake)

## GSL
set(_COMPONENT_NAME "GSL")
set(_COMPONENT_HEADER "gsl/gsl") 
set(_COMPONENT_LIB_NAME "GSL")
set(_HEADER_ONLY TRUE)
include(cmake/utils/import-library.cmake)


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/clear_version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/clear_version.h.in1")
configure_file("${CMAKE_CURRENT_BINARY_DIR}/include/clear_version.h.in1" "${CMAKE_CURRENT_SOURCE_DIR}/include/clear_version.h")

target_sources(${PROJECT_NAME} PRIVATE ${be_cleartext_SOURCES} ${be_cleartext_HEADERS})

###########################################################################################

include(GNUInstallDirs)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
