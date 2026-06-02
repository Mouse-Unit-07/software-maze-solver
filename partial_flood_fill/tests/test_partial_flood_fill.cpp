/*================================ FILE INFO =================================*/
/* Filename           : test_partial_flood_fill.cpp                           */
/*                                                                            */
/* Test implementation for partial_flood_fill.c                               */
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
#include "partial_flood_fill.h"

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

uint32_t get_maze_size(void)
{
    return mock().actualCall("get_maze_size")
        .returnUnsignedIntValue();
}

bool is_goal_cell(struct coordinates coord)
{
    return mock()
        .actualCall("is_goal_cell")
        .withUnsignedIntParameter("x", coord.x)
        .withUnsignedIntParameter("y", coord.y)
        .returnBoolValue();
}

struct coordinates get_current_coordinates(void)
{
    struct coordinates coord;

    coord.x = (uint8_t)mock().actualCall("get_current_coordinates")
                  .returnUnsignedIntValue();

    coord.y = (uint8_t)mock().actualCall("get_current_coordinates_y")
                  .returnUnsignedIntValue();

    return coord;
}

enum direction get_current_direction(void)
{
    return (enum direction)mock().actualCall("get_current_direction")
        .returnUnsignedIntValue();
}

bool is_wall_known_at_coordinate(struct coordinates coord, enum direction dir)
{
    return mock()
        .actualCall("is_wall_known_at_coordinate")
        .withUnsignedIntParameter("x", coord.x)
        .withUnsignedIntParameter("y", coord.y)
        .withUnsignedIntParameter("dir", (unsigned int)dir)
        .returnBoolValue();
}

bool is_wall_present_at_coordinate(struct coordinates coord, enum direction dir)
{
    return mock()
        .actualCall("is_wall_present_at_coordinate")
        .withUnsignedIntParameter("x", coord.x)
        .withUnsignedIntParameter("y", coord.y)
        .withUnsignedIntParameter("dir", (unsigned int)dir)
        .returnBoolValue();
}

enum movement get_turn_required(enum direction from,
                                enum direction to)
{
    return (enum movement)mock().actualCall("get_turn_required")
        .withUnsignedIntParameter("from", (unsigned int)from)
        .withUnsignedIntParameter("to", (unsigned int)to)
        .returnUnsignedIntValue();
}

struct map_cell get_maze_cell(uint8_t x, uint8_t y)
{
    struct map_cell cell;

    cell.flags = (uint8_t)mock().actualCall("get_maze_cell")
                     .withUnsignedIntParameter("x", x)
                     .withUnsignedIntParameter("y", y)
                     .returnUnsignedIntValue();

    return cell;
}

}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(PartialFloodFillTests)
{
    void setup() override
    {
        mock().clear();
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
TEST(PartialFloodFillTests, RunPartialFloodFillReturnsImmediatelyOnTimeout)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(true);

    run_partial_flood_fill(false);
}

TEST(PartialFloodFillTests, RunPartialFloodFillExecutesSpeedRunWhenGoalAlreadyReached)
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

    mock().ignoreOtherCalls();

    run_partial_flood_fill(false);
}

TEST(PartialFloodFillTests, RunPartialFloodFillExploresMaze)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(false);

    mock().expectOneCall("update_current_cell_walls");

    mock().expectOneCall("execute_move")
        .withUnsignedIntParameter("move", MOVE_FORWARD);

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

    mock().ignoreOtherCalls();

    run_partial_flood_fill(false);
}

TEST(PartialFloodFillTests, RunPartialFloodFillPrintsWhenEnabled)
{
    mock().expectOneCall("reset_maze_solver_state");

    mock().expectOneCall("is_solver_timeout")
          .andReturnValue(false);

    mock().expectOneCall("is_mouse_at_goal")
          .andReturnValue(false);

    mock().expectOneCall("update_current_cell_walls");

    mock().expectOneCall("execute_move")
        .withUnsignedIntParameter("move", MOVE_FORWARD);

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

    mock().ignoreOtherCalls();

    run_partial_flood_fill(true);
}
