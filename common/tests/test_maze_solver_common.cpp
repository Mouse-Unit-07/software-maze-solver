/*================================ FILE INFO =================================*/
/* Filename           : test_maze_solver_common.cpp                           */
/*                                                                            */
/* Test implementation for maze_solver_common.c                               */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/*                               Include Files                                */
/*============================================================================*/
extern "C"
{

#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include "maze_solver_common.h"

}

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

/*============================================================================*/
/*                             Public Definitions                             */
/*============================================================================*/
struct maze_solver_config create_default_maze_solver_config(void)
{
    struct maze_solver_config cfg = {0};

    cfg.maze_size = 16u;
    cfg.total_timeout_sec = 300u;
    cfg.move_forward_time_sec = 1u;
    cfg.rotate_90_deg_time_sec = 2u;
    cfg.rotate_180_deg_time_sec = 3u;

    return cfg;
}

void initialize_4_by_4_maze(void)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 4u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();
}

/*============================================================================*/
/*                            Mock Implementations                            */
/*============================================================================*/
extern "C"
{

uint32_t get_current_global_time_sec(void)
{
    return mock().actualCall("get_current_global_time_sec")
        .returnUnsignedIntValue();
}

uint32_t get_elapsed_global_time_sec(uint32_t start_time_sec)
{
    return mock().actualCall("get_elapsed_global_time_sec")
        .returnUnsignedIntValue();
}

void move_forward(void)
{
    mock().actualCall("move_forward");
}

void rotate_clockwise_90_deg(void)
{
    mock().actualCall("rotate_clockwise_90_deg");
}

void rotate_counter_clockwise_90_deg(void)
{
    mock().actualCall("rotate_counter_clockwise_90_deg");
}

void rotate_180_deg(void)
{
    mock().actualCall("rotate_180_deg");
}

bool is_left_wall_present(void)
{
    return mock().actualCall("is_left_wall_present")
        .returnBoolValue();
}

bool is_right_wall_present(void)
{
    return mock().actualCall("is_right_wall_present")
        .returnBoolValue();
}

bool is_front_wall_present(void)
{
    return mock().actualCall("is_front_wall_present")
        .returnBoolValue();
}

}

/*============================================================================*/
/*                                 Test Group                                 */
/*============================================================================*/
TEST_GROUP(MazeSolverCommonTests)
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
TEST(MazeSolverCommonTests, InitMazeSolverCommonResetsAllState)
{
    mock().expectOneCall("get_current_global_time_sec")
          .andReturnValue(123u);

    init_maze_solver_common();

    struct maze_solver_config cfg{get_maze_solver_config()};

    CHECK(cfg.maze_size == 0u);
    CHECK(cfg.total_timeout_sec == 0u);
    CHECK(cfg.move_forward_time_sec == 0u);
    CHECK(cfg.rotate_90_deg_time_sec == 0u);
    CHECK(cfg.rotate_180_deg_time_sec == 0u);

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == 0u);
    CHECK(mouse.direction == 0u);

    CHECK(get_solver_start_time_sec() == 123u);

    CHECK_FALSE(get_goal_found());

    struct maze_solver_path path{get_fastest_path()};

    CHECK(path.length == 0u);

    for (uint32_t y{0u}; y < MAX_DIMENSION_CELL_COUNT; y++) {
        for (uint32_t x{0u}; x < MAX_DIMENSION_CELL_COUNT; x++) {
            CHECK(get_maze_cell(x, y).flags == 0u);
        }
    }
}

TEST(MazeSolverCommonTests, DeinitMazeSolverCommonResetsAllState)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    set_maze_solver_config(cfg);
    set_goal_found(true);

    mock().expectOneCall("get_current_global_time_sec")
          .andReturnValue(456u);

    deinit_maze_solver_common();

    cfg = get_maze_solver_config();

    CHECK(cfg.maze_size == 0u);
    CHECK(cfg.total_timeout_sec == 0u);
    CHECK(cfg.move_forward_time_sec == 0u);
    CHECK(cfg.rotate_90_deg_time_sec == 0u);
    CHECK(cfg.rotate_180_deg_time_sec == 0u);

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == 0u);
    CHECK(mouse.direction == 0u);

    CHECK(get_solver_start_time_sec() == 456u);

    CHECK_FALSE(get_goal_found());

    struct maze_solver_path path{get_fastest_path()};

    CHECK(path.length == 0u);

    for (uint32_t y{0u}; y < MAX_DIMENSION_CELL_COUNT; y++) {
        for (uint32_t x{0u}; x < MAX_DIMENSION_CELL_COUNT; x++) {
            CHECK(get_maze_cell(x, y).flags == 0u);
        }
    }
}

