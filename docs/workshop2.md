# Workshop 2 — State machines

[← Workshop 1](workshop1.md) · [Task 2 →](../tasks/task2.md)

## Goal

By the end of this workshop you should be able to:

- explain what a state machine is, in your own words;
- draw a state diagram before writing any code;
- translate that diagram into an `enum` and a `switch`;
- make the robot run a sequence of moves on its own, with no help from you.

You still use only the four functions from Workshop 1. Nothing new gets
added to the robot — what changes is how you organise your program.

---

## The problem with Workshop 1 code

Your Task 1 solution probably looked like this:

```c
motors_set(30, 30);
wait_ms(1000);
motors_stop();
wait_ms(300);
motors_set(-30, -30);
wait_ms(1000);
motors_stop();
```

It works. But look at what it actually is: a straight list of instructions,
each one blocking the program until it finishes. While the robot is inside
`wait_ms(1000)`, your program can do **nothing else**. It cannot check a
sensor. It cannot react to anything. It cannot stop early.

That is fine for four moves in a row. It falls apart the moment the robot
needs to make a decision — which is Workshop 3.

So before we get there, we reorganise.

---

## What is a state machine?

A **state machine** is a way of describing behaviour as a small set of
situations, and the rules for moving between them.

Two ideas, that's all:

- A **state** is *what the robot is doing right now*. Driving forward.
  Turning. Waiting. Finished. The robot is in exactly **one** state at any
  moment.
- A **transition** is a rule: *when this happens, go to that state*. The
  "this happens" part is called the **event** — in this workshop, the event
  is always "enough time has passed".

Two states are special:

- the **initial state** — where the machine starts;
- the **final state** — where it ends and stays.

You have used state machines all your life without the name. A traffic
light has three states and moves between them on a timer. A vending machine
waits, takes money, dispenses, returns to waiting.

---

## Drawing one

Always draw the diagram before you write code. States are bubbles, arrows
are transitions, and every arrow gets a label saying *when* it is taken.

Here is a machine that drives forward, comes back, and stops:

```
        (start here)
             |
             v
      +--------------+
      |    START     |   motors stopped
      +--------------+
             |
             | after 2 s   <- gives you time to put the robot down
             v
      +--------------+
      |      GO      |   motors_set(30, 30)
      +--------------+
             |
             | after 1.5 s
             v
      +--------------+
      |    RETURN    |   motors_set(-30, -30)
      +--------------+
             |
             | after 1.5 s
             v
      +==============+
      ||    DONE    ||   motors stopped, forever
      +==============+
        (final state)
```

Read it out loud: *"Start with the motors stopped. After two seconds, go
forward. After a second and a half, reverse. After another second and a
half, stop and stay stopped."*

Notice each bubble carries an **action** (what the motors do while you are
in that state) and each arrow carries a **condition** (when you leave).
That is everything you need to write the code.

---

## Turning a diagram into code

### Step 1 — name the states

One `enum` value per bubble. Nothing else goes in here.

```c
typedef enum {
    STATE_START,
    STATE_GO,
    STATE_RETURN,
    STATE_DONE
} State;
```

### Step 2 — remember which state you are in, and for how long

```c
static State    state;
static uint32_t elapsed_ms;
```

`elapsed_ms` is the stopwatch. It counts how long you have been in the
current state, and it resets every time you switch. That reset is the whole
trick, so give it its own function and always use it:

```c
static void go_to(State next)
{
    state = next;
    elapsed_ms = 0;   /* restart the stopwatch */
}
```

### Step 3 — the tick loop

The robot wakes up every 10 ms, decides what to do, and goes back to sleep.

```c
#define TICK_MS 10

int main(void)
{
    robot_init();
    motors_stop();

    go_to(STATE_START);          /* the initial state */

    while (1)
    {
        switch (state)
        {
            /* ... one case per state, see below ... */
        }

        wait_ms(TICK_MS);
        elapsed_ms += TICK_MS;
    }
}
```

Every state gets one `case`, and every `case` does exactly two things:

