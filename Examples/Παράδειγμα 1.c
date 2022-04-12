#include <avr/io.h>
#include <util/delay.h>

#define del 10

int main(void){
	PORTD.DIR |= PIN1_bm; //PIN is output
	PORTD.OUT |= PIN1_bm; //LED is off
	while (1) {
		PORTD.OUTCLR= PIN1_bm; //on
		_delay_ms(del); //wait for 10ms
		PORTD.OUT |= PIN1_bm; //off
		_delay_ms(del); //wait for 10ms
	}
}
