/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Project
 *
 * Description: Timer for MSP432
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: Heather McLean
*******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"
#include "msp.h"
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <timer_a.h>

void TA0_0_ISR_Handler(void);
volatile bool TimerA0_CCR0_interrupt_hit = 0;
//#define TimerA0_period 4096;

#define main_TIMER_MODULE        (TIMER_A0_BASE)
#define main_TIMER_CLOCK         (TIMER_A_CLOCKSOURCE_SMCLK)
#define main_TIMER_CLOCKDIVIDER      (TIMER_A_CLOCKSOURCE_DIVIDER_1)
#define main_TIMER_PERIOD        (625) // Assumes 62.5kHz clock with divider of 1

#define INT_TA0_0 (24) /* TA0_0 IRQ */


int main(void){
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    Timer_A_UpModeConfig TimerConfig;

    WDT_A_holdTimer();

    // Configure SMCLK
    CS_setDCOFrequency(8000000); // Set DCO clock frequency to 8MHz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_128); // Set SMCLK to DCO/128 (62.5kHz)

    // Initialize timers and timer interrupts
    TimerConfig.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
    TimerConfig.clockSource = main_TIMER_CLOCK;
    TimerConfig.clockSourceDivider = main_TIMER_CLOCKDIVIDER;
    TimerConfig.timerClear = TIMER_A_DO_CLEAR;
    TimerConfig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;

    TimerConfig.timerPeriod = main_TIMER_PERIOD;
    Timer_A_configureUpMode(main_TIMER_MODULE , &TimerConfig);

    Timer_A_registerInterrupt(main_TIMER_MODULE , TIMER_A_CCR0_INTERRUPT, TA0_0_ISR_Handler);
    Timer_A_enableInterrupt(main_TIMER_MODULE );
    MAP_Timer_A_startCounter(main_TIMER_MODULE , TIMER_A_UP_MODE);


    int pinState;
    while (1) {

        if( TimerA0_CCR0_interrupt_hit==1){
            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            TimerA0_CCR0_interrupt_hit=0;
            //__delay_cycles(1000000);  //Delay 10ms

        }
    }
}

    void TA0_0_ISR_Handler(void){
        //GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        TimerA0_CCR0_interrupt_hit=1;
        //GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        TA0CCTL0 &= ~CCIFG;
        MAP_Timer_A_clearCaptureCompareInterrupt(main_TIMER_MODULE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    }

        





