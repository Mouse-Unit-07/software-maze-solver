/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : maze_solver_common.c                                  */
/*                                                                            */
/* Common helper implementations for each maze solver                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "navigation.h"
#include "global_time.h"
#include "maze_solver_common.h"

/*----------------------------------------------------------------------------*/
/*                         Private Function Prototypes                        */
/*----------------------------------------------------------------------------*/
static enum direction get_left_direction(enum direction dir);
static enum direction get_right_direction(enum direction dir);
static enum direction get_opposite_direction(enum direction dir);
static enum movement get_turn_required(enum direction from, enum direction to);

static void update_wall_flags(struct map_cell *cell, uint8_t known_flag, uint8_t present_flag,
                              bool present);
static void set_wall(struct coordinates coord, enum direction dir, bool present);
static void move_mouse_to_next_cell(void);
static bool is_wall_known_in_map(enum direction dir);
static bool is_wall_present_in_map(enum direction dir);
static bool can_move(struct coordinates coord, enum direction dir);

static uint32_t estimate_path_time_sec(const struct maze_solver_path *path,
                                       enum direction start_dir);
static bool find_shortest_path_to_goal(struct coordinates start, struct maze_solver_path *path);
static bool find_shortest_path(struct coordinates start, struct coordinates target,
                               struct maze_solver_path *path);
static void follow_path(const struct maze_solver_path *path);

static const char direction_to_char(enum direction dir);

/*----------------------------------------------------------------------------*/
/*                               Private Globals                              */
/*----------------------------------------------------------------------------*/
static struct maze_solver_config maze_solver_cfg = {0};
static struct mouse mouse;
static struct map_cell maze[MAX_DIMENSION_CELL_COUNT][MAX_DIMENSION_CELL_COUNT] = {{{0}}};
static struct maze_solver_path fastest_path;
static uint32_t solver_start_time_sec = 0u;
static bool goal_found = false;

/*----------------------------------------------------------------------------*/
/*                         Public Function Definitions                        */
/*----------------------------------------------------------------------------*/
void init_maze_solver_common(void)
{
    memset(&maze_solver_cfg, 0, sizeof(maze_solver_cfg));
    memset(&mouse, 0, sizeof(mouse));
    memset(maze, 0, sizeof(maze));
    memset(&fastest_path, 0, sizeof(fastest_path));
    solver_start_time_sec = get_current_global_time_sec();
    goal_found = false;
}

void deinit_maze_solver_common(void)
{
    memset(&maze_solver_cfg, 0, sizeof(maze_solver_cfg));
    memset(&mouse, 0, sizeof(mouse));
    memset(maze, 0, sizeof(maze));
    memset(&fastest_path, 0, sizeof(fastest_path));
    solver_start_time_sec = get_current_global_time_sec();
    goal_found = false;
}

void set_maze_solver_config(struct maze_solver_config cfg)
{
    maze_solver_cfg = cfg;
}

struct maze_solver_config get_maze_solver_config(void)
{
    return maze_solver_cfg;
}

void reset_maze_solver_state(void)
{
    memset(&mouse, 0, sizeof(mouse));
    memset(maze, 0, sizeof(maze));

    solver_start_time_sec = get_current_global_time_sec();
    goal_found = false;

    mouse.coordinates.x = 0;
    mouse.coordinates.y = 0;
    mouse.direction = DIRECTION_NORTH;

    uint32_t size = maze_solver_cfg.maze_size;

    for (uint32_t x = 0u; x < size; x++) {
        set_wall((struct coordinates){x, 0u}, DIRECTION_SOUTH, true);
        set_wall((struct coordinates){x, size - 1u}, DIRECTION_NORTH, true);
    }

    for (uint32_t y = 0u; y < size; y++) {
        set_wall((struct coordinates){0u, y}, DIRECTION_WEST, true);
        set_wall((struct coordinates){size - 1u, y}, DIRECTION_EAST, true);
    }
}

void set_goal_found(bool found)
{
    goal_found = found;
}

