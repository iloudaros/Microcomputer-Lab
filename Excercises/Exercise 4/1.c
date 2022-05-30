/*
 *
 *
 * Created: 5/16/2022 7:20:32 PM
 * Author : Ioannis Loudaros - Christina Kratimenou
 */

#include "iheader.h"


//************  Main  ************//

int main(void)
{
	initialise_led();
	initialise_switches();
	set_TCA();
	sei();

    while (1)
    {
		if(access_granted==1)

		{
			 start_TCA();
		}
    }
}
