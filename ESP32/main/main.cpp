#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "ESP_log.h"
#include "hx711.h"
#include <list>
#include <string.h>

#include "lib/tft/tft.h"
#include "filter.h"
#include "settings.h"



HX711 hx711(GPIO_NUM_21, GPIO_NUM_19);
Filter<uint32_t> scaleFilter(64);

TFT tft = TFT::Get_ILI9341();
Font *font = 0;


extern "C" {
   void app_main();
}

class Sample
{
	const double y1 = 0.f;
	const double y2 = 9.f;
	const double x1 = 8535875.f;	
	const double x2 = 8590340.f;
	const double dy = y2 - y1;
	const double dx = x2 - x1;
	const double a = dy / dx;
	const double b = y1 - (a*x1);
public:
	int Samples = 0;
	uint32_t Raw = 0;
	double Weight = 0;
	Sample()
	{
		
	}
	void Set(uint32_t raw) 
	{
		Raw = raw;
		Weight = a * ((double)raw) + b;
	}
};

void TakeSample(Sample* sample, int samples)
{
	uint64_t bigMeas = 0;
	for (int i = 0; i < samples; i++)
		bigMeas += hx711.Read();
	sample->Set(bigMeas / samples);
	sample->Samples = samples;
}


void TestPost()
{

}


void app_main(void)
{
    nvs_flash_init();

	tft.Init();
	font = new Font(ILGH32XB);
	tft.FillScreen(Color(0, 0, 0));
	tft.SetFontFill(Color(0, 0, 0));
	tft.DrawString(font, 0, 32, "Init", Color(255, 0, 0));

	hx711.SetGain(2);
	char buf[32];
	Sample s;
	while (true)
	{
		
		tft.DisplayOff();
		
		do
		{
			TakeSample(&s, 5);
		} while (s.Weight < 10);

		tft.DisplayOn();
		TestPost();
		
		do
		{
			TakeSample(&s, 5);
			
			snprintf(buf, 32, "%d", s.Raw);
			tft.DrawString(font, 0, 64, buf, Color(255, 0, 0));
			snprintf(buf, 32, "%f", s.Weight);
			tft.DrawString(font, 0, 64 + 32, buf, Color(255, 0, 0));
			
		} while (s.Weight > 9);
	}
}

