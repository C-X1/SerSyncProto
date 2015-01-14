/*
 * sersyncproto.c
 *
 *  Created on: Jan 13, 2015
 *      Author: Christian Holl
 */
#include "sersyncproto.h"

#include "../uart/uart.h"

uint8_t sersyncproto(sersyncproto_data_t* data, uint8_t cur_byte)
{
	if(data->_state > __SERSYNCPROTO_CHKSUM_START
	&& data->_state < __SERSYNCPROTO_CHKSUM_END)
	{
		data->_chksum^=cur_byte;
	}

	switch(data->_state)
	{

		case SERSYNCPROTO_STATE_HEADER:

			if(cur_byte==data->_header[data->_length])
			{

				data->_length++;
				if(data->_length == data->_header_len)
				{
					data->_state=SERSYNCPROTO_STATE_CMD;
					data->_length=0;
					data->_chksum=0;
				}
			}
			else
			{
				data->_state=SERSYNCPROTO_STATE_HEADER;
				data->_length=0;
			}
			break;

		case SERSYNCPROTO_STATE_CMD:
			for (int c = 0; c < data->_cmd_cnt; ++c)
			{
				if(data->_cmd_array[c]==cur_byte)
				{

					if(data->_payload_lengths[c]>0)
					{
						data->_state=SERSYNCPROTO_STATE_PAYLOAD;
					}
					else
					{
						data->_state=SERSYNCPROTO_STATE_CHKSUM;
					}
					data->cmd=c;
					break;
				}
			}

			if(data->_state==SERSYNCPROTO_STATE_CMD) //Command not found?
			{
				data->_state=SERSYNCPROTO_STATE_HEADER;
				data->_length=0;
			}
			break;

		case SERSYNCPROTO_STATE_PAYLOAD:
			data->payload[data->_length]=cur_byte;
			data->_length++;

			if(data->_length==data->_payload_lengths[data->cmd])
			{
				data->_state=SERSYNCPROTO_STATE_CHKSUM;
			}

			break;

		case SERSYNCPROTO_STATE_CHKSUM:
			if(data->_chksum==cur_byte)
			{
				data->_state=SERSYNCPROTO_STATE_HEADER;
				data->_length=0;
				return 1;
			}
			break;

		default:
			data->_state=SERSYNCPROTO_STATE_HEADER;
			data->_length=0;
			break;
	}
	return 0;
}
