/*
* Ergastirio_1.c
*
* Created: 4/12/2022 11:56:17 AM
* Author : Ioannis Loudaros and Christina Kratimenou
*/


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>


int cars;
int button=0;
#define ped 600

int main(){
	PORTD.DIRSET = PIN0_bm|PIN1_bm|PIN2_bm; // 0:cars big road (cbr) 1:pedestrian big road (pbr) 2:cars small road (csr)
	PORTD.OUT |= (PIN1_bm|PIN2_bm); //only cbr is on

	//pullup enable and Interrupt enabled with sense on both edges for the button
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

	//setting up the counter
	TCA0.SINGLE.CNT = 0; //clear counter
	TCA0.SINGLE.CTRLB = 0; //Normal Mode
	TCA0.SINGLE.CMP0 = ped; //When reaches this value -> interrupt CLOCK Frequency/1024
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc; //= 0x7<<1
	TCA0.SINGLE.CTRLA |=1; //Enable
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; //Interrupt Enable (=0x10)


	sei(); //enable interrupts

	while (1) {
		cars=random();
		cars= cars%10;

		if (button==0) // If there are no pedestrians
		{
			if (cars==0 || cars==5 || cars==8) //If there are cars on the small road
			{
				PORTD.OUT |= (PIN0_bm|PIN2_bm); //roads off
				PORTD.OUTCLR= PIN2_bm; //csr on
			}
			else
			{
				PORTD.OUT |= (PIN0_bm|PIN2_bm); //roads off
				PORTD.OUTCLR= PIN0_bm; //cbr on
			}
		}

		else
		{
			TCA0.SINGLE.CTRLA |=1; //Start the timer
		}

	}
}



ISR(PORTF_PORT_vect)
{

	//clear the interrupt flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS=intflags;
	button=1;
	PORTD.OUT |= PIN0_bm; //roads off
	PORTD.OUTCLR= PIN1_bm|PIN2_bm; // pbr and csr on

}

ISR(TCA0_CMP0_vect)
{

	TCA0.SINGLE.CTRLA = 0; //Disable
	//clear flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS=intflags;
	button=0;
	PORTD.OUT |= PIN0_bm; //pbr off
}
