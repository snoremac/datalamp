#ifndef DATALAMP_PROGRAM_H
#define DATALAMP_PROGRAM_H

#define PROGRAM_EVENT_HEADER_SIZE 3
#define PROGRAM_EVENT_DATA_SIZE 1
#define PROGRAM_EVENT_SIZE PROGRAM_EVENT_HEADER_SIZE + PROGRAM_EVENT_DATA_SIZE
#define PROGRAM_EVENT_END_MARKER 255

struct program_config {
  
};

struct program_event {
  uint16_t address;
  char data[PROGRAM_EVENT_DATA_SIZE];
};

typedef void (*program_init)(struct program_config config);
typedef void (*program_on_event)(struct program_event* event);


#endif