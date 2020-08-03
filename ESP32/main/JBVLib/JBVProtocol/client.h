/*
 * client.h
 *
 *  Created on: 3 Aug 2020
 *      Author: Bas
 */

#ifndef MAIN_JBVLIB_JBVPROTOCOL_CLIENT_H_
#define MAIN_JBVLIB_JBVPROTOCOL_CLIENT_H_

#include "IO/Connection.h"
#include "stdint.h"
#include "../Callback.h"


class Client
{

public:
	//this, senderID, dataPtr, dataLength;
	Callback<void, Client*, uint16_t, uint8_t*, uint16_t> *OnBroadcastRecieved = NULL;
	Callback<void, Client*, uint16_t, uint8_t*, uint16_t> *OnMessageRecieved = NULL;

private:
	Connection *connection = NULL;


	void Connection_OnFrameReceived(Connection* con, Frame* frame)
	{
		if(frame->GetRoutingInfo())
		{
			if(frame->RID == ID)
			{
			    Frame reply;
			    reply.HOP = 0;
	            reply.SID = ID;
	            reply.RID = 0;
	            reply.SetBroadcast(true);
	            reply.SetRoutingInfo(true);
				connection->SendFrame(&reply);
			}
		}
		else
		{
			if(frame->GetBroadcast())
			{
				if(OnBroadcastRecieved != NULL)
					(*OnBroadcastRecieved)(this, frame->SID, frame->PAY, frame->LEN);
			}
			else
			{
				if(OnMessageRecieved != NULL)
					(*OnMessageRecieved)(this, frame->SID, frame->PAY, frame->LEN);
			}
		}
	}


public:



	uint16_t ID = 0;

	Client(uint16_t id, Connection *con)
	{
		ID = id;
		connection = con;
		connection->OnFrameReceived = new Callback<void, Connection*, Frame*>(this, &Client::Connection_OnFrameReceived);
	}

	~Client()
	{
		delete connection;
	}


	void SendMessage(uint16_t rxID, uint8_t *data, uint16_t length)
	{
	    Frame frame;
		frame.RID = rxID;
		frame.SID = ID;
		frame.LEN = length;
		frame.PAY = (uint8_t*) malloc(frame.LEN);
		memcpy(frame.PAY, data, length);
		connection->SendFrame(&frame);
	}

	void SendBroadcast(uint8_t *data, uint16_t length)
	{
		Frame frame;
		frame.SID = ID;
		frame.SetBroadcast(true);
		frame.LEN = length;
		frame.PAY = (uint8_t*) malloc(frame.LEN);
		memcpy(frame.PAY, data, length);
		connection->SendFrame(&frame);
	}


};



#endif /* MAIN_JBVLIB_JBVPROTOCOL_CLIENT_H_ */
