#include <avr/io.h>
#include <avr/interrupt.h>

/********** Allocation ************
Αριστερή Ρόδα: 8LSBs, PORTD.PIN1,PORTF.PIN6
Δεξιά Ρόδα: 8MSBs, PORTD.PIN0,PORTF.PIN5
************************************/

//*********** Variables ************//

int hcount = 0;
int lcount = 0;
int right_turns = 0;
int left_turns = 0;
int wall = 0; // ADC interrupt
int turning_right = 0; // SW5 interrupt (right movement)
int turning_left = 0; // SW6 interrupt (left movement)


//*********** Functions ************//

//***TCA0***//

// Αρχικοποίηση TCA0
void initialise_TCA()
{

	/* set waveform output on PORT A */
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTA_gc;

	/* enable split mode */
	TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;

	/* enable compare channel 0 for the lower and higher byte */
	TCA0.SPLIT.CTRLB = TCA_SPLIT_HCMP0EN_bm | TCA_SPLIT_LCMP0EN_bm;
};


void set_TCA()
{
	/* μέγιστη τιμή TOP μέχρι την οποία θα μετρήσουν οι δύο 8-bit timers  */
	TCA0.SPLIT.LPER = 16;
	TCA0.SPLIT.HPER = 16;

	/* το duty cycle του παλμού του κάθε 8-bit timer μέσω της χρήσης του καταχωρητή CMPx */
	TCA0.SPLIT.LCMP0 = 8;
	TCA0.SPLIT.HCMP0 = 8;
};


// start TCA0
void start_TCA()
{
	/* set clock source and start timer */
	TCA0.SPLIT.CTRLA |= TCA_SPLIT_CLKSEL_DIV16_gc | TCA_SPLIT_ENABLE_bm;

	/* enable interrupts */
	TCA0.SPLIT.INTCTRL = TCA_SPLIT_HUNF_bm | TCA_SPLIT_LUNF_bm;
};


void disable_TCA()
{
	// disable TCA in order to stop flashing the LEDs
	TCA0.SPLIT.CTRLA = 0x00;
};


// higher & lower byte timer
ISR(TCA0_HUNF_vect) {

	int intflags = TCA0.SPLIT.INTFLAGS;
	TCA0.SPLIT.INTFLAGS = intflags;

	// hcount interrupt
	if ((TCA0.SPLIT.INTFLAGS & 00000010)==1)
	{
		hcount++;
		if (hcount!=2) {}
		else {
				PORTD.OUTTGL = 0x01;
				hcount = 0;
			}
	}

	// lcount interrupt
	else if ((TCA0.SPLIT.INTFLAGS & 00000001)==1)
	{
		lcount++;
		if (lcount!=2) {}
		else {
			PORTD.OUTTGL = 0x02;
			lcount = 0;
		}
	}
};


//***Movements***//

void right_movement()
{

	PORTD.OUT = 0x07; // all LEDs are off
	initialise_TCA();
	TCA0.SPLIT.LPER = 16;
	TCA0.SPLIT.HPER = 8;
	TCA0.SPLIT.LCMP0 = 8;
	TCA0.SPLIT.HCMP0 = 4;
	start_TCA();
};

void left_movement()
{

	PORTD.OUT = 0x07; // all LEDs are off
	initialise_TCA();
	TCA0.SPLIT.LPER = 8;
	TCA0.SPLIT.HPER = 16;
	TCA0.SPLIT.LCMP0 = 4;
	TCA0.SPLIT.HCMP0 = 8;
	start_TCA();
};


//***Pins***//

// initialise LEDs
void initialise_pins()
{
	/* pin 0 and 1 of port D as output */
	PORTD.DIR |= PIN0_bm | PIN1_bm | PIN2_bm;
	PORTD.OUT = 0x07; // led's are off
};

// Enable switches for left and right movement
void initialise_switches() {

	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
};

ISR(PORTF_PORT_vect)
{
	// clear the interrupt flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;

	// if SW5 is enabled
	if((PORTF.INTFLAGS & 00100000)==1)
	{
		right_turns++;
		turning_right = 1;
		if (right_turns==2) {
			turning_right = 0;
			wall = 0;
			right_turns = 0;
		}
	}

	// SW6 is enabled
	else
	{
		left_turns++;
		turning_left = 1;
		if (left_turns==2) {
			turning_left = 0;
			wall = 0;
			left_turns = 0;
		}
	}
};

//***ADC0***//

// initialise ADC0
void initialise_ADC()
{

	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; // 10-bit resolution
	ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; // Select ADC channel

	ADC0.DBGCTRL |= ADC_DBGRUN_bm; // enable debug mode
};

// Enable free-running mode
void free_running()
{

	// Window Comparator Mode
	ADC0.CTRLA |= ADC_FREERUN_bm; // Free-running mode enabled
	ADC0.WINLT |= 20; // set threshold
	ADC0.INTCTRL |= ADC_WCMP_bm; // enable interrupt for WCM
	ADC0.CTRLE |= ADC_WINCM0_bm; // interrupt when RESULT < WINLT
};

// Start Conversion
void start_ADC()
{

	ADC0.COMMAND |= ADC_STCONV_bm;
};

ISR(ADC0_WCOMP_vect)
{

	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;
	wall = 1;
	disable_TCA();
	PORTD.OUT = 0x04; // only PIN2 on 
};
