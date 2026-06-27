#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for wall_follower source files                           #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(wall_follower_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/wall_follower.c
)

target_include_directories(wall_follower_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    $<TARGET_PROPERTY:maze_solver_common_lib,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:mouse_driver_lib,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(wall_follower_lib PRIVATE
    maze_solver_common_lib
    mouse_driver_lib
)
