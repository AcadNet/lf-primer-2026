# Workshop 3 — Sensors and line following

[← Workshop 2](workshop2.md) · [Task 3 →](../tasks/task3.md)

## Goal

By the end of this workshop you should be able to:

- read the eight infrared sensors and understand what the byte means;
- write a **bang-bang** controller and see exactly why it wobbles;
- turn the sensor pattern into a single number — the error;
- write a **proportional** controller and tune it;
- get the robot round the real track.

Until now the robot has been driving blind. It followed a script and hoped.
Now it gets to look where it is going, and everything changes: instead of
*"drive forward for 1500 ms"* you can finally say *"drive forward until
something tells me not to"*.

---

## The sensor bar

Eight infrared sensors sit in a row across the front of the robot. Each one
shines a little invisible light at the floor and measures how much bounces
back. A white floor bounces a lot. A black line bounces almost none. A
comparator chip on the shield turns that difference into a plain yes-or-no.

So each sensor answers one question: **am I over the line right now?**

One new function gives you all eight answers at once:

```c
uint8_t sensors_read(void);
```

It returns one byte, one bit per sensor:

```
    bit 0  bit 1  bit 2  bit 3  bit 4  bit 5  bit 6  bit 7
    left <------------------------------------------> right

    1 = this sensor is over the line
    0 = this sensor is over the background
```

Bit 0 is always the leftmost sensor, and 1 always means "line", no matter
how the bar happens to be wired. That is sorted out for you inside
`robot.c`.

Reading one bit:

```c
uint8_t s = sensors_read();

if (s & (1 << 3)) {
    /* sensor 3 is over the line */
}
```

`(1 << 3)` is "a byte with only bit 3 set", and `&` keeps only the bits that
are set in both. So the `if` is true exactly when sensor 3 sees the line.

### What the patterns look like

The line is wider than one sensor, so normally two or three light up
together, and the group slides left and right as the robot drifts:

```
   line under the middle          0 0 0 1 1 0 0 0     you are centred
   robot drifted right            1 1 0 0 0 0 0 0     line is off to the LEFT
   robot drifted left             0 0 0 0 0 0 1 1     line is off to the RIGHT
   sharp corner                   0 0 0 0 1 1 1 1
   lifted off the track           0 0 0 0 0 0 0 0     nothing sees anything
```

Read the second line again, because it catches everybody: if the line
appears under the **left** sensors, the robot has wandered off to the
**right**. The sensors tell you where the line is relative to the robot, so
to get back you steer *towards* the lit sensors.

---

## Step 1 — Look before you code

Do not write a controller yet. Find out what your sensors actually say.

```c
#include "robot.h"

int main(void)
{
    robot_init();
    motors_stop();

    while (1)
    {
        sensors_print();
        wait_ms(200);
    }
}
```

`sensors_print()` writes the byte to the debug console with the **leftmost
sensor first**, so what you read on screen is laid out the same way as the
bar on the robot:

```
S = 00011000
```

Flash it, open the console, and move the robot by hand:

1. Hold it well away from the line. Everything should read `0`.
2. Put the line under the middle. The middle digits should light up.
3. Slide it slowly left, then right, and watch the group of `1`s travel
   across the display in the same direction as the line moves.

If all eight read `1` when the robot is off the track, or the digits travel
the wrong way, stop and tell the instructor — there are two switches at the
top of `source/robot.c` that fix exactly those two problems, and they need
setting once for your robot.

Spend a few minutes here. Every bug in the rest of this workshop is easier
to find when you already know what a good reading looks like.

---

## Bang-bang control

The simplest possible controller. Look at the sensors, pick one of a few
fixed responses, repeat. No in-between.

The rule in words: *if the line is in the middle, go straight; if it is off
to the left, steer left; if it is off to the right, steer right.*

Split the bar into three groups:

```c
#define LEFT_GROUP    0x07   /* 0b00000111 — sensors 0,1,2   */
#define CENTRE_GROUP  0x18   /* 0b00011000 — sensors 3,4     */
#define RIGHT_GROUP   0xE0   /* 0b11100000 — sensors 5,6,7   */
```

and turn the rule straight into code:

```c
#define BASE_SPEED  30
#define SLOW_SPEED  10

uint8_t s = sensors_read();

if (s & CENTRE_GROUP) {
    motors_set(BASE_SPEED, BASE_SPEED);        /* straight on */
} else if (s & LEFT_GROUP) {
    motors_set(SLOW_SPEED, BASE_SPEED);        /* steer left  */
} else if (s & RIGHT_GROUP) {
    motors_set(BASE_SPEED, SLOW_SPEED);        /* steer right */
} else {
    /* nothing sees the line — decide what to do */
}
```

Check the middle group **first**. On a gentle curve the line often covers a
centre sensor and a side one at the same time, and if you test the sides
first the robot twitches away from a line it was already following
perfectly.

Steering left means slowing the left wheel and letting the right one push
the robot round. If yours turns the wrong way, swap the two arguments —
and check Task 1 again, because it means your left/right are crossed.

### Run it, and watch it wobble

It works. It also weaves down the straights like it can't quite make up its
mind. That is not a bug in your code — it is what bang-bang control *is*.

The controller only has three answers. It cannot tell "barely off centre"
from "almost falling off the track", so it applies the same hard correction
to both. It over-steers, shoots past the middle, over-steers back, and
repeats forever. Turn `SLOW_SPEED` down to make the corrections harder and
the wobble gets *worse*, not better.

