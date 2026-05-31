#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for maze_solver_common test files                        #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_maze_solver_common.cpp
    ${CMAKE_SOURCE_DIR}/src/common/maze_solver_common.c
    ${CMAKE_SOURCE_DIR}/src/common/maze_solver_common.h
)

add_executable(test_maze_solver_common
    ${CMAKE_CURRENT_SOURCE_DIR}/test_maze_solver_common.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_maze_solver_common PRIVATE
    maze_solver_common_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_maze_solver_common COMMAND test_maze_solver_common)
