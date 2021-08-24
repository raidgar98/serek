MACRO(add_compiler_flags)

	set(variadic ${ARGN})
	foreach(var ${variadic})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${var}")
	endforeach()
	message("current compile command flags: ${CMAKE_CXX_FLAGS}")

ENDMACRO()