/*
 * em4095.h
 *
 *  Created on: 29 Jun 2021
 *      Author: raffael
 */

#ifndef INC_EM4095_H_
#define INC_EM4095_H_


#include <stdint.h>
#include <stdio.h>

#define RFID_KNOWN_CUSTOMER_ID		0x07

#define RFID_N_CALIBRATION_IDS		10


/**
 * @brief   EM4095 return values
 */
enum {
    EM4095_OK    = 0,			/**< all went as expected */
    EM4095_GPIO_FAULT = -1,	/**< error GPIO configuation */
    EM4095_CRC_NOT_OK = -2,	/**< crc is wrong */
	EM4095_DATA_OK = -3		/**< Data read complete */
};

/**
 * @brief   EM4095 device descriptor
 */
typedef struct {
	uint8_t counter;	  		/**< counter for data bits*/
	uint8_t counter_header;		/**< counter for Header bits "10000000000"*/
	uint8_t nibble_counter;		/**< counter for the 4-bit groups of the EM4100 */
	uint8_t data[128];			/**< raw data*/
	uint16_t timediff[128];
	uint16_t int_time[128];

	uint16_t last_timestamp;
	uint8_t tagId[10];			/**< EM4100 only: 2x4 version bits + 8x4 data bits*/
	uint8_t id_counter;
	uint8_t vertical_crc[4];
} em4095_t;

/**
 * @brief   EM4095 Tag Data
 */
typedef struct {
	uint64_t tagId;				/**< Tag ID */
	uint16_t countryCode;		/**< Country Code of Tag */
	uint16_t dataBlock;			/**< Data Block is used */
	uint16_t animalTag;			/**< For Animal Identification */
	uint8_t checksumData[8];	/**< 64bit for checksum calculation */
	uint16_t checksumArr[2];	/**< Checksum 2byte of 64bit block, from Tag */
	uint16_t checksum16;		/**< 16bit checksum, calcuated */
	uint8_t dataB[3];			/**< for Data on Tag, if DataBlock is 1 */
	uint8_t valid;
	uint8_t detection_counts;
} tagdata;


/**
 * @brief              Extract the Data from the Raw data of the FDX (EM4100) tag and performs a CRC.
 *
 * @param[out] dev     the device descriptor
 * @param[out] tag     the Tag Data
 *
 * @return             0 if CRC is ok
 * @return             -2 if CRC is not ok
 */
int16_t fdx_format(em4095_t *dev, tagdata *tag);

int16_t em4100_format(em4095_t *dev, tagdata *tag);


#endif /* INC_EM4095_H_ */
