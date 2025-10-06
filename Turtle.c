#include "Turtle.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>

#define SCALE 10000

STATIC CONST INTN SinTable[91] = {
    0,    174,  349,  523,  698,  872,  1045, 1219, 1392, 1564, 1736, 1908,
    2079, 2250, 2419, 2588, 2756, 2924, 3090, 3256, 3420, 3584, 3746, 3907,
    4067, 4226, 4384, 4540, 4695, 4848, 5000, 5150, 5299, 5446, 5592, 5736,
    5878, 6018, 6157, 6293, 6428, 6561, 6691, 6820, 6947, 7071, 7193, 7314,
    7431, 7547, 7660, 7771, 7880, 7986, 8090, 8191, 8290, 8386, 8480, 8571,
    8660, 8746, 8829, 8910, 8988, 9063, 9135, 9205, 9272, 9336, 9397, 9455,
    9511, 9563, 9613, 9659, 9703, 9744, 9781, 9816, 9848, 9877, 9903, 9925,
    9945, 9961, 9975, 9986, 9994, 9998, 10000};

STATIC INTN FixedSin(INTN Degrees) {
  Degrees %= 360;
  if (Degrees < 0)
    Degrees += 360;

  if (Degrees <= 90)
    return SinTable[Degrees];
  else if (Degrees <= 180)
    return SinTable[180 - Degrees];
  else if (Degrees <= 270)
    return -SinTable[Degrees - 180];
  else
    return -SinTable[360 - Degrees];
}

STATIC INTN FixedCos(INTN Degrees) { return FixedSin(Degrees + 90); }

STATIC VOID SetPixel(EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop, UINTN X, UINTN Y,
                     EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Color) {
  Gop->Blt(Gop, Color, EfiBltVideoFill, 0, 0, X, Y, 1, 1, 0);
}

STATIC VOID DrawLineBresenham(EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop, INTN X1,
                              INTN Y1, INTN X2, INTN Y2,
                              EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color) {
  UINTN ScreenWidth = Gop->Mode->Info->HorizontalResolution;
  UINTN ScreenHeight = Gop->Mode->Info->VerticalResolution;

  INTN Dx = (X2 > X1) ? (X2 - X1) : (X1 - X2);
  INTN Dy = (Y2 > Y1) ? (Y2 - Y1) : (Y1 - Y2);
  INTN Sx = (X2 > X1) ? 1 : -1;
  INTN Sy = (Y2 > Y1) ? 1 : -1;
  INTN Err = Dx - Dy;
  INTN E2;

  while (1) {
    if (X1 >= 0 && (UINTN)X1 < ScreenWidth && Y1 >= 0 &&
        (UINTN)Y1 < ScreenHeight) {
      SetPixel(Gop, (UINTN)X1, (UINTN)Y1, &Color);
    }

    // Stop when the end of the line is reached
    if ((X1 == X2) && (Y1 == Y2)) {
      break;
    }

    E2 = 2 * Err;
    if (E2 > -Dy) {
      Err -= Dy;
      X1 += Sx;
    }
    if (E2 < Dx) {
      Err += Dx;
      Y1 += Sy;
    }
  }
}

TURTLE *TurtleCreate(INTN x, INTN y, INTN heading,
                     EFI_GRAPHICS_OUTPUT_BLT_PIXEL color) {
  TURTLE *Turtle = AllocateZeroPool(sizeof(TURTLE));

  Turtle->x = x;
  Turtle->y = y;
  Turtle->heading = heading % 360;
  if (Turtle->heading < 0)
    Turtle->heading += 360;
  Turtle->color = color;
  Turtle->previous = NULL;

  return Turtle;
}

VOID TurtleForwardNoLine(TURTLE *Turtle, INTN distance) {
  INTN CosVal = FixedCos(Turtle->heading);
  INTN SinVal = FixedSin(Turtle->heading);

  INTN OffsetX = (distance * CosVal) / SCALE;
  INTN OffsetY = (distance * SinVal) / SCALE;

  INTN EndX = Turtle->x + OffsetX;
  INTN EndY = Turtle->y - OffsetY;

  Turtle->x = EndX;
  Turtle->y = EndY;
}

VOID TurtleForward(TURTLE *Turtle, INTN distance,
                   EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop) {
  INTN CosVal = FixedCos(Turtle->heading);
  INTN SinVal = FixedSin(Turtle->heading);

  INTN OffsetX = (distance * CosVal) / SCALE;
  INTN OffsetY = (distance * SinVal) / SCALE;

  INTN EndX = Turtle->x + OffsetX;
  INTN EndY = Turtle->y - OffsetY;

  DrawLineBresenham(Gop, Turtle->x, Turtle->y, EndX, EndY, Turtle->color);

  Turtle->x = EndX;
  Turtle->y = EndY;
}

VOID TurtleRotate(TURTLE *Turtle, INTN delta) {
  INTN h = (Turtle->heading + delta) % 360;
  if (h < 0)
    h += 360;
  Turtle->heading = h;
}

VOID TurtleSave(TURTLE *Turtle) {
  TURTLE *saved = AllocateCopyPool(sizeof(TURTLE), Turtle);
  saved->previous = Turtle->previous;
  Turtle->previous = saved;
}

VOID TurtleLoad(TURTLE *Turtle) {
  TURTLE *previous = Turtle->previous;
  if (!previous)
    return;

  Turtle->x = previous->x;
  Turtle->y = previous->y;
  Turtle->heading = previous->heading;
  Turtle->color = previous->color;
  Turtle->previous = previous->previous;

  FreePool(previous);
}

VOID TurtleFree(TURTLE *Turtle) {
  TURTLE *current = Turtle->previous;
  TURTLE *next;

  while (current != NULL) {
    next = current->previous;
    FreePool(current);
    current = next;
  }

  FreePool(Turtle);
}
