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

if(MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    add_definitions(-DWIN32 -D_WINDOWS)
    add_compile_options(/W4 /GR /EHsc /utf-8)
    add_compile_options("$<$<CONFIG:Release>:/Ox;/Ob2>")

    if(WARNINGS_AS_ERRORS)
        add_compile_options(/WX)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    add_compile_options(-Wall -Wextra -pedantic -Wold-style-cast)
    add_compile_options("$<$<CONFIG:Release>:-O3>")

    if(WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()

    if(COVERAGE)
        message("Coverage is On")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
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
