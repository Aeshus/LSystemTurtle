#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "LSystem.h"

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

  String *input = string_create(L"CACA");
  Rule *rule = rule_create(L'A', L"AB");

  Rule *rules[1] = {rule};

  String *val = input;
  for (int i = 0; i < 10; i++) {
    String *output = evaluate_string(1, rules, val);
    string_free(val);
    val = output;
  }

  Print(val->str);

  while (1);

  return EFI_SUCCESS;
}