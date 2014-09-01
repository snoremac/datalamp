#ifndef DATALAMP_CHASER_BASIC_H
#define DATALAMP_CHASER_BASIC_H

#include "datalamp/program.h"

struct program_event_descriptor chaser_basic_program_get_descriptor(void);
void chaser_basic_program_init(struct program_config config);
void chaser_basic_on_event(struct program_event* event);


#endif