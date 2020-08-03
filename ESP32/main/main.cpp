#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "ESP_log.h"
#include "hx711.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "JBVLib/JBVProtocol/Connections/TCPConnection.h"
#include "JBVLib/JBVProtocol/Connections/TCPListener.h"
#include <list>
#include <string.h>
#include "settings.h"

#define SSID "vasBassum"
#define WPWD "pedaalemmerzak"

std::list<Connection*> clients;
HX711 hx711(GPIO_NUM_18, GPIO_NUM_19);

extern "C" {
   void app_main();
}

template <class T>
class Filter
{
public:
  int N = 0;                                                                  // Aantal te middelen waarde
  T *meetwaardes;                                                             // Opslag individuele meetwaarden
  T accumulator = 0;                                                          // Optelsom van alle individuele meetwaarden
  int index = 0;                                                              // Meetwaarden teller

  Filter(const int n)
  {
    meetwaardes = new T[n];
    N = n;
    for(int i = 0; i < n; i++)
    {
      meetwaardes[i] = 0;
    }
  }

  ~Filter()
  {
    delete[] meetwaardes;
  }

  void AddSample(T NewSample)
  {
    accumulator -= meetwaardes[index];
    meetwaardes[index] = NewSample;                                          // Bewaar nieuwe meetwaarde
    accumulator += meetwaardes[index];                                       // Accumuleer nieuwste meetwaarde
    index++;
    index %= N;                                                              // Teller loopt van 0 tot N-1
  }

  T Average()
  {
    return accumulator/N;                                                    // Bereken de gemmiddelde waarde
  }
};

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

Filter<uint32_t> scaleFilter(64);
uint8_t b1 = 0;
uint8_t b2 = 0;
bool change = false;
gpio_num_t disp[] = {
		GPIO_NUM_26,
		GPIO_NUM_12,
		GPIO_NUM_14,
		GPIO_NUM_27,
		GPIO_NUM_15,
		GPIO_NUM_2 ,
		GPIO_NUM_0 ,
		GPIO_NUM_4 ,
		GPIO_NUM_16,
		GPIO_NUM_17,
		GPIO_NUM_5 ,
		GPIO_NUM_21,
		GPIO_NUM_22,
};



void ConFrameRecieved(Connection *connection, Frame *frame)
{
	Frame *reply = new Frame();

	reply->LEN = 32;
	reply->PAY = (uint8_t *) malloc(reply->LEN);
	uint32_t scaleResult;

	switch(frame->PAY[0])
	{
	case 0:
		scaleResult = scaleFilter.Average();
		ESP_LOGI("Main", "Scale read = %d", scaleResult);
		memcpy(reply->PAY, &scaleResult, 4);
		break;
	case 1:
		b1 = frame->PAY[1];
		b2 = frame->PAY[2];
		change = true;
		ESP_LOGI("Main", "Set = %d, %d", b1, b2);
		memcpy(reply->PAY, "OK", reply->LEN);
		break;

	default:
		ESP_LOGW("Main", "Unknown command");
		memcpy(reply->PAY, "Unknown command", reply->LEN);
		break;
	}

	connection->SendFrame(reply);
	delete reply;
}

void ClientAccepted(TCPConnection *con)
{
	con->OnFrameReceived = new Callback<void, Connection*, Frame*>(&ConFrameRecieved);
	clients.push_back(con);
}

TCPConnection *con = NULL;


void Disconnected(Connection *connection)
{
	delete con;
	con = NULL;
}

void SetupConnection()
{
	if(con != NULL)
	{
		delete con;
		con = NULL;
	}
	char host_ip[] = "192.168.0.50";
	int addr_family = 0;
	int ip_protocol = 0;
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = inet_addr(host_ip);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(1000);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;




	int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
	if (sock < 0)
	{
		ESP_LOGE("main", "Unable to create socket: errno %d", errno);
		return;
	}
	ESP_LOGI("main", "Socket created, connecting to %s:%d", host_ip, 1000);

	int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
	if (err != 0)
	{
		ESP_LOGE("main", "Socket unable to connect: errno %d", errno);
		return;
	}
	ESP_LOGI("main", "Successfully connected");


	con = new TCPConnection(sock);
	con->OnDisconnected = new Callback<void, Connection*>(&Disconnected);
	ESP_LOGI("main", "Socket passed to connection");
}






