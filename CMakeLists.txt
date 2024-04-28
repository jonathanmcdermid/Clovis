cmake_minimum_required(VERSION 3.12)
project(Clovis)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# If CMAKE_BUILD_TYPE is not specified, set it to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)   
endif()

message("Using CMake version: ${CMAKE_VERSION}")

# Add source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

message("CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})

# Set compiler flags
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-Wall -Werror -Wfatal-errors -Wold-style-cast -Wextra -pedantic -Wno-error=restrict -Wno-deprecated-enum-enum-conversion -Wno-error=deprecated-enum-enum-conversion")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "/WX /W4 /DWIN32 /D_WINDOWS /GR /EHsc /utf-8")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Ob2 /DNDEBUG")
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

message("CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
message("CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG})
message("CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE})

# Set output directory
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}"
	ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}"
	LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}"
)

# Add custom clean target
add_custom_target(clean-all COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIR}")

# Specify include directories
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
