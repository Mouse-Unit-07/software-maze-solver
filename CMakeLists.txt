#--------------------------------- FILE INFO ----------------------------------#
# Filename           : CMakeLists.txt                                          #
#                                                                              #
# Top level CMakeLists.txt file (for this software repo)                       #
#                                                                              #
#------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 4.0.2)
project(maze_solver_project LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
    mouse_driver
    GIT_REPOSITORY git@github.com:Mouse-Unit-07/software-mouse-driver.git
    GIT_TAG main
)
FetchContent_MakeAvailable(mouse_driver)

add_subdirectory(common)
add_subdirectory(partial_flood_fill)
add_subdirectory(wall_follower)

if(TARGET_WINDOWS AND SUPPORTS_WINDOWS)
    if(ENABLE_MAZE_SOLVER_TESTS)
        add_subdirectory(common/tests)
        add_subdirectory(partial_flood_fill/tests)
        add_subdirectory(wall_follower/tests)
    endif()
endif()

add_library(maze_solver_lib INTERFACE)

target_link_libraries(maze_solver_lib INTERFACE
    maze_solver_common_lib
    partial_flood_fill_lib
    wall_follower_lib
)