TEST(MazeSolverCommonTests, SetMazeSolverConfigStoresConfiguration)
{
    struct maze_solver_config expected{create_default_maze_solver_config()};

    set_maze_solver_config(expected);

    struct maze_solver_config actual{get_maze_solver_config()};

    CHECK(expected.maze_size == actual.maze_size);
    CHECK(expected.total_timeout_sec == actual.total_timeout_sec);
    CHECK(expected.move_forward_time_sec == actual.move_forward_time_sec);
    CHECK(expected.rotate_90_deg_time_sec == actual.rotate_90_deg_time_sec);
    CHECK(expected.rotate_180_deg_time_sec == actual.rotate_180_deg_time_sec);
}

TEST(MazeSolverCommonTests, ResetMazeSolverStateResetsMouseAndGoalFound)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    set_maze_solver_config(cfg);
    set_goal_found(true);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == 0u);
    CHECK(mouse.direction == DIRECTION_NORTH);

    CHECK_FALSE(get_goal_found());
}

TEST(MazeSolverCommonTests, ResetMazeSolverStateInitializesMazeBoundaryWalls)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 4u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    struct map_cell south_west = get_maze_cell(0u, 0u);
    struct map_cell north_east = get_maze_cell(3u, 3u);

    CHECK((south_west.flags & CELL_SOUTH_WALL_KNOWN) != 0u);
    CHECK((south_west.flags & CELL_SOUTH_WALL_PRESENT) != 0u);

    CHECK((south_west.flags & CELL_WEST_WALL_KNOWN) != 0u);
    CHECK((south_west.flags & CELL_WEST_WALL_PRESENT) != 0u);

    CHECK((north_east.flags & CELL_NORTH_WALL_KNOWN) != 0u);
    CHECK((north_east.flags & CELL_NORTH_WALL_PRESENT) != 0u);

    CHECK((north_east.flags & CELL_EAST_WALL_KNOWN) != 0u);
    CHECK((north_east.flags & CELL_EAST_WALL_PRESENT) != 0u);

    CHECK(is_wall_known_at_coordinate({1u,0u}, DIRECTION_SOUTH));
    CHECK(is_wall_present_at_coordinate({1u,0u}, DIRECTION_SOUTH));
}

TEST(MazeSolverCommonTests, GetMazeSizeReturnsConfiguredSize)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 8u;

    set_maze_solver_config(cfg);

    CHECK(get_maze_size() == 8u);
}

TEST(MazeSolverCommonTests, GoalFoundCanBeSet)
{
    set_goal_found(false);
    CHECK_FALSE(get_goal_found());

    set_goal_found(true);
    CHECK(get_goal_found());

    set_goal_found(false);
    CHECK_FALSE(get_goal_found());
}

TEST(MazeSolverCommonTests, IsGoalCellReturnsTrueForCenterOfOddMaze)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 5u;

    set_maze_solver_config(cfg);

    CHECK(is_goal_cell({2u, 2u}));

    CHECK_FALSE(is_goal_cell({1u, 2u}));
}

TEST(MazeSolverCommonTests, IsGoalCellReturnsTrueForFourCenterCellsOfEvenMaze)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 4u;

    set_maze_solver_config(cfg);

    CHECK(is_goal_cell({1u,1u}));
    CHECK(is_goal_cell({1u,2u}));
    CHECK(is_goal_cell({2u,1u}));
    CHECK(is_goal_cell({2u,2u}));

    CHECK_FALSE(is_goal_cell({0u,0u}));
}

TEST(MazeSolverCommonTests, IsMouseAtGoalReturnsFalseAtStartForOddMaze)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 3u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    CHECK_FALSE(is_mouse_at_goal());
}

TEST(MazeSolverCommonTests, IsMouseAtGoalReturnsTrueAtStartForTwoByTwoMaze)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 2u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    CHECK(is_mouse_at_goal());
}