bool is_mouse_at_goal(void)
{
    uint32_t size = maze_solver_cfg.maze_size;
    uint32_t center_low = (size - 1u) / 2u;
    uint32_t center_high = size / 2u;

    if ((size % 2u) != 0u) {
        return (mouse.coordinates.x == center_low) && (mouse.coordinates.y == center_low);
    }

    return ((mouse.coordinates.x == center_low) || (mouse.coordinates.x == center_high))
           && ((mouse.coordinates.y == center_low) || (mouse.coordinates.y == center_high));
}

void update_current_cell_walls(void)
{
    switch (mouse.direction) {
        case DIRECTION_NORTH:
            set_wall(mouse.coordinates, DIRECTION_WEST, is_left_wall_present());
            set_wall(mouse.coordinates, DIRECTION_NORTH, is_front_wall_present());
            set_wall(mouse.coordinates, DIRECTION_EAST, is_right_wall_present());
            break;
        case DIRECTION_EAST:
            set_wall(mouse.coordinates, DIRECTION_NORTH, is_left_wall_present());
            set_wall(mouse.coordinates, DIRECTION_EAST, is_front_wall_present());
            set_wall(mouse.coordinates, DIRECTION_SOUTH, is_right_wall_present());
            break;
        case DIRECTION_SOUTH:
            set_wall(mouse.coordinates, DIRECTION_EAST, is_left_wall_present());
            set_wall(mouse.coordinates, DIRECTION_SOUTH, is_front_wall_present());
            set_wall(mouse.coordinates, DIRECTION_WEST, is_right_wall_present());
            break;
        case DIRECTION_WEST:
            set_wall(mouse.coordinates, DIRECTION_SOUTH, is_left_wall_present());
            set_wall(mouse.coordinates, DIRECTION_WEST, is_front_wall_present());
            set_wall(mouse.coordinates, DIRECTION_NORTH, is_right_wall_present());
            break;
        default:
            break;
    }
}

bool is_front_wall_known_in_map(void)
{
    return is_wall_known_in_map(mouse.direction);
}

bool is_front_wall_present_in_map(void)
{
    return is_wall_present_in_map(mouse.direction);
}

bool is_left_wall_known_in_map(void)
{
    return is_wall_known_in_map(get_left_direction(mouse.direction));
}

bool is_left_wall_present_in_map(void)
{
    return is_wall_present_in_map(get_left_direction(mouse.direction));
}

bool is_right_wall_known_in_map(void)
{
    return is_wall_known_in_map(get_right_direction(mouse.direction));
}

bool is_right_wall_present_in_map(void)
{
    return is_wall_present_in_map(get_right_direction(mouse.direction));
}

bool is_solver_timeout(void)
{
    uint32_t elapsed_time_sec = get_elapsed_global_time_sec(solver_start_time_sec);

    return elapsed_time_sec >= maze_solver_cfg.total_timeout_sec;
}

uint32_t get_solver_remaining_time_sec(void)
{
    uint32_t elapsed_time_sec = get_elapsed_global_time_sec(solver_start_time_sec);

    if (elapsed_time_sec >= maze_solver_cfg.total_timeout_sec) {
        return 0u;
    }

    return maze_solver_cfg.total_timeout_sec - elapsed_time_sec;
}

uint32_t estimate_return_to_start_time_sec(void)
{
    struct maze_solver_path path;

    if (!find_shortest_path(mouse.coordinates, (struct coordinates){0u, 0u}, &path)) {
        return UINT32_MAX;
    }

    return estimate_path_time_sec(&path, mouse.direction);
}

uint32_t estimate_best_path_to_goal_time_sec(void)
{
    struct maze_solver_path path;

    if (!find_shortest_path_to_goal((struct coordinates){0u, 0u}, &path)) {
        return UINT32_MAX;
    }

    return estimate_path_time_sec(&path, DIRECTION_NORTH);
}

