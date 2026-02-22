#ifndef EDK2_TURTLE_H
#define EDK2_TURTLE_H

#include "LSystem.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>

#define TRIG_SCALE 1000

INTN integer_sine(INTN deg);
int sin_distance(UINTN distance, INTN deg);
int cos_distance(UINTN distance, INTN deg);
int isqrt(int n);
INTN abs(INTN val);

typedef struct Turtle Turtle;
struct Turtle {
  INTN forward_speed;
  INTN rotation_speed;

  INTN x;
  INTN y;
  INTN heading;
};

Turtle *turtle_copy(Turtle *t);
Turtle *turtle_create(UINTN screen_width, UINTN screen_height,
                      INTN forward_speed, INTN rotation_speed);
Turtle *turtle_forward(Turtle *t);
void turtle_free(Turtle *t);

typedef struct TurtleStack TurtleStack;
struct TurtleStack {
  TurtleStack *next;
  Turtle *turtle;
};

TurtleStack *turtle_stack_init();
TurtleStack *turtle_stack_create(Turtle *t);
void turtle_stack_push(TurtleStack **stack, Turtle *t);
void turtle_stack_free(TurtleStack *ts);
Turtle *turtle_stack_pop(TurtleStack **stack);

void blend_pixel(UINTN screen_width, UINTN screen_height,
                 EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer, INTN x, INTN y,
                 INTN alpha);
void draw_line(UINTN screen_width, UINTN screen_height,
               EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer, INTN x0, INTN y0, INTN x1,
               INTN y1);

void turtle_run(UINTN screen_width, UINTN screen_height,
                EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer, String *input,
                INTN forward_speed, INTN rotation_speed);

#endif // EDK2_TURTLE_H