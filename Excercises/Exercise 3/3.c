#include "iheader.h"

int main() {

	initialise_pins();
	initialise_TCA();
	set_TCA();
	start_TCA();
	initialise_ADC();
	free_running();
	start_ADC();
	initialise_switches();

	sei();

	while(1) {

		// straight movement
		while(wall==0) {
		}

		// right movement
		if (turning_right==1) {
			right_movement();
			while (turning_right==1) {/*re-enable SW5*/}
		}

		// left movement
		if (turning_left==1) {
			left_movement();
			while (turning_left==1) {/*re-enable SW6*/}
		}

		initialise_TCA();
		set_TCA();
		start_TCA();

	}

	cli();

	return 0;
}