void execute_move(enum movement move)
{
    switch (move) {
        case MOVE_FORWARD:
            move_forward();
            move_mouse_to_next_cell();
            break;

        case MOVE_LEFT:
            rotate_counter_clockwise_90_deg();
            mouse.direction = get_left_direction(mouse.direction);
            move_forward();
            move_mouse_to_next_cell();
            break;

        case MOVE_RIGHT:
            rotate_clockwise_90_deg();
            mouse.direction = get_right_direction(mouse.direction);
            move_forward();
            move_mouse_to_next_cell();
            break;

        case MOVE_TURN_AROUND:
            rotate_180_deg();
            mouse.direction = get_opposite_direction(mouse.direction);
            move_forward();
            move_mouse_to_next_cell();
            break;
        default:
            break;
    }
}

void return_to_start(void)
{
    struct maze_solver_path path;

    if (!find_shortest_path(mouse.coordinates, (struct coordinates){0u, 0u}, &path)) {
        return;
    }

    follow_path(&path);
}

void calculate_fastest_path(void)
{
    fastest_path.length = 0u;

    find_shortest_path_to_goal((struct coordinates){0u, 0u}, &fastest_path);
}

void execute_speed_run_path(void)
{
    follow_path(&fastest_path);
}

void print_maze_solver_state(void)
{
    uint32_t size = maze_solver_cfg.maze_size;

    printf("\n");
    printf("========================================\n");
    printf("Mouse: (%u,%u) Dir:%c\n", mouse.coordinates.x, mouse.coordinates.y,
           direction_to_char(mouse.direction));
    printf("Goal Found: %s\n", goal_found ? "YES" : "NO");
    printf("Remaining Time: %lu sec\n", (unsigned long)get_solver_remaining_time_sec());
    printf("========================================\n");

    for (int32_t y = (int32_t)size - 1; y >= 0; y--) {

        /* horizontal walls */
        for (uint32_t x = 0u; x < size; x++) {
            printf("+");
            struct map_cell *cell = &maze[y][x];
            if ((cell->flags & CELL_NORTH_WALL_KNOWN) != 0u) {
                if ((cell->flags & CELL_NORTH_WALL_PRESENT) != 0u) {
                    printf("---");
                } else {
                    printf("   ");
                }
            } else {
                printf("???");
            }
        }
        printf("+\n");

        /* vertical walls and cell contents */
        for (uint32_t x = 0u; x < size; x++) {
            struct map_cell *cell = &maze[y][x];
            if ((cell->flags & CELL_WEST_WALL_KNOWN) != 0u) {
                if ((cell->flags & CELL_WEST_WALL_PRESENT) != 0u) {
                    printf("|");
                } else {
                    printf(" ");
                }
            } else {
                printf("?");
            }

            if ((mouse.coordinates.x == x) && (mouse.coordinates.y == y)) {
                printf(" %c ", direction_to_char(mouse.direction));
            } else {
                printf("   ");
            }
        }
        printf("|\n");
    }

    for (uint32_t x = 0u; x < size; x++) {
        printf("+---");
    }
    printf("+\n");
}

/*----------------------------------------------------------------------------*/
/*                        Private Function Definitions                        */
/*----------------------------------------------------------------------------*/
static enum direction get_left_direction(enum direction dir)
{
    switch (dir) {
        case DIRECTION_NORTH:
            return DIRECTION_WEST;

        case DIRECTION_EAST:
            return DIRECTION_NORTH;

        case DIRECTION_SOUTH:
            return DIRECTION_EAST;

        case DIRECTION_WEST:
            return DIRECTION_SOUTH;

        default:
            return DIRECTION_NORTH;
    }
}

static enum direction get_right_direction(enum direction dir)
{
    switch (dir) {
        case DIRECTION_NORTH:
            return DIRECTION_EAST;

        case DIRECTION_EAST:
            return DIRECTION_SOUTH;

        case DIRECTION_SOUTH:
            return DIRECTION_WEST;

        case DIRECTION_WEST:
            return DIRECTION_NORTH;

        default:
            return DIRECTION_NORTH;
    }
}

