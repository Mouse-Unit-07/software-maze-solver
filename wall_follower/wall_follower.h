/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : wall_follower.h                                       */
/*                                                                            */
/* Interface for wall follower maze solving algorithm                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef WALL_FOLLOWER_H_
#define WALL_FOLLOWER_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
enum wall_follower_mode
{
    WALL_FOLLOWER_LEFT,
    WALL_FOLLOWER_RIGHT
};

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void run_wall_follower(enum wall_follower_mode mode, bool enable_print);

/* helpers exposed for testing */
enum movement determine_wall_follower_move(enum wall_follower_mode mode);

#endif /* WALL_FOLLOWER_H_ */
