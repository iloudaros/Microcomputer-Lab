#include "iheader.h"

int main() {

	initialise_pins();
	initialise_TCA();
	set_TCA();
	start_TCA();
	initialise_ADC();
	free_running();
	start_ADC();

	sei();

	while(wall==0) {
	}
}
