/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Sabin Andrei
 *
 * robot.h - the only header a Workshop 1 student needs.
 *
 * Everything about pins, timers and the H-bridge is hidden behind these
 * four functions. See docs/workshop1.md.
 */
#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>

/* Prepares the motors. Call this once, first thing in main().
 * When it returns, both motors are stopped. */
void robot_init(void);

/*
 * Signed motor commands in the range -100..100.
 *
 *   positive -> forward
 *   negative -> reverse
 *   zero     -> stop
 *
 * The absolute value is a power command (PWM), not an RPM and not a
 * physical speed. 30 means "drive the motor at 30% power".
 *
 * Values outside -100..100 are clamped.
 *
 *   motors_set( 30,  30);   forward
 *   motors_set(-30, -30);   backward
 *   motors_set(-30,  30);   rotate in place
 *   motors_set( 30, -30);   rotate the other way
 *   motors_set(  0,   0);   stop
 */
void motors_set(int left, int right);

/* Same as motors_set(0, 0). Both motors coast to a stop. */
void motors_stop(void);

/* Blocking delay, in milliseconds. */
void wait_ms(uint32_t ms);

#endif /* ROBOT_H */
