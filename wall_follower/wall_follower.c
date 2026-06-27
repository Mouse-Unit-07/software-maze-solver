/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : wall_follower.c                                       */
/*                                                                            */
/* Implementation for wall follower maze solving algorithm                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "maze_solver_common.h"
#include "wall_follower.h"

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void run_wall_follower(enum wall_follower_mode mode, bool enable_print)
{
    reset_maze_solver_state();

    while (!is_solver_timeout() && !is_mouse_at_goal()
           && !(is_maze_fully_explored() && !can_reach_goal())) {
        execute_move(determine_wall_follower_move(mode));

        if (enable_print) {
            print_maze_solver_state();
        }
    }

    if (is_solver_timeout() || (is_maze_fully_explored() && !can_reach_goal())) {
        return;
    }
    set_goal_found(true);

    while (!is_solver_timeout() && !is_maze_fully_explored()) {
        uint32_t estimated_return_time_sec = estimate_return_to_start_time_sec();
        uint32_t estimated_speed_run_time_sec = estimate_best_path_to_goal_time_sec();
        uint32_t remaining_time_sec = get_solver_remaining_time_sec();

        if (remaining_time_sec <= (estimated_return_time_sec + estimated_speed_run_time_sec)) {
            break;
        }

        execute_move(determine_wall_follower_move(mode));

        if (enable_print) {
            print_maze_solver_state();
        }
    }

    return_to_start();
    if (enable_print) {
        print_maze_solver_state();
    }

    calculate_fastest_path();

    execute_speed_run_path();
    if (enable_print) {
        print_maze_solver_state();
    }
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
enum movement determine_wall_follower_move(enum wall_follower_mode mode)
{
    if (mode == WALL_FOLLOWER_LEFT) {
        if (!is_left_wall_present_in_map()) {
            return MOVE_LEFT;
        }

        if (!is_front_wall_present_in_map()) {
            return MOVE_FORWARD_CONTINUOUS;
        }

        if (!is_right_wall_present_in_map()) {
            return MOVE_RIGHT;
        }

        return MOVE_TURN_AROUND;
    }

    if (!is_right_wall_present_in_map()) {
        return MOVE_RIGHT;
    }

    if (!is_front_wall_present_in_map()) {
        return MOVE_FORWARD_CONTINUOUS;
    }

    if (!is_left_wall_present_in_map()) {
        return MOVE_LEFT;
    }

    return MOVE_TURN_AROUND;
}
