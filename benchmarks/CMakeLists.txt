cmake_minimum_required(VERSION 3.21)
set(PROJECT_NAME "benchmark_server")
project(${PROJECT_NAME} LANGUAGES CXX)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.cpp ${PROJECT_SOURCE_DIR}/controllers/benchmark_controllers.cpp)

# flags because of: https://github.com/drogonframework/drogon/issues/1253#issuecomment-1140426219
set(CMAKE_CXX_FLAGS "-Wno-error=restrict")

# serek
target_link_libraries(${PROJECT_NAME} PRIVATE serek)

# disable tests and examples in subprojects
set(BUILD_TESTS OFF)
set(BUILD_TESTING OFF)
set(BUILD_EXAMPLES OFF)

# drogon
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/drogon drogon)
target_link_libraries(${PROJECT_NAME} PRIVATE drogon)

# fc
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/fc fc)
target_link_libraries(${PROJECT_NAME} PRIVATE fc)

target_include_directories( ${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/controllers PRIVATE ${Boost_INCLUDE_DIRS} drogon )
