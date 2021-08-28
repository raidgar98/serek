cmake_minimum_required(VERSION 3.21)
set(PROJECT_NAME "serek")
project(${PROJECT_NAME} LANGUAGES CXX)

# prevent build in source
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not allowed, remove CMakeFiles and CMakeCache.txt")
endif()

# setup cmake
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# update submodules
find_program(GIT_PROGRAM git)
if(NOT GIT_PROGRAM)
	message(WARNING "git not found, make sure that submodules are updated")
else()
	message("git found, updating submodules")
	execute_process(COMMAND ${GIT_PROGRAM} submodule update --init --recursive WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
endif()

# CMAKE paths
set(CUSTOM_CMAKE_SCRIPTS_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(SOURCE_PATH ${PROJECT_SOURCE_DIR}/src)
set(HEADER_PATH ${PROJECT_SOURCE_DIR}/include)

#includes
include("${CUSTOM_CMAKE_SCRIPTS_DIR}/attach_package.cmake")
include("${CUSTOM_CMAKE_SCRIPTS_DIR}/add_compiler_flags.cmake")

# setup threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
if(NOT ${Threads_FOUND})
	set(CMAKE_THREAD_LIBS_INIT "-lpthread")
	set(CMAKE_HAVE_THREADS_LIBRARY 1)
	set(CMAKE_USE_WIN32_THREADS_INIT 0)
	set(CMAKE_USE_PTHREADS_INIT 1)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

# C++ setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compiler_flags( -fcoroutines )
add_compiler_flags(
	-Werror
	-Wall
	-Wextra
	-pedantic

	# allowed
	-Wno-missing-field-initializers
	-Wno-overloaded-virtual
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compiler_flags( -Weverything )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compiler_flags(
		-Wshadow
		-Wnon-virtual-dtor
		-Wold-style-cast
		-Wunused
		-Wsign-conversion
		-Wmisleading-indentation
		-Wduplicated-cond
		-Wduplicated-branches
		-Wlogical-op
		-Wuseless-cast
		-Wdouble-promotion
	)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	add_compiler_flags( -O3 )
else()
	add_compiler_flags( -fconcepts-diagnostics-depth=5 )
endif()

# pre-configuring boost
	# stacktrace
	add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)
	IF( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" )
		message("attachibng debug symbols")
		add_compiler_flags(-g3)
	ENDIF()

# adding boost
find_library(BOOST_STACKTRACE_BACKTRACE_LIBRARY NAMES boost_stacktrace_backtrace)

# create library
add_library( ${PROJECT_NAME} STATIC ${SOURCE_PATH}/serek.cpp)
target_include_directories( ${PROJECT_NAME} PUBLIC ${HEADER_PATH} PRIVATE ${Boost_INCLUDE_DIRS} )
target_link_libraries(serek PRIVATE dl ${Boost_LIBRARIES} backtrace )

# doxygen
option(DOXYGEN "determines to generate docs or not" OFF)
IF(${DOXYGEN})

	# Preparing tree driectory
	set(DOXYFILE_PATH_SOURCE "${CMAKE_SOURCE_DIR}/docs/Doxyfile.in")
	set(DOXYFILE_PATH_DEST "${CMAKE_BINARY_DIR}/docs")
	file(MAKE_DIRECTORY ${DOXYFILE_PATH_DEST})
	file(COPY ${DOXYFILE_PATH_SOURCE} DESTINATION ${DOXYFILE_PATH_DEST})
	set(DOXYFILE_PATH_DEST "${DOXYFILE_PATH_DEST}/Doxyfile")
	file(RENAME ${DOXYFILE_PATH_DEST}.in ${DOXYFILE_PATH_DEST})

	# Setting up sources
	file(APPEND ${DOXYFILE_PATH_DEST} "\nINPUT = ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src \n")
	file(APPEND ${DOXYFILE_PATH_DEST} "OUTPUT_DIRECTORY = ${CMAKE_BINARY_DIR}/docs \n")
	# file(APPEND ${DOXYFILE_PATH_DEST} "EXCLUDE_PATTERNS = \n")
	
	# Image
	file(APPEND ${DOXYFILE_PATH_DEST} "PROJECT_LOGO = \"${CMAKE_SOURCE_DIR}/gen/docs/image.png\" \n")


	# Adding target
	find_program(DOXYGEN_BINARY doxygen HINTS /usr/bin)
	# To run: `make docs`
	add_custom_target(docs COMMAND ${DOXYGEN_BINARY} ${DOXYFILE_PATH_DEST} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
ENDIF()

# formatting
option(FORMAT "determines to create make format that performes format all files !!! IN SOURCE DIR !!!" ON)
IF(${FORMAT})

	# get all files to format
	file(GLOB_RECURSE FILES_TO_FORMAT LIST_DIRECTORIES false RELATIVE ${CMAKE_SOURCE_DIR} *.hpp *.cpp *.h *.test.h)

	# filter out direcotries
	set(FILES_FOR_CLANG_FORMAT)
	foreach(x ${FILES_TO_FORMAT})
		if(x MATCHES "tests/src/tests.cpp.in")
			continue() 
		endif()
		list(APPEND FILES_FOR_CLANG_FORMAT "${x}")
	endforeach()

	# locate program
	find_program(CLANG_FORMAT_BINARY clang-format HINTS /usr/bin)

	# To run: `make format` in build directory
	add_custom_target(format COMMAND ${CLANG_FORMAT_BINARY} -style=file -i ${FILES_FOR_CLANG_FORMAT} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
ENDIF()

# cppcheck
option(STATIC_CHECK "if set to on [default] create target 'static_check' that allows to execute static code analysis of profect" ON)
if(${STATIC_CHECK})
	find_program (CPPCHECK "cppcheck")
	find_program(PYTHON "python")
	set( CPP_CHECK_OUTPUT_FILE cpp_check.log )

	add_custom_target (static_check
							COMMAND cppcheck
							--enable=all
							--std=c++20
							--suppress=missingIncludeSystem
							--project=compile_commands.json
							--check-config
							--verbose
							--check-library
							--output-file=${CPP_CHECK_OUTPUT_FILE}
							--report-progress
							--error-exitcode=-1
	)
endif()

# add tests
# add_subdirectory(tests)

if( EXISTS ${CMAKE_SOURCE_DIR}/workbook.cpp )
	add_executable(main ${CMAKE_SOURCE_DIR}/workbook.cpp)
	target_link_libraries(main PRIVATE serek)
endif()

