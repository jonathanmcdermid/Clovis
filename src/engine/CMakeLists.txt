set(TARGET_NAME "clovis_engine")
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${TARGET_NAME} ${SOURCES})
add_library(${PROJECT_NAME}::engine ALIAS ${TARGET_NAME})
target_include_directories(${TARGET_NAME} PUBLIC include)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "engine"
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)
