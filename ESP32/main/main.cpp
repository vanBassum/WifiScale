#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "ESP_log.h"
#include "hx711.h"
#include "JBVLib/JBVProtocol/Connections/TCPConnection.h"
#include "JBVLib/JBVProtocol/Connections/TCPListener.h"
#include <list>
#include <string.h>

#include "JBVLib/JBVProtocol/client.h"
#include "filter.h"
#include "settings.h"
#include "JBVLib/wifi.h"


HX711 hx711(GPIO_NUM_18, GPIO_NUM_19);
Filter<uint32_t> scaleFilter(64);

Client *client;


extern "C" {
   void app_main();
}




void Client_OnMessageRecieved(Client* sender, uint16_t txID, uint8_t* rxData, uint16_t rxLen)
{
	uint32_t scaleResult;
	uint8_t txLen = 64;
	char *txData = (char *)malloc(txLen);
	uint8_t actLen = 0;

	ESP_LOGI("main", "Rec from %d", txID);

	if(rxLen == 0)
	{
		actLen = snprintf(txData, txLen-1, "No data recieved.");
	}
	else
	{
		switch(rxData[0])
		{
		case 0:
			scaleResult = scaleFilter.Average();
			ESP_LOGI("Main", "Scale read = %d", scaleResult);
			actLen = snprintf(txData, txLen-1, "ScaleReading = %d", scaleResult);
			break;

		default:
			ESP_LOGW("Main", "Unknown command");
			actLen = snprintf(txData, txLen-1, "Unknown command.");
			break;
		}
	}

	sender->SendMessage(txID, (uint8_t*)txData, actLen+1);
	free(txData);
}


void app_main(void)
{
    nvs_flash_init();

    Wifi::Connect();
    hx711.SetGain(2);

    TCPConnection *tcpCon = new TCPConnection("192.168.0.50", 1000, 5000);
    client = new Client(10, tcpCon);
    client->OnMessageRecieved = new Callback<void, Client* , uint16_t, uint8_t*, uint16_t>(&Client_OnMessageRecieved);

    ESP_LOGI("TAG", "Initializer done");


	uint32_t prevScaleReading = 0;

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0);

	bool blinkFast = false;
    while(true)
    {
    	scaleFilter.AddSample(hx711.Read());

    	uint32_t scaleReading = scaleFilter.Average();
		int32_t diff = prevScaleReading - scaleReading; // negative = still not done.

		if(scaleReading > 8600664) // 1/8 my weight
		{
			//if(((i++) % 8) == 0)
			{
				if((diff < 50) && (diff >= 0))	//58 = approx 10 gram
				{
					//Somewhat stable.

					uint8_t txLen = 64;
					char txData[txLen];

					txData[0] = 0;
					uint8_t actLen = snprintf(&txData[1], txLen-2, "ScaleReading = %d", scaleReading);

					client->SendMessage(0xFFFE, (uint8_t *)txData, actLen+1);

					ESP_LOGI("main", 	"Measurement accuired 	%d", scaleReading);
					gpio_set_level(GPIO_NUM_2, 1);
					while(scaleFilter.Average() > 8600664)
					{
						scaleFilter.AddSample(hx711.Read());
						vTaskDelay(10);
					}
					gpio_set_level(GPIO_NUM_2, 0);
					prevScaleReading = 0;
				}
				else
				{
					//Reading not steady yet.
					prevScaleReading = scaleReading;
					gpio_set_level(GPIO_NUM_2, blinkFast);
					ESP_LOGI("main", 	"No stable reading  	%d", scaleReading);
				}
			}
		}



    	blinkFast = !blinkFast;
    	vTaskDelay(10 / portTICK_PERIOD_MS);
    }



}


void * operator new( size_t size )
{
    return pvPortMalloc( size );
}

void * operator new[]( size_t size )
{
    return pvPortMalloc(size);
}

void operator delete( void * ptr )
{
    vPortFree ( ptr );
}

void operator delete[]( void * ptr )
{
    vPortFree ( ptr );
}

