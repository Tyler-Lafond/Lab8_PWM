/*	Author: tlafo001
 *  Partner(s) Name: Tyler Lafond
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum Sound_States { Sound_Start, Sound_Off_Unpressed, Sound_On_Pressed, Sound_On_Unpressed, Sound_Off_Pressed } Sound_State;

enum Scale_States { Scale_Start, Scale_Unpressed, Scale_Pressed } Scale_State;

unsigned char tmpA;
unsigned char scale;
double frequency;

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

void Tick_Sound();

void Tick_Scale();

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
	scale = 0;
	frequency = 0.00;
	Sound_State = Sound_Start;

    while (1) {
	tmpA = PINA & 0x07;
	Tick_Scale();
	Tick_Sound();
    }
    return 1;
}

void Tick_Sound() {
	switch (Sound_State)
	{
		case Sound_Start:
			Sound_State = Sound_Off_Unpressed;
			break;
		case Sound_Off_Unpressed:
			if ((tmpA & 0x01) == 0x00)
			{
				Sound_State = Sound_On_Pressed;
				PWM_on();
			}
			break;
		case Sound_On_Pressed:
			if ((tmpA & 0x01) == 0x01)
			{
				Sound_State = Sound_On_Unpressed;
			}
			break;
		case Sound_On_Unpressed:
			if ((tmpA & 0x01) == 0x00)
			{
				Sound_State = Sound_Off_Pressed;
				PWM_off();
			}
			break;
		case Sound_Off_Pressed:
			if ((tmpA & 0x01) == 0x01)
			{
				Sound_State = Sound_Off_Unpressed;
			}
			break;
		default:
			Sound_State = Sound_Start;
			break;
	}

	switch (Sound_State)
	{
		case Sound_Off_Unpressed:
			break;
		case Sound_On_Pressed:
			set_PWM(frequency);
			break;
		case Sound_On_Unpressed:
			set_PWM(frequency);
			break;
		case Sound_Off_Pressed:
			break;
		default:
			break;
	}
}

void Tick_Scale() {

	switch (Scale_State)
	{
		case Scale_Start:
			Scale_State = Scale_Unpressed;
			break;
		case Scale_Unpressed:
			if ((tmpA & 0x06) == 0x02)
			{
				if (scale < 7)
				{
					scale++;
				}
				Scale_State = Scale_Pressed;
			}
			else if ((tmpA & 0x06) == 0x04)
			{
				if (scale > 0)
				{
					scale--;
				}
				Scale_State = Scale_Pressed;
			}
			break;
		case Scale_Pressed:
			if ((tmpA & 0x06) == 0x00)
			{
				Scale_State = Scale_Unpressed;
			}
			break;
		default:
			Scale_State = Scale_Start;
	}

	switch (Scale_State)
	{
		case Scale_Unpressed:
			if (scale == 0) //C4
			{
				frequency = 261.63;
			}
			else if (scale == 1) //D4
			{
				frequency = 293.66;
			}
			else if (scale == 2) //E4
			{
				frequency = 329.63;
			}
			else if (scale == 3) //F4
			{
				frequency = 349.23;
			}
			else if (scale == 4) //G4
			{
				frequency = 392.00;
			}
			else if (scale == 5) //A4
			{
				frequency = 440.00;
			}
			else if (scale == 6) //B4
			{
				frequency = 493.88;
			}
			else if (scale == 7) //C5
			{
				frequency = 523.25;
			}
			break;
		case Scale_Pressed:
			if (scale == 0) //C4
			{
				frequency = 261.63;
			}
			else if (scale == 1) //D4
			{
				frequency = 293.66;
			}
			else if (scale == 2) //E4
			{
				frequency = 329.63;
			}
			else if (scale == 3) //F4
			{
				frequency = 349.23;
			}
			else if (scale == 4) //G4
			{
				frequency = 392.00;
			}
			else if (scale == 5) //A4
			{
				frequency = 440.00;
			}
			else if (scale == 6) //B4
			{
				frequency = 493.88;
			}
			else if (scale == 7) //C5
			{
				frequency = 523.25;
			}
			break;
		default:
			break;
	}
}
