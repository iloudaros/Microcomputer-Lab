#include <util/delay.h>
#include <avr/interrupt.h>

#define del 10
int interr=0; //logic flag

int main() {
	PORTD.DIR |= PIN1_bm; //PIN is output
	PORTD.OUT |= PIN1_bm; //LED is off
//pullup enable and Interrupt enabled with sense on both edges
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	sei(); //enable interrupts
	while (interr==0) {
		
		PORTD.OUTCLR= PIN1_bm; //on
		_delay_ms(del);
		PORTD.OUT=  PIN1_bm;  //off
		_delay_ms(del);
	}
	cli() //disenable interrupts
}


ISR(PORTF_PORT_vect){

//clear the interrupt flag
int intflags = PORTF.INTFLAGS;
PORTF.INTFLAGS=intflags;
interr=1;
}