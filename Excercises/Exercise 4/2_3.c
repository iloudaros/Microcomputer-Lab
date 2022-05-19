/*
 *
 * Created: 5/17/2022
 * Author : Ioannis Loudaros - Christina Kratimenou
 *
 */

#include <iheader.h>



//************  Main  ************//

int main(void)
{
	initialise_led();
	initialise_switches();
    initilise_ADC();
    free_running();
    start_ADC();
	set_TCA();
	sei();

    while (1)
    {
		if (access_granted==1 && locked==0) // Ο χρήστης βάζει τον κωδικό του για να κλειδώσει το σπίτι του

		{
			 start_TCA();
		}

        if (intruder==1 && locked==1)
        {
            if (!waiting_for_password)
            {
                PORTD.OUTCLR |= PIN0_bm; //LED is on
                set_TCA();
                start_TCA();
                waiting_for_password=1;
            }
            else
            {
                while (!access_granted)
                {

                }
                access_granted=0;
                set_TCA();
            }
        }
    }
}