TEST(MazeSolverCommonTests, GetCurrentCoordinatesAndDirectionReturnMouseState)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_RIGHT);

    struct coordinates coord{get_current_coordinates()};

    CHECK(coord.x == 1u);
    CHECK(coord.y == 0u);

    CHECK(get_current_direction() == DIRECTION_EAST);
}

TEST(MazeSolverCommonTests, GetLeftDirectionReturnsExpectedDirection)
{
    CHECK(get_left_direction(DIRECTION_NORTH) == DIRECTION_WEST);
    CHECK(get_left_direction(DIRECTION_EAST) == DIRECTION_NORTH);
    CHECK(get_left_direction(DIRECTION_SOUTH) == DIRECTION_EAST);
    CHECK(get_left_direction(DIRECTION_WEST) == DIRECTION_SOUTH);
}

TEST(MazeSolverCommonTests, GetRightDirectionReturnsExpectedDirection)
{
    CHECK(get_right_direction(DIRECTION_NORTH) == DIRECTION_EAST);
    CHECK(get_right_direction(DIRECTION_EAST) == DIRECTION_SOUTH);
    CHECK(get_right_direction(DIRECTION_SOUTH) == DIRECTION_WEST);
    CHECK(get_right_direction(DIRECTION_WEST) == DIRECTION_NORTH);
}

TEST(MazeSolverCommonTests, GetOppositeDirectionReturnsExpectedDirection)
{
    CHECK(get_opposite_direction(DIRECTION_NORTH) == DIRECTION_SOUTH);
    CHECK(get_opposite_direction(DIRECTION_EAST) == DIRECTION_WEST);
    CHECK(get_opposite_direction(DIRECTION_SOUTH) == DIRECTION_NORTH);
    CHECK(get_opposite_direction(DIRECTION_WEST) == DIRECTION_EAST);
}

TEST(MazeSolverCommonTests, GetTurnRequiredReturnsExpectedMovement)
{
    CHECK(get_turn_required(DIRECTION_NORTH, DIRECTION_NORTH) == MOVE_FORWARD);
    CHECK(get_turn_required(DIRECTION_NORTH, DIRECTION_WEST) == MOVE_LEFT);
    CHECK(get_turn_required(DIRECTION_NORTH, DIRECTION_EAST) == MOVE_RIGHT);
    CHECK(get_turn_required(DIRECTION_NORTH, DIRECTION_SOUTH) == MOVE_TURN_AROUND);
    CHECK(get_turn_required(DIRECTION_EAST, DIRECTION_NORTH) == MOVE_LEFT);
    CHECK(get_turn_required(DIRECTION_EAST, DIRECTION_SOUTH) == MOVE_RIGHT);
    CHECK(get_turn_required(DIRECTION_EAST, DIRECTION_WEST) == MOVE_TURN_AROUND);
}

