/*
 * SPDX-License-Identifier: MIT
 *
 * hbridge.h - project-level H-bridge wrapper for the NXPCUP shield.
 *
 * Derived from the upstream NXP-Linefollower project:
 *   https://gitfront.io/r/cristiannxp/xHvo63TFm7h4/NXP-Linefollower/
 *
 * This is NOT part of the NXP SDK. It sits on top of it.
 * Students should use robot.h instead; see docs/workshop1.md.
 */
#ifndef HBRIDGE_H
#define HBRIDGE_H

#include "fsl_ctimer.h"
#include "fsl_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    CTIMER_Type   *pwmPeripheral;
    ctimer_match_t periodChannel;
    ctimer_match_t pwm1Channel;
    ctimer_match_t pwm2Channel;
    GPIO_Type     *motor1DirPort;
    uint32_t       motor1DirPin;
    GPIO_Type     *motor2DirPort;
    uint32_t       motor2DirPin;
} Hbridge;

extern Hbridge g_hbridge;

void HbridgeInit(Hbridge *h,
                 CTIMER_Type *pwmPeriph,
                 ctimer_match_t periodCh,
                 ctimer_match_t pwm1Ch,
                 ctimer_match_t pwm2Ch,
                 GPIO_Type *m1DirPort, uint32_t m1DirPin,
                 GPIO_Type *m2DirPort, uint32_t m2DirPin);

/* speed1/speed2 in -100..100. Values outside the range are clamped. */
void HbridgeSpeed(Hbridge *h, int16_t speed1, int16_t speed2);

/* Active brake: shorts both motor windings (IN1 = IN2 = HIGH). */
void HbridgeBrake(Hbridge *h);

#ifdef __cplusplus
}
#endif

#endif /* HBRIDGE_H */
