/* ========================================================================
 * File Name: nbiot_time.c
 * This file is part of the TI-MSP432 LibNbiot example
 *
 * Author: Edgar Hindemith
 *
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
 * Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 *
 * This file is distributed under the conditions of the Apache License,
 * Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * For details see the file LICENSE at the toplevel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/Timer.h>

/* Board Header file */
#include "Board.h"

#include "libnbiotcore.h"
#include "nbiot_time.h"

/* Callback used for toggling the LED. */
void timerCallback(Timer_Handle myHandle);

void startNbiotTimer()
{
    /* Period and duty in microseconds */
    Timer_Handle timer0;
    Timer_Params params;

    /* Call driver init functions */
    Timer_init();

    /* Setting up the timer in continuous callback mode that calls the callback
     * function every 1,000 microseconds, or 1 millisecond.
     */
    Timer_Params_init(&params);
    params.period = 1000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(Board_TIMER0, &params);

    if (timer0 == NULL) {
        /* Failed to initialized timer */
        while (1);
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1);
    }
}

void timerCallback(Timer_Handle myHandle)
{
    (void) myHandle;
    tick();
}
/* [] END OF FILE */
