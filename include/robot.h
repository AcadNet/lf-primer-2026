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

/* ------------------------------------------------------------------
 * Sensors (Workshop 3)
 * ------------------------------------------------------------------ */

/*
 * Reads the eight infrared sensors across the front of the robot and
 * returns them as one byte, one bit per sensor:
 *
 *     bit 0  bit 1  bit 2  bit 3  bit 4  bit 5  bit 6  bit 7
 *     left <------------------------------------------> right
 *
 *     1 = this sensor is over the line
 *     0 = this sensor is over the background
 *
 * The bit is always 1 over the line, whichever way the comparators on the
 * shield happen to be wired, and bit 0 is always the leftmost sensor. Both
 * of those are sorted out inside robot.c so you never have to think about
 * them.
 *
 * Example: the line is under the two middle sensors
 *
 *     sensors_read()  ->  0b00011000
 *                              ^^
 *                              bits 3 and 4
 */
uint8_t sensors_read(void);

/*
 * Prints the current sensor byte to the debug console, leftmost sensor
 * first, so what you read on screen matches what you see on the robot:
 *
 *     S = 00011000
 *
 * Handy while you are working out what the sensors are doing. Do not call
 * it on every pass of a fast control loop — printing is slow.
 */
void sensors_print(void);

#endif /* ROBOT_H */