static enum direction get_opposite_direction(enum direction dir)
{
    switch (dir) {
        case DIRECTION_NORTH:
            return DIRECTION_SOUTH;

        case DIRECTION_EAST:
            return DIRECTION_WEST;

        case DIRECTION_SOUTH:
            return DIRECTION_NORTH;

        case DIRECTION_WEST:
            return DIRECTION_EAST;

        default:
            return DIRECTION_NORTH;
    }
}

static enum movement get_turn_required(enum direction from, enum direction to)
{
    if (from == to) {
        return MOVE_FORWARD;
    }

    if (get_left_direction(from) == to) {
        return MOVE_LEFT;
    }

    if (get_right_direction(from) == to) {
        return MOVE_RIGHT;
    }

    if (get_opposite_direction(from) == to) {
        return MOVE_TURN_AROUND;
    }

    return MOVE_FORWARD;
}

static void update_wall_flags(struct map_cell *cell, uint8_t known_flag, uint8_t present_flag,
                              bool present)
{
    cell->flags |= known_flag;

    if (present) {
        cell->flags |= present_flag;
    } else {
        cell->flags &= (uint8_t)(~present_flag);
    }
}

static void set_wall(struct coordinates coord, enum direction dir, bool present)
{
    struct map_cell *cell = &maze[coord.y][coord.x];

    switch (dir) {
        case DIRECTION_NORTH:
            update_wall_flags(cell, CELL_NORTH_WALL_KNOWN, CELL_NORTH_WALL_PRESENT, present);

            if ((coord.y + 1u) < maze_solver_cfg.maze_size) {
                update_wall_flags(&maze[coord.y + 1u][coord.x], CELL_SOUTH_WALL_KNOWN,
                                  CELL_SOUTH_WALL_PRESENT, present);
            }
            break;

        case DIRECTION_EAST:
            update_wall_flags(cell, CELL_EAST_WALL_KNOWN, CELL_EAST_WALL_PRESENT, present);

            if ((coord.x + 1u) < maze_solver_cfg.maze_size) {
                update_wall_flags(&maze[coord.y][coord.x + 1u], CELL_WEST_WALL_KNOWN,
                                  CELL_WEST_WALL_PRESENT, present);
            }
            break;

        case DIRECTION_SOUTH:
            update_wall_flags(cell, CELL_SOUTH_WALL_KNOWN, CELL_SOUTH_WALL_PRESENT, present);

            if (coord.y > 0u) {
                update_wall_flags(&maze[coord.y - 1u][coord.x], CELL_NORTH_WALL_KNOWN,
                                  CELL_NORTH_WALL_PRESENT, present);
            }
            break;

        case DIRECTION_WEST:
            update_wall_flags(cell, CELL_WEST_WALL_KNOWN, CELL_WEST_WALL_PRESENT, present);

            if (coord.x > 0u) {
                update_wall_flags(&maze[coord.y][coord.x - 1u], CELL_EAST_WALL_KNOWN,
                                  CELL_EAST_WALL_PRESENT, present);
            }
            break;

        default:
            break;
    }
}

static void move_mouse_to_next_cell(void)
{
    switch (mouse.direction) {
        case DIRECTION_NORTH:
            mouse.coordinates.y++;
            break;

        case DIRECTION_EAST:
            mouse.coordinates.x++;
            break;

        case DIRECTION_SOUTH:
            mouse.coordinates.y--;
            break;

        case DIRECTION_WEST:
            mouse.coordinates.x--;
            break;

        default:
            break;
    }
}

static bool is_wall_known_in_map(enum direction dir)
{
    uint8_t known_flag = 0u;
    struct map_cell *cell = &maze[mouse.coordinates.y][mouse.coordinates.x];

    switch (dir) {
        case DIRECTION_NORTH:
            known_flag = CELL_NORTH_WALL_KNOWN;
            break;

        case DIRECTION_EAST:
            known_flag = CELL_EAST_WALL_KNOWN;
            break;

        case DIRECTION_SOUTH:
            known_flag = CELL_SOUTH_WALL_KNOWN;
            break;

        case DIRECTION_WEST:
            known_flag = CELL_WEST_WALL_KNOWN;
            break;

        default:
            return false;
    }

    return (cell->flags & known_flag) != 0u;
}

