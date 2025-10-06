#include "LSystem.h"
#include "Turtle.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>

typedef struct {
  CHAR16 *Name;
  UINTN Iterations;
  INTN Angle;
  INTN SideLength;
  CHAR16 *Axiom;
  L_SYSTEM_RULE *Rules;
} FRACTAL_PRESET;

L_SYSTEM_RULE Rules_KochIsland[] = {{L'F', L"F+FF-FF-F-F+F+FF-F-F+F+FF+FF-F"},
                                    {0, NULL}};

L_SYSTEM_RULE Rules_Triangle[] = {{L'F', L"F+F-F-F+F"}, {0, NULL}};

L_SYSTEM_RULE Rules_IslandLake[] = {
    {L'F', L"F+f-FF+F+FF+Ff+FF-f+FF-F-FF-FfFFF"}, {L'f', L"ffffff"}, {0, NULL}};

L_SYSTEM_RULE Rules_Circular[] = {{L'F', L"FF-F-F-F-F-F+F"}, {0, NULL}};

L_SYSTEM_RULE Rules_Sierpinski[] = {{L'F', L"FF-F-F-F-FF"}, {0, NULL}};

L_SYSTEM_RULE Rules_Weave[] = {{L'F', L"FF-F+F-F-FF"}, {0, NULL}};

L_SYSTEM_RULE Rules_UnfilledSquare[] = {{L'F', L"FF-F--F-F"}, {0, NULL}};

L_SYSTEM_RULE Rules_CoolCurve[] = {{L'F', L"F-FF--F-F"}, {0, NULL}};

L_SYSTEM_RULE Rules_CoolUnfilled[] = {{L'F', L"F-F+F-F-F"}, {0, NULL}};

FRACTAL_PRESET gPresetLibrary[] = {
    {L"Quadratic Koch Island", 2, 90, 5, L"F-F-F-F", Rules_KochIsland},
    {L"Triangle", 4, 90, 5, L"-F", Rules_Triangle},
    {L"Island and Lake", 2, 90, 5, L"F+F+F+F", Rules_IslandLake},
    {L"Circular Curve", 4, 90, 5, L"F-F-F-F", Rules_Circular},
    {L"Sierpinski Square", 4, 90, 5, L"F-F-F-F", Rules_Sierpinski},
    {L"Weave", 3, 90, 5, L"F-F-F-F", Rules_Weave},
    {L"Unfilled Square", 4, 90, 5, L"F-F-F-F", Rules_UnfilledSquare},
    {L"Cool Curve", 5, 90, 5, L"F-F-F-F", Rules_CoolCurve},
    {L"Cool Unfilled Curve", 4, 90, 5, L"F-F-F-F", Rules_CoolUnfilled}};

EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL Black = {0, 0, 0, 0};
EFI_GRAPHICS_OUTPUT_BLT_PIXEL White = {255, 255, 255, 0};

VOID ReadInputLine(CHAR16 *Buffer, UINTN BufferSize) {
  EFI_INPUT_KEY Key;
  UINTN Cursor = 0;

  // Zero buffer
  SetMem(Buffer, BufferSize * sizeof(CHAR16), 0);

  while (Cursor < BufferSize - 1) {
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, NULL);
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);

    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Print(L"\n");
      break;
    } else if (Key.UnicodeChar == CHAR_BACKSPACE) {
      if (Cursor > 0) {
        Cursor--;
        Print(L"\b \b");
      }
    } else if (Key.UnicodeChar >= ' ') {
      Buffer[Cursor++] = Key.UnicodeChar;
      Print(L"%c", Key.UnicodeChar);
    }
  }
  Buffer[Cursor] = L'\0';
}

FRACTAL_PRESET CustomFractalCreate() {
  CHAR16 Axiom[256];
  Print(L"Axiom: ");
  ReadInputLine(Axiom, 256);

  L_SYSTEM_RULE *customRules = NULL;
  Print(L"RULES: (Put an empty rule to stop.)\n");
  Print(L"e.g. \"F=FF+F\"\n");
  CHAR16 RuleBuffer[256];
  UINTN RuleCount = 0;
  for (;; RuleCount++) {
    Print(L"Rule %u: ", RuleCount + 1);
    ReadInputLine(RuleBuffer, 256);
    if (StrLen(RuleBuffer) == 0) {
      break; // No new rules
    }

    if (StrLen(RuleBuffer) < 3 || RuleBuffer[1] != L'=') {
      Print(L"Invalid rule format. Skipping. e.g. F=F+F\n");
      RuleCount--; // Decrement to retry this rule number
      continue;
    }

    CHAR16 symbol = RuleBuffer[0];
    CHAR16 *ruleString = &RuleBuffer[2];

    customRules =
        ReallocatePool(RuleCount * sizeof(L_SYSTEM_RULE),
                       (RuleCount + 1) * sizeof(L_SYSTEM_RULE), customRules);

    customRules[RuleCount].Symbol = symbol;
    customRules[RuleCount].String =
        AllocateCopyPool(StrSize(ruleString), ruleString);
  }

  // Add null terminator rule
  customRules =
      ReallocatePool(RuleCount * sizeof(L_SYSTEM_RULE),
                     (RuleCount + 1) * sizeof(L_SYSTEM_RULE), customRules);
  customRules[RuleCount].Symbol = 0;
  customRules[RuleCount].String = NULL;

  Print(L"\nEnter number of iterations: ");
  CHAR16 IterationsBuffer[256];
  ReadInputLine(IterationsBuffer, 256);
  UINTN Iterations = StrDecimalToUintn(IterationsBuffer);

  Print(L"\nEnter the angle: ");
  CHAR16 AngleBuffer[256];
  ReadInputLine(AngleBuffer, 256);
  INTN turnAngle = (INTN)StrDecimalToUintn(AngleBuffer);

  Print(L"\nEnter the side length: ");
  CHAR16 LengthBuffer[256];
  ReadInputLine(LengthBuffer, 256);
  INTN sideLength = (INTN)StrDecimalToUintn(LengthBuffer);

  // FIXED: Create a struct on the stack, populate it, and return it.
  FRACTAL_PRESET customPreset = {L"Custom Fractal",
                                 Iterations,
                                 turnAngle,
                                 sideLength,
                                 AllocateCopyPool(StrSize(Axiom), Axiom),
                                 customRules};
  return customPreset;
}

