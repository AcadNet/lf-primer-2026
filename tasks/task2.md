# Task 2 — Drive a square

[← Workshop 2](../docs/workshop2.md)

## Goal

Make the robot drive a square all by itself, and stop when it is done.

Four straight edges, three or four turns, back roughly where it started.
No sensors, no help from you once it is running.

You must build it as a **finite state machine**, and you must **draw the
state diagram before you write the code**.

## Part 1 — Design the machine (on paper)

Draw the state diagram. Show the instructor before you type anything.

Your diagram must have:

- **exactly one initial state** — the robot sits still here, motors stopped,
  long enough for you to put it down and step back;
- **one state per edge of the square** — four of them, so four separate
  states, not one state used four times;
- **states for the turns** between the edges;
- **exactly one final state** — motors stopped, and no arrow leaving it.

Every bubble needs the action it performs. Every arrow needs the condition
that triggers it.

Answer these before you start coding:

1. How many states does your machine have in total? Count them.
2. Do you need three turns or four? What is the difference in where the
   robot ends up? Both are acceptable — say which you chose and why.
3. Which state is entered exactly once and never returned to?

## Part 2 — Code it

Start from [`starters/workshop2/main.c`](../starters/workshop2/main.c).
Copy it over `source/main.c` in your MCUXpresso project, then fill in the
`TODO`s.

### Rules

- Use the tick-loop structure from the starter: one `switch (state)`, one
  `case` per state, `go_to()` for every transition.
- **No chains of `wait_ms()` to sequence the moves.** `wait_ms(TICK_MS)`
  at the bottom of the loop is the only one you need. If your program still
  reads like a list of instructions, it is not a state machine yet.
- One `enum` value per bubble in your diagram. The code and the drawing must
  match — if you change one, change the other.
- Keep motor values around `30`.
- Do not modify `board/pin_mux.c`, `board/peripherals.c` or
  `board/clock_config.c`.
- Test with the drive wheels lifted first, then put it on the floor.

### Calibrate

Your two numbers are how long an edge takes and how long a 90-degree turn
takes. Measure them, don't guess them — see
[Calibrating your timings](../docs/workshop2.md#calibrating-your-timings).

Put them in one place at the top of the file so you can adjust them without
hunting through the code:

```c
#define EDGE_MS  1500
#define TURN_MS   600
```

## Success criteria

- [ ] A state diagram exists on paper, and the code matches it
- [ ] Exactly one initial state, and the robot waits there before starting
- [ ] Four separate edge states
- [ ] Turn states between the edges
- [ ] Exactly one final state, with no transition out of it
- [ ] The path is recognisably a square
- [ ] The robot ends near where it started
- [ ] The robot stops at the end and stays stopped

## Scoring

```
30%   the FSM structure is correct — states, transitions, one initial,
      one final, and no wait_ms() sequencing
40%   four corners, roughly 90 degrees each (10% per corner)
20%   the path closes: the robot finishes near its starting point
10%   it stops cleanly at the end and stays stopped
```

The square does **not** have to be neat. Open-loop drift is expected and is
not penalised — a wonky but clearly square-ish path with a correct machine
behind it scores better than a lucky-looking path built out of `wait_ms()`
calls.

## Bonus

**A. Restore the heading.** If you used three turns, the robot finishes the
square pointing a quarter-turn away from how it started. Add the fourth turn
so it ends facing its original direction. Does that change how many states
you need?

**B. Add pause states.** Put a short state between every edge and every turn
where the motors are stopped. Run it again — the square should come out
noticeably better. Explain to the instructor why.

**C. Shrink the machine.** Four edge states that differ only in which state
comes next is a lot of copy-paste. Rewrite it with one `STATE_EDGE`, one
`STATE_TURN`, and a counter:

```c
static int edges_done;
```

You should end up with about four states instead of nine. Keep your old
version — you will be asked to say which one is easier to read, and which
one you would rather extend.

**D. Draw it.** Tape a marker to the robot and let it draw its own square on
a big sheet of paper. Run it three times without recharging. Compare the
three squares, and explain what you see.
