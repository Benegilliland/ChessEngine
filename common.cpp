#include "common.h"

int count_bits(u64 b)
{
	int counter = 0;

	while (b) {
		b &= (b - 1);
		counter++;
	}

	return counter;
}
