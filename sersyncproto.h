/*
 * sersyncproto.h
 *
 *  Created on: Jan 13, 2015
 *      Author: Christian Holl
 */

#ifndef SERSYNCPROTO_H_
#define SERSYNCPROTO_H_

#include <inttypes.h>

typedef enum
{
	SERSYNCPROTO_STATE_HEADER,
	__SERSYNCPROTO_CHKSUM_START,
	SERSYNCPROTO_STATE_CMD,
	SERSYNCPROTO_STATE_PAYLOAD,
	__SERSYNCPROTO_CHKSUM_END,
	SERSYNCPROTO_STATE_CHKSUM,
}sersyncproto_state_t;

typedef struct
{
	uint8_t const _cmd_cnt;
	uint8_t const * const _cmd_array;
	uint8_t const * const _payload_lengths;

	uint8_t const _header_len;
	uint8_t const * const _header;

	sersyncproto_state_t _state;
	uint8_t _length;
	uint8_t cmd;
	uint8_t _chksum;

	uint8_t *payload;
}sersyncproto_data_t;

#define INIT_SERSYNCPROTO_DATA(DATA, CMD_ARRAY, PAYLOAD_LEN_ARRAY, HEADER_ARRAY, PAYLOAD_BUFFER)\
		static sersyncproto_data_t DATA={sizeof(CMD_ARRAY), CMD_ARRAY, PAYLOAD_LEN_ARRAY, sizeof(HEADER_ARRAY), HEADER_ARRAY, SERSYNCPROTO_STATE_HEADER,0,0,0, PAYLOAD_BUFFER};

#define SERSYNCPROTO_GET_CUR_CMD(DATA)\
		DATA._cmd_array[DATA.cmd]


uint8_t sersyncproto(sersyncproto_data_t* data, uint8_t cur_byte);

#endif /* SERSYNCPROTO_H_ */
