#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# CMakeLists.txt file for partial_flood_fill source files                      #
#                                                                              #
#------------------------------------------------------------------------------#
add_library(partial_flood_fill_lib STATIC
    ${CMAKE_CURRENT_LIST_DIR}/partial_flood_fill.c
)

target_include_directories(partial_flood_fill_lib PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(partial_flood_fill_lib PUBLIC
    maze_solver_common_lib
    mouse_driver_lib
)
