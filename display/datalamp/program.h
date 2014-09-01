#ifndef DATALAMP_PROGRAM_H
#define DATALAMP_PROGRAM_H

#define PROGRAM_EVENT_END_MARKER 255
#define PROGRAM_EVENT_MAX_DATA_SIZE 4

struct program_config {
  
};

struct program_event_descriptor {
  uint8_t header_size;
  uint8_t data_size;
};

struct program_event {
  uint16_t address;
  char data[PROGRAM_EVENT_MAX_DATA_SIZE];
};

typedef struct program_event_descriptor (*program_get_descriptor)(void);
typedef void (*program_init)(struct program_config config);
typedef void (*program_on_event)(struct program_event* event);


#endif