#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#pragma once


class Scaling
{
	double a = 0;
	double b = 0;

public:
	void Set(double x1, double x2, double y1, double y2)
	{
		double dy = y2 - y1;
		double dx = x2 - x1;
		a = dy / dx;
		b = y1 - (a * x1);
	}

	double GetY(double x)
	{
		return a * x + b;
	}

	double GetX(double y)
	{
		return (y - b) / a;
	}
};
