#ifndef _TURTLE_H
#define _TURTLE_H

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>

typedef struct TURTLE {
  INTN x;
  INTN y;
  INTN heading;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL color;
  struct TURTLE *previous;
} TURTLE;

TURTLE *TurtleCreate(INTN x, INTN y, INTN heading,
                     EFI_GRAPHICS_OUTPUT_BLT_PIXEL color);

VOID TurtleForward(TURTLE *Turtle, INTN distance,
                   EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop);
VOID TurtleForwardNoLine(TURTLE *Turtle, INTN distance);
VOID TurtleRotate(TURTLE *Turtle, INTN delta);

VOID TurtleSave(TURTLE *Turtle);
VOID TurtleLoad(TURTLE *Turtle);

VOID TurtleFree(TURTLE *Turtle);

#endif
