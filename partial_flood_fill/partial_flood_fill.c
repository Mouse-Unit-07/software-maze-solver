/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : partial_flood_fill.c                                  */
/*                                                                            */
/* Implementation for partial flood fill maze solving algorithm               */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "maze_solver_common.h"
#include "partial_flood_fill.h"

/*----------------------------------------------------------------------------*/
/*                           Struct, Enum, Typedefs                           */
/*----------------------------------------------------------------------------*/
enum
{
    MAP_DIMENSION_SIZE = MAX_DIMENSION_CELL_COUNT,
    MAP_SIZE = MAX_PATH_LENGTH,
};

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static bool is_cell_frontier(struct coordinates coord);
static void build_distance_map_to_goal(uint16_t distance[][MAP_DIMENSION_SIZE]);
static void build_distance_map_to_frontier(uint16_t distance[][MAP_DIMENSION_SIZE]);
static bool enqueue_neighbor(struct coordinates current, enum direction dir, uint16_t next_distance,
                             uint16_t distance[][MAP_DIMENSION_SIZE], struct coordinates queue[],
                             uint32_t *tail);
static enum movement choose_best_neighbor(uint16_t distance[][MAP_DIMENSION_SIZE]);
static struct coordinates step_coordinate(struct coordinates coord, enum direction dir);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
static const uint16_t FLOOD_UNREACHABLE = 0xFFFFu;

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void run_partial_flood_fill(bool enable_print)
{
    reset_maze_solver_state();

    while (!is_solver_timeout() && !is_mouse_at_goal()) {
        update_current_cell_walls();
        execute_move(determine_partial_flood_fill_move());

        if (enable_print) {
            print_maze_solver_state();
        }
    }

    if (is_solver_timeout()) {
        return;
    }
    set_goal_found(true);

    while (!is_solver_timeout()) {
        uint32_t remaining_time_sec = get_solver_remaining_time_sec();
        uint32_t return_time_sec = estimate_return_to_start_time_sec();
        uint32_t speed_run_time_sec = estimate_best_path_to_goal_time_sec();

        if (remaining_time_sec <= (return_time_sec + speed_run_time_sec)) {
            break;
        }

        update_current_cell_walls();
        execute_move(determine_partial_flood_fill_move());

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
enum movement determine_partial_flood_fill_move(void)
{
    uint16_t distance[MAP_DIMENSION_SIZE][MAP_DIMENSION_SIZE];

    build_distance_map_to_goal(distance);

    struct coordinates current = get_current_coordinates();

    if (distance[current.y][current.x] == FLOOD_UNREACHABLE) {
        build_distance_map_to_frontier(distance);
    }

    return choose_best_neighbor(distance);
}

static bool is_cell_frontier(struct coordinates coord)
{
    struct map_cell cell = get_maze_cell(coord.x, coord.y);

    if ((cell.flags & CELL_NORTH_WALL_KNOWN) == 0u) {
        return true;
    }

    if ((cell.flags & CELL_EAST_WALL_KNOWN) == 0u) {
        return true;
    }

    if ((cell.flags & CELL_SOUTH_WALL_KNOWN) == 0u) {
        return true;
    }

    if ((cell.flags & CELL_WEST_WALL_KNOWN) == 0u) {
        return true;
    }

    return false;
}

static void build_distance_map_to_goal(uint16_t distance[][MAP_DIMENSION_SIZE])
{
    struct coordinates queue[MAP_SIZE];

    uint32_t head = 0u;
    uint32_t tail = 0u;

    uint32_t size = get_maze_size();

    memset(distance, 0xFF, sizeof(uint16_t) * MAP_SIZE);

    for (uint32_t y = 0u; y < size; y++) {
        for (uint32_t x = 0u; x < size; x++) {

            if (!is_goal_cell((struct coordinates){x, y})) {
                continue;
            }

            distance[y][x] = 0u;
            queue[tail++] = (struct coordinates){x, y};
        }
    }

    while (head < tail) {
        struct coordinates current = queue[head++];
        uint16_t next_distance = distance[current.y][current.x] + 1u;

        enqueue_neighbor(current, DIRECTION_NORTH, next_distance, distance, queue, &tail);
        enqueue_neighbor(current, DIRECTION_EAST, next_distance, distance, queue, &tail);
        enqueue_neighbor(current, DIRECTION_SOUTH, next_distance, distance, queue, &tail);
        enqueue_neighbor(current, DIRECTION_WEST, next_distance, distance, queue, &tail);
    }
}

static void build_distance_map_to_frontier(uint16_t distance[][MAP_DIMENSION_SIZE])
{
    struct coordinates queue[MAP_SIZE];

    uint32_t head = 0u;
    uint32_t tail = 0u;

    uint32_t size = get_maze_size();

    memset(distance, 0xFF, sizeof(uint16_t) * MAP_SIZE);

    for (uint32_t y = 0u; y < size; y++) {
        for (uint32_t x = 0u; x < size; x++) {

            struct coordinates coord = {x, y};

            if (!is_cell_frontier(coord)) {
                continue;
            }

            distance[y][x] = 0u;
            queue[tail++] = coord;
        }
    }

    while (head < tail) {
        struct coordinates current = queue[head++];
        uint16_t next_distance = distance[current.y][current.x] + 1u;

        enqueue_neighbor(current, DIRECTION_NORTH, next_distance, distance, queue, &tail);
        enqueue_neighbor(current, DIRECTION_EAST, next_distance, distance, queue, &tail);
        enqueue_neighbor(current, DIRECTION_SOUTH, next_distance, distance, queue, &tail);
        enqueue_neighbor(current, DIRECTION_WEST, next_distance, distance, queue, &tail);
    }
}

static bool enqueue_neighbor(struct coordinates current, enum direction dir, uint16_t next_distance,
                             uint16_t distance[][MAP_DIMENSION_SIZE], struct coordinates queue[],
                             uint32_t *tail)
{
    struct coordinates next;

    uint32_t size = get_maze_size();

    if (!is_wall_known_at_coordinate(current, dir)) {
        return false;
    }

    if (is_wall_present_at_coordinate(current, dir)) {
        return false;
    }

    next = step_coordinate(current, dir);

    if ((next.x >= size) || (next.y >= size)) {
        return false;
    }

    if (distance[next.y][next.x] != FLOOD_UNREACHABLE) {
        return false;
    }

    distance[next.y][next.x] = next_distance;
    queue[(*tail)++] = next;

    return true;
}

static enum movement choose_best_neighbor(uint16_t distance[][MAP_DIMENSION_SIZE])
{
    struct coordinates current = get_current_coordinates();
    enum direction current_dir = get_current_direction();

    uint16_t best_distance = FLOOD_UNREACHABLE;
    enum direction best_direction = current_dir;

    for (uint32_t dir = 0u; dir < 4u; dir++) {
        enum direction candidate = (enum direction)dir;

        struct coordinates next;

        if (!is_wall_known_at_coordinate(current, candidate)) {
            continue;
        }

        if (is_wall_present_at_coordinate(current, candidate)) {
            continue;
        }

        next = step_coordinate(current, candidate);

        uint32_t size = get_maze_size();
        if ((next.x >= size) || (next.y >= size)) {
            continue;
        }

        if (distance[next.y][next.x] < best_distance) {
            best_distance = distance[next.y][next.x];
            best_direction = candidate;
        }
    }

    return get_turn_required(current_dir, best_direction);
}

static struct coordinates step_coordinate(struct coordinates coord, enum direction dir)
{
    switch (dir) {
        case DIRECTION_NORTH:
            coord.y++;
            break;

        case DIRECTION_EAST:
            coord.x++;
            break;

        case DIRECTION_SOUTH:
            coord.y--;
            break;

        case DIRECTION_WEST:
            coord.x--;
            break;

        default:
            break;
    }

    return coord;
}
