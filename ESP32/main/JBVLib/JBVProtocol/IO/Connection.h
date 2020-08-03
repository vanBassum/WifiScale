/*
 * Connection.h
 *
 *  Created on: 1 Aug 2020
 *      Author: Bas
 */

#ifndef COMPONENTS_JBVPROTOCOL_IO_CONNECTION_H_
#define COMPONENTS_JBVPROTOCOL_IO_CONNECTION_H_

#include "ESP_log.h"
#include "../../Callback.h"
#include "stdint.h"
#include "Frame.h"
#include "Framing.h"
#include <stdlib.h>

class Connection
{

public:
	Callback<void, Connection*, Frame*> *OnFrameReceived = NULL;
	Callback<void, Connection*> *OnDisconnected = NULL;

private:
	Framing framing;

	void Framing_OnFrameCollected(Frame* rx)
	{
		if(OnFrameReceived != NULL)
			(*OnFrameReceived)(this, rx);
	}

public:


    Connection()
    {
    	framing.OnFrameCollected = new Callback<void, Frame*>(this, &Connection::Framing_OnFrameCollected);
	}

    virtual ~Connection()
    {

    }

    virtual bool SendData(uint8_t *data, uint8_t length) = 0;

    void SendFrame(Frame *frame)
	{
    	uint32_t escapedSize = Framing::GetEscapedSize(frame)+8;
		uint8_t encodedFrame[escapedSize];

		uint32_t size = Framing::EscapeFrame(frame, encodedFrame, escapedSize);

		if(size != 0)
		{
			if(!SendData(encodedFrame, size))
			{
				if(OnDisconnected != NULL)
				{
					(*OnDisconnected)(this);
				}
			}
		}
		else
		{
			ESP_LOGE("Connection.h", "Escaping failed.");
		}

	}

protected:

    void HandleData(uint8_t *data, uint8_t length)
	{
		framing.Unstuff(data, length);
	}

	void HandleData(uint8_t data)
	{
		framing.Unstuff(data);
	}


};


#endif /* COMPONENTS_JBVPROTOCOL_IO_CONNECTION_H_ */
