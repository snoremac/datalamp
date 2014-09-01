#ifndef DATALAMP_CHASER_TRAIL_H
#define DATALAMP_CHASER_TRAIL_H

#include "datalamp/program.h"

struct program_event_descriptor chaser_trail_program_get_descriptor(void);
void chaser_trail_program_init(struct program_config config);
void chaser_trail_on_event(struct program_event* event);


#endif