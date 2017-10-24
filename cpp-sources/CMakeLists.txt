cmake_minimum_required(VERSION 2.8)

project(Caustic-lasertag-system)

add_subdirectory(universal-device)

# This is to run all tests using 'make run-unit-tests' command.
# Another way is simply run 'ctest -VV', but output is not colorized by default :(
# This project may be a part of other cmake project so this trik ma be implementerd yet
if (NOT COMMAND run_test)
	add_custom_target(run-unit-tests)
	macro(run_test test_target)
	  add_custom_target(${test_target}_runtest
	      COMMAND ${test_target}
	      DEPENDS ${test_target}
	      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
	  add_dependencies(run-unit-tests ${test_target}_runtest)
	endmacro()
endif()

enable_testing()

#add_subdirectory(unit-tests)
