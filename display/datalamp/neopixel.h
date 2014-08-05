#ifndef DATALAMP_NEOPIXEL_H
#define DATALAMP_NEOPIXEL_H

extern void neopixel_write(volatile uint8_t* port, uint8_t pin, uint8_t* data, uint8_t size);

#endif