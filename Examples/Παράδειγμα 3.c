#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define ped 20

int interr=0;

int main() {

	PORTD.DIR |= PIN1_bm; //PIN is output
	PORTD.OUTCLR= PIN1_bm; //LED is on

	// σελ 219, 224, 205) 16-bit counter high and low

	TCA0.SINGLE.CNT = 0; //clear counter
	TCA0.SINGLE.CTRLB = 0; //Normal Mode (TCA_SINGLE_WGMODE_NORMAL_gc σελ 207)
	TCA0.SINGLE.CMP0 = ped; //When reaches this value -> interrupt CLOCK Frequency/1024
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc; //= 0x7<<1 σελ 224)
	TCA0.SINGLE.CTRLA |=1; //Enable
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; //Interrupt Enable (=0x10)
	sei(); //begin accepting interrupt signals
	while (interr==0){
	}
	PORTD.OUT |= PIN1_bm; //LED is off
	cli();
}

ISR(TCA0_CMP0_vect){

	TCA0.SINGLE.CTRLA = 0; //Disable
	//clear flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS=intflags;
	interr=1;
}
