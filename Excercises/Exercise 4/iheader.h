#include <avr/io.h>
#include <avr/interrupt.h>


//*********** Variables ************//

int intruder = 0; //Υποδεικνύει αν ο ADC έχει εντοπίσει κίνηση
int wrong_tries = 0; //Το πλήθος των λάθος προσπαθειών εισαγωγής κωδικού
int password[4] = {6,5,5,6}; //Ο κωδικός
int access_granted = 0; //Υποδεικνύει αν ο κωδικός είναι σωστός
int current_digit; //Το ψηφίο που εισάχθηκε τελευταίο από το πληκτρολόγιο
int checking_digit = 0; //Η θέση του ψηφίου που εξετάζουμε τώρα μέσα στον κωδικό
int locked=0; //Αν ο συναγερμός είναι κλειδωμένος
int siren=0; //Ενεργοποίηση σειρήνας
int waiting_for_password=0; // Υποδεικνύει αν το σύστημα έχει εντοπίσει κίνηση και περιμένει κωδικό για να απενεργοποιήσει την σειρήνα.



//********** Constants *************//

#define ped 10
#define threshold 10
#define resolution 20
#define d_cycle 10



//*********** Function Declaration ************//

//***Declarations***//

void check_password();
void set_TCA();
void start_TCA();
void disable_TCA();
void set_pwm();
void initialise_led();
void initialise_switches();
void initialise_ADC();
void free_running();
void start_ADC();




//*********** Function Definition ************//


//***Logic***//


void check_password()
{
	if (checking_digit==4)// This means that the first 4 digits (so all of them) were correct
	{
		access_granted=1;
		checking_digit=0;
		wrong_tries=0;
		waiting_for_password=0;
		siren=0;
		PORTD.OUT |= PIN0_bm; //led is off

		if (locked)
		{
			disable_TCA();
			locked=0;
			PORTD.OUT |= PIN0_bm; //led is off
		}

	}

	else
	{
		access_granted=0;
	}

	if (wrong_tries>=3 && locked==1) set_pwm();
}



//***TCA0 as a timer***//

// Initialise TCA0
void set_TCA()
{
	TCA0.SINGLE.CNT = 0; //clear counter
	TCA0.SINGLE.CTRLB = 0; //Normal Mode (TCA_SINGLE_WGMODE_NORMAL_gc  ??? 207)
	TCA0.SINGLE.CMP0 = ped; //When reaches this value -> interrupt CLOCK FREQUENCY/1024
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc; //(= 0x7<<1 ??? 224 )
}

void start_TCA()
{
	TCA0.SINGLE.CTRLA |=1;//Enable
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm; //Interrupt Enable (=0x10)
	access_granted=0;
}

void disable_TCA()
{
	// disable TCA in order to stop flashing the LEDs
	TCA0.SINGLE.CTRLA = 0x00;
}



//***TCA0 as a PWM***//

void set_pwm()
{
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc;
	TCA0.SINGLE.PER = resolution; //select the resolution
	TCA0.SINGLE.CMP0 = d_cycle; //select the duty cycle
	//select Single_SLope_PWM
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	//enable interrupt Overflow
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	//enable interrupt COMP0
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; //Enable
}



//***Interrupts for TCA***//

ISR(TCA0_CMP0_vect){
	//clear the interrupt flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;


	if (locked)
	{
		siren=1;
		set_pwm();
	}
	else
	{
		locked=1;
		disable_TCA();

	}

}

ISR(TCA0_OVF_vect){
	//clear the interrupt flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	PORTD.OUT |= PIN0_bm; //led is off
}




//***Pins***//

// initialise LEDs
void initialise_led()
{
	/* pin 0 and 1 of port D as output */
	PORTD.DIR |= PIN0_bm;
	PORTD.OUT = PIN0_bm; // led's are off
}

// Enable switches for the keyboard
void initialise_switches() {

	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

ISR(PORTF_PORT_vect)// Button Pressed
{
	// if SW5 is enabled
	if(PORTF.INTFLAGS == 0x20) current_digit=5;
	// SW6 is enabled
	else current_digit=6;
		
	// clear the interrupt flag
  	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;

	if (current_digit==password[checking_digit]) checking_digit++;
	else
	{
		checking_digit=0;
		wrong_tries++;
	}

	check_password();

}



//***ADC0***//

// initialise ADC0
void initialise_ADC()
{

	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; // 10-bit resolution
	ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; // Select ADC channel

	ADC0.DBGCTRL |= ADC_DBGRUN_bm; // enable debug mode
}

// Enable free-running mode
void free_running()
{

	// Window Comparator Mode
	ADC0.CTRLA |= ADC_FREERUN_bm; // Free-running mode enabled
	ADC0.WINLT |= threshold; // set threshold
	ADC0.INTCTRL |= ADC_WCMP_bm; // enable interrupt for WCM
	ADC0.CTRLE |= ADC_WINCM0_bm; // interrupt when RESULT < WINLT
}


// Start Conversion
void start_ADC()
{

	ADC0.COMMAND |= ADC_STCONV_bm;
}

// ADC Interrupt
ISR(ADC0_WCOMP_vect) //intruder found
{
	int intflags = ADC0.INTFLAGS;
	ADC0.INTFLAGS = intflags;
	intruder = 1;

}
