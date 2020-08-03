

#ifndef __HX711_H__
#define __HX711_H__
class HX711
{
	uint8_t gain = 0;
	gpio_num_t SCK 	= GPIO_NUM_18;		// Power Down and Serial Clock Input Pin
	gpio_num_t DOUT = GPIO_NUM_19;		// Serial Data Output Pin


public:

	HX711(gpio_num_t sck, gpio_num_t dout)
	{
		SCK = sck;
		DOUT = dout;

		gpio_set_direction(SCK, GPIO_MODE_OUTPUT);
		gpio_set_direction(DOUT, GPIO_MODE_INPUT);

		gpio_set_pull_mode(DOUT, GPIO_PULLDOWN_ONLY);
	}



	uint32_t Read()
	{
		uint32_t count = 0;
		uint8_t i;

		vTaskSuspendAll ();

		gpio_set_level(SCK, 0);

		while(gpio_get_level(DOUT));

		for (i=0;i<24;i++)
		{
			gpio_set_level(SCK, 1);
			ets_delay_us(1);

			count=count<<1;
			gpio_set_level(SCK, 0);
			ets_delay_us(1);

			if(gpio_get_level(DOUT)) count++;
		}


		for(i = 0; i<(gain+1); i++)
		{
			gpio_set_level(SCK, 1);
			ets_delay_us(1);
			gpio_set_level(SCK, 0);
			ets_delay_us(1);
		}

		xTaskResumeAll ();

		count=count^0x800000;
		return count;
	}


	void SetGain(uint8_t newGain)
	{
		gain = newGain;
		Read();	//Next read the gain will be set.
	}


};
#endif /* __HX711_H__ */
