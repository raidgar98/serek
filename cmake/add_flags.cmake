MACRO(add_compiler_flags)

	set(variadic ${ARGN})
	foreach(var ${variadic})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${var}")
	endforeach()

ENDMACRO()

MACRO(add_ctest_flags)

	set(variadic ${ARGN})
	foreach(var ${variadic})
		list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
	endforeach()

ENDMACRO()
