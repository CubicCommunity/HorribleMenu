cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

project(HorribleIdeas VERSION 1.0.0)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE MODULES CONFIGURE_DEPENDS src/*.cppm)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
    PRIVATE ${SOURCES}
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES ${MODULES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

if(PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE AWCW_HORRIBLE_API_EXPORTING)
endif()

if(NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

setup_geode_mod(${PROJECT_NAME})
