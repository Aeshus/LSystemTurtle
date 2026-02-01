#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

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

  BOOLEAN stop = false;
  while (!stop) {
    Print(L"L-System Turtle\r\n");

    Print(L"Select options:\r\n");
    Print(L"1) Make New\r\n");
    Print(L"2) Use Saved\r\n");

    EFI_INPUT_KEY key;
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);

    Print(&key.UnicodeChar);

    stop = true;
  }

  while (1);

  return EFI_SUCCESS;
}