/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2023, 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "fsl_pint.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef EXAMPLE_PINT_BASE
#define EXAMPLE_PINT_BASE PINT
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Call back for PINT Pin interrupt 0-7.
 */
void pint_intr_callback(pint_pin_int_t pintr, pint_status_t *status)
{
    PRINTF("\f\r\nPINT Pin Interrupt %d event detected. PatternMatch status = %8b\r\n", pintr, status->pmstatus);
}

/*!
 * @brief Main function
 */
int main(void)
{
    pint_pmatch_cfg_t pmcfg;

    /* Board pin, clock, debug console init */
    BOARD_InitHardware();

    /* Clear screen*/
    PRINTF("%c[2J", 27);
    /* Set cursor location at [0,0] */
    PRINTF("%c[0;0H", 27);
    PRINTF("\f\r\nPINT Pattern Match example\r\n");

    /* Initialize PINT */
    PINT_Init(EXAMPLE_PINT_BASE);

#if (defined(PINT_USE_LEGACY_CALLBACK) && (PINT_USE_LEGACY_CALLBACK == 0))
    PINT_SetCallback(EXAMPLE_PINT_BASE, pint_intr_callback);
#endif

    /* configure kPINT_PatternMatchBSlice0 to show the single inputsrc */
    /* Setup Pattern Match Bit Slice 0 */
    pmcfg.bs_src    = DEMO_PINT_BSLICE0_SRC;
    pmcfg.bs_cfg    = kPINT_PatternMatchStickyFall;
#if (defined(PINT_USE_LEGACY_CALLBACK) && PINT_USE_LEGACY_CALLBACK)
    pmcfg.callback  = pint_intr_callback;
#endif
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(EXAMPLE_PINT_BASE, kPINT_PatternMatchBSlice0, &pmcfg);

    /* Enable callbacks for PINT0 by Index */
    PINT_EnableCallbackByIndex(EXAMPLE_PINT_BASE, kPINT_PinInt0);

    /* configure kPINT_PatternMatchBSlice1 and kPINT_PatternMatchBSlice2 to show the combined inputsrc */
#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 1U)
    /* Setup Pattern Match Bit Slice 1 */
    pmcfg.bs_src    = DEMO_PINT_BSLICE1_SRC;
    pmcfg.bs_cfg    = kPINT_PatternMatchStickyRise;
#if (defined(PINT_USE_LEGACY_CALLBACK) && PINT_USE_LEGACY_CALLBACK)
    pmcfg.callback  = pint_intr_callback;
#endif
    pmcfg.end_point = false;
    PINT_PatternMatchConfig(EXAMPLE_PINT_BASE, kPINT_PatternMatchBSlice1, &pmcfg);
#endif

#if (FSL_FEATURE_PINT_NUMBER_OF_CONNECTED_OUTPUTS > 2U)
    /* Setup Pattern Match Bit Slice 2 for falling edge detection */
    pmcfg.bs_src    = DEMO_PINT_BSLICE2_SRC;
    pmcfg.bs_cfg    = kPINT_PatternMatchStickyRise;
#if (defined(PINT_USE_LEGACY_CALLBACK) && PINT_USE_LEGACY_CALLBACK)
    pmcfg.callback  = pint_intr_callback;
#endif
    pmcfg.end_point = true;
    PINT_PatternMatchConfig(EXAMPLE_PINT_BASE, kPINT_PatternMatchBSlice2, &pmcfg);

    /* Enable callbacks for PINT2 by Index */
    PINT_EnableCallbackByIndex(EXAMPLE_PINT_BASE, kPINT_PinInt2);
#endif

    /* Enable PatternMatch */
    PINT_PatternMatchEnable(EXAMPLE_PINT_BASE);

    PRINTF("\r\nPINT Pattern match events are configured\r\n");
    PRINTF("\r\nPress corresponding switches to generate events\r\n");
    while (1)
    {
        __WFI();
    }
}
