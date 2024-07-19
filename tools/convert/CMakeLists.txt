set(TARGET_NAME "convert")
add_executable(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "tools")
target_link_libraries(${TARGET_NAME} PUBLIC clovis_lib)
