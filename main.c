#include "msp.h"
#include <string.h>
#include <lidar.h>

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;	// stop watchdog timer

	float distance_cm;
	float distance_in;

	i2c_init();

	while (1) {
	    distance_cm = get_distance("cm");
	    distance_in = get_distance("in");
	}
}