static bool is_wall_present_in_map(enum direction dir)
{
    uint8_t present_flag = 0u;
    struct map_cell *cell = &maze[mouse.coordinates.y][mouse.coordinates.x];

    switch (dir) {
        case DIRECTION_NORTH:
            present_flag = CELL_NORTH_WALL_PRESENT;
            break;

        case DIRECTION_EAST:
            present_flag = CELL_EAST_WALL_PRESENT;
            break;

        case DIRECTION_SOUTH:
            present_flag = CELL_SOUTH_WALL_PRESENT;
            break;

        case DIRECTION_WEST:
            present_flag = CELL_WEST_WALL_PRESENT;
            break;

        default:
            return false;
    }

    return (cell->flags & present_flag) != 0u;
}

static bool can_move(struct coordinates coord, enum direction dir)
{
    struct map_cell *cell = &maze[coord.y][coord.x];

    switch (dir) {
        case DIRECTION_NORTH:
            return ((cell->flags & CELL_NORTH_WALL_PRESENT) == 0u)
                   && ((cell->flags & CELL_NORTH_WALL_KNOWN) != 0u);

        case DIRECTION_EAST:
            return ((cell->flags & CELL_EAST_WALL_PRESENT) == 0u)
                   && ((cell->flags & CELL_EAST_WALL_KNOWN) != 0u);

        case DIRECTION_SOUTH:
            return ((cell->flags & CELL_SOUTH_WALL_PRESENT) == 0u)
                   && ((cell->flags & CELL_SOUTH_WALL_KNOWN) != 0u);

        case DIRECTION_WEST:
            return ((cell->flags & CELL_WEST_WALL_PRESENT) == 0u)
                   && ((cell->flags & CELL_WEST_WALL_KNOWN) != 0u);

        default:
            return false;
    }
}

static uint32_t estimate_path_time_sec(const struct maze_solver_path *path,
                                       enum direction start_dir)
{
    uint32_t time_sec = 0u;
    enum direction current_dir = start_dir;

    if (path->length < 2u) {
        return 0u;
    }

    for (uint32_t i = 1u; i < path->length; i++) {
        struct coordinates curr = path->cells[i - 1u];
        struct coordinates next = path->cells[i];

        enum direction target_dir;

        if (next.y > curr.y) {
            target_dir = DIRECTION_NORTH;
        } else if (next.x > curr.x) {
            target_dir = DIRECTION_EAST;
        } else if (next.y < curr.y) {
            target_dir = DIRECTION_SOUTH;
        } else {
            target_dir = DIRECTION_WEST;
        }

        switch (get_turn_required(current_dir, target_dir)) {
            case MOVE_FORWARD:
                break;

            case MOVE_LEFT:
            case MOVE_RIGHT:
                time_sec += maze_solver_cfg.rotate_90_deg_time_sec;
                break;

            case MOVE_TURN_AROUND:
                time_sec += maze_solver_cfg.rotate_180_deg_time_sec;
                break;

            default:
                break;
        }
        time_sec += maze_solver_cfg.move_forward_time_sec;

        current_dir = target_dir;
    }

    return time_sec;
}

static void reverse_path(struct maze_solver_path *path)
{
    for (uint32_t i = 0u; i < (path->length / 2u); i++) {
        struct coordinates temp = path->cells[i];
        path->cells[i] = path->cells[path->length - 1u - i];
        path->cells[path->length - 1u - i] = temp;
    }
}

static bool find_shortest_path(struct coordinates start, struct coordinates target,
                               struct maze_solver_path *path)
{
    struct coordinates queue[MAX_PATH_LENGTH] = {{0}};
    struct coordinates parent[MAX_DIMENSION_CELL_COUNT][MAX_DIMENSION_CELL_COUNT] = {{{0}}};

    bool visited[MAX_DIMENSION_CELL_COUNT][MAX_DIMENSION_CELL_COUNT] = {{0}};

