/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_dac.h"
#include "fsl_pit.h"
#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define PIT_IRQ_ID PIT0_IRQn
#define PIT_DAC_HANDLER PIT0_IRQHandler
#define DIVISIONS 0.002
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Constant Variables
 ******************************************************************************/

static const uint16_t G_chord[200] = { 512, 565, 617, 666, 710, 750, 784, 811,
		833, 848, 858, 863, 865, 864, 862, 859, 857, 857, 859, 864, 872, 882,
		895, 910, 925, 941, 956, 970, 981, 989, 993, 993, 990, 982, 971, 956,
		939, 919, 898, 876, 854, 832, 810, 789, 770, 751, 733, 717, 700, 685,
		669, 653, 638, 623, 607, 592, 577, 562, 549, 536, 525, 516, 508, 502,
		497, 494, 493, 492, 492, 493, 493, 494, 493, 493, 491, 488, 485, 481,
		477, 473, 468, 464, 461, 458, 457, 456, 457, 459, 461, 465, 469, 474,
		479, 484, 489, 493, 498, 502, 506, 510, 514, 518, 522, 526, 530, 535,
		540, 545, 550, 555, 559, 563, 565, 567, 568, 567, 566, 563, 560, 556,
		552, 547, 543, 539, 536, 533, 531, 531, 530, 531, 531, 532, 532, 531,
		530, 527, 522, 516, 508, 499, 488, 475, 462, 448, 433, 417, 402, 386,
		371, 355, 340, 324, 308, 291, 273, 255, 235, 214, 193, 171, 148, 126,
		105, 86, 68, 53, 42, 34, 31, 31, 35, 43, 54, 67, 82, 98, 114, 128, 141,
		152, 160, 165, 167, 167, 165, 162, 160, 159, 161, 166, 176, 191, 212,
		240, 273, 313, 358, 406, 458, 511 };

volatile bool pitIsrFlag = false;
volatile bool pit1IsrFlag = false;

static uint16_t count = 0;
/*******************************************************************************
 * Handlers
 ******************************************************************************/
void PIT_DAC_HANDLER(void)
{
	if(PIT_GetStatusFlags(PIT, kPIT_Chnl_0))
	{
		/* Clear interrupt flag.*/
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
		pitIsrFlag = true;
	}
//	if(PIT_GetStatusFlags(PIT, kPIT_Chnl_1))
//	{
//		/* Clear interrupt flag.*/
//		PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
//		pit1IsrFlag = true;
//	}

	if(PIT_GetStatusFlags(PIT, kPIT_Chnl_2))
	{
		/* Clear interrupt flag.*/
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);
		count = (count == 500) ? 0 : count + 1;

	}

}
/*******************************************************************************
 * Main Function
 ******************************************************************************/
int main(void)
{
	/*******************************************************************************
	 * Variables
	 ******************************************************************************/
	/* Structure of initialize PIT */
	pit_config_t pitConfig;
    dac_config_t dacConfigStruct;

	/*******************************************************************************
	 * Code
	 ******************************************************************************/

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

	/*******************************************************************************
	 * Pit
	 ******************************************************************************/
	/*
	 * pitConfig.enableRunInDebug = false;
	 */
	PIT_GetDefaultConfig(&pitConfig);

	/* Init pit module */
	PIT_Init(PIT, &pitConfig);

	/* Set timer period for channel 0 */
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,
			USEC_TO_COUNT(20U, PIT_SOURCE_CLOCK));

//	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1,
//			USEC_TO_COUNT(4000000U, PIT_SOURCE_CLOCK));

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_2,
			MSEC_TO_COUNT(10U, PIT_SOURCE_CLOCK));

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);

//	/* Enable timer interrupts for channel 0 */
//	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);


	/* Enable at the NVIC */
	EnableIRQ(PIT_IRQ_ID);

	PIT_StartTimer(PIT, kPIT_Chnl_0);
	PIT_StartTimer(PIT, kPIT_Chnl_2);
//	PIT_StartTimer(PIT, kPIT_Chnl_1);
	/*******************************************************************************
	 * SIMPLE DAC CONFIG
	 ******************************************************************************/

	/*
	 * dacConfigStruct.referenceVoltageSource = kDAC_ReferenceVoltageSourceVref2;
	 * dacConfigStruct.enableLowPowerMode = false;
	 */
	DAC_GetDefaultConfig(&dacConfigStruct);
	/* DAC Initialization */
	DAC_Init(DAC0, &dacConfigStruct);
	/* Enable output. */
	DAC_Enable(DAC0, true);
	/* Make sure the read pointer to the start. */
	DAC_SetBufferReadPointer(DAC0, 0U);
	/*
	 * The buffer is not enabled, so the read pointer can not move automatically. However, the buffer's read pointer
	 * and items can be written manually by user.
	 */

	/*******************************************************************************
	 * CONV CODE
	 ******************************************************************************/
	uint8_t sample = 0;
	uint16_t var;
	for(;;)
    {

		/* Check whether occur interupt and toggle LED */
		if (true == pitIsrFlag)
		{

			var = (uint16_t)((float)(G_chord[sample]) - (((float)G_chord[sample]) * DIVISIONS * count));

			/* DAC value to be send */
			DAC_SetBufferValue(DAC0, 0U, var);
			sample = (199 == sample) ? 0 : sample + 1;



			pitIsrFlag = 0;
		}

	}
}
