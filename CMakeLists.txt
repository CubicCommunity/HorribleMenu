cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if(ANDROID)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(
            TARGET "${PROJECT_NAME}" POST_BUILD
            DEPENDS "${PROJECT_NAME}"
            COMMAND ${CMAKE_OBJCOPY}
            ARGS --only-keep-debug "$<TARGET_FILE:${PROJECT_NAME}>" "$<TARGET_FILE:${PROJECT_NAME}>.dbg"
        )
        add_custom_command(
            TARGET "${PROJECT_NAME}" POST_BUILD
            DEPENDS "${PROJECT_NAME}"
            COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}>
            ARGS --strip-all $<TARGET_FILE:${PROJECT_NAME}>
        )
        add_custom_command(
            TARGET "${PROJECT_NAME}" POST_BUILD
            DEPENDS "${PROJECT_NAME}"
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink="$<TARGET_FILE:${PROJECT_NAME}>.dbg" "$<TARGET_FILE:${PROJECT_NAME}>"
        )
    endif()
elseif(APPLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_custom_command(
            TARGET "${PROJECT_NAME}" POST_BUILD
            DEPENDS "${PROJECT_NAME}"
            COMMAND dsymutil $<TARGET_FILE:${PROJECT_NAME}>
            ARGS -o $<TARGET_FILE:${PROJECT_NAME}>.dSYM
        )
        add_custom_command(
            TARGET "${PROJECT_NAME}" POST_BUILD
            DEPENDS "${PROJECT_NAME}"
            COMMAND $<$<CONFIG:release>:${CMAKE_STRIP}>
            ARGS -x $<TARGET_FILE:${PROJECT_NAME}>
        )
    endif()
endif()

project(HorribleMenu VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

if(PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE AWCW_HORRIBLE_API_EXPORTING)
endif()

if(NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
