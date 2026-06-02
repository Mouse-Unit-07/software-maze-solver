/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : maze_solver_common.h                                  */
/*                                                                            */
/* Common helper interface for each maze solver                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef MAZE_SOLVER_COMMON_H_
#define MAZE_SOLVER_COMMON_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
struct maze_solver_config {
    uint32_t maze_size;
    uint32_t total_timeout_sec;
    uint32_t move_forward_time_sec;
    uint32_t rotate_90_deg_time_sec;
    uint32_t rotate_180_deg_time_sec;
};

enum movement
{
    MOVE_FORWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_TURN_AROUND
};

enum direction
{
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_WEST
};

enum
{
    MAX_DIMENSION_CELL_COUNT = 16,
    MAX_PATH_LENGTH = MAX_DIMENSION_CELL_COUNT * MAX_DIMENSION_CELL_COUNT
};

/*----------------------------------------------------------------------------*/
/* helper structs exposed for testing */
enum map_cell_flags
{
    CELL_NORTH_WALL_KNOWN = (1u << 0),
    CELL_EAST_WALL_KNOWN = (1u << 1),
    CELL_SOUTH_WALL_KNOWN = (1u << 2),
    CELL_WEST_WALL_KNOWN = (1u << 3),

    CELL_NORTH_WALL_PRESENT = (1u << 4),
    CELL_EAST_WALL_PRESENT = (1u << 5),
    CELL_SOUTH_WALL_PRESENT = (1u << 6),
    CELL_WEST_WALL_PRESENT = (1u << 7)
};

struct map_cell {
    uint8_t flags;
};

struct coordinates {
    uint8_t x;
    uint8_t y;
};

struct mouse {
    struct coordinates coordinates;
    enum direction direction;
};

struct maze_solver_path {
    uint16_t length;
    struct coordinates cells[MAX_PATH_LENGTH];
};

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void init_maze_solver_common(void);
void deinit_maze_solver_common(void);

void set_maze_solver_config(struct maze_solver_config cfg);
struct maze_solver_config get_maze_solver_config(void);

void reset_maze_solver_state(void);
uint32_t get_maze_size(void);
void set_goal_found(bool found);
bool is_goal_cell(struct coordinates coord);
bool is_mouse_at_goal(void);
void update_current_cell_walls(void);
bool is_cell_frontier(struct coordinates coord);
struct coordinates get_current_coordinates(void);
enum direction get_current_direction(void);

enum direction get_left_direction(enum direction dir);
enum direction get_right_direction(enum direction dir);
enum direction get_opposite_direction(enum direction dir);
enum movement get_turn_required(enum direction from, enum direction to);

bool is_wall_known_at_coordinate(struct coordinates coord, enum direction dir);
bool is_wall_present_at_coordinate(struct coordinates coord, enum direction dir);
bool is_front_wall_known_in_map(void);
bool is_front_wall_present_in_map(void);
bool is_left_wall_known_in_map(void);
bool is_left_wall_present_in_map(void);
bool is_right_wall_known_in_map(void);
bool is_right_wall_present_in_map(void);

bool is_solver_timeout(void);
uint32_t get_solver_remaining_time_sec(void);
uint32_t estimate_return_to_start_time_sec(void);
uint32_t estimate_best_path_to_goal_time_sec(void);

void execute_move(enum movement move);
void return_to_start(void);
void calculate_fastest_path(void);
void execute_speed_run_path(void);

void print_maze_solver_state(void);

/*----------------------------------------------------------------------------*/
/* accessors to private globals exposed for testing */
struct mouse get_mouse(void);
struct map_cell get_maze_cell(uint8_t x, uint8_t y);
uint32_t get_solver_start_time_sec(void);
struct maze_solver_path get_fastest_path(void);
bool get_goal_found(void);

#endif /* MAZE_SOLVER_COMMON_H_ */
