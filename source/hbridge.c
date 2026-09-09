/*
 * SPDX-License-Identifier: MIT
 *
 * hbridge.c - project-level H-bridge wrapper for the NXPCUP shield.
 *
 * Derived from the upstream NXP-Linefollower project:
 *   https://gitfront.io/r/cristiannxp/xHvo63TFm7h4/NXP-Linefollower/
 *
 * Drop-in replacement for the upstream source/hbridge.c. Behaviour on the
 * motors is unchanged; the differences are an unused local removed and the
 * duty-cycle rule written out explicitly (see below).
 *
 * ---------------------------------------------------------------------------
 * How one motor is wired
 * ---------------------------------------------------------------------------
 * Each motor uses one DRV8833 half (IN1 / IN2):
 *
 *     IN1 <- CTIMER0 PWM output   (P0_25 / CT0_MAT1, P0_26 / CT0_MAT2)
 *     IN2 <- plain GPIO level     (GPIO0.24, GPIO0.27)
 *
 * The DRV8833 truth table for that pair is:
 *
 *     IN1   IN2   result
 *     ----  ----  ---------------------------------
 *     0     0     coast (outputs high-impedance)
 *     PWM   0     forward, drive strength = duty          (fast decay)
 *     PWM   1     reverse, drive strength = 100 - duty    (slow decay)
 *     1     1     brake (both outputs shorted low)
 *
 * That is why the reverse duty is 100 - |speed| and not |speed|: with IN2
 * held HIGH the PWM is inverted by the bridge itself. A naive
 * "duty = |speed|" makes reverse run backwards in intensity - speed -100
 * would barely move and speed -1 would slam full reverse.
 *
 * The same table explains HbridgeBrake(): direction HIGH plus duty 100 puts
 * both inputs HIGH, which is the brake row, not full reverse.
 *
 * If you ever rewire the shield so both motor inputs are PWM-capable (sign-
 * magnitude drive), set HBRIDGE_REVERSE_SLOW_DECAY to 0 and re-test with the
 * wheels lifted.
 * ---------------------------------------------------------------------------
 */

#include "hbridge.h"

#include "fsl_ctimer.h"
#include "fsl_gpio.h"

#ifndef HBRIDGE_REVERSE_SLOW_DECAY
#define HBRIDGE_REVERSE_SLOW_DECAY 1
#endif

Hbridge g_hbridge;

static int16_t clamp100(int16_t v)
{
    if (v >  100) return  100;
    if (v < -100) return -100;
    return v;
}

static uint8_t duty_for(int16_t speed)
{
    if (speed >= 0)
    {
        return (uint8_t)speed;
    }

#if HBRIDGE_REVERSE_SLOW_DECAY
    /* IN2 is HIGH, so the bridge inverts the PWM: ask for the complement. */
    return (uint8_t)(100 + speed);   /* speed is negative: 100 - |speed| */
#else
    return (uint8_t)(-speed);
#endif
}

void HbridgeInit(Hbridge *h,
                 CTIMER_Type *pwmPeriph,
                 ctimer_match_t periodCh,
                 ctimer_match_t pwm1Ch,
                 ctimer_match_t pwm2Ch,
                 GPIO_Type *m1DirPort, uint32_t m1DirPin,
                 GPIO_Type *m2DirPort, uint32_t m2DirPin)
{
    h->pwmPeripheral = pwmPeriph;
    h->periodChannel = periodCh;
    h->pwm1Channel   = pwm1Ch;
    h->pwm2Channel   = pwm2Ch;
    h->motor1DirPort = m1DirPort;
    h->motor1DirPin  = m1DirPin;
    h->motor2DirPort = m2DirPort;
    h->motor2DirPin  = m2DirPin;

    /* The CTIMER itself is started by BOARD_InitBootPeripherals(). Leave the
     * motors stopped: both inputs LOW is the coast row of the truth table. */
    HbridgeSpeed(h, 0, 0);
}

void HbridgeSpeed(Hbridge *h, int16_t speed1, int16_t speed2)
{
    speed1 = clamp100(speed1);
    speed2 = clamp100(speed2);

    GPIO_PinWrite(h->motor1DirPort, h->motor1DirPin, (speed1 < 0) ? 1U : 0U);
    GPIO_PinWrite(h->motor2DirPort, h->motor2DirPin, (speed2 < 0) ? 1U : 0U);

    CTIMER_UpdatePwmDutycycle(h->pwmPeripheral, h->periodChannel,
                              h->pwm1Channel, duty_for(speed1));
    CTIMER_UpdatePwmDutycycle(h->pwmPeripheral, h->periodChannel,
                              h->pwm2Channel, duty_for(speed2));
}

void HbridgeBrake(Hbridge *h)
{
    GPIO_PinWrite(h->motor1DirPort, h->motor1DirPin, 1U);
    GPIO_PinWrite(h->motor2DirPort, h->motor2DirPin, 1U);

    CTIMER_UpdatePwmDutycycle(h->pwmPeripheral, h->periodChannel,
                              h->pwm1Channel, 100U);
    CTIMER_UpdatePwmDutycycle(h->pwmPeripheral, h->periodChannel,
                              h->pwm2Channel, 100U);
}
