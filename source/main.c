/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Sabin Andrei
 *
 * Workshop 1 starter.
 *
 * Everything you need is in robot.h:
 *
 *     robot_init();
 *     motors_set(left, right);   // -100..100 each
 *     motors_stop();
 *     wait_ms(milliseconds);
 *
 * Task: tasks/task1.md
 */

#include "robot.h"

int main(void)
{
    robot_init();
    motors_stop();

    /*
     * Write your motor commands below.
     *
     * Example - drive forward for one second, then stop:
     *
     *     motors_set(30, 30);
     *     wait_ms(1000);
     *     motors_stop();
     */

    /* TODO: solve Task 1 */

    motors_stop();

    while (1)
    {
    }
}
