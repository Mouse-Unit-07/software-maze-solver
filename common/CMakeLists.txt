#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for maze_solver_common source files                      #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(maze_solver_common_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/maze_solver_common.c
)

target_include_directories(maze_solver_common_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    $<TARGET_PROPERTY:mouse_driver_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(maze_solver_common_lib PRIVATE
    mouse_driver_lib
)
