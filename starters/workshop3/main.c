/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Sabin Andrei
 *
 * Workshop 3 starter — line following.
 *
 * Copy this file over source/main.c in your MCUXpresso project.
 *
 * The control loop is done for you. Writing the controllers is your job:
 * bang-bang first, then proportional.
 *
 * Read docs/workshop3.md first, and check your sensors by hand before you
 * write anything — see Step 1 in that document.
 *
 * Task: tasks/task3.md
 */

#include "robot.h"

/* ------------------------------------------------------------------
 * Which controller to run.
 *
 * Start on CONTROLLER_BANG_BANG for Part 1. Change this line to
 * CONTROLLER_PROPORTIONAL for Part 2.
 * ------------------------------------------------------------------ */

#define CONTROLLER_BANG_BANG     0
#define CONTROLLER_PROPORTIONAL  1

#define CONTROLLER  CONTROLLER_BANG_BANG

/* ------------------------------------------------------------------
 * Numbers to tune. Change these, not the code below them.
 * ------------------------------------------------------------------ */

#define TICK_MS      5     /* control loop runs 200 times a second */
#define START_DELAY_MS 2000 /* time to put the robot down and step back */

#define BASE_SPEED  30     /* how fast the robot travels when centred */
#define SLOW_SPEED  10     /* the slowed wheel, bang-bang only        */
#define KP           3     /* correction strength, proportional only  */

/* ------------------------------------------------------------------
 * Sensor groups, for the bang-bang controller.
 *
 * Bit 0 is the leftmost sensor, bit 7 the rightmost.
 * ------------------------------------------------------------------ */

#define LEFT_GROUP    0x07   /* 0b00000111 — sensors 0, 1, 2 */
#define CENTRE_GROUP  0x18   /* 0b00011000 — sensors 3, 4    */
#define RIGHT_GROUP   0xE0   /* 0b11100000 — sensors 5, 6, 7 */

/* ------------------------------------------------------------------
 * Part 1 — bang-bang control
 *
 * Look at the sensors, pick one of a few fixed responses.
 * ------------------------------------------------------------------ */

static void follow_bang_bang(void)
{
    uint8_t s = sensors_read();

    (void)s;   /* delete this line once you use s below */

    /*
     * TODO:
     *
     *   if the CENTRE group sees the line   -> drive straight
     *   else if the LEFT group sees it      -> steer left
     *   else if the RIGHT group sees it     -> steer right
     *   else                                -> nothing sees the line.
     *                                          Your call. Be ready to say why.
     *
     * Test the centre group first, and use motors_set() for each case.
     *
     * Steering left means slowing the LEFT wheel:
     *
     *     motors_set(SLOW_SPEED, BASE_SPEED);
     */

    motors_stop();   /* replace this with your controller */
}

/* ------------------------------------------------------------------
 * Part 2 — proportional control
 * ------------------------------------------------------------------ */

/* Remembers the last known error, for when the line disappears. */
static int last_error = 0;

/*
 * Turns the sensor byte into one number:
 *
 *     negative -> the line is off to the LEFT
 *     zero     -> centred
 *     positive -> the line is off to the RIGHT
 */
static int line_error(void)
{
    uint8_t s = sensors_read();

    (void)s;   /* delete this line once you use s below */

    /*
     * TODO:
     *
     *   - walk over the eight bits;
     *   - for every bit that is set, add its position, (2 * i) - 7,
     *     to a running total, and count it;
     *   - return the total divided by the count.
     *
     * If the count is zero, NOTHING sees the line. Do not divide — that
     * will crash the robot. Return last_error instead, so it carries on
     * turning the way it was.
     *
     * Remember to store the answer in last_error before returning it.
     */

    return last_error;   /* replace this */
}

static void follow_proportional(void)
{
    int error = line_error();

    (void)error;   /* delete this line once you use error below */

    /*
     * TODO:
     *
     *     correction = KP * error
     *     left  wheel = BASE_SPEED + correction
     *     right wheel = BASE_SPEED - correction
     *
     * Work through one case on paper first: if the line is off to the
     * right, error is positive — which wheel needs to speed up?
     *
     * You do not need to check for values beyond -100..100.
     * motors_set() clamps them for you.
     */

    motors_stop();   /* replace this with your controller */
}

/* ------------------------------------------------------------------ */

int main(void)
{
    robot_init();
    motors_stop();

    /* Sit still long enough to put the robot on the line and step back. */
    wait_ms(START_DELAY_MS);

    while (1)
    {
        if (CONTROLLER == CONTROLLER_BANG_BANG)
        {
            follow_bang_bang();
        }
        else
        {
            follow_proportional();
        }

        wait_ms(TICK_MS);
    }
}