TEST(MazeSolverCommonTests, WallQueriesReturnExpectedValues)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present")
          .andReturnValue(false);

    mock().expectOneCall("is_right_wall_present")
          .andReturnValue(true);

    update_current_cell_walls();

    CHECK(is_wall_known_at_coordinate({0u,0u}, DIRECTION_NORTH));

    CHECK_FALSE(is_wall_present_at_coordinate({0u,0u}, DIRECTION_NORTH));

    CHECK(is_wall_present_at_coordinate({0u,0u}, DIRECTION_WEST));
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingNorthUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present")
          .andReturnValue(false);

    mock().expectOneCall("is_right_wall_present")
          .andReturnValue(true);

    update_current_cell_walls();

    CHECK(is_left_wall_known_in_map());
    CHECK(is_left_wall_present_in_map());

    CHECK(is_front_wall_known_in_map());
    CHECK_FALSE(is_front_wall_present_in_map());

    CHECK(is_right_wall_known_in_map());
    CHECK(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingEastUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_RIGHT);

    mock().expectOneCall("is_left_wall_present")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present")
          .andReturnValue(false);

    mock().expectOneCall("is_right_wall_present")
          .andReturnValue(true);

    update_current_cell_walls();

    CHECK(is_left_wall_known_in_map());
    CHECK(is_left_wall_present_in_map());

    CHECK(is_front_wall_known_in_map());
    CHECK_FALSE(is_front_wall_present_in_map());

    CHECK(is_right_wall_known_in_map());
    CHECK(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingSouthUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_180_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_TURN_AROUND);

    mock().expectOneCall("is_left_wall_present")
          .andReturnValue(false);

    mock().expectOneCall("is_front_wall_present")
          .andReturnValue(true);

    mock().expectOneCall("is_right_wall_present")
          .andReturnValue(false);

    update_current_cell_walls();

    CHECK(is_left_wall_known_in_map());
    CHECK_FALSE(is_left_wall_present_in_map());

    CHECK(is_front_wall_known_in_map());
    CHECK(is_front_wall_present_in_map());

    CHECK(is_right_wall_known_in_map());
    CHECK_FALSE(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingWestUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_counter_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_LEFT);

    mock().expectOneCall("is_left_wall_present")
          .andReturnValue(false);

    mock().expectOneCall("is_front_wall_present")
          .andReturnValue(true);

    mock().expectOneCall("is_right_wall_present")
          .andReturnValue(true);

    update_current_cell_walls();

    CHECK(is_left_wall_known_in_map());
    CHECK_FALSE(is_left_wall_present_in_map());

    CHECK(is_front_wall_known_in_map());
    CHECK(is_front_wall_present_in_map());

    CHECK(is_right_wall_known_in_map());
    CHECK(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdatingWallUpdatesNeighborCell)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present")
          .andReturnValue(true);

    mock().expectOneCall("is_front_wall_present")
          .andReturnValue(false);

    mock().expectOneCall("is_right_wall_present")
          .andReturnValue(true);

    update_current_cell_walls();

    CHECK(is_wall_known_at_coordinate({0u,1u}, DIRECTION_SOUTH));

    CHECK_FALSE(is_wall_present_at_coordinate({0u,1u}, DIRECTION_SOUTH));
}

TEST(MazeSolverCommonTests, IsSolverTimeoutReturnsFalseBeforeTimeout)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.total_timeout_sec = 300u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_elapsed_global_time_sec")
          .andReturnValue(299u);

    CHECK_FALSE(is_solver_timeout());
}

TEST(MazeSolverCommonTests, IsSolverTimeoutReturnsTrueAtTimeout)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.total_timeout_sec = 300u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_elapsed_global_time_sec")
          .andReturnValue(300u);

    CHECK(is_solver_timeout());
}

TEST(MazeSolverCommonTests, GetSolverRemainingTimeSecReturnsRemainingTime)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.total_timeout_sec = 300u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_elapsed_global_time_sec")
          .andReturnValue(123u);

    CHECK(get_solver_remaining_time_sec() == 177u);
}

TEST(MazeSolverCommonTests, GetSolverRemainingTimeSecReturnsZeroAfterTimeout)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.total_timeout_sec = 300u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_elapsed_global_time_sec")
          .andReturnValue(400u);

    CHECK(get_solver_remaining_time_sec() == 0u);
}

TEST(MazeSolverCommonTests, EstimateReturnToStartTimeSecReturnsPathTime)
{
    struct maze_solver_config cfg = {create_default_maze_solver_config()};
    cfg.maze_size = 4u;
    cfg.move_forward_time_sec = 1u;
    cfg.rotate_90_deg_time_sec = 2u;
    cfg.rotate_180_deg_time_sec = 3u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    /* Open north wall of start cell */
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);

    update_current_cell_walls();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    /* Open east wall at (0,1) */
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);

    update_current_cell_walls();

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward");
    execute_move(MOVE_RIGHT);

    /* to return, we need to turn around, move forward, turn right, move forward */
    CHECK(estimate_return_to_start_time_sec() == 7u);
}

TEST(MazeSolverCommonTests, EstimateReturnToStartTimeSecReturnsMaxWhenNoPathExists)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    CHECK(estimate_return_to_start_time_sec() == UINT32_MAX);
}

TEST(MazeSolverCommonTests, EstimateReturnToStartTimeIsZeroWhenAlreadyAtStart)
{
    initialize_4_by_4_maze();

    CHECK(estimate_return_to_start_time_sec() == 0u);
}