VOID CustomFractalFree(FRACTAL_PRESET *preset) {
  if (preset == NULL)
    return;

  // Free the axiom string
  if (preset->Axiom != NULL) {
    FreePool(preset->Axiom);
  }

  // Free each rule string and then the rules array itself
  if (preset->Rules != NULL) {
    for (UINTN i = 0; preset->Rules[i].String != NULL; i++) {
      FreePool(preset->Rules[i].String);
    }
    FreePool(preset->Rules);
  }
}

TURTLE *ExecuteTurtle(CHAR16 *ExpandedString, INTN sideLength, INTN turnAngle,
                      UINTN xInitial, UINTN yInitial, INTN headingInitial,
                      EFI_GRAPHICS_OUTPUT_BLT_PIXEL color) {
  TURTLE *Turtle = TurtleCreate(xInitial, yInitial, headingInitial, color);
  TurtleRotate(Turtle, 90);

  for (UINTN i = 0; i < StrLen(ExpandedString); i++) {
    CHAR16 command = ExpandedString[i];

    if (command == L'F') {
      TurtleForward(Turtle, sideLength, Gop);
    }
    if (command == L'f') {
      TurtleForwardNoLine(Turtle, sideLength);
    }
    if (command == L'+') {
      TurtleRotate(Turtle, turnAngle);
    }
    if (command == L'-') {
      TurtleRotate(Turtle, -turnAngle);
    }
    if (command == L'[') {
      TurtleSave(Turtle);
    }
    if (command == L']') {
      TurtleLoad(Turtle);
    }
  }

  return Turtle;
}

EFI_STATUS EFIAPI AppMain(EFI_HANDLE ImageHandle,
                           EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status;

  Status =
      gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&Gop);
  if (EFI_ERROR(Status)) {
    Print(L"Failed to locate GOP: %r\n", Status);
    return Status;
  }

  gST->ConOut->ClearScreen(gST->ConOut);
  for (;;) {
    Print(L"UEFI L-System Turtle\n");

    UINTN gPresetCount = sizeof(gPresetLibrary) / sizeof(FRACTAL_PRESET);
    Print(L"[1]: Make your own fractal\n");
    for (UINTN i = 0; i < gPresetCount; i++) {
      Print(L"[%u]: %s\n", i + 2, gPresetLibrary[i].Name);
    }
    Print(L"Choose a fractal (empty = quit): ");

    CHAR16 FractalBuffer[256];
    ReadInputLine(FractalBuffer, 256);
    if (StrLen(FractalBuffer) == 0) {
      return EFI_SUCCESS;
    }

    UINTN Number = StrDecimalToUintn(FractalBuffer);

    FRACTAL_PRESET customFractal;
    FRACTAL_PRESET *selectedFractal;
    BOOLEAN isCustom = FALSE;

    if (Number == 1) {
      customFractal = CustomFractalCreate();
      selectedFractal = &customFractal;
      isCustom = TRUE;
    } else if (Number > 1 && Number <= gPresetCount + 1) {
      selectedFractal = &gPresetLibrary[Number - 2];
    } else {
      Print(L"Invalid selection.\n");
      return EFI_ABORTED;
    }

    L_SYSTEM *sys = LSystemCreate(selectedFractal->Axiom);
    for (UINTN i = 0; selectedFractal->Rules[i].String != NULL; i++) {
      LSystemAddRule(sys, selectedFractal->Rules[i].Symbol,
                     selectedFractal->Rules[i].String);
    }
    CHAR16 *ExpandedString = LSystemExpand(sys, selectedFractal->Iterations);

    UINTN xInitial = Gop->Mode->Info->HorizontalResolution / 2;
    UINTN yInitial = Gop->Mode->Info->VerticalResolution / 2;
    UINTN headingInitial = 0;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = {0xFF, 0xFF, 0xFF, 0};

    TURTLE *Turtle = ExecuteTurtle(ExpandedString, selectedFractal->SideLength,
                                   selectedFractal->Angle, xInitial, yInitial,
                                   headingInitial, color);

    if (isCustom) {
      CustomFractalFree(selectedFractal);
    }

    TurtleFree(Turtle);
    FreePool(ExpandedString);
    LSystemFree(sys);

    Print(L"\nDone! Press any key to choose another.");
    ReadInputLine(FractalBuffer, 256);
    gST->ConOut->ClearScreen(gST->ConOut);
  }

  return EFI_SUCCESS;
}