void app_main(void)
{
    nvs_flash_init();
    //Settings::Init();
    //Settings::Load();

    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    wifi_config_t sta_config = {};

    memcpy(sta_config.sta.ssid, "vasBassum", 10);
    memcpy(sta_config.sta.password, "pedaalemmerzak", 15);


    sta_config.sta.bssid_set = false;

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );


    TCPListener listener(31600);

    listener.ConnectionRecieved = new Callback<void, TCPConnection*>(&ClientAccepted);


    hx711.SetGain(2);

    //ESP_LOGI("main", "setting = %d", Settings::someSetting.Get());

    //Settings::someSetting.Set(8);

    //Settings::Save();

    ESP_LOGI("TAG", "Initializer done");




    Frame frame;

	frame.RID = 0xFFFE;
	frame.LEN = 64;
	frame.PAY = (uint8_t*) malloc(frame.LEN);
	frame.PAY[0] = 0;


	int p = 0;
	int i = 0;
	uint32_t prevScaleReading = 0;

	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0);

	bool blinkFast = false;
    while(true)
    {
    	scaleFilter.AddSample(hx711.Read());
    	if(con == NULL)
    	{

    		if(((p++) % 64) == 0)
			{
    			SetupConnection();
			}
    		//gpio_set_level(GPIO_NUM_2, blinkFast);
    	}
    	else
    	{
    		uint32_t scaleReading = scaleFilter.Average();
			int32_t diff = prevScaleReading - scaleReading; // negative = still not done.

			if(scaleReading > 8600664) // 1/8 my weight
			{
				//if(((i++) % 8) == 0)
				{
					if((diff < 50) && (diff >= 0))	//58 = approx 10 gram
					{
						//Somewhat stable.
						frame.LEN = snprintf((char*)&frame.PAY[1], frame.LEN, "ScaleReading = %d", scaleReading)+1;
						con->SendFrame(&frame);
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
			else
			{
				//To low, stay off.
			}
    	}


    	blinkFast = !blinkFast;

    	vTaskDelay(10 / portTICK_PERIOD_MS);
    }






	bool pwr = false;
	while (true)
	{
		if(change)
		{
			for(int i=0; i<13; i++)
			{
				if(b1 == i || b2 == i)
					gpio_set_direction(disp[i], GPIO_MODE_OUTPUT);
				else
					gpio_set_direction(disp[i], GPIO_MODE_INPUT);
			}
			change = false;
		}



		gpio_set_level(disp[b1], pwr);
		gpio_set_level(disp[b2], !pwr);


		pwr = !pwr;

		vTaskDelay(1);


	}



    while (true)
	{




	}


/*




	gpio_set_direction(B0, GPIO_MODE_OUTPUT);
	gpio_set_direction(B1, GPIO_MODE_OUTPUT);
	gpio_set_direction(B2, GPIO_MODE_OUTPUT);
	gpio_set_direction(B3, GPIO_MODE_OUTPUT);
	gpio_set_direction(B4, GPIO_MODE_OUTPUT);
	gpio_set_direction(B5, GPIO_MODE_OUTPUT);
	gpio_set_direction(B6, GPIO_MODE_OUTPUT);
	gpio_set_direction(B7, GPIO_MODE_OUTPUT);
	gpio_set_direction(B8, GPIO_MODE_OUTPUT);


	gpio_set_direction(A0, GPIO_MODE_OUTPUT);
	gpio_set_direction(A1, GPIO_MODE_INPUT);
	gpio_set_direction(A2, GPIO_MODE_INPUT);
	gpio_set_direction(A3, GPIO_MODE_INPUT);

	gpio_set_level(A0, 0);
	//gpio_set_level(A1, 0);
	//gpio_set_level(A2, 0);


	bool pwr = false;
    while (true)
    {
    	gpio_set_level(B0, pwr);
		gpio_set_level(B1, pwr);
		gpio_set_level(B2, pwr);
		gpio_set_level(B3, pwr);

		gpio_set_level(B4, pwr);
		gpio_set_level(B5, pwr);
		gpio_set_level(B6, pwr);
		gpio_set_level(B7, pwr);
		gpio_set_level(B8, pwr);


		pwr = !pwr;

		vTaskDelay(1);


    }
*/
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

