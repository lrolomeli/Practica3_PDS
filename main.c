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

#define A_SIZE 200
#define C_SIZE 170
#define D_SIZE 151
#define E_SIZE 134
#define F_SIZE 127
#define G_SIZE 113
#define B_SIZE 101

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void C_tone(void);
void D_tone(void);
void E_tone(void);
void F_tone(void);
void G_tone(void);
void B_tone(void);
/*******************************************************************************
 * Constant Variables
 ******************************************************************************/

static const uint16_t A_chord[A_SIZE] = { 512, 565, 617, 666, 710, 750, 784, 811,  ///arreglo de muestras de la cuerda 5 de la guitarra
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

static const uint16_t C_chord[C_SIZE] = { 512, 574, 634, 690, 739, 780, 813, 837,
		853, 862, 865, 864, 861, 858, 857, 858, 863, 872, 885, 901, 918, 937,
		955, 971, 983, 991, 994, 991, 982, 969, 951, 929, 905, 879, 853, 827,
		802, 778, 756, 735, 715, 696, 677, 659, 641, 623, 605, 587, 569, 553,
		538, 525, 514, 505, 499, 495, 493, 492, 492, 493, 493, 493, 493, 491,
		487, 483, 478, 473, 468, 463, 460, 457, 456, 457, 459, 462, 467, 472,
		478, 484, 490, 495, 500, 505, 510, 514, 519, 524, 529, 534, 540, 546,
		552, 557, 562, 565, 567, 568, 567, 564, 561, 556, 551, 546, 541, 537,
		534, 531, 531, 531, 531, 532, 532, 531, 529, 525, 519, 510, 499, 486,
		471, 455, 438, 420, 402, 383, 365, 347, 328, 309, 290, 269, 246, 222,
		197, 171, 145, 120, 95, 74, 55, 42, 33, 30, 33, 41, 53, 69, 87, 105,
		123, 139, 152, 161, 166, 167, 166, 163, 160, 159, 162, 171, 187, 211,
		243, 284, 333, 389, 449, 511 };

static const uint16_t D_chord[D_SIZE] = { 512, 582, 649, 709, 761, 802, 832, 851,
		862, 865, 863, 860, 857, 857, 862, 871, 885, 903, 924, 944, 964, 979,
		990, 994, 991, 980, 964, 942, 916, 888, 858, 829, 801, 774, 749, 726,
		704, 683, 663, 642, 621, 601, 581, 562, 544, 529, 516, 506, 499, 494,
		492, 492, 493, 493, 494, 493, 490, 487, 482, 476, 470, 464, 460, 457,
		456, 457, 460, 464, 470, 476, 483, 490, 496, 502, 507, 512, 517, 522,
		528, 534, 541, 547, 554, 559, 564, 567, 568, 567, 564, 560, 554, 548,
		543, 538, 534, 531, 530, 531, 531, 532, 532, 530, 526, 519, 509, 496,
		480, 463, 443, 423, 403, 382, 362, 341, 320, 298, 275, 250, 224, 196,
		166, 137, 108, 82, 60, 44, 33, 30, 34, 44, 60, 79, 100, 121, 139, 153,
		162, 166, 167, 164, 161, 159, 162, 172, 192, 222, 262, 314, 374, 441,
		511 };

static const uint16_t E_chord[E_SIZE] = { 512, 591, 665, 730, 783, 822, 848, 861,
		865, 863, 859, 857, 859, 867, 882, 902, 924, 948, 969, 985, 993, 992,
		983, 965, 940, 911, 878, 845, 812, 782, 753, 727, 702, 679, 655, 632,
		609, 586, 564, 544, 527, 513, 503, 496, 493, 492, 493, 493, 493, 492,
		489, 484, 478, 471, 465, 460, 457, 456, 458, 462, 468, 475, 483, 490,
		497, 503, 509, 515, 521, 527, 534, 541, 549, 556, 562, 566, 568, 567,
		564, 559, 553, 546, 540, 535, 532, 531, 531, 531, 532, 531, 528, 521,
		511, 497, 480, 460, 438, 415, 392, 369, 346, 322, 297, 271, 243, 212,
		180, 146, 114, 84, 59, 41, 32, 31, 39, 55, 76, 99, 122, 142, 156, 165,
		167, 165, 161, 159, 163, 176, 201, 240, 293, 358, 432, 511 };

static const uint16_t F_chord[F_SIZE] = { 512, 595, 673, 740, 793, 831, 853, 863,
		865, 861, 858, 858, 864, 877, 897, 921, 945, 968, 985, 993, 992, 980,
		960, 933, 900, 865, 830, 797, 766, 737, 710, 685, 661, 636, 612, 588,
		564, 543, 525, 511, 501, 495, 492, 492, 493, 494, 493, 490, 486, 480,
		473, 466, 460, 457, 456, 458, 463, 469, 477, 485, 492, 499, 506, 512,
		518, 525, 532, 539, 547, 555, 561, 566, 568, 567, 564, 558, 551, 544,
		538, 534, 531, 530, 531, 532, 532, 529, 523, 513, 499, 481, 460, 437,
		413, 388, 364, 339, 314, 287, 259, 228, 194, 159, 124, 92, 64, 44, 32,
		31, 39, 56, 78, 103, 127, 147, 160, 166, 166, 163, 159, 160, 170, 193,
		230, 283, 350, 428, 511 };

static const uint16_t G_chord[G_SIZE] = { 512, 605, 691, 762, 814, 847, 862, 865,
		861, 857, 859, 868, 886, 911, 939, 965, 985, 993, 990, 974, 947, 913,
		874, 835, 797, 762, 730, 701, 673, 645, 618, 591, 564, 541, 521, 507,
		497, 493, 492, 493, 494, 493, 489, 484, 476, 468, 462, 457, 456, 459,
		464, 472, 481, 490, 498, 505, 512, 519, 526, 534, 543, 552, 560, 565,
		568, 567, 562, 556, 548, 540, 535, 531, 530, 531, 532, 531, 527, 517,
		503, 483, 460, 434, 406, 379, 351, 324, 294, 263, 228, 190, 150, 112,
		77, 50, 34, 31, 39, 58, 84, 112, 137, 156, 165, 167, 163, 159, 162, 177,
		210, 262, 332, 418, 511 };

static const uint16_t B_chord[B_SIZE] = { 512, 616, 709, 783, 832, 858, 865, 862,
		857, 859, 871, 894, 924, 954, 979, 993, 991, 973, 942, 902, 858, 815,
		774, 737, 704, 673, 642, 611, 581, 553, 529, 510, 499, 493, 492, 493,
		494, 492, 487, 479, 470, 462, 457, 456, 460, 467, 476, 486, 496, 504,
		512, 520, 528, 538, 547, 557, 564, 568, 567, 562, 554, 545, 538, 532,
		530, 531, 532, 531, 526, 514, 496, 472, 443, 413, 382, 351, 320, 287,
		250, 210, 166, 122, 82, 51, 33, 31, 44, 69, 100, 130, 153, 165, 167,
		162, 159, 166, 192, 241, 314, 407, 511 };


volatile bool pitIsrFlag = false;
volatile bool pit1IsrFlag = false;

static uint16_t count = 0;
static uint8_t song_count = 0;
/*******************************************************************************
 * Handlers
 * interrupciones del pit
 ******************************************************************************/
void PIT_DAC_HANDLER(void)
{
	if(PIT_GetStatusFlags(PIT, kPIT_Chnl_0))
	{
		/* Clear interrupt flag.*/
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
		pitIsrFlag = true;
	}
	if(PIT_GetStatusFlags(PIT, kPIT_Chnl_1))
	{
		/* Clear interrupt flag.*/
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag);
		song_count = (song_count == 42) ? 0 : song_count + 1;
	}

	if(PIT_GetStatusFlags(PIT, kPIT_Chnl_2))
	{
		/* Clear interrupt flag.*/
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, kPIT_TimerFlag);
		count = (count == 100) ? 0 : count + 1;

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
			USEC_TO_COUNT(45U, PIT_SOURCE_CLOCK));

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1,
			MSEC_TO_COUNT(1000U, PIT_SOURCE_CLOCK));

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_2,
			MSEC_TO_COUNT(10U, PIT_SOURCE_CLOCK));

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);


	/* Enable at the NVIC */
	EnableIRQ(PIT_IRQ_ID);

	PIT_StartTimer(PIT, kPIT_Chnl_0);
	PIT_StartTimer(PIT, kPIT_Chnl_2);
	PIT_StartTimer(PIT, kPIT_Chnl_1);
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

	for (;;) {

		/* Check whether occur interrupt and send to DAC */
		if (true == pitIsrFlag) {

			switch (song_count) {
			case 0:
				C_tone();
				break;

			case 1:
				C_tone();
				break;

			case 2:
				G_tone();
				break;

			case 3:
				G_tone();
				break;

			case 4:
				A_tone();
				break;

			case 5:
				A_tone();
				break;

			case 6:
				G_tone();
				break;
			case 7:
				F_tone();
				break;

			case 8:
				F_tone();
				break;

			case 9:
				E_tone();
				break;

			case 10:
				E_tone();
				break;

			case 11:
				D_tone();
				break;

			case 12:
				D_tone();
				break;

			case 13:
				C_tone();
				break;

			case 14:
				G_tone();
				break;

			case 15:
				G_tone();
				break;
			case 16:
				F_tone();
				break;

			case 17:
				F_tone();
				break;

			case 18:
				E_tone();
				break;

			case 19:
				E_tone();
				break;

			case 20:
				D_tone();
				break;

			case 21:
				G_tone();
				break;

			case 22:
				G_tone();
				break;

			case 23:
				F_tone();
				break;

			case 24:
				F_tone();
				break;

			case 25:
				E_tone();
				break;

			case 26:
				E_tone();
				break;

			case 27:
				D_tone();
				break;

			case 28:
				C_tone();
				break;

			case 29:
				C_tone();
				break;

			case 30:
				G_tone();
				break;

			case 31:
				G_tone();
				break;
			case 32:
				A_tone();
				break;
			case 33:
				A_tone();
				break;

			case 34:
				G_tone();
				break;

			case 35:
				F_tone();
				break;

			case 36:
				F_tone();
				break;

			case 37:
				E_tone();
				break;

			case 38:
				E_tone();
				break;
			case 39:

				D_tone();
				break;
			case 40:
				D_tone();
				break;

			case 41:
				C_tone();
				break;

			default:

				break;
			}

			pitIsrFlag = 0;
		}

	}
}


