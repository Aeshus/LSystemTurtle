#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

typedef struct Rule Rule;
struct Rule {
  CHAR16 match;
  UINTN len;
  CHAR16 *replace;
};

Rule *rule_create(CHAR16 match, CHAR16 *replace) {
  Rule *r = (Rule *)AllocatePool(sizeof(Rule));

  r->match = match;
  r->len = StrLen(replace);
  r->replace = replace;

  return r;
}

typedef struct String String;
struct String {
  UINTN len;
  CHAR16 *str;
};

String *string_create(CHAR16 *str) {
  String *s = (String *)AllocatePool(sizeof(String));
  s->len = StrLen(str);

  s->str = (CHAR16 *)AllocatePool(sizeof(CHAR16) * (s->len + 1));
  for (UINTN i = 0; i <= s->len; i++) {
    s->str[i] = str[i];
  }

  return s;
}

void string_free(String *str) {
  FreePool(str->str);
  FreePool(str);
}

String *generate_string(UINTN count, Rule *rules[count], String *input) {
  UINTN new_size = input->len;
  for (int i = 0; i < input->len; i++) {
    for (int j = 0; j < count; j++) {
      if (input->str[i] == rules[j]->match) {
        new_size += rules[j]->len - 1;
        break;
      }
    }
  }

  CHAR16 *str = AllocatePool(sizeof(CHAR16) * (new_size + 1));

  int offset = 0;
  for (int i = 0; i < input->len; i++) {
    int matched = 0;

    for (int j = 0; j < count; j++) {
      if (input->str[i] == rules[j]->match) {
        for (int k = 0; k < rules[j]->len; k++) {
          str[offset++] = rules[j]->replace[k];
        }
        matched = 1;
        break;
      }
    }

    if (!matched) {
      str[offset++] = input->str[i];
    }
  }
  str[offset] = L'\0';

  String *s = AllocatePool(sizeof(String));
  s->len = new_size;
  s->str = str;

  return s;
}

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
  String *output = generate_string(1, rules, input);
  string_free(input);

  Print(output->str);

  while (1);

  return EFI_SUCCESS;
}