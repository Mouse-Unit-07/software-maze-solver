/*-------------------------------- FILE INFO ---------------------------------*/
/* Filename           : partial_flood_fill.h                                  */
/*                                                                            */
/* Interface for partial flood fill maze solving algorithm                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef PARTIAL_FLOOD_FILL_H_
#define PARTIAL_FLOOD_FILL_H_

/*----------------------------------------------------------------------------*/
/*                             Public Definitions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                         Public Function Prototypes                         */
/*----------------------------------------------------------------------------*/
void run_partial_flood_fill(bool enable_print);

/* helpers exposed for testing */
enum movement determine_partial_flood_fill_move(void);

#endif /* PARTIAL_FLOOD_FILL_H_ */