void C_tone(void)
{

	static uint8_t number_of_sample = 0;
	static uint16_t dac_signal;


	dac_signal = (uint16_t) ((float) (C_chord[number_of_sample])
			- (((float) C_chord[number_of_sample]) * DIVISIONS
					* count));

	/* DAC value to be send */
	DAC_SetBufferValue(DAC0, 0U, dac_signal);
	number_of_sample =
			((C_SIZE-1) == number_of_sample) ? 0 : number_of_sample + 1;

}

void D_tone(void)
{

	static uint8_t number_of_sample = 0;
	static uint16_t dac_signal;


	dac_signal = (uint16_t) ((float) (D_chord[number_of_sample])
			- (((float) D_chord[number_of_sample]) * DIVISIONS
					* count));

	/* DAC value to be send */
	DAC_SetBufferValue(DAC0, 0U, dac_signal);
	number_of_sample =
			((D_SIZE-1) == number_of_sample) ? 0 : number_of_sample + 1;

}

void E_tone(void)
{

	static uint8_t number_of_sample = 0;
	static uint16_t dac_signal;


	dac_signal = (uint16_t) ((float) (E_chord[number_of_sample])
			- (((float) E_chord[number_of_sample]) * DIVISIONS
					* count));

	/* DAC value to be send */
	DAC_SetBufferValue(DAC0, 0U, dac_signal);
	number_of_sample =
			((E_SIZE-1) == number_of_sample) ? 0 : number_of_sample + 1;

}

