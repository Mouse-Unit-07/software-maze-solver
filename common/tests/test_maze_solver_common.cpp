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

    mock().expectOneCall("get_current_global_time_sec");

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

uint32_t move_forward_until_turn_or_intersection_and_return_steps(void)
{
    return mock().actualCall("move_forward_until_turn_or_intersection_and_return_steps")
        .returnUnsignedIntValue();
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

    mock().expectOneCall("get_current_global_time_sec");

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

    mock().expectOneCall("get_current_global_time_sec");

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

    CHECK(is_wall_known_at_coordinate({1u, 0u}, DIRECTION_SOUTH));
    CHECK(is_wall_present_at_coordinate({1u, 0u}, DIRECTION_SOUTH));
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

TEST(MazeSolverCommonTests, IsMouseAtGoalReturnsFalseAtStartForOddMaze)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 3u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec");

    reset_maze_solver_state();

    CHECK_FALSE(is_mouse_at_goal());
}

TEST(MazeSolverCommonTests, IsMouseAtGoalReturnsTrueAtStartForTwoByTwoMaze)
{
    struct maze_solver_config cfg{create_default_maze_solver_config()};

    cfg.maze_size = 2u;

    set_maze_solver_config(cfg);

    mock().expectOneCall("get_current_global_time_sec");

    reset_maze_solver_state();

    CHECK(is_mouse_at_goal());
}

TEST(MazeSolverCommonTests, IsMazeFullyExploredReturnsFalseWhenStartCellUnknown)
{
    initialize_4_by_4_maze();

    CHECK_FALSE(is_maze_fully_explored());
}

TEST(MazeSolverCommonTests, IsMazeFullyExploredReturnsTrueWhenSingleCellMazeKnown)
{
    struct maze_solver_config cfg{};
    cfg.maze_size = 1u;
    cfg.total_timeout_sec = 100u;

    set_maze_solver_config(cfg);
    mock().expectOneCall("get_current_global_time_sec");
    reset_maze_solver_state();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    CHECK_TRUE(is_maze_fully_explored());
}

TEST(MazeSolverCommonTests, IsMazeFullyExploredReturnsFalseWhenReachableUnknownCellExists)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    CHECK_FALSE(is_maze_fully_explored());
}

TEST(MazeSolverCommonTests, IsMazeFullyExploredReturnsTrueWhenUnknownCellsAreNotReachable)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    execute_move(MOVE_FORWARD);

    CHECK_TRUE(is_maze_fully_explored());
}

TEST(MazeSolverCommonTests, CanReachGoalReturnsTrueWhenGoalReachableAndFalseOtherwise)
{
    initialize_4_by_4_maze();

    CHECK_FALSE(can_reach_goal());

    /* closed east from (0,1) */
    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    execute_move(MOVE_FORWARD);

    CHECK_FALSE(can_reach_goal());

    /* open east from (0,2) */
    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    CHECK_TRUE(can_reach_goal());
}

