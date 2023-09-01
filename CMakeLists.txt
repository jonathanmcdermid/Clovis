cmake_minimum_required(VERSION 3.12)
project(Clovis)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message("Using CMake version: ${CMAKE_VERSION}")

# Add source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Set compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wshadow -pedantic -Wuseless-cast -Wold-style-cast -fopenmp -Wno-deprecated-enum-enum-conversion -std=c++20")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fPIC -D_LINUX_")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -DNDEBUG")
	message("CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
	message("CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG})
	message("CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE})
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /openmp")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /openmp")
	message("CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
	message("CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG})
	message("CMAKE_CXX_FLAGS_RELEASE: " ${CMAKE_CXX_FLAGS_RELEASE})
else()
	message(WARNING "Unsupported compiler")
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()

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
