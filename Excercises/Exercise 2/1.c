/*
 * Ergastirio 2.c
 *
 * Created: 4/16/2022 12:32:02 AM
 * Author : Ioannis Loudaros and Christina Kratimenou
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int turn_left=0;
int forward=1;
int turn_right=0;

int end=0;

#define ped 10

int main(){
	PORTD.DIRSET = PIN0_bm|PIN1_bm|PIN2_bm; // 2:left 1:forward 0:right
	PORTD.OUT |= (PIN0_bm|PIN2_bm); //start going forward


	//initialize the ADC for Free-Running mode
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; //10-bit resolution
	ADC0.CTRLA |= ADC_FREERUN_bm; //Free-Running mode enabled
	ADC0.CTRLA |= ADC_ENABLE_bm; //Enable ADC
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; //The bit
	//Enable Debug Mode
	ADC0.DBGCTRL |= ADC_DBGRUN_bm;
	//Window Comparator Mode
	ADC0.WINLT |= 5; //Set threshold
	ADC0.INTCTRL |= ADC_WCMP_bm; //Enable Interrupts for WCM
	ADC0.CTRLE |= ADC_WINCM0_bm; //Interrupt when RESULT < WINLT
	sei();
	ADC0.COMMAND |= ADC_STCONV_bm; //Start Conversion



	while(1){
		if (end==4) break;

		while (turn_left)
		{
			//Set the timer
			TCA0.SINGLE.CNT = 0; //clear counter
			TCA0.SINGLE.CTRLB = 0; //Normal Mode
			TCA0.SINGLE.CMP0 = ped; //When reaches this value -> interrupt CLOCK Frequency/1024
			TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc; //= 0x7<<1
			TCA0.SINGLE.CTRLA |=1; //Enable
			TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; //Interrupt Enable (=0x10)
			while (turn_left){}

		}
		while (turn_right)
		{

		}

	}
return 0;
}

ISR(ADC0_WCOMP_vect){
//clear the flags of the ADC
	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;

	PORTD.OUT |= (PIN0_bm|PIN1_bm|PIN2_bm); //no movement
	PORTD.OUTCLR |= PIN2_bm; //turn_left
	forward=0;
	turn_left=1;

}

ISR(TCA0_CMP0_vect)
{
	TCA0.SINGLE.CTRLA = 0; //Disable
	//clear flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS=intflags;

	turn_left=0;
	forward=1;

	PORTD.OUT |= (PIN0_bm|PIN1_bm|PIN2_bm); //no movement
	PORTD.OUTCLR |= PIN1_bm; //forward


	end++;
}
