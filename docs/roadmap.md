# Workshop roadmap

Four workshops, one task each. Workshops 1 and 2 are written up in full;
Workshops 3 and 4 are the plan the repository is built to grow into.

| # | Workshop | Task | Scored on |
|---|---|---|---|
| 1 | Installation + motor control | Forward, backward, rotate left, rotate right | 25% per movement, final stop required |
| 2 | Finite state machines | Design an FSM and drive a square | FSM structure, corners, closing the path |
| 3 | Sensor bar + line following | Complete the real track | Completion, no time limit |
| 4 | Speed and tuning | Complete the same track as fast as possible | Time, relative to the best run |

## Workshop 1 — motors

[docs/workshop1.md](workshop1.md) · [tasks/task1.md](../tasks/task1.md)

Two motors, signed commands, PWM, direction, differential drive. Deliberately
nothing else — no sensors, no state machines, no control theory.

## Workshop 2 — state machines

[docs/workshop2.md](workshop2.md) · [tasks/task2.md](../tasks/task2.md) ·
starter in [starters/workshop2/](../starters/workshop2/)

Introduce `enum State`, `switch (state)` and a 10 ms tick loop with a
per-state stopwatch, replacing the blocking `wait_ms()` chains of Workshop 1.

```
START -> EDGE -> TURN -> EDGE -> TURN -> ... -> DONE
```

Students design the machine themselves: one initial state, one state per
edge of the square, turn states between them, one final state. The diagram
comes before the code.

A marker on the robot draws its approximate square. Still fully open loop —
the robot has no idea where it actually is, and discovering that limitation
is the point. The tick loop is chosen so that Workshop 3 can swap the
"enough time has passed" transition condition for "the sensors saw
something" without restructuring anything.

## Workshop 3 — sensors and line following

Expose one new function:

```c
uint8_t sensors_read(void);
```

Then the chain:

```
sensor pattern -> line position -> error -> motor correction
```

Start with bang-bang or plain proportional control. The task is only to
finish the real course; speed does not matter.

The primer API should normalise polarity so students always see
`1 = sensor sees the line`, regardless of what the LM339 comparators output.
See [hardware.md](hardware.md).

Adds: `tasks/task3.md`, `docs/workshop3.md`.

## Workshop 4 — speed and tuning

Same track, now scored by time. Introduces:

- PD control on top of the Workshop 3 P controller;
- different speeds for straights and turns;
- acceleration on straights, slowdown into turns;
- hysteresis on hard turns;
- lost-line recovery;
- motor start compensation.

A tested controller of this kind already exists on the physical robot, running
its control loop at about 5 ms (200 Hz). It is a reference for Workshop 4 and
proof that this small API scales — it is deliberately **not** the Workshop 1
starter.

Adds: `tasks/task4.md`, `docs/workshop4.md`.

## What stays out of Workshop 1

No sensor reads, no calibration, no state machines, no weighted line position,
no P/PID/PD, no lost-line recovery, no intersections, no speed scheduling, no
hard-turn logic. Students need exactly: two motors, signed commands, PWM,
direction, differential drive.
