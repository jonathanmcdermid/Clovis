cmake_minimum_required(VERSION 3.15)
include(cmake/third_party.cmake)
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
    add_compile_options(/W4 /GR /EHsc /utf-8 /wd5054 /wd4244 /wd4127)
    add_compile_options("$<$<CONFIG:Release>:/Ox;/Ob2>")

    if(WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    add_compile_options(-Wall -Wextra -pedantic -Wold-style-cast -Wno-deprecated-enum-enum-conversion)
    add_compile_options("$<$<CONFIG:Release>:-O3>")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wno-restrict)
    endif()

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

add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/engine/test)
endif()

add_custom_target(clean-all COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIR}")

if(WIN32)
    # Copy third-party DLLs to the build directory for tests
    copy_third_party_dlls()
    # Copy C++ runtime DLL for non-MSVC compilers
    if(NOT MSVC)
        if(NOT EXISTS CMAKE_CXX_COMPILER)
            find_program(CXX_COMPILER_PATH NAMES "${CMAKE_CXX_COMPILER}")
        else()
            set(CXX_COMPILER_PATH "${CMAKE_CXX_COMPILER}")
        endif()
        get_filename_component(COMPILER_BIN_DIR "${CXX_COMPILER_PATH}" DIRECTORY)
        foreach(stdcpp_library libstdc++-6.dll libc++.dll)
            if(EXISTS "${COMPILER_BIN_DIR}/${stdcpp_library}")
                foreach(config ${CMAKE_BUILD_TYPE} ${CMAKE_CONFIGURATION_TYPES})
                    file(COPY "${COMPILER_BIN_DIR}/${stdcpp_library}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${config}")
                endforeach()
            endif()
        endforeach()
    endif()
endif()
