/*
 * Frame.h
 *
 *  Created on: 1 Aug 2020
 *      Author: Bas
 */

#ifndef COMPONENTS_JBVPROTOCOL_IO_FRAME_H_
#define COMPONENTS_JBVPROTOCOL_IO_FRAME_H_

#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include "ESP_log.h"

#define PROTOCOLVERSION	1


class Frame
{
	uint8_t 	OPT = 0;


public:

	uint8_t 	VER = PROTOCOLVERSION;
	uint8_t 	HOP = 0;
	uint16_t 	SID = 0;
	uint16_t 	RID = 0;
	uint8_t 	LEN = 0;
	uint8_t 	*PAY = 0;



	~Frame()
	{
		if(PAY != 0)
			free(PAY);
	}


	uint8_t *GetByteAddr(uint32_t pos)
	{
		switch(pos)
		{
		case 0:
			return &OPT;
		case 1:
			return &VER;
		case 2:
			return &HOP;
		case 3:
			return (uint8_t *)&SID;
		case 4:
			return ((uint8_t *)(&SID))+1;
		case 5:
			return (uint8_t *)&RID;
		case 6:
			return ((uint8_t *)(&RID))+1;
		case 7:
			return (uint8_t *)&LEN;
		case 8:
			return ((uint8_t *)(&LEN))+1;
		default:
			if(PAY != 0 && (pos - 9) < LEN)
				return &PAY[pos - 9];
		}

		return 0;
	}


	void SetByteNo(uint8_t data, uint32_t pos)
	{
		uint8_t *bt = GetByteAddr(pos);
		if(bt != 0)
			*bt = data;
		else //TODO If this fails, return a package with the overflow bit set.
			ESP_LOGE("Frame.h", "Writing outside payload buffer!");


		if(pos == 8)
		{
			//We have recieved the length of the package, allocate memory;
			PAY = (uint8_t*)malloc(LEN);
			if(PAY == 0)
				ESP_LOGE("Frame.h", "Couln't allocate memory %d bytes", LEN);
		}
	}

	uint8_t GetByteNo(uint32_t pos)
	{
		uint8_t *bt = GetByteAddr(pos);
		if(bt != 0)
			return *bt;
		else //TODO If this fails, return a package with the overflow bit set.
			ESP_LOGE("Frame.h", "Reading outside payload buffer!");

		return 0;
	}



	bool GetBroadcast()
	{
		return (OPT & (0x01 << 0)) > 0;
	}

	bool SetBroadcast(bool val)
	{
		return OPT |= (val << 0);
	}

	bool GetRoutingInfo()
	{
		return (OPT & (0x01 << 1)) > 0;
	}

	bool SetRoutingInfo(bool val)
	{
		return OPT |= (val << 1);
	}

	bool GetOverflow()
	{
		return (OPT & (0x01 << 2)) > 0;
	}

	bool SetOverflow(bool val)
	{
		return OPT |= (val << 2);
	}

	uint32_t Size()
	{
		return LEN + 9;
	}





};


#endif /* COMPONENTS_JBVPROTOCOL_IO_FRAME_H_ */
