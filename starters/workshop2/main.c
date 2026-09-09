/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Sabin Andrei
 *
 * Workshop 2 starter — drive a square with a state machine.
 *
 * Copy this file over source/main.c in your MCUXpresso project.
 *
 * The boilerplate is done for you: the tick loop, the stopwatch, and
 * go_to(). Designing the machine is your job.
 *
 * Read docs/workshop2.md first, then draw your state diagram, THEN fill in
 * the TODOs below.
 *
 * Task: tasks/task2.md
 */

#include "robot.h"

/* ------------------------------------------------------------------
 * Timings — calibrate these on your own robot. See
 * docs/workshop2.md, "Calibrating your timings".
 * ------------------------------------------------------------------ */

#define TICK_MS         10     /* how often the machine wakes up          */
#define START_DELAY_MS  2000   /* time to put the robot down and step back */
#define EDGE_MS         1500   /* how long one side of the square takes   */
#define TURN_MS          600   /* how long a 90-degree spin takes         */

#define DRIVE_SPEED      30
#define TURN_SPEED       30

/* ------------------------------------------------------------------
 * The states
 *
 * One value per bubble in your state diagram.
 *
 * TODO: add your edge states and your turn states between these two.
 *       Task 2 requires one separate state per edge of the square.
 * ------------------------------------------------------------------ */

typedef enum {
    STATE_START,     /* initial state: sitting still, waiting to begin */

    /* TODO: your states go here */

    STATE_DONE       /* final state: stopped, and never leaves */
} State;

/* ------------------------------------------------------------------
 * Machine bookkeeping — you should not need to change this part.
 * ------------------------------------------------------------------ */

static State    state;
static uint32_t elapsed_ms;   /* how long we have been in `state` */

/* Switch to another state and restart the stopwatch.
 * Always use this. Never assign to `state` directly. */
static void go_to(State next)
{
    state = next;
    elapsed_ms = 0;
}

/* ------------------------------------------------------------------ */

int main(void)
{
    robot_init();
    motors_stop();

    go_to(STATE_START);

    while (1)
    {
        switch (state)
        {
        case STATE_START:
            motors_stop();
            if (elapsed_ms >= START_DELAY_MS)
            {
                /* TODO: go to your first edge state instead of STATE_DONE. */
                go_to(STATE_DONE);
            }
            break;

        /*
         * TODO: one `case` per state you added to the enum above.
         *
         * Every case does exactly two things:
         *
         *     1. the action    - set the motors for this state
         *     2. the transition - if the condition is met, go_to() the next
         *
         * An edge looks like this:
         *
         *     case STATE_EDGE_1:
         *         motors_set(DRIVE_SPEED, DRIVE_SPEED);
         *         if (elapsed_ms >= EDGE_MS) {
         *             go_to(STATE_TURN_1);
         *         }
         *         break;
         *
         * A turn uses opposite signs so the robot spins in place. Work out
         * which sign goes where — Task 1 already told you.
         *
         * Do not forget the `break;` at the end of every case.
         */

        case STATE_DONE:
            motors_stop();
            /* No transition out. This is the final state. */
            break;
        }

        wait_ms(TICK_MS);
        elapsed_ms += TICK_MS;
    }
}
