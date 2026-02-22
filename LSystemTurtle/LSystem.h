#ifndef EDK2_LSYSTEM_H
#define EDK2_LSYSTEM_H

typedef struct Rule Rule;

struct Rule {
  CHAR16 match;
  UINTN len;
  CHAR16 *replace;
};

Rule *rule_create(CHAR16 match, CHAR16 *replace);
void rule_free(Rule *rule);

typedef struct String String;
struct String {
  UINTN len;
  CHAR16 *str;
};

String *string_create(CHAR16 *str);
void string_free(String *str);
String *evaluate_string(UINTN count, Rule *rules[count], String *input);

#endif // EDK2_LSYSTEM_H