    uint32_t head = 0u;
    uint32_t tail = 0u;
    uint32_t size = maze_solver_cfg.maze_size;

    queue[tail++] = start;
    visited[start.y][start.x] = true;

    while (head < tail) {
        struct coordinates curr = queue[head++];

        if ((curr.x == target.x) && (curr.y == target.y)) {
            uint32_t length = 0u;

            while (!(curr.x == start.x && curr.y == start.y)) {
                path->cells[length].x = curr.x;
                path->cells[length].y = curr.y;

                curr = parent[curr.y][curr.x];
                length++;
            }

            path->cells[length].x = start.x;
            path->cells[length].y = start.y;
            length++;

            path->length = (uint16_t)length;
            reverse_path(path);

            return true;
        }

        static const int8_t dx[4] = {0, 1, 0, -1};
        static const int8_t dy[4] = {1, 0, -1, 0};

        for (uint32_t dir = 0u; dir < 4u; dir++) {
            if (!can_move(curr, (enum direction)dir)) {
                continue;
            }

            uint32_t nx = (uint32_t)((int32_t)curr.x + dx[dir]);
            uint32_t ny = (uint32_t)((int32_t)curr.y + dy[dir]);

            if ((nx >= size) || (ny >= size)) {
                continue;
            }

            if (visited[ny][nx]) {
                continue;
            }

            visited[ny][nx] = true;
            parent[ny][nx] = curr;
            queue[tail++] = (struct coordinates){nx, ny};
        }
    }

    return false;
}

static bool find_shortest_path_to_goal(struct coordinates start, struct maze_solver_path *path)
{
    uint8_t size = maze_solver_cfg.maze_size;
    uint8_t center_low = (size - 1u) / 2u;
    uint8_t center_high = size / 2u;

    struct maze_solver_path candidate;
    bool found = false;

    path->length = 0u;

    if ((size % 2u) != 0u) {
        return find_shortest_path(start, (struct coordinates){center_low, center_low}, path);
    }

    struct coordinates goals[4] = {{center_low, center_low},
                                   {center_low, center_high},
                                   {center_high, center_low},
                                   {center_high, center_high}};

    for (uint32_t i = 0u; i < 4u; i++) {
        if (!find_shortest_path(start, (struct coordinates){goals[i].x, goals[i].y}, &candidate)) {
            continue;
        }

        if ((!found) || (candidate.length < path->length)) {
            *path = candidate;
            found = true;
        }
    }

    return found;
}

static void follow_path(const struct maze_solver_path *path)
{
    for (uint32_t i = 1u; i < path->length; i++) {
        uint32_t curr_x = path->cells[i - 1u].x;
        uint32_t curr_y = path->cells[i - 1u].y;

        uint32_t next_x = path->cells[i].x;
        uint32_t next_y = path->cells[i].y;

        enum direction target_dir;

        if (next_y > curr_y) {
            target_dir = DIRECTION_NORTH;
        } else if (next_x > curr_x) {
            target_dir = DIRECTION_EAST;
        } else if (next_y < curr_y) {
            target_dir = DIRECTION_SOUTH;
        } else {
            target_dir = DIRECTION_WEST;
        }

        execute_move(get_turn_required(mouse.direction, target_dir));
    }
}

static const char direction_to_char(enum direction dir)
{
    switch (dir) {
        case DIRECTION_NORTH:
            return 'N';

        case DIRECTION_EAST:
            return 'E';

        case DIRECTION_SOUTH:
            return 'S';

        case DIRECTION_WEST:
            return 'W';

        default:
            return '?';
    }
}

/*----------------------------------------------------------------------------*/
/* accessors to private globals exposed for testing */
struct mouse get_mouse(void)
{
    return mouse;
}

struct map_cell get_maze_cell(uint8_t x, uint8_t y)
{
    return maze[y][x];
}

uint32_t get_solver_start_time_sec(void)
{
    return solver_start_time_sec;
}

struct maze_solver_path get_fastest_path(void)
{
    return fastest_path;
}

bool get_goal_found(void)
{
    return goal_found;
}
