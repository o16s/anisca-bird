/*
 * em4095.c
 *
 *  Created on: 29 Jun 2021
 *      Author: raffael
 */

#include "rfid_tags.h"

#include <assert.h>


#define UINT8_BIT_SIZE 8U
#define RIGHTMOST_BIT_SET(value) ((value) & 0x0001U)

uint16_t ucrc16_calc_le(const uint8_t *buf, size_t len, uint16_t poly,
                        uint16_t seed)
{
    assert(buf != NULL);
    unsigned int c,i;
    for (c = 0; c < len; c++, buf++) {
        seed ^= (*buf);
        for (i = 0; i < UINT8_BIT_SIZE; i++) {
            seed = RIGHTMOST_BIT_SET(seed) ? ((seed >> 1) ^ poly) : (seed >> 1);
        }
    }
    return seed;
}

int16_t fdx_format(em4095_t *dev, tagdata *tag)
{
	uint8_t i=0;
	uint8_t k=0;
	uint16_t crc = 0;

	dev->counter = 0;
	dev->counter_header = 0;

	// Data for Checksum
	for (k=0; k<8; k++) //8 rows of 8+1 bits
	{
		tag->checksumData[k] = 0;
		for (i=0; i<8; i++) //the 8 first bits of each row
		{
			tag->checksumData[k] |= ((dev->data[i+(k*9)]) << i);
		}
	}

	// Checksum format
	tag->checksumArr[0] = 0; tag->checksumArr[1] = 0;
	for (i=72; i<=79; i++)
	{
		tag->checksumArr[0] |= (dev->data[i] << (i-72));
	}

	for (i=81; i<=88; i++)
	{
		tag->checksumArr[1] |= (dev->data[i] << (i-81));
	}

	tag->checksum16 = 0;
	tag->checksum16 = tag->checksum16 | tag->checksumArr[0] | (uint16_t)tag->checksumArr[1] << 8;
	// Checksum calculaton
	crc = ucrc16_calc_le(&tag->checksumData[0], sizeof(tag->checksumData), 0x8408 , 0x0000);


#ifdef VERBOSE
	uint8_t outbuffer[8];
	int strlen = ui2a(crc, 16, 1, outbuffer);
	uart_serial_write(&debug_uart, outbuffer, strlen);
	uart_serial_putc(&debug_uart, ',');
	strlen = ui2a(tag->checksum16, 16, 1, outbuffer);
	uart_serial_write(&debug_uart, outbuffer, strlen);
	uart_serial_putc(&debug_uart, '\n');
#endif


	if ((tag->checksum16 != crc))
	{
		tag->tagId = 0;
		return EM4095_CRC_NOT_OK;
	}

	// Tag ID format
	tag->tagId = 0;
	tag->tagId = tag->tagId | (uint64_t)tag->checksumData[0] | (uint64_t)tag->checksumData[1] << 8 | (uint64_t)tag->checksumData[2] << 16 | (uint64_t)tag->checksumData[3] << 24 | (uint64_t)(tag->checksumData[4] & 0x3F) << 32 ;

	// Tag Countrycode format
	tag->countryCode = 0;
	tag->countryCode = tag->countryCode | ((uint16_t)tag->checksumData[5] << 2) | ((tag->checksumData[4] & 0xC0) >> 6);

	// Tag Data Block used and Tag for Animal Identification
	tag->dataBlock = 0;
	tag->dataBlock = tag->checksumData[6] & 0x01;
	tag->animalTag = 0;
	tag->animalTag = (tag->checksumData[7] & 0x80) >> 7;

	// Datablock for additional data and individual application
	for (i=90; i<=97; i++)
	{
		tag->dataB[0] |= (dev->data[i] << (i-90));
	}

	for (i=99; i<=106; i++)
	{
		tag->dataB[1] |= (dev->data[i] << (i-99));
	}

	for (i=108; i<=115; i++)
	{
		tag->dataB[2] |= (dev->data[i] << (i-108));
	}

	return EM4095_OK;
}

int16_t em4100_format(em4095_t *dev, tagdata *tag)
{
	// TODO-OK: refactor code using this function. Delete this source file as it is no longer needed.
	// CRC is all checked in EM4095 code. This function only refactors the ID bytes into one integer.
	int i=0;
	tag->tagId = 0;
	for(i=0;i<10;i++)
	{
		tag->tagId = (tag->tagId) << (4);
		tag->tagId += (dev->tagId[i]);
	}

	return EM4095_OK;
}

