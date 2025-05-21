/*
 * log_types.h
 *
 *  Created on: Jan 18, 2022
 *      Author: raffael
 */

#ifndef INC_LOG_TYPES_H_
#define INC_LOG_TYPES_H_

#include <stdint.h>


#define CLI_ENCODE_B64

#define LOG_MSG_LENGTH 12
typedef uint8_t crc_type;
#define LOG_CRC_LENGTH	sizeof(crc_type)

#define LOG_TIMESTAMP_LENGTH	4 // uint32

#define LOG_HEADER_LENGTH		3
#define LOG_HEADER_CHAR			'S'

#define LOG_CONTENT_LENGTH		(LOG_TIMESTAMP_LENGTH + LOG_MSG_LENGTH)
#define LOG_STRUCT_LENGTH		(LOG_TIMESTAMP_LENGTH + LOG_MSG_LENGTH + LOG_CRC_LENGTH + LOG_HEADER_LENGTH)

typedef struct
{
	char header[LOG_HEADER_LENGTH]; // also used as padding.
	crc_type crc;
	uint8_t msg[LOG_MSG_LENGTH]; // do not change order
	uint32_t timestamp;
}LOG_message_t;


#endif /* INC_LOG_TYPES_H_ */
