#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(){
	PORTD.DIR= PIN1_bm; //PIN is output
//initialize the ADC for Free-Running mode
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; //10-bit resolution
	ADC0.CTRLA |= ADC_FREERUN_bm; //Free-Running mode enabled
	ADC0.CTRLA |= ADC_ENABLE_bm; //Enable ADC
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; //The bit
//Enable Debug Mode
	ADC0.DBGCTRL |= ADC_DBGRUN_bm;
//Window Comparator Mode
	ADC0.WINLT |= 10; //Set threshold
	ADC0.INTCTRL |= ADC_WCMP_bm; //Enable Interrupts for WCM
	ADC0.CTRLE |= ADC_WINCM0_bm; //Interrupt when RESULT < WINLT
	sei();
	ADC0.COMMAND |= ADC_STCONV_bm; //Start Conversion
	while(1){}
}

ISR(ADC0_WCOMP_vect){

	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;
	PORTD.OUTCLR= PIN1_bm; //LED is on
	_delay_ms(5);
	PORTD.OUT |= PIN1_bm; //LED is off
}