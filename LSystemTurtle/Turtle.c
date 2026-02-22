#include "Turtle.h"

Turtle *turtle_copy(Turtle *t) {
  Turtle *new = (Turtle *)AllocatePool(sizeof(Turtle));

  new->forward_speed = t->forward_speed;
  new->rotation_speed = t->rotation_speed;
  new->x = t->x;
  new->y = t->y;
  new->heading = t->heading;

  return new;
}

Turtle *turtle_create(UINTN screen_width, UINTN screen_height,
                      INTN forward_speed, INTN rotation_speed) {
  Turtle *t = (Turtle *)AllocatePool(sizeof(Turtle));

  t->x = screen_width / 2;
  t->y = screen_height / 2;
  t->heading = 180;
  t->forward_speed = forward_speed;
  t->rotation_speed = rotation_speed;

  return t;
}

INTN integer_sine(INTN deg) {
  INTN sign = 1;
  if (deg >= 180) {
    deg -= 180;
    sign = -1;
  }

  INTN term = deg * (180 - deg);
  INTN numerator = 4 * term * TRIG_SCALE;
  INTN denominator = 40500 - term;
  return numerator / denominator * sign;
}

int sin_distance(UINTN distance, INTN deg) {
  return (int)((integer_sine(deg) * (INTN)distance) / TRIG_SCALE);
}

int cos_distance(UINTN distance, INTN deg) {
  return sin_distance(distance, (deg + 90) % 360);
}

Turtle *turtle_forward(Turtle *t) {
  Turtle *copy = turtle_copy(t);

  INTN dx = sin_distance(t->forward_speed, t->heading);
  INTN dy = cos_distance(t->forward_speed, t->heading);

  copy->x = t->x + dx;
  copy->y = t->y + dy;

  return copy;
}

void turtle_free(Turtle *t) { FreePool(t); }

TurtleStack *turtle_stack_init() { return NULL; }

TurtleStack *turtle_stack_create(Turtle *t) {
  TurtleStack *ts = (TurtleStack *)AllocatePool(sizeof(TurtleStack));

  ts->next = NULL;
  ts->turtle = turtle_copy(t);

  return ts;
}

void turtle_stack_push(TurtleStack **stack, Turtle *t) {
  TurtleStack *ts = turtle_stack_create(t);

  if (*stack == NULL) {
    *stack = ts;
    return;
  }

  ts->next = *stack;
  *stack = ts;
}

void turtle_stack_free(TurtleStack *ts) {
  if (ts->next != NULL)
    turtle_stack_free(ts->next);

  turtle_free(ts->turtle);
  FreePool(ts);
}

Turtle *turtle_stack_pop(TurtleStack **stack) {
  if (*stack == NULL)
    return NULL;

  TurtleStack *curr = *stack;
  *stack = curr->next;

  Turtle *t = turtle_copy(curr->turtle);
  curr->next = NULL;

  turtle_stack_free(curr);

  return t;
}

int isqrt(int n) {
  if (n <= 1)
    return n;

  INTN x = n;
  INTN y = (x + 1) / 2;

  while (y < x) {
    x = y;
    y = (x + n / x) / 2;
  }
  return x;
}

void blend_pixel(UINTN screen_width, UINTN screen_height,
                 EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer, INTN x, INTN y,
                 INTN alpha) {
  if (x < 0 || x >= (INTN)screen_width || y < 0 || y >= (INTN)screen_height ||
      alpha <= 0) {
    return;
  }

  if (alpha > 255)
    alpha = 255;

  UINTN index = y * screen_width + x;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *bg = &buffer[index];

  UINT8 r = 255, g = 255, b = 255;
  bg->Red = (r * alpha + bg->Red * (255 - alpha)) / 255;
  bg->Green = (g * alpha + bg->Green * (255 - alpha)) / 255;
  bg->Blue = (b * alpha + bg->Blue * (255 - alpha)) / 255;
}

INTN abs(INTN val) { return val < 0 ? -val : val; }

void draw_line(UINTN screen_width, UINTN screen_height,
               EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer, INTN x0, INTN y0, INTN x1,
               INTN y1) {

  INTN dx = abs(x1 - x0);
  INTN dy = abs(y1 - y0);

  INTN is_steep = dy > dx;

  if (is_steep) {
    INTN temp = x0;
    x0 = y0;
    y0 = temp;

    temp = x1;
    x1 = y1;
    y1 = temp;

    temp = dx;
    dx = dy;
    dy = temp;
  }

  if (x0 > x1) {
    INTN temp = x0;
    x0 = x1;
    x1 = temp;

    temp = y0;
    y0 = y1;
    y1 = temp;
  }

  INTN y_step = (y0 < y1) ? 1 : -1;
  INTN error = 0;
  INTN y = y0;

  INTN length = isqrt((int)((dx * dx) + (dy * dy)));
  if (length == 0)
    length = 1;
  INTN distance_scale = (256 * dx) / length;

  for (INTN x = x0; x <= x1; x++) {
    INTN error_fraction = (error * 256) / (dx == 0 ? 1 : dx);

    INTN dist = (error_fraction * distance_scale) / 256;

    INTN alpha_center = 255;
    INTN alpha_above = 255 - abs(dist - distance_scale);
    INTN alpha_below = 255 - abs(dist + distance_scale);
    INTN alpha_far_above = 255 - abs(dist - 2 * distance_scale);
    INTN alpha_far_below = 255 - abs(dist + 2 * distance_scale);

    if (is_steep) {
      blend_pixel(screen_width, screen_height, buffer, y, x, alpha_center);
      blend_pixel(screen_width, screen_height, buffer, y - y_step, x, 255);
      blend_pixel(screen_width, screen_height, buffer, y + y_step, x,
                  alpha_below);
      blend_pixel(screen_width, screen_height, buffer, y - 2 * y_step, x,
                  alpha_far_above);
    } else {
      blend_pixel(screen_width, screen_height, buffer, x, y, alpha_center);
      blend_pixel(screen_width, screen_height, buffer, x, y - y_step, 255);
      blend_pixel(screen_width, screen_height, buffer, x, y + y_step,
                  alpha_below);
      blend_pixel(screen_width, screen_height, buffer, x, y - 2 * y_step,
                  alpha_far_above);
    }

    error -= dy;
    if (error < 0) {
      y += y_step;
      error += dx;
    }
  }
}

void turtle_run(UINTN screen_width, UINTN screen_height,
                EFI_GRAPHICS_OUTPUT_BLT_PIXEL *buffer, String *input,
                INTN forward_speed, INTN rotation_speed) {
  Turtle *t =
      turtle_create(screen_width, screen_height, forward_speed, rotation_speed);
  TurtleStack *ts = turtle_stack_init();

  for (INTN i = 0; i < input->len; i++) {
    switch (input->str[i]) {
    case L'F': {
      Turtle *new = turtle_forward(t);
      draw_line(screen_width, screen_height, buffer, t->x, t->y, new->x,
                new->y);
      turtle_free(t);
      t = new;
      break;
    }

    case L'f': {
      Turtle *new = turtle_forward(t);
      turtle_free(t);
      t = new;
      break;
    }

    case L'+': {
      t->heading = ((t->heading + t->rotation_speed) % 360 + 360) % 360;
      break;
    }

    case L'-': {
      t->heading = ((t->heading - t->rotation_speed) % 360 + 360) % 360;
      break;
    }

    case L'[': {
      turtle_stack_push(&ts, t);
      break;
    }

    case L']': {
      Turtle *n = turtle_stack_pop(&ts);
      if (n == NULL)
        break;

      turtle_free(t);
      t = n;
      break;
    }
    }
  }
}