1. **the action** — set the motors for this state;
2. **the transition check** — if the condition is met, `go_to()` the next
   state.

```c
case STATE_START:
    motors_stop();                        /* action */
    if (elapsed_ms >= 2000) {             /* transition */
        go_to(STATE_GO);
    }
    break;

case STATE_GO:
    motors_set(30, 30);
    if (elapsed_ms >= 1500) {
        go_to(STATE_RETURN);
    }
    break;

case STATE_RETURN:
    motors_set(-30, -30);
    if (elapsed_ms >= 1500) {
        go_to(STATE_DONE);
    }
    break;

case STATE_DONE:
    motors_stop();                        /* no transition out — final */
    break;
```

Compare that with the blocking version at the top of this page. Same
behaviour, but now the robot passes through the loop a hundred times a
second, and in Workshop 3 you will replace `elapsed_ms >= 1500` with
"a sensor saw the line" without changing anything else.

### Why call `motors_set()` every tick?

Because it costs nothing and it keeps each `case` self-contained: read one
case and you know exactly what the motors are doing in that state. Sending
the same command again does nothing new to the hardware.

---

## Open loop — and why your square will drift

Everything here is **open loop**. The robot has no idea where it is or which
way it is pointing. It is following a script and hoping.

`wait 600 ms while spinning` is not the same thing as `turn 90 degrees`. It
only comes out at 90 degrees if the battery, the floor, the wheels and both
motors behave exactly as they did when you measured it. Change any of those
and the angle changes.

So expect this:

- the square will not close perfectly;
- a slightly flatter battery makes every turn shallower;
- carpet and a desk give different results;
- running the same program twice gives two slightly different squares.

None of that is a bug in your code. It is the honest limitation of driving
blind, and it is exactly the reason Workshop 3 gives the robot sensors.

Get it close. Do not chase perfection with ever-finer numbers — you will
run out of patience before the robot runs out of drift.

---

## Calibrating your timings

Do this before you fight the code.

**Turns.** Put the robot on the floor, mark its starting heading (a strip of
tape helps), and run a single spin with one guess:

```c
motors_set(-30, 30);
wait_ms(600);
motors_stop();
```

Did it overshoot 90 degrees? Lower the number. Undershoot? Raise it. Five
minutes of this gets you within a few degrees, which is good enough.

**Edges.** Same method with `motors_set(30, 30)`. Pick a length that fits
your table or floor space — 30–40 cm is plenty. Write the number down.

**Speed.** Keep it around 30 for both. Faster turns are much harder to
repeat, because the wheels slip more and the robot keeps coasting after you
cut power.

**Tip:** adding a short stop between a straight and a turn makes the whole
run noticeably more repeatable, because the robot stops coasting before it
starts spinning. You can do it with a `wait_ms()` — or, better, as its own
state.

---

## Step-by-step

1. Read this page and make sure the there-and-back diagram makes sense.
2. Type in the there-and-back machine above and run it. Confirm you get
   forward, then back, then a stop.
3. Change `STATE_RETURN` into a turn (`motors_set(-30, 30)`) and watch what
   happens. You now have every piece the square needs.
4. Calibrate your turn and edge timings as described above.
5. Do [Task 2](../tasks/task2.md) — draw the square's diagram **first**,
   then write it.

---

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| The robot does the first move forever | You forgot `go_to()` in that case, or you set `state` directly and never reset `elapsed_ms`. |
| States fly past instantly | `elapsed_ms += TICK_MS` is missing from the loop, so the stopwatch never advances. |
| It skips a state entirely | Two `case` labels transition to the same target, or a missing `break` falls through into the next case. |
| Every timing is slightly long | `wait_ms(TICK_MS)` is not exact — it waits *at least* that long. Normal; calibrate around it. |
| Turns are inconsistent run to run | Battery charge, or the robot is coasting. Recharge, slow the turn down, add a stop before it. |
| It runs the square once then repeats | Your final state transitions back to the first one. A final state has no way out. |