void F_tone(void)
{

	static uint8_t number_of_sample = 0;
	static uint16_t dac_signal;


	dac_signal = (uint16_t) ((float) (F_chord[number_of_sample])
			- (((float) F_chord[number_of_sample]) * DIVISIONS
					* count));

	/* DAC value to be send */
	DAC_SetBufferValue(DAC0, 0U, dac_signal);
	number_of_sample =
			((F_SIZE-1) == number_of_sample) ? 0 : number_of_sample + 1;

}

void G_tone(void)
{

	static uint8_t number_of_sample = 0;
	static uint16_t dac_signal;


	dac_signal = (uint16_t) ((float) (G_chord[number_of_sample])
			- (((float) G_chord[number_of_sample]) * DIVISIONS
					* count));

	/* DAC value to be send */
	DAC_SetBufferValue(DAC0, 0U, dac_signal);
	number_of_sample =
			((G_SIZE-1) == number_of_sample) ? 0 : number_of_sample + 1;

}

void A_tone(void)
{

	static uint8_t number_of_sample = 0;
	static uint16_t dac_signal;

	dac_signal = (uint16_t) ((float) (B_chord[number_of_sample])
			- (((float) B_chord[number_of_sample]) * DIVISIONS
					* count));

	/* DAC value to be send */
	DAC_SetBufferValue(DAC0, 0U, dac_signal);
	number_of_sample =
			((B_SIZE-1) == number_of_sample) ? 0 : number_of_sample + 1;

}