That is the whole argument for the next section: to steer gently when the
robot is nearly right and hard when it is badly off, the controller needs
to know **how far** off it is. One bit per sensor cannot say that. A number
can.

---

## From a pattern to a number

Give every sensor a position, running from -7 on the far left to +7 on the
far right, with 0 in the middle where there is no sensor:

```
   sensor    0    1    2    3    4    5    6    7
   position -7   -5   -3   -1   +1   +3   +5   +7
                            \    /
                          the middle is 0
```

Now average the positions of whichever sensors are lit. Two middle sensors
on: `(-1 + 1) / 2 = 0`, dead centre. The two leftmost: `(-7 + -5) / 2 = -6`,
a long way off to the left.

That average is the **error** — how far the line is from where you want it,
and which side:

```c
static int last_error = 0;

static int line_error(void)
{
    uint8_t s = sensors_read();
    int sum = 0;
    int count = 0;
    int i;

    for (i = 0; i < 8; i++)
    {
        if (s & (1 << i))
        {
            sum += (2 * i) - 7;    /* -7, -5, -3, -1, +1, +3, +5, +7 */
            count++;
        }
    }

    if (count == 0)
    {
        /* Line lost. Keep the last error so the robot carries on turning
         * the way it was — it is probably mid-corner. */
        return last_error;
    }

    last_error = sum / count;
    return last_error;
}
```

Read the result like this:

```
    error < 0    line is to the LEFT   -> robot must turn left
    error = 0    centred               -> straight on
    error > 0    line is to the RIGHT  -> robot must turn right
```

`(2 * i) - 7` is just the table above written as arithmetic — put `i = 0`
in and you get -7, put `i = 7` in and you get +7.

The `count == 0` check matters. Without it you divide by zero the first
time the robot leaves the track, and dividing by zero on a microcontroller
does not politely stop your program. Returning the last error is a cheap
trick, not a real fix — a robot that properly hunts for a line it has lost
is Workshop 4.

---

## Proportional control

Now the payoff. Steer by an amount **proportional** to the error: a little
bit off centre, a little correction; a long way off, a big one.

```c
correction = KP * error;

left  = BASE_SPEED + correction;
right = BASE_SPEED - correction;
```

`KP` is a number you choose. That is the entire controller — three lines.

Check the signs by trying a case. Say the line is off to the right, so
`error = +6`, and `KP = 3`, so `correction = +18`. The left wheel gets
`30 + 18 = 48` and the right gets `30 - 18 = 12`. The left wheel is now much
faster, so the robot swings to the right — towards the line. Correct.

Notice what happens as it comes back: as the line slides towards the middle,
the error shrinks on its own, so the correction shrinks with it, and the
robot eases into the centre instead of slamming past it. That is the
difference from bang-bang, and it is the whole idea.

In full:

```c
#define BASE_SPEED  30
#define KP           3

int error      = line_error();
int correction = KP * error;

motors_set(BASE_SPEED + correction, BASE_SPEED - correction);
```

With `BASE_SPEED = 30` and `KP = 3`, a full-scale error of ±7 gives ±21, so
a wheel can be asked for 51 or 9. If a big error pushes a value past ±100,
`motors_set()` clamps it for you — you do not have to check.

### Tuning KP

Start at `KP = 3` and change it one step at a time. There is no correct
value; it depends on your motors, your battery and your track.

| What the robot does | What it means | What to do |
|---|---|---|
| Drifts wide, leaves the line on corners | Corrections too weak | Raise `KP` |
| Shakes rapidly, buzzes, jitters on straights | Corrections too strong | Lower `KP` |
| Smooth on straights, wide on corners only | Going too fast for the corner | Lower `BASE_SPEED` |
| Sluggish and slow to react | `KP` too low, or loop too slow | Raise `KP`, check `TICK_MS` |

Change one thing at a time and run the same piece of track after each
change. Changing `KP` and `BASE_SPEED` together tells you nothing about
either.

---

## The control loop

Same shape as Workshop 2 — read, decide, act, wait — but faster, because
now the robot is reacting to the world rather than to a stopwatch:

```c
#define TICK_MS 5     /* 200 times a second */

while (1)
{
    /* read the sensors, work out the error, set the motors */

    wait_ms(TICK_MS);
}
```

A slower loop means the robot travels further between decisions, and by the
time it notices the line has moved it is already too late to steer gently.
Line following at 10 times a second looks drunk; at 200 it looks deliberate.

Do not call `sensors_print()` inside this loop. Printing is far slower than
everything else put together and will wreck your timing. Use it in a
separate test program, like in Step 1.

---

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| Robot drives off immediately, ignoring the line | Sensors reading all `0` or all `1`. Go back to Step 1. |
| It steers away from the line, every time | Left and right swapped somewhere — the `motors_set()` arguments, or the sign of your correction. |
| Works on one side of the track, not the other | Sensor bar mounted crooked, or one motor is weaker. |
| Weaves badly on straights | Bang-bang: expected. Proportional: `KP` too high. |
| Loses the line on corners only | `BASE_SPEED` too high, or `KP` too low. |
| Program freezes when the robot leaves the track | Dividing by zero — you are missing the `count == 0` check. |
| Fine on the desk, hopeless on the floor | Sensor height. The bar wants to sit 3–5 mm above the surface. |
| Everything got worse and you changed nothing | Flat battery. Check it before you change any numbers. |
