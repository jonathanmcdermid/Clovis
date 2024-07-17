set(TARGET_NAME "clovis")
add_executable(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/uci.cpp)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC clovis::engine)
