set(TEST_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SHARED_TOOLS_ROOT_PATH ${TEST_DIRECTORY_PATH}/shared)
add_subdirectory(ut)

add_library(shared_tools ${SHARED_TOOLS_ROOT_PATH}/tools.cpp )
target_link_libraries(shared_tools PUBLIC serek ut)
target_include_directories(shared_tools PUBLIC ${SHARED_TOOLS_ROOT_PATH}/include PRIVATE ${${PROJECT_NAME}_INCLUDE_DIRS})
