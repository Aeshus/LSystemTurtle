#include "main.h"
#include "LSystem.h"
#include "Turtle.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

void ReadInputLine(CHAR16 *Buffer, UINTN BufferSize) {
  UINTN Index = 0;
  EFI_INPUT_KEY Key;
  while (Index < BufferSize - 1) {
    UINTN WaitIndex;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &WaitIndex);
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    if (Key.UnicodeChar == L'\r' || Key.UnicodeChar == L'\n') {
      Print(L"\n");
      break;
    }
    if (Key.UnicodeChar == L'\b') {
      if (Index > 0) {
        Index--;
        Print(L"\b \b");
        Buffer[Index] = L'\0';
      }
      continue;
    }
    if (Key.UnicodeChar >= L' ') {
      Buffer[Index++] = Key.UnicodeChar;
      Print(L"%c", Key.UnicodeChar);
    }
  }
  Buffer[Index] = L'\0';
}



/**
 * The main function of the LSystemTurtle.
 */
EFI_STATUS
EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  Print(L"UEFI L-System Turtle\n");
  Print(L"[1]: Make your own fractal\n");
  for (UINTN i = 0; i < preset_fractals_count; i++) {
    Print(L"[%u]: %s\n", i + 2, preset_fractals[i].name);
  }
  Print(L"Choose a fractal (empty = quit): ");

  CHAR16 input_buffer[256];
  ReadInputLine(input_buffer, 256);
  if (StrLen(input_buffer) == 0) {
    return EFI_SUCCESS;
  }

  UINTN selection = StrDecimalToUintn(input_buffer);

  Fractal chosen = {0};
  INTN is_custom = 0;

  if (selection == 1) {
    get_fractal(&chosen);
    is_custom = 1;
  } else if (selection >= 2 && selection < preset_fractals_count + 2) {
    chosen = preset_fractals[selection - 2];
  } else {
    Print(L"Invalid selection.\n");
    return EFI_ABORTED;
  }

  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  run_fractal(&chosen);

  while (1)
    ;

  return EFI_SUCCESS;
}

void get_fractal(Fractal *f) {
  f->name = L"Custom";

  Print(L"Initial/Axiom: ");
  CHAR16 axiom_buf[256];
  ReadInputLine(axiom_buf, 256);
  f->initial = AllocateCopyPool(StrSize(axiom_buf), axiom_buf);

  Print(L"RULES: (Empty rule to stop)\n");

  Rule *tempRules = NULL;
  UINTN RuleCount = 0;
  CHAR16 RuleBuffer[256];

  for (;; RuleCount++) {
    Print(L"Rule %u: ", RuleCount + 1);
    ReadInputLine(RuleBuffer, 256);
    if (StrLen(RuleBuffer) == 0)
      break;

    if (StrLen(RuleBuffer) < 3 || RuleBuffer[1] != L'=') {
      Print(L"Invalid format. Use F=FF\n");
      RuleCount--;
      continue;
    }

    tempRules = ReallocatePool(RuleCount * sizeof(Rule),
                               (RuleCount + 1) * sizeof(Rule), tempRules);

    tempRules[RuleCount].match = RuleBuffer[0];
    tempRules[RuleCount].len = StrLen(&RuleBuffer[2]);
    tempRules[RuleCount].replace =
        AllocateCopyPool(StrSize(&RuleBuffer[2]), &RuleBuffer[2]);
  }

  f->rules = tempRules;
  f->rule_count = RuleCount;

  Print(L"\nEnter number of iterations: ");
  CHAR16 iterations_buffer[256];
  ReadInputLine(iterations_buffer, 256);
  f->iterations = StrDecimalToUintn(iterations_buffer);

  Print(L"\nEnter the rotation speed/angle: ");
  CHAR16 rotation_buffer[256];
  ReadInputLine(rotation_buffer, 256);
  f->rotation_speed = (INTN)StrDecimalToUintn(rotation_buffer);

  Print(L"\nEnter the forward speed/side length: ");
  CHAR16 forward_buffer[256];
  ReadInputLine(forward_buffer, 256);
  f->forward_speed = (INTN)StrDecimalToUintn(forward_buffer);
}

void run_fractal(Fractal *f) {
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&gop);

  String *input = string_create(f->initial);

  String *val = input;
  for (int i = 0; i < f->iterations; i++) {
    String *output = evaluate_string(f->rule_count, f->rules, val);
    string_free(val);
    val = output;
  }

  UINTN width = gop->Mode->Info->HorizontalResolution;
  UINTN height = gop->Mode->Info->VerticalResolution;
  UINTN buffer_size = width * height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer =
      (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)AllocateZeroPool(buffer_size);

  turtle_run(width, height, buffer, val, f->forward_speed, f->rotation_speed);

  gop->Blt(gop, buffer, EfiBltBufferToVideo, 0, 0, 0, 0, width, height, 0);

  if (val != input) {
    string_free(val);
  }
  FreePool(buffer);
}
