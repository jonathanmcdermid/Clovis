cmake_minimum_required(VERSION 3.15)

project(clovis LANGUAGES CXX)

option(BUILD_TESTS "Build tests" ON)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(COVERAGE "Coverage" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)   
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "/W4 /DWIN32 /D_WINDOWS /GR /EHsc /utf-8 /wd5054 /wd4244 /wd4127")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Ob2 /DNDEBUG")

    if(WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
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
	message(FATAL_ERROR "Unsupported compiler")
endif()

# Output all binaries in the same directory for easier testing
foreach(config ${CMAKE_BUILD_TYPE} ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} BUILD_TYPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${BUILD_TYPE} "${CMAKE_BINARY_DIR}/bin/${config}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${BUILD_TYPE} "${CMAKE_BINARY_DIR}/bin/${config}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE} "${CMAKE_BINARY_DIR}/bin/${config}")
endforeach()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
add_library("engine" STATIC ${SOURCES})

add_custom_target(clean-all COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
