/*	Author: tlafo001
 *  Partner(s) Name: Tyler Lafond
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum Sound_States { Sound_Start, Sound_Off, Sound_OnC4, Sound_OnD4, Sound_OnE4 } Sound_State;

volatile unsigned char TimerFlag = 0;
unsigned char frequency;

void TimerISR() { TimerFlag = 1; }

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter

		//prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }

		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }

		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
		// COM3A0: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
		// WGM32: When counter (TCNT3) matches OCR3a, resets counter
		// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void Tick_Sound() {
	switch (Sound_State)
	{
		case Sound_Start:
			Sound_State = Sound_Off;
			break;
		case Sound_Off:
			if (tmpA == 0x00)
			{
				Sound_State = Sound_OnC4;
				frequency = 261.63;
				PWM_On();
			}
			else if (tmpA == 0x03)
			{
				Sound_State = Sound_OnD4;
				frequency = 293.66;
				PWM_On();
			}
			else if (tmpA == 0x05)
			{
				Sound_State = Sound_OnE4;
				frequency = 329.63;
				PWM_On();
			}
			break;
		case Sound_OnC4:
			if (((tmpA == 0x01 || tmpA == 0x02) || (tmpA == 0x07 || tmpA = 0x06)) || tmpA == 0x04)
			{
				Sound_State = Sound_Off;
			}
			else if (tmpA == 0x03)
			{
				Sound_State = Sound_OnD4;
				frequency = 293.66;
				PWM_On();
			}
			else if (tmpA == 0x05)
			{
				Sound_State = Sound_OnE4;
				frequency = 329.63;
				PWM_On();
			}
			break;
		case Sound_OnD4:
			if (((tmpA == 0x01 || tmpA == 0x02) || (tmpA == 0x07 || tmpA = 0x06)) || tmpA == 0x04)
			{
				Sound_State = Sound_Off;
			}
			else if (tmpA == 0x00)
			{
				Sound_State = Sound_OnC4;
				frequency = 261.63;
				PWM_On();
			}
			else if (tmpA == 0x05)
			{
				Sound_State = Sound_OnE4;
				frequency = 329.63;
				PWM_On();
			}
			break;
		case Sound_OnE4:
			if (((tmpA == 0x01 || tmpA == 0x02) || (tmpA == 0x07 || tmpA = 0x06)) || tmpA == 0x04)
			{
				Sound_State = Sound_Off;
			}
			else if (tmpA == 0x00)
			{
				Sound_State = Sound_OnC4;
				frequency = 261.63;
				PWM_On();
			}
			else if (tmpA == 0x03)
			{
				Sound_State = Sound_OnD4;
				frequency = 293.66;
				PWM_On();
			}
			break;
		default:
			Sound_State = Sound_Start;
			break;
	}

	switch (Sound_State)
	{
		case Sound_Off:
			PWM_Off();
			break;
		case Sound_OnC4:
			Set_PWM(frequency);
			break;
		case Sound_OnD4:
			Set_PWM(frequency);
			break;
		case Sound_OnE4:
			Set_PWM(frequency);
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
	unsigned char tmpA = 0x00;
    while (1) {
	tmpA = PINA & 0x07;
	Tick_Sound();
	while (!TimerFlag) {}
	TimerFlag = 0;
    }
    return 1;
}
