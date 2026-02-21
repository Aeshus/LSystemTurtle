#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include "LSystem.h"
#include "Turtle.h"

/**
 * The main function of the LSystemTurtle.
 */
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  String *input = string_create(L"F-F-F-F");
  Rule *rule = rule_create(L'F', L"FF-F-F-F-FF");

  Rule *rules[1] = {rule};

  String *val = input;
  for (int i = 0; i < 4; i++) {
    String *output = evaluate_string(1, rules, val);
    string_free(val);
    val = output;
  }

  Print(val->str);

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&gop);
  UINTN width = gop->Mode->Info->HorizontalResolution;
  UINTN height = gop->Mode->Info->VerticalResolution;
  UINTN buffer_size = width * height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocateZeroPool(buffer_size);

  turtle_run(width, height, buffer, val, 5, 90);

  //gop->Blt(
  //      gop,
  //      buffer,
  //      EfiBltBufferToVideo,
  //      0, 0,
  //      0, 0,
  //      width, height,
  //      0
  //);


  while (1);

  return EFI_SUCCESS;
}