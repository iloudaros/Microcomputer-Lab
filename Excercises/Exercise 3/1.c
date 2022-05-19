#include "iheader.h"



int main() {

	initialise_pins();
	initialise_TCA();
	set_TCA();
	start_TCA();

	sei();

	while(1) {
  }
}
