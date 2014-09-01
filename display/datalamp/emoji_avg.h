#ifndef DATALAMP_EMOJI_AVG_H
#define DATALAMP_EMOJI_AVG_H

#include "datalamp/program.h"

struct program_event_descriptor emoji_avg_program_get_descriptor(void);
void emoji_avg_program_init(struct program_config config);
void emoji_avg_on_event(struct program_event* event);


#endif