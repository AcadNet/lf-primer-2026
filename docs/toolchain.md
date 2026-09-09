# Toolchain — install, build, flash

Condensed from the upstream
[programming guide](https://gitfront.io/r/cristiannxp/xHvo63TFm7h4/NXP-Linefollower/),
which has the full version with screenshots for every dialog.

## 1. Check the board works

Connect a USB-C cable from connector **J17** (MCU-Link) on the FRDM-MCXN947
to your computer. The board ships with a blinky demo, so the onboard RGB LED
should start blinking. If it does, the board is fine.

## 2. Install the tools

You need an NXP account first (personal or school email is fine) —
<https://www.nxp.com/mynxp/home>.

1. **MCUXpresso IDE** — <https://www.nxp.com/mcuxpresso/ide>.
   Pick the installer for your OS. Accept the driver installation prompts.
2. **The MCXN947 SDK** — launch the IDE, and on the Welcome page click
   *Download and Install SDKs*. Search for `mcxn947` and install it.
3. **MCUXpresso Config Tools** — only needed if you want to change pins,
   clocks or peripherals. Not needed for Workshops 1 and 2.

## 3. Optional sanity check: LED blinky

Import an SDK example (`frdmmcxn947` → `demo_apps` → `led_blinky`), build it,
and flash it. It confirms your IDE, SDK and debug probe all work before you
add the robot into the mix. Skip it if the class is short on time.

## 4. Import the line follower project

The pre-configured MCUXpresso project lives in the upstream repository at
`code/NXP_line_follower.zip`
([download from upstream](https://gitfront.io/r/cristiannxp/xHvo63TFm7h4/NXP-Linefollower/)).
It already has the pins, the CTIMER PWM and the clocks configured — you do
not have to build an SDK workspace by hand.

1. **File → Import…**
2. Expand **General**, choose **Existing Projects into Workspace**, **Next**.
3. Select **Select archive file**, browse to `NXP_line_follower.zip`,
   **Finish**.

## 5. Add the primer files

Copy the four files from this repository into the imported project:

| From this repo | Into the project |
|---|---|
| `include/robot.h` | `include/` |
| `include/hbridge.h` | `include/` (replaces the existing one) |
| `source/robot.c` | `source/` |
| `source/hbridge.c` | `source/` (replaces the existing one) |
| `source/main.c` | `source/` (replaces the existing one) |

Both `hbridge` files are drop-in replacements — same API, same behaviour on
the motors, with the duty-cycle rule documented and an unused local removed.
Keeping the upstream ones works too; you only lose the comments.

Then refresh the project in the IDE (**F5**) so it picks up the new files.

## 6. Build

Select the project in Project Explorer and press **Build** (or use the
Quickstart Panel). It should compile with no errors.

If it fails on `fsl_ctimer.h` or similar, the MCXN947 SDK is not installed —
go back to step 2.

## 7. Flash and run

1. Connect the board via the **J17 MCU-Link** port.
2. Click **Debug**.
3. Choose **MCU-Link CMSIS-DAP** as the probe.
4. Click **Run**.

Turn the battery on only once the code is flashed, and keep the drive wheels
off the table the first time.

## Serial output (optional)

The project prints over the debug console at the usual SDK settings. In
MCUXpresso IDE the semihosting console picks it up automatically; over a
terminal, open the MCU-Link virtual COM port. Useful for Workshop 3 when you
want to see the sensor byte.
