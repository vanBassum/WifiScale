/*
 * Framing.h
 *
 *  Created on: 1 Aug 2020
 *      Author: Bas
 */


#ifndef COMPONENTS_JBVPROTOCOL_IO_FRAMING_H_
#define COMPONENTS_JBVPROTOCOL_IO_FRAMING_H_

#include "../../Callback.h"
#include "stdint.h"
#include "Frame.h"


#define JSOF	'&'
#define JEOF	'%'
#define JESC	'\\'
#define JNOP	'*'

class Framing
{
    bool startFound = false;
    bool esc = false;
    Frame *rxFrame = NULL;
    uint8_t wrPtr = 0;
public:

    Callback<void, Frame*> *OnFrameCollected = NULL;



    void Unstuff(uint8_t data)
    {
    	bool record = false;

		if (esc)
		{
			record = true;
			esc = false;
		}
		else
		{
			switch (data)
			{
				case JESC:
					esc = true;
					break;
				case JSOF:
					startFound = true;
					wrPtr = 0;
					rxFrame = new Frame();
					break;
				case JEOF:
					startFound = false;
					if(OnFrameCollected != NULL)
						(*OnFrameCollected)(rxFrame);
					delete rxFrame;
					rxFrame = 0;

					break;
				case JNOP:
					break;
				default:
					record = true;
					break;
			}
		}
		if (record && startFound)
		{
			rxFrame->SetByteNo(data, wrPtr++);
		}

    }


    void Unstuff(uint8_t *bt, uint32_t length)
	{
    	for(int i=0; i<length; i++)
    		Unstuff(bt[i]);
	}


    static uint32_t GetEscapedSize(Frame *txFrame)
    {
    	uint32_t unEscapedSize = txFrame->Size();
    	uint32_t escapedSize = 0;

    	escapedSize++;

    	for(int i=0; i<unEscapedSize; i++)
    	{
    		uint8_t in = txFrame->GetByteNo(i);
    		if(in == JSOF || in == JEOF || in == JESC || in == JNOP)
    			escapedSize++;
    		escapedSize++;
    	}

    	escapedSize++;

    	return escapedSize;
    }

    static uint32_t EscapeFrame(Frame *txFrame, uint8_t *data, uint32_t size)
    {
    	uint32_t unEscapedSize = txFrame->Size();
    	uint32_t wr = 0;

    	data[wr++] = JSOF;

		for(int i=0; i<unEscapedSize; i++)
		{
			if(wr >= size)
				return 0;

			uint8_t in = txFrame->GetByteNo(i);
			if(in == JSOF || in == JEOF || in == JESC || in == JNOP)
				data[wr++] = JESC;

			if(wr >= size)
				return 0;

			data[wr++] = in;
		}

		if(wr >= size)
			return 0;

		data[wr++] = JEOF;
		return wr;
    }


};




#endif /* COMPONENTS_JBVPROTOCOL_IO_FRAMING_H_ */
