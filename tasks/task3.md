# Task 3 — Follow the line

[← Workshop 3](../docs/workshop3.md)

## Goal

Get the robot around the real track, on its own, from start to finish.

You will build two controllers to do it: first a bang-bang controller, then
a proportional one. Both must work. Keep both, because you will be asked to
say what the difference felt like.

There is no time limit. Speed is Workshop 4's problem — for now the robot
only has to make it round.

## Before you start

Check your sensors, exactly as in
[Step 1](../docs/workshop3.md#step-1--look-before-you-code): print the
sensor byte and move the robot over the line by hand until you are sure the
readings make sense. Do not skip this. A controller built on sensors you
have not checked will waste your whole session.

Start from [`starters/workshop3/main.c`](../starters/workshop3/main.c).
Copy it over `source/main.c` in your MCUXpresso project.

## Part 1 — Bang-bang control

Fill in `follow_bang_bang()`.

Split the sensor byte into a left group, a centre group and a right group,
and pick one of three responses:

- the line is under the **centre** — drive straight;
- the line is under the **left** sensors — steer left;
- the line is under the **right** sensors — steer right.

Test the centre group first. Decide for yourself what should happen when no
sensor sees anything at all, and be ready to explain your choice.

Run it on the track. Watch how it moves on the straights, and write down
what you see — you will want it for Part 2.

## Part 2 — Proportional control

Set `CONTROLLER` to `CONTROLLER_PROPORTIONAL` at the top of the file.

First fill in `line_error()`, which turns the sensor byte into a single
number:

- give sensor `i` the position `(2 * i) - 7`, so the bar runs from -7 on the
  far left to +7 on the far right;
- average the positions of the sensors that are lit;
- negative means the line is off to the left, positive means off to the
  right, zero means centred;
- handle the case where **no** sensor is lit. Dividing by zero here will
  crash the robot, not just give a wrong answer.

Then fill in `follow_proportional()`:

```
correction = KP * error
left  wheel = BASE_SPEED + correction
right wheel = BASE_SPEED - correction
```

Check the signs by working through one case on paper before you flash it.
If the line is off to the right, which wheel has to speed up?

Then tune `KP`. Start at 3, change it one step at a time, and run the same
piece of track after every change. Note down what each value did.

## Rules

- Use `sensors_read()`. Do not call `GPIO_PinRead()` yourself.
- Use `motors_set()`. Do not call `HbridgeSpeed()` yourself.
- Keep the control loop in the starter — read, decide, act, wait.
- No `sensors_print()` inside the control loop. It is too slow and it will
  ruin your timing.
- Keep `BASE_SPEED` around 30 while you are getting it working.
- Do not modify `board/pin_mux.c`, `board/peripherals.c` or
  `board/clock_config.c`.
- Test with the drive wheels lifted before you put the robot on the track.

## You are done when

- [ ] You have checked your sensor readings by hand
- [ ] `follow_bang_bang()` gets the robot round the track
- [ ] `line_error()` returns a sensible number as you slide the robot across
      the line, and survives the line disappearing entirely
- [ ] `follow_proportional()` gets the robot round the track
- [ ] You have tried at least three values of `KP` and can say what each did
- [ ] You can explain why the proportional controller runs straighter than
      the bang-bang one
