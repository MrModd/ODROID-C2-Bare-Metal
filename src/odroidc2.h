#ifndef ODROIDC2_H
#define ODROIDC2_H

#include "odroidc2_gpio.h"

/* Utility functions */

#define BIT2MASK(bit)            \
		(1u<<(bit))

#define SET_MASK(reg, mask)      \
		reg |= (mask)

#define CLR_MASK(reg, mask)      \
		reg &= ~(mask)

#endif
