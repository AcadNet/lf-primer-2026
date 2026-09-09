# Workshop 1 — Controlling the motors

[← README](../README.md) · [Task 1 →](../tasks/task1.md) · [Workshop 2 →](workshop2.md)

## Goal

By the end of this workshop you should be able to:

- build the project and flash it onto the board;
- understand what the two motor command values mean;
- move forward and backward;
- rotate left and right on the spot;
- stop safely.

Nothing about sensors, state machines or line following yet.

---

## The three ideas you need

### 1. PWM — how we set "how hard"

The microcontroller does not send the motor a real speed value. It switches
the motor power on and off very quickly. The percentage of time the power is
ON is called the **duty cycle**. A larger duty cycle usually makes the motor
turn faster.

```
100%:  ████████████████████

 50%:  ████    ████    ████

 25%:  ██      ██      ██
```

On this robot the switching happens about 1000 times per second, far faster
than you can see or hear as movement.

So when you write `30`, you are saying *"drive this motor at 30% power"*.
You are **not** saying 30 RPM, and not 30 cm/s.

Real motors are not perfect. Two motors given the same command can turn
slightly differently. Notice it now — it is what Workshop 4 is about.

### 2. The H-bridge — how we set "which way"

The microcontroller pins cannot power a motor directly; they supply far too
little current. The **H-bridge** (a DRV8833 chip on the shield) sits between
them. It handles the motor current, it lets us choose the rotation
direction, and it passes the PWM through so we can control the power.

Battery → H-bridge → motor. Microcontroller → H-bridge → *instructions only*.

### 3. Differential drive — how we steer

The robot has no steering wheel. It turns by driving its two wheels
differently.

```
left = +30, right = +30   ->  forward
left = -30, right = -30   ->  backward
left = -30, right = +30   ->  rotate in place
left = +30, right = -30   ->  rotate in place, the other way
left = +30, right = +15   ->  a wide curve to the right
```

That's it. Every movement the robot will ever make in these workshops is one
pair of numbers.

---

## The API

Four functions, from `include/robot.h`:

```c
void robot_init(void);
void motors_set(int left, int right);
void motors_stop(void);
void wait_ms(uint32_t ms);
```

`motors_set()` takes signed values from `-100` to `+100`:

| Code | Result |
|---|---|
| `motors_set(30, 30)` | move forward |
| `motors_set(-30, -30)` | move backward |
| `motors_set(-30, 30)` | rotate in place |
| `motors_set(30, -30)` | rotate the other way |
| `motors_set(0, 0)` | stop |

Values outside `-100..100` are clamped for you. `robot_init()` always leaves
the motors stopped, so the robot never drives off the table the moment you
flash it.

---

## Step 1 — Start stopped

Build and flash this. Nothing should move.

```c
#include "robot.h"

int main(void)
{
    robot_init();
    motors_stop();

    while (1) {
    }
}
```

If a wheel twitches or creeps, stop and ask the instructor before going on.

## Step 2 — Move forward

**Lift the drive wheels off the table for this one.**

```c
robot_init();
motors_stop();

motors_set(30, 30);
wait_ms(1000);
motors_stop();
```

Before you press run: which way will the wheels turn, and for how long?

## Step 3 — Reverse

```c
motors_set(-30, -30);
wait_ms(1000);
motors_stop();
```

Same magnitude, opposite sign. The sign controls direction; the number
controls power.

## Step 4 — Rotate

Try both:

```c
motors_set(-30, 30);
wait_ms(1000);
motors_stop();
```

```c
motors_set(30, -30);
wait_ms(1000);
motors_stop();
```

One wheel goes forward, one goes backward, so the robot spins around its own
centre instead of driving anywhere. That is differential drive.

Then try `motors_set(30, 15)` and watch it curve instead of spin.

## Step 5 — Task 1

Now do [Task 1](../tasks/task1.md).

---

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| Nothing moves at all | Battery switch off, or battery unplugged. The USB cable powers the board but **not** the motors. |
| Motors hum but don't turn | Command value too low. Motors often need 20+ to actually start. |
| Only one wheel turns | Motor screw terminal loose, or one command is 0. |
| Robot turns when you asked for forward | Normal — motors differ. Do not "fix" it in your code yet; that's Workshop 4. |
| Both wheels turn backwards | The left/right mapping in `source/robot.c` needs checking. Tell the instructor. |
| Build fails on `fsl_ctimer.h` | The MCXN947 SDK is not installed in MCUXpresso IDE. |

---

## "How does it actually work?" (optional)

You do not need any of this to finish Task 1.

`motors_set()` maps your semantic *left*/*right* onto the two physical motor
channels, then calls the project-level wrapper `HbridgeSpeed()`. That in turn
calls two NXP SDK functions:

- `GPIO_PinWrite()` — sets the direction pin of each motor;
- `CTIMER_UpdatePwmDutycycle()` — sets the PWM duty cycle of each motor.

The layering:

```
your code
    |
    v
motors_set(left, right)       <- primer / student-facing API   (robot.h)
    |
    v
HbridgeSpeed(...)             <- project-level wrapper         (hbridge.c)
    |
    +--> GPIO_PinWrite(...)                <- NXP SDK
    +--> CTIMER_UpdatePwmDutycycle(...)    <- NXP SDK
    |
    v
DRV8833
    |
    v
motors
```

`GPIO_PinWrite()` and `CTIMER_UpdatePwmDutycycle()` are NXP SDK functions.
`HbridgeInit()` and `HbridgeSpeed()` are **not** — they are this project's own
code. `motors_set()` is this primer's own code.

Hardware details are in [docs/hardware.md](hardware.md).
