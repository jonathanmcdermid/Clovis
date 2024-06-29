cmake_minimum_required(VERSION 3.15)

project(clovis VERSION 3.3.7 LANGUAGES CXX)

option(BUILD_TESTS "Build tests" ON)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(COVERAGE "Coverage" OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Check platforms
if(CMAKE_HOST_WIN32)
    set(WINDOWS 1)
    cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
    set(DISTRIB_NAME ${OS_NAME})
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
    cmake_host_system_information(RESULT DISTRIB_NAME QUERY DISTRIB_NAME)
    cmake_host_system_information(RESULT DISTRIB_VERSION_ID QUERY DISTRIB_VERSION_ID)
    set(DISTRIB_NAME "${DISTRIB_NAME}-${DISTRIB_VERSION_ID}")
else()
    message(WARNING "Unable to identify OS. Update script to support distribution or OS")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-Wall -Wfatal-errors -Wno-deprecated-enum-enum-conversion -Wno-error=deprecated-enum-enum-conversion")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
	if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "/WX /W4 /DWIN32 /D_WINDOWS /GR /EHsc /utf-8 /wd5054 /wd4244 /wd4127")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Ob2 /DNDEBUG")
else()
    message(WARNING "Unable to identify compiler.")
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