TEST(MazeSolverCommonTests, WallQueriesReturnExpectedValues)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    CHECK(is_wall_known_at_coordinate({0u, 0u}, DIRECTION_NORTH));
    CHECK_FALSE(is_wall_present_at_coordinate({0u, 0u}, DIRECTION_NORTH));
    CHECK(is_wall_present_at_coordinate({0u, 0u}, DIRECTION_WEST));
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingNorthUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    CHECK(is_left_wall_present_in_map());
    CHECK_FALSE(is_front_wall_present_in_map());
    CHECK(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingEastUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(false);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_RIGHT);

    CHECK_FALSE(is_left_wall_present_in_map());
    CHECK(is_front_wall_present_in_map());
    CHECK_FALSE(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingSouthUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("rotate_180_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    execute_move(MOVE_TURN_AROUND);

    CHECK(is_left_wall_present_in_map());
    CHECK(is_front_wall_present_in_map());
    CHECK(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdateCurrentCellWallsFacingWestUpdatesMap)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("rotate_counter_clockwise_90_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(false);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    execute_move(MOVE_LEFT);

    CHECK_FALSE(is_left_wall_present_in_map());
    CHECK(is_front_wall_present_in_map());
    CHECK(is_right_wall_present_in_map());
}

TEST(MazeSolverCommonTests, UpdatingWallUpdatesNeighborCell)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    update_current_cell_walls();

    CHECK(is_wall_known_at_coordinate({0u, 1u}, DIRECTION_SOUTH));
    CHECK_FALSE(is_wall_present_at_coordinate({0u, 1u}, DIRECTION_SOUTH));
}

TEST(MazeSolverCommonTests, KnownWallsAreNotOverwritten)
{
    initialize_4_by_4_maze();

    /* Start cell east wall initialized as present */

    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);  /* attempt to clear east wall */
    update_current_cell_walls();

    CHECK(is_wall_known_at_coordinate({0u, 0u}, DIRECTION_EAST));
    CHECK(is_wall_present_at_coordinate({0u, 0u}, DIRECTION_EAST));
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

    mock().expectOneCall("get_current_global_time_sec");

    reset_maze_solver_state();

    /* Open north wall of start cell */
    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    /* Open east wall at (0,1) */
    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    execute_move(MOVE_RIGHT);

    /* to return, we need to turn around, move forward, turn left, move forward */
    LONGS_EQUAL(7u, estimate_return_to_start_time_sec());
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

    mock().expectOneCall("get_current_global_time_sec");

    reset_maze_solver_state();

    /* Open north wall from start */
    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

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
    mock().expectOneCall("is_left_wall_present").andReturnValue(false);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
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
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(false);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);

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
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(false);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);

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
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(false);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);

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
    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(false);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    /* open east from (0,1) */
    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_RIGHT);

    /*
     * Expected return:
     * (1,1,E) -> (0,1,W)
     * (0,1,W) -> (0,0,S)
     */
    mock().expectOneCall("rotate_180_deg");
    mock().expectOneCall("move_forward")
        .andReturnValue(1u);

    mock().expectOneCall("rotate_counter_clockwise_90_deg");
    mock().expectOneCall("move_forward")
        .andReturnValue(1u);

    return_to_start();

    struct mouse mouse{get_mouse()};

    LONGS_EQUAL(0u, mouse.coordinates.x);
    LONGS_EQUAL(0u, mouse.coordinates.y);
}

TEST(MazeSolverCommonTests, CalculateFastestPathStoresShortestPathToGoal)
{
    initialize_4_by_4_maze();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(false);
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().expectOneCall("is_left_wall_present").andReturnValue(true);
    mock().expectOneCall("is_front_wall_present").andReturnValue(true);
    mock().expectOneCall("is_right_wall_present").andReturnValue(true);
    execute_move(MOVE_RIGHT);

    calculate_fastest_path();

    struct maze_solver_path path{get_fastest_path()};

    LONGS_EQUAL(3u, path.length);

    LONGS_EQUAL(0u, path.cells[0].x);
    LONGS_EQUAL(0u, path.cells[0].y);

    LONGS_EQUAL(1u, path.cells[path.length - 1u].x);
    LONGS_EQUAL(1u, path.cells[path.length - 1u].y);
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

    mock().expectOneCall("move_forward");
    mock().ignoreOtherCalls();
    execute_move(MOVE_FORWARD);

    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward_until_turn_or_intersection_and_return_steps")
        .andReturnValue(1u);
    mock().ignoreOtherCalls();
    execute_move(MOVE_RIGHT);

    calculate_fastest_path();

    struct maze_solver_path path{get_fastest_path()};

    LONGS_EQUAL(3u, path.length);

    LONGS_EQUAL(0u, path.cells[0].x);
    LONGS_EQUAL(0u, path.cells[0].y);

    LONGS_EQUAL(0u, path.cells[1].x);
    LONGS_EQUAL(1u, path.cells[1].y);

    LONGS_EQUAL(1u, path.cells[2].x);
    LONGS_EQUAL(1u, path.cells[2].y);

    mock().expectOneCall("get_current_global_time_sec");

    reset_maze_solver_state();

    mock().expectOneCall("move_forward");
    mock().expectOneCall("rotate_clockwise_90_deg");
    mock().expectOneCall("move_forward")
        .andReturnValue(1u);
    mock().ignoreOtherCalls();

    execute_speed_run_path();

    struct mouse mouse{get_mouse()};

    LONGS_EQUAL(1u, mouse.coordinates.x);
    LONGS_EQUAL(1u, mouse.coordinates.y);
}

TEST(MazeSolverCommonTests, ExecuteSpeedRunPathDoesNothingWhenPathEmpty)
{
    initialize_4_by_4_maze();

    execute_speed_run_path();

    struct mouse mouse{get_mouse()};

    LONGS_EQUAL(0u, mouse.coordinates.x);
    LONGS_EQUAL(0u, mouse.coordinates.y);
    LONGS_EQUAL(DIRECTION_NORTH, mouse.direction);
}
