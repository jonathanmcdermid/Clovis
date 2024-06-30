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

unset(CMAKE_CXX_FLAGS)
unset(CMAKE_CXX_FLAGS_DEBUG)
unset(CMAKE_CXX_FLAGS_RELEASE)

if(MSVC)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    add_definitions(-DWIN32 -D_WINDOWS)
    add_compile_options(/W4 /GR /EHsc /utf-8)
    add_compile_options("$<$<CONFIG:Release>:/Ox;/Ob2>")

    if(WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	add_compile_options(-Wall -Wextra -pedantic -Wold-style-cast)
    add_compile_options("$<$<CONFIG:Release>:-O3>")

    if(COVERAGE)
        message("Coverage is On")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()

    if(WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
else()
    message(WARNING "Unable to identify compiler.")
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
