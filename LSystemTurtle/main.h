#ifndef EDK2_MAIN_H
#define EDK2_MAIN_H

#include "LSystem.h"

typedef struct Fractal Fractal;
struct Fractal {
  CHAR16 *name;
  UINTN iterations;

  INTN forward_speed;
  INTN rotation_speed;

  CHAR16 *initial;
  Rule *rules;
  UINTN rule_count;
};

#define R(m, s) {m, sizeof(s)/sizeof(CHAR16) - 1, (CHAR16 *)s}

static Rule koch_island_rules[] = {
    R(L'F', L"F+FF-FF-F-F+F+FF-F-F+F+FF+FF-F")};
static Rule triangle_rules[] = {R(L'F', L"F+F-F-F+F")};
static Rule island_lake_rules[] = {
    R(L'F', L"F+f-FF+F+FF+Ff+FF-f+FF-F-FF-FfFFF"), R(L'f', L"ffffff")};
static Rule circular_rules[] = {R(L'F', L"FF-F-F-F-F-F+F")};
static Rule sierpinski_rules[] = {R(L'F', L"FF-F-F-F-FF")};
static Rule weave_rules[] = {R(L'F', L"FF-F+F-F-FF")};
static Rule unfilled_square_rules[] = {R(L'F', L"FF-F--F-F")};
static Rule cool_curve_rules[] = {R(L'F', L"F-FF--F-F")};
static Rule cool_unfilled_rules[] = {R(L'F', L"F-F+F-F-F")};

const UINTN preset_fractals_count = 9;
Fractal preset_fractals[9] = {
    {(CHAR16 *)L"Quadratic Koch Island", 2, 2, 90, (CHAR16 *)L"F-F-F-F", koch_island_rules, 1},
    {(CHAR16 *)L"Triangle", 4, 5, 90, (CHAR16 *)L"-F", triangle_rules, 1},
    {(CHAR16 *)L"Island and Lake", 2, 2, 90, (CHAR16 *)L"F+F+F+F", island_lake_rules, 2},
    {(CHAR16 *)L"Circular Curve", 4, 3, 90, (CHAR16 *)L"F-F-F-F", circular_rules, 1},
    {(CHAR16 *)L"Sierpinski Square", 4, 5, 90, (CHAR16 *)L"F-F-F-F", sierpinski_rules, 1},
    {(CHAR16 *)L"Weave", 3, 5, 90, (CHAR16 *)L"F-F-F-F", weave_rules, 1},
    {(CHAR16 *)L"Unfilled Square", 4, 5, 90, (CHAR16 *)L"F-F-F-F", unfilled_square_rules, 1},
    {(CHAR16 *)L"Cool Curve", 5, 3, 90, (CHAR16 *)L"F-F-F-F", cool_curve_rules, 1},
    {(CHAR16 *)L"Cool Unfilled Curve", 4, 5, 90, (CHAR16 *)L"F-F-F-F", cool_unfilled_rules, 1}};


void get_fractal(Fractal *f);
void run_fractal(Fractal *f);


#endif // EDK2_MAIN_H