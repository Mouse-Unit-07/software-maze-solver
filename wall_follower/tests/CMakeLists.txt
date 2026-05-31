#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for wall_follower test files                             #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_wall_follower.cpp
    ${CMAKE_SOURCE_DIR}/src/wall_follower/wall_follower.c
    ${CMAKE_SOURCE_DIR}/src/wall_follower/wall_follower.h
)

add_executable(test_wall_follower
    ${CMAKE_CURRENT_SOURCE_DIR}/test_wall_follower.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_wall_follower PRIVATE
    wall_follower_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_wall_follower COMMAND test_wall_follower)
