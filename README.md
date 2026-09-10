# Line Follower Primer — AcadNet Summer School

You are going to program a small two-wheeled robot.

First you will make its motors turn. Then you will make it run a sequence of
moves on its own using a state machine. Then you will give it eyes — an
infrared sensor bar — and teach it to follow a line. Finally you will make it
do all that as fast as it can.

Four workshops, one task each. This repository is where the starter code and
the tasks live.

**Start here:** [Workshop 1 — Controlling the motors](docs/workshop1.md)

---

## The robot

```
FRDM-MCXN947 board
+ NXPCUP shield
+ 2 DC motors
+ 2 DRV8833 motor drivers
+ 8-sensor infrared bar
+ 7.4 V LiPo battery
```

Full details, wiring and safety notes: [docs/hardware.md](docs/hardware.md).

The robot itself, the shield and the pre-configured MCUXpresso project come
from the **NXP Line Follower** project:

> <https://gitfront.io/r/cristiannxp/xHvo63TFm7h4/NXP-Linefollower/>

That is where you will find the bill of materials, the shield schematic, and
the photo-by-photo soldering and assembly guides. This repository is a primer
on top of it — it does not replace it.

## What you need to install

- **MCUXpresso IDE** (free, needs an NXP account)
- **the MCXN947 SDK**, installed from inside the IDE
- a **USB-C cable** into the board's J17 MCU-Link port
- the pre-configured project from the upstream repository

You do **not** need to assemble an SDK workspace with `west`, and you do not
need to touch the clock, pin or peripheral configuration.

Step by step: [docs/toolchain.md](docs/toolchain.md).

## Build and flash, short version

1. Import `code/NXP_line_follower.zip` from the upstream repository into
   MCUXpresso IDE (**File → Import → General → Existing Projects into
   Workspace → Select archive file**).
2. Copy this repo's `include/*.h` and `source/*.c` into the project.
3. Build.
4. Connect the board via the **J17 MCU-Link** USB port.
5. **Debug**, choose **MCU-Link CMSIS-DAP**, then **Run**.

Keep the drive wheels off the table the first time you flash motor code.
The USB cable powers the board; the motors need the battery switched on.

## The motor API

Four functions. That is the whole interface for Workshop 1.

```c
#include "robot.h"

robot_init();                 // call once, first; leaves motors stopped
motors_set(left, right);      // -100..100 each
motors_stop();                // both motors off
wait_ms(1000);                // wait, in milliseconds
```

| Code | Result |
|---|---|
| `motors_set(30, 30)` | move forward |
| `motors_set(-30, -30)` | move backward |
| `motors_set(-30, 30)` | rotate in place |
| `motors_set(30, -30)` | rotate the other way |
| `motors_set(0, 0)` | stop |

**Positive is forward, negative is reverse, zero is stop.** The number itself
is a power command — `30` means "drive the motor at 30% power". It is not
30 RPM and not a distance per second. Values beyond ±100 are clamped.

The robot has no steering wheel: it turns by driving the two wheels
differently. That is called **differential drive**, and it is the one idea
Workshop 1 is really about.

Under the hood `motors_set()` calls the project's `HbridgeSpeed()` wrapper,
which calls the NXP SDK's `GPIO_PinWrite()` and
`CTIMER_UpdatePwmDutycycle()`. You never call those yourself.

## The sensor API

From Workshop 3 onwards the robot can also see. Two more functions, and
that is the whole interface:

```c
uint8_t sensors_read(void);   // one bit per sensor, 1 = over the line
void    sensors_print(void);  // print the byte to the debug console
```

The eight infrared sensors come back as one byte, **bit 0 leftmost** through
**bit 7 rightmost**, and a bit is `1` whenever that sensor is over the line:

```
sensors_read()  ->  0b00011000     the line is under the middle two
```

Polarity and left-to-right order are normalised inside `robot.c`, so this
holds however the bar happens to be wired.

## Repository layout

```
include/robot.h            the four functions above — the student-facing API
include/hbridge.h          project-level H-bridge wrapper (not NXP SDK)
source/robot.c             maps LEFT/RIGHT onto the physical motors
source/hbridge.c           PWM + direction, documented DRV8833 truth table
source/main.c              Workshop 1 starter — this is the file you edit
starters/workshop2/main.c  Workshop 2 starter — copy it over source/main.c
starters/workshop3/main.c  Workshop 3 starter — copy it over source/main.c
tasks/task1.md             Task 1 — basic movement
tasks/task2.md             Task 2 — drive a square
tasks/task3.md             Task 3 — follow the line
docs/workshop1.md          Workshop 1 walkthrough
docs/workshop2.md          Workshop 2 walkthrough
docs/workshop3.md          Workshop 3 walkthrough
docs/hardware.md           board, shield, pins, sensors, safety
docs/toolchain.md          install, import, build, flash
docs/roadmap.md            what Workshop 4 will cover
```

## Workshops

| # | Workshop | Task |
|---|---|---|
| 1 | [Controlling the motors](docs/workshop1.md) | [Task 1](tasks/task1.md) — forward, backward, rotate both ways |
| 2 | [State machines](docs/workshop2.md) | [Task 2](tasks/task2.md) — design an FSM and drive a square |
| 3 | [Sensors and line following](docs/workshop3.md) | [Task 3](tasks/task3.md) — bang-bang and proportional control |
| 4 | Speed and tuning | Task 4 — complete it fastest |

See [docs/roadmap.md](docs/roadmap.md) for what is coming.

## Further reading

Official NXP resources, for when you want to go past the primer:

- [MCUXpresso SDK portal](https://mcuxpresso.nxp.com/)
- [MCUXpresso SDK core & drivers](https://github.com/nxp-mcuxpresso/mcuxsdk-core)
- [MCUXpresso SDK examples](https://github.com/nxp-mcuxpresso/mcuxsdk-examples)
- [MCUXpresso SDK manifests](https://github.com/nxp-mcuxpresso/mcuxsdk-manifests)
- [FRDM-MCXN947 board documentation](https://github.com/nxp-mcuxpresso/mcuxsdk-doc/blob/main/boards/MCX/frdmmcxn947/index.rst)
- [FRDM-MCXN947 product page](https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-MCXN947)
- [DRV8833 datasheet](https://www.ti.com/product/DRV8833) ·
  [LM339 datasheet](https://www.ti.com/product/LM339)
- [NXP Cup](https://www.nxp.com/nxpcup)

## Credits and licence

Hardware, shield, assembly documentation and the pre-configured MCUXpresso
project: the [NXP Line Follower](https://gitfront.io/r/cristiannxp/xHvo63TFm7h4/NXP-Linefollower/)
project by Cristian (NXP), documentation under CC BY-SA 4.0, firmware under
MIT.

This primer: Sabin Andrei. Code under MIT, documentation under CC BY-SA 4.0.
See [LICENSE](LICENSE) and [NOTICE](NOTICE).
