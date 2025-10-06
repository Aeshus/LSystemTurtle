#ifndef _LSYSTEM_H
#define _LSYSTEM_H

typedef struct {
  CHAR16 Symbol;
  CHAR16 *String;
} L_SYSTEM_RULE;

typedef struct {
  CHAR16 *Axiom;
  L_SYSTEM_RULE *Rules; // pointer to start of Rules array
  UINTN RuleCount;
} L_SYSTEM;

L_SYSTEM *LSystemCreate(CHAR16 *Axiom);

VOID LSystemAddRule(L_SYSTEM *System, CHAR16 Symbol, CHAR16 *String);

CHAR16 *LSystemExpand(L_SYSTEM *System, UINTN Iterations);

VOID LSystemFree(L_SYSTEM *System);

#endif
