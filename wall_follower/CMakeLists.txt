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
)

target_link_libraries(wall_follower_lib PRIVATE
    mouse_driver_lib
)
