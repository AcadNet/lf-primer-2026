# Task 1 — Basic Movement

[← Workshop 1](../docs/workshop1.md) · [Workshop 2 →](../docs/workshop2.md)

## Goal

Program the robot so that it demonstrates all four basic movements:

1. move forward;
2. move backward;
3. rotate left in place;
4. rotate right in place.

At the end, the robot must stop.

## Rules

- Use the provided `motors_set(left, right)` function.
- Use safe motor values. Start around `30`.
- Each movement should be clearly visible — roughly one second each.
- The robot must stop between actions, and it must stop at the end.
- Do not modify `board/pin_mux.c`, `board/peripherals.c` or `board/clock_config.c`.
- Test the first time with the drive wheels lifted off the table.

## Useful commands

```c
motors_set(30, 30);     // forward
motors_set(-30, -30);   // backward
motors_set(-30, 30);    // rotate in place
motors_set(30, -30);    // rotate the other way
motors_stop();          // stop
wait_ms(1000);          // wait one second
```

Think about what each pair of left/right values does before you test it.

## Suggested skeleton

```c
#include "robot.h"

int main(void)
{
    robot_init();
    motors_stop();

    // 1. Forward

    // 2. Stop briefly

    // 3. Backward

    // 4. Stop briefly

    // 5. Rotate left

    // 6. Stop briefly

    // 7. Rotate right

    motors_stop();

    while (1) {
    }
}
```

## Success criteria

Your solution passes when the robot clearly demonstrates:

- [ ] forward
- [ ] backward
- [ ] rotate left
- [ ] rotate right
- [ ] final stop

Scoring is 25% per movement. A run without a final stop does not count.

Distance and angle are **not** scored in Task 1 — precision belongs to
Workshop 2.

## Bonus

Create four helper functions:

```c
static void forward(void)      { motors_set(30, 30);  }
static void backward(void)     { motors_set(-30, -30); }
static void rotate_left(void)  { motors_set(-30, 30); }
static void rotate_right(void) { motors_set(30, -30); }
```

and rewrite your program using them. Reading

```c
forward();
wait_ms(1000);
rotate_left();
```

is much easier than reading pairs of numbers.

If `rotate_left()` turns the robot to the right, swap the two sign pairs —
and tell the instructor, because it means the left/right mapping in
`source/robot.c` needs checking.