TEST(MazeSolverCommonTests, EstimateBestPathToGoalTimeSecReturnsShortestKnownPathTime)
{
    struct maze_solver_config cfg = {create_default_maze_solver_config()};
    cfg.maze_size = 4u;
    cfg.move_forward_time_sec = 1u;
    cfg.rotate_90_deg_time_sec = 2u;
    cfg.rotate_180_deg_time_sec = 3u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    /* Open north wall from start */
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);

    update_current_cell_walls();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    /* Open east wall at (0,1) */
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);

    update_current_cell_walls();

    /* shortest from start to a goal cell is move forward, turn right, move forward */
    CHECK(estimate_best_path_to_goal_time_sec() == 4u);
}

TEST(MazeSolverCommonTests, EstimateBestPathToGoalTimeSecReturnsMaxWhenNoPathExists)
{
    initialize_4_by_4_maze();

    CHECK(estimate_best_path_to_goal_time_sec() == UINT32_MAX);
}

TEST(MazeSolverCommonTests, ExecuteMoveForwardMovesMouseNorthOneCell)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");

    execute_move(MOVE_FORWARD);

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == 1u);
    CHECK(mouse.direction == DIRECTION_NORTH);
}

TEST(MazeSolverCommonTests, ExecuteMoveLeftRotatesAndMovesMouse)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_counter_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_LEFT);

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == (uint8_t)-1); /* expected underflow */
    CHECK(mouse.coordinates.y == 0u);
    CHECK(mouse.direction == DIRECTION_WEST);
}

TEST(MazeSolverCommonTests, ExecuteMoveRightRotatesAndMovesMouse)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_RIGHT);

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 1u);
    CHECK(mouse.coordinates.y == 0u);
    CHECK(mouse.direction == DIRECTION_EAST);
}

TEST(MazeSolverCommonTests, ExecuteMoveTurnAroundRotatesAndMovesMouse)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("rotate_180_deg");
    mock().expectOneCall("move_forward");

    execute_move(MOVE_TURN_AROUND);

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == (uint8_t)-1); /* expected underflow */
    CHECK(mouse.direction == DIRECTION_SOUTH);
}

TEST(MazeSolverCommonTests, ReturnToStartFollowsShortestPathBackToOrigin)
{
    initialize_4_by_4_maze();

    /* open north from start */
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    /* open east from (0,1) */
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    update_current_cell_walls();

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward");
    execute_move(MOVE_RIGHT);

    /*
     * Expected return:
     * (1,1,E) -> (0,1,W)
     * (0,1,W) -> (0,0,S)
     */
    mock().expectOneCall("rotate_180_deg");
    mock().expectOneCall("move_forward");

    mock().expectOneCall("rotate_counter_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    return_to_start();

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == 0u);
}

TEST(MazeSolverCommonTests, ReturnToStartDoesNothingWhenNoPathExists)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    return_to_start();

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 0u);
    CHECK(mouse.coordinates.y == 1u);
}

TEST(MazeSolverCommonTests, CalculateFastestPathStoresShortestPathToGoal)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    update_current_cell_walls();

    calculate_fastest_path();

    struct maze_solver_path path{get_fastest_path()};

    CHECK(path.length > 0u);

    CHECK(path.cells[0].x == 0u);
    CHECK(path.cells[0].y == 0u);

    CHECK(path.cells[path.length - 1u].x == 1u);
    CHECK(path.cells[path.length - 1u].y == 1u);
}

TEST(MazeSolverCommonTests, CalculateFastestPathStoresEmptyPathWhenGoalNotReachable)
{
    initialize_4_by_4_maze();

    calculate_fastest_path();

    struct maze_solver_path path{get_fastest_path()};

    CHECK(path.length == 0u);
}

TEST(MazeSolverCommonTests, ExecuteSpeedRunPathFollowsStoredPath)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    mock().expectOneCall("move_forward");
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    update_current_cell_walls();

    calculate_fastest_path();

    struct maze_solver_path path{get_fastest_path()};

    CHECK(path.length == 3u);

    CHECK(path.cells[0].x == 0u);
    CHECK(path.cells[0].y == 0u);

    CHECK(path.cells[1].x == 0u);
    CHECK(path.cells[1].y == 1u);

    CHECK(path.cells[2].x == 1u);
    CHECK(path.cells[2].y == 1u);

    mock().expectOneCall("get_current_global_time_sec")
        .andReturnValue(123u);

    reset_maze_solver_state();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward");

    execute_speed_run_path();

    struct mouse mouse{get_mouse()};

    CHECK(mouse.coordinates.x == 1u);
    CHECK(mouse.coordinates.y == 1u);
}
