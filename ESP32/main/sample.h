#pragma once
#include <stdint.h>
#include "lib/misc/datetime.h"
#include "scale.h"


class Sample
{
	Scaling scale;

public:
	int Samples = 0;
	uint32_t Raw = 0;
	double Weight = 0;
	DateTime Timestamp;

	Sample()
	{
		const double y1 = 0.f;
		const double y2 = 9.f;
		const double x1 = 8535875.f;
		const double x2 = 8590340.f;
		scale.Set(x1, x2, y1, y2);
	}

	void Set(uint32_t raw)
	{
		Raw = raw;
		Weight = scale.GetY(raw);
		Timestamp = DateTime::Now();
	}
};

