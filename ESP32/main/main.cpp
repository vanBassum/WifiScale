#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "ESP_log.h"
#include "hx711.h"
#include <list>
#include <string.h>
#include <cjson.h>
#include "lib/tft/tft.h"
#include "filter.h"
#include "settings.h"
#include "lib/tcpip/tcpsocket.h"
#include "lib/misc/datetime.h"

#define SSID "vanBassum"
#define PSWD "pedaalemmerzak"

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
	DateTime Timestamp;
	Sample()
	{
		
	}
	void Set(uint32_t raw) 
	{
		Raw = raw;
		Weight = a * ((double)raw) + b;
		Timestamp = DateTime::Now();
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


void PostMeasurement(Sample sample)
{
	cJSON* command = cJSON_CreateObject();
	cJSON_AddNumberToObject(command, "CMD", 0x01);	//Post measurement
	cJSON* data = cJSON_CreateObject();
	cJSON_AddNumberToObject(command, "RawWeight", sample.Raw);
	cJSON_AddNumberToObject(command, "Weight", sample.Weight);
	cJSON_AddStringToObject(command, "MeasurementTimestamp", sample.Timestamp.ToString().c_str());
	cJSON_AddItemToObject(command, "Data", data);
	
	char* json = cJSON_PrintUnformatted(command);
	int len = strlen(json);
	
	TCPSocket socket;
	socket.Connect("192.168.11.50", 1000, true);
	while (!socket.IsConnected()) ;
	socket.Send((uint8_t*)json, len);
	cJSON_free(command);
	free(json);
	
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	return ESP_OK;
}

void StartWIFI()
{
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	wifi_config_t sta_config = { };
	memcpy(sta_config.sta.ssid, SSID, strlen(SSID));
	memcpy(sta_config.sta.password, PSWD, strlen(PSWD));
	sta_config.sta.bssid_set = false;
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());

	setenv("TZ", "UTC-1UTC,M3.5.0,M10.5.0/3", 1);
	tzset();
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}


void DisplaySample(Sample s, Color c)
{
	char buf[32];
	snprintf(buf, 32, "%d    ", s.Raw);
	tft.DrawString(font, 0, 64, buf, c);
	snprintf(buf, 32, "%f    ", s.Weight);
	tft.DrawString(font, 0, 64 + 32, buf, c);
}

enum class State : uint8_t		//Don't change the existing numbers, its used for logging. You can add extra states.
{
	Standby,
	WaitForstable,
	CollectSample,
	Cooldown,
};

void app_main(void)
{
    nvs_flash_init();
	StartWIFI();
	tft.Init();
	font = new Font(ILGH32XB);
	tft.FillScreen(Color(0, 0, 0));
	tft.SetFontFill(Color(0, 0, 0));
	hx711.SetGain(2);

	Sample s;
	State prevState = State::Standby;
	State actState 	= State::Standby;
	State nextState = State::Standby;
		
	bool onEntry = true;
		
	while (true)
	{
		
		switch (actState)
		{
		case State::Standby:
			tft.DrawString(font, 0, 128, "Standby        ", Color(255, 0, 0));
			TakeSample(&s, 5);
			DisplaySample(s, Color(255, 0, 0));
			if (s.Weight > 2)
				nextState = State::WaitForstable;
			break;
			
		case State::WaitForstable:	
			tft.DrawString(font, 0, 128, "WaitForstable  ", Color(255, 0, 0));
			TakeSample(&s, 5);
			DisplaySample(s, Color(255, 0, 0));
			if (s.Weight > 70)
				nextState = State::CollectSample;
			else if(s.Weight < 2)
				nextState = State::Standby;
			break;
			
		case State::CollectSample:
			tft.DrawString(font, 0, 128, "CollectSample  ", Color(255, 0, 0));
			TakeSample(&s, 20);		
			DisplaySample(s, Color(0, 255, 0));
			PostMeasurement(s);
			nextState = State::Cooldown;
			break;
			
		case State::Cooldown:
			tft.DrawString(font, 0, 128, "Cooldown       ", Color(255, 0, 0));
			TakeSample(&s, 5);
			if (s.Weight < 2)
				nextState = State::Standby;
			break;
			
		}
		onEntry = false;
		if (actState != nextState)
		{
			prevState = actState;
			actState = nextState;
			onEntry = true;
			//task->Notify((uint32_t) Events::StateChanged); //Setting this will skip the 1sec delay and let the new state know its its first cycle.
		}
	}
}

	