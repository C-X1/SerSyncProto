/*
 * sersyncproto.h
 *
 *  Created on: Jan 13, 2015
 *      Author: Christian Holl
 */

#ifndef SERSYNCPROTO_H_
#define SERSYNCPROTO_H_

#include <inttypes.h>

#ifdef __cplusplus
	#include <boost/function.hpp>
#endif


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
	uint8_t const _cmd_cnt; //!< Amount of cmds available
	uint8_t const * const _cmd_array; //!< Array of possible commands
	uint8_t const * const _payload_lengths; //!< payload length of each command in cmd_array

	uint8_t const _header_len; //!< Length of header
	uint8_t const * const _header; //!< Header definition

	sersyncproto_state_t _state; //!< Current state of the protocol state machine
	uint8_t _length; //!< Variable store the current length of a step (not for user)
	uint8_t cmd; //!< When the parser function returns true this contains the cmd
	uint8_t _chksum; //!< Variable stores the checksum

	uint8_t *payload; //!< Points to the space where the parser should store payload, minsize==max payload size
}sersyncproto_data_t;

#define INIT_SERSYNCPROTO_DATA(DATA, CMD_ARRAY, PAYLOAD_LEN_ARRAY, HEADER_ARRAY, PAYLOAD_BUFFER)\
		static sersyncproto_data_t DATA={sizeof(CMD_ARRAY), CMD_ARRAY, PAYLOAD_LEN_ARRAY, sizeof(HEADER_ARRAY), HEADER_ARRAY, SERSYNCPROTO_STATE_HEADER,0,0,0, (uint8_t*)PAYLOAD_BUFFER};

#define SERSYNCPROTO_GET_CUR_CMD(DATA)\
		DATA._cmd_array[DATA.cmd]

/**
 * Receiving function
 * needs to be called for each byte.
 * @param cur_byte The current byte
 * @param data The data struct containing the definitions
 */
uint8_t sersyncproto_rec(sersyncproto_data_t* data, uint8_t cur_byte);


#ifdef __cplusplus
	/**
	 * Sending function
	 * @param data The data struct containing the definitions
	 * @param cmd The command to be issued
	 * @param payload The payload of the current function
	 * @param sendbyte The USER_IMPLEMENTATION of a function which sends the byte over the serial interface
	 */

	uint8_t sersyncproto_send(sersyncproto_data_t* data, uint8_t cmd, uint8_t* payload, const boost::function< void (uint8_t) > sendbyte);
#else
	/**
	 * Sending function
	 * @param data The data struct containing the definitions
	 * @param cmd The command to be issued
	 * @param payload The payload of the current function
	 * @param sendbyte The USER_IMPLEMENTATION of a function which sends the byte over the serial interface
	 */

	uint8_t sersyncproto_send(sersyncproto_data_t* data, uint8_t cmd, uint8_t* payload, void (*sendbyte)(uint8_t) );
#endif


#endif /* SERSYNCPROTO_H_ */
