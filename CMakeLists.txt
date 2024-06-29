cmake_minimum_required(VERSION 3.12)
project(clovis)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)   
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "/WX /W4 /DWIN32 /D_WINDOWS /GR /EHsc /utf-8 /wd5054 /wd4244 /wd4127")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Ob2 /DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	set(CMAKE_CXX_FLAGS "-Werror -Wall -Wfatal-errors -Wno-deprecated-enum-enum-conversion -Wno-error=deprecated-enum-enum-conversion")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-restrict")
	endif()
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")

# Add source files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
add_library("engine" STATIC ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}"
	ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}"
	LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}"
)

add_custom_target(clean-all COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

add_subdirectory(test)
