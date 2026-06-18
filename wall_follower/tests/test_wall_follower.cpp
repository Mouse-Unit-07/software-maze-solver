/*================================ FILE INFO =================================*/
/* Filename           : test_wall_follower.cpp                                */
/*                                                                            */
/* Test implementation for wall_follower.c                                    */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdbool.h>
#include <stdint.h>
#include "maze_solver_common.h"
#include "wall_follower.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
/* none */

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

void reset_maze_solver_state(void)
{
    mock().actualCall("reset_maze_solver_state");
}

bool is_solver_timeout(void)
{
    return mock().actualCall("is_solver_timeout")
        .returnBoolValue();
}

bool is_mouse_at_goal(void)
{
    return mock().actualCall("is_mouse_at_goal")
        .returnBoolValue();
}

void update_current_cell_walls(void)
{
    mock().actualCall("update_current_cell_walls");
}

void execute_move(enum movement move)
{
    mock().actualCall("execute_move")
        .withUnsignedIntParameter("move", (unsigned int)move);
}

void set_goal_found(bool found)
{
    mock().actualCall("set_goal_found")
        .withBoolParameter("found", found);
}

uint32_t estimate_return_to_start_time_sec(void)
{
    return mock().actualCall("estimate_return_to_start_time_sec")
        .returnUnsignedIntValue();
}

uint32_t estimate_best_path_to_goal_time_sec(void)
{
    return mock().actualCall("estimate_best_path_to_goal_time_sec")
        .returnUnsignedIntValue();
}

uint32_t get_solver_remaining_time_sec(void)
{
    return mock().actualCall("get_solver_remaining_time_sec")
        .returnUnsignedIntValue();
}

void return_to_start(void)
{
    mock().actualCall("return_to_start");
}

void calculate_fastest_path(void)
{
    mock().actualCall("calculate_fastest_path");
}

void execute_speed_run_path(void)
{
    mock().actualCall("execute_speed_run_path");
}

void print_maze_solver_state(void)
{
    mock().actualCall("print_maze_solver_state");
}

bool is_left_wall_present_in_map(void)
{
    return mock().actualCall("is_left_wall_present_in_map")
        .returnBoolValue();
}

bool is_front_wall_present_in_map(void)
{
    return mock().actualCall("is_front_wall_present_in_map")
        .returnBoolValue();
}

bool is_right_wall_present_in_map(void)
{
    return mock().actualCall("is_right_wall_present_in_map")
        .returnBoolValue();
}

}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(WallFollowerTests)
{
    void setup() override
    {
        mock().clear();
        mock().strictOrder();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/*============================================================================*/
/*                                    Tests                                   */
/*============================================================================*/
TEST(WallFollowerTests, LeftFollowerPrefersLeft)
{
    mock().expectOneCall("is_left_wall_present_in_map")
          .andReturnValue(false);

    CHECK(determine_wall_follower_move(WALL_FOLLOWER_LEFT) == MOVE_LEFT);
}

TEST(WallFollowerTests, LeftFollowerMovesForwardWhenLeftBlocked)
{
    mock().expectOneCall("is_left_wall_present_in_map")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present_in_map")
          .andReturnValue(false);

    CHECK(determine_wall_follower_move(WALL_FOLLOWER_LEFT) == MOVE_FORWARD);
}

TEST(WallFollowerTests, LeftFollowerTurnsRightWhenLeftAndFrontBlocked)
{
    mock().expectOneCall("is_left_wall_present_in_map")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present_in_map")
          .andReturnValue(true);

    mock().expectOneCall("is_right_wall_present_in_map")
          .andReturnValue(false);

    CHECK(determine_wall_follower_move(WALL_FOLLOWER_LEFT) == MOVE_RIGHT);
}

TEST(WallFollowerTests, LeftFollowerTurnsAroundWhenTrapped)
{
    mock().expectOneCall("is_left_wall_present_in_map")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present_in_map")
          .andReturnValue(true);

    mock().expectOneCall("is_right_wall_present_in_map")
          .andReturnValue(true);

    CHECK(determine_wall_follower_move(WALL_FOLLOWER_LEFT) == MOVE_TURN_AROUND);
}

TEST(WallFollowerTests, RunWallFollowerReturnsImmediatelyOnTimeout)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    run_wall_follower(WALL_FOLLOWER_LEFT, false);
}

TEST(WallFollowerTests, RunWallFollowerExecutesSpeedRunWhenGoalAlreadyReached)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("set_goal_found")
          .withBoolParameter("found", true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    mock().expectOneCall("return_to_start");

    mock().expectOneCall("calculate_fastest_path");

    mock().expectOneCall("execute_speed_run_path");

    run_wall_follower(WALL_FOLLOWER_LEFT, false);
}

TEST(WallFollowerTests, RunWallFollowerExploresMaze)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(false);

    mock().expectOneCall("is_left_wall_present_in_map")
          .andReturnValue(false);

    mock().expectOneCall("execute_move")
          .withUnsignedIntParameter("move", MOVE_LEFT);

    mock().expectOneCall("update_current_cell_walls");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("set_goal_found")
          .withBoolParameter("found", true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    mock().expectOneCall("return_to_start");
    mock().expectOneCall("calculate_fastest_path");
    mock().expectOneCall("execute_speed_run_path");

    run_wall_follower(WALL_FOLLOWER_LEFT, false);
}

TEST(WallFollowerTests, RunWallFollowerPrintsWhenEnabled)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(false);

    mock().expectOneCall("is_left_wall_present_in_map")
          .andReturnValue(false);

    mock().expectOneCall("execute_move")
          .withUnsignedIntParameter("move", MOVE_LEFT);

    mock().expectOneCall("update_current_cell_walls");

    mock().expectOneCall("print_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("set_goal_found")
          .withBoolParameter("found", true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    mock().expectOneCall("return_to_start");

    mock().expectOneCall("print_maze_solver_state");

    mock().expectOneCall("calculate_fastest_path");

    mock().expectOneCall("execute_speed_run_path");

    mock().expectOneCall("print_maze_solver_state");

    run_wall_follower(WALL_FOLLOWER_LEFT, true);
}
