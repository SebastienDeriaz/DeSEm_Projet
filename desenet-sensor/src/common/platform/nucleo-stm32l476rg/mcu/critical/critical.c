#include "critical.h"


volatile unsigned char bInISR = 0;
volatile unsigned char bEnterCriticalRegionNested = 0;


void enterCritical()
{
	// Only disable interrupts when not calling from an ISR
	if (!inISR())
	{
		if (!bEnterCriticalRegionNested)
		{
			// Turn off the priority configurable interrupts
			asm volatile ("cpsid i");
		}
		bEnterCriticalRegionNested++;
	}

}

void exitCritical()
{
	// Only enable interrupts when not calling from an ISR
	if (!inISR())
	{
		bEnterCriticalRegionNested--;

		if (!bEnterCriticalRegionNested)
		{
			// Turn on the interrupts with configurable priority
			asm volatile("cpsie i");
		}
	}
}
