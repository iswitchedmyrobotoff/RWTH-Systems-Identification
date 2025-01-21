#ifndef ENUMS_H
#define ENUMS_H

typedef enum
{
  IDLE,
  ASK_CAPACITOR,
  ASK_RESISTANCE,
  READY,
  OPERATING,
  SHOW_RESULTS,
  CLEANING_UP
} State;

typedef enum
{
  RED,
  GREEN,
  BLUE,
  BLINKING
} LED;

#endif
