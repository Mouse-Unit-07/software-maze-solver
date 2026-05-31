#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for partial_flood_fill test files                        #
#                                                                              #
#------------------------------------------------------------------------------#
set_property(GLOBAL APPEND PROPERTY
    ALL_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_partial_flood_fill.cpp
    ${CMAKE_SOURCE_DIR}/src/partial_flood_fill/partial_flood_fill.c
    ${CMAKE_SOURCE_DIR}/src/partial_flood_fill/partial_flood_fill.h
)

add_executable(test_partial_flood_fill
    ${CMAKE_CURRENT_SOURCE_DIR}/test_partial_flood_fill.cpp
    ${CMAKE_SOURCE_DIR}/platforms/windows/test_main.cpp
)

target_link_libraries(test_partial_flood_fill PRIVATE
    partial_flood_fill_lib
    CppUTest::CppUTest
    CppUTestExt
)

add_test(NAME test_partial_flood_fill COMMAND test_partial_flood_fill)
