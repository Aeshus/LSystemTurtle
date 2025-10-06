#include "LSystem.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Uefi.h>

L_SYSTEM *LSystemCreate(CHAR16 *Axiom) {
  // Allocate struct
  L_SYSTEM *System = AllocateZeroPool(sizeof(L_SYSTEM));

  // Alocate axiom string
  System->Axiom = AllocateCopyPool(StrSize(Axiom), Axiom);

  System->Rules = NULL;
  System->RuleCount = 0;

  return System;
}

VOID LSystemAddRule(L_SYSTEM *System, CHAR16 Symbol, CHAR16 *String) {
  // Reallocate rules array
  L_SYSTEM_RULE *NewRules = ReallocatePool(
      System->RuleCount * sizeof(L_SYSTEM_RULE),
      (System->RuleCount + 1) * sizeof(L_SYSTEM_RULE), System->Rules);

  System->Rules = NewRules;

  // Allocate a new string and symbol
  System->Rules[System->RuleCount].Symbol = Symbol;
  System->Rules[System->RuleCount].String =
      AllocateCopyPool(StrSize(String), String);
  System->RuleCount++;
}

VOID LSystemFree(L_SYSTEM *System) {
  // Free all rule strings
  for (UINTN i = 0; i < System->RuleCount; i++) {
    if (System->Rules[i].String != NULL) {
      FreePool(System->Rules[i].String);
    }
  }

  // Free all rules
  if (System->Rules != NULL) {
    FreePool(System->Rules);
  }

  // Free axiom string
  if (System->Axiom != NULL) {
    FreePool(System->Axiom);
  }

  // Free system struct
  FreePool(System);
}

CHAR16 *LSystemExpand(L_SYSTEM *System, UINTN Iterations) {
  CHAR16 *CurrentString =
      AllocateCopyPool(StrSize(System->Axiom), System->Axiom);
  CHAR16 *OutputString = NULL;

  // Don't expand if no axiom
  for (UINTN i = 0; i < Iterations; i++) {
    UINTN OutputStringLength = 0;

    // Calculate new string allocation size
    for (UINTN j = 0; j < StrLen(CurrentString); j++) {
      BOOLEAN Found = FALSE;
      for (UINTN k = 0; k < System->RuleCount; ++k) {
        if (CurrentString[j] == System->Rules[k].Symbol) {
          OutputStringLength += StrLen(System->Rules[k].String);
          Found = TRUE;
          break; // Rule found, onto next
        }
      }
      if (!Found) {
        OutputStringLength++; // Rule not found, so we'll copy as-is later
      }
    }

    // Alloacate our new string (+1 is because of null terminator)
    OutputString = AllocateZeroPool((OutputStringLength + 1) * sizeof(CHAR16));

    // Build string
    UINTN Cursor = 0;
    for (UINTN j = 0; j < StrLen(CurrentString); ++j) {
      BOOLEAN FoundRule = FALSE;
      for (UINTN k = 0; k < System->RuleCount; ++k) {
        if (CurrentString[j] == System->Rules[k].Symbol) {
          // Rule found, copy it over
          StrCpyS(&OutputString[Cursor], OutputStringLength + 1 - Cursor,
                  System->Rules[k].String);
          Cursor += StrLen(System->Rules[k].String);
          FoundRule = TRUE;
          break;
        }
      }
      if (!FoundRule) {
        // Rule not found, copy as is
        OutputString[Cursor++] = CurrentString[j];
      }
    }

    // Free the old string
    FreePool(CurrentString);
    CurrentString = OutputString;
  }

  return CurrentString;
}
