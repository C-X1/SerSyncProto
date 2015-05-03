/*
 * sersyncproto.c
 *
 *  Created on: Jan 13, 2015
 *      Author: Christian Holl
 */
#include <sersyncproto.h>

uint8_t sersyncproto_send(sersyncproto_data_t* data, uint8_t cmd, uint8_t* payload, void (*sendbyte)(uint8_t) )
{
	uint8_t found=0;
	//Check for the command
	for (uint8_t c = 0; c < data->_cmd_cnt; ++c)
	{
		if(data->_cmd_array[c]==cmd)
		{
			found=1;
			cmd=c;
			break;
		}
	}
	//if command found
	if(found)
	{
		//send header
		for (int hd = 0; hd < data->_header_len; ++hd)
		{
			sendbyte(data->_header[hd]);
		}

		//start checksum with cmd
		uint8_t chksum=cmd;

		//send cmd byte
		sendbyte(cmd);

		//send payloads if there is any payload for that command
		for (int pb = 0; pb < data->_payload_lengths[cmd]; ++pb)
		{
			chksum^=payload[pb];
			sendbyte(payload[pb]);
		}

		//send checksum
		sendbyte(chksum);
	}
	else //Command not found
	{
		return 0;
	}
	return 1;//Ok
}

uint8_t sersyncproto_rec(sersyncproto_data_t* data, uint8_t cur_byte)
{
	if(data->_state > __SERSYNCPROTO_CHKSUM_START
	&& data->_state < __SERSYNCPROTO_CHKSUM_END)
	{
		data->_chksum^=cur_byte;
	}

	switch(data->_state)
	{
		//Waiting for the header to pass
		case SERSYNCPROTO_STATE_HEADER:

			//If current data is the same like it should be in the header
			if(cur_byte==data->_header[data->_length])
			{
				//Increase current length (header)
				data->_length++;
				if(data->_length == data->_header_len)//Header length reached?
				{
					data->_state=SERSYNCPROTO_STATE_CMD; //Go to command identification
					data->_length=0; //Length reset
					data->_chksum=0; //checksum reset
				}
			}
			else
			{
				//Restart header parser if not the header
				data->_state=SERSYNCPROTO_STATE_HEADER;
				data->_length=0;
			}
			break;

			//Get the command number
		case SERSYNCPROTO_STATE_CMD:
			for (int c = 0; c < data->_cmd_cnt; ++c)
			{
				//Check if we have that command
				if(data->_cmd_array[c]==cur_byte)
				{
					//Does it have a payload?
					if(data->_payload_lengths[c]>0)
					{
						//Goto payload state
						data->_state=SERSYNCPROTO_STATE_PAYLOAD;
					}
					else
					{
						//Go directly to checksum
						data->_state=SERSYNCPROTO_STATE_CHKSUM;
					}
					data->cmd=c;
					break;
				}
			}

			if(data->_state==SERSYNCPROTO_STATE_CMD) //Command not found?
			{
				//Ignore it and wait for next header
				data->_state=SERSYNCPROTO_STATE_HEADER;
				data->_length=0;
			}
			break;

			//Store payload
		case SERSYNCPROTO_STATE_PAYLOAD:
			data->payload[data->_length]=cur_byte;
			data->_length++;

			//Payload received?
			if(data->_length==data->_payload_lengths[data->cmd])
			{
				//Goto checksum
				data->_state=SERSYNCPROTO_STATE_CHKSUM;
			}

			break;

			//Checksum
		case SERSYNCPROTO_STATE_CHKSUM:
			//Reset for next header, return 1
		    data->_state=SERSYNCPROTO_STATE_HEADER;
			data->_length=0;

			if(data->_chksum==cur_byte)//checksum equal to current byte?
			{
				return 1;
			}
			break;

		default:
			//Reset for next header
			data->_state=SERSYNCPROTO_STATE_HEADER;
			data->_length=0;
			break;
	}
	return 0;
}
