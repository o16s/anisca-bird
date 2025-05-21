/*
 * comm_types.h
 *
 *  Created on: 29 Sep 2021
 *      Author: raffael
 */

#ifndef INC_COMM_TYPES_H_
#define INC_COMM_TYPES_H_

// message headers
#define COMM_HEADER_HEARTBEAT	0xff	// 255
#define COMM_HEADER_OFFSET		0xfa	// 250
#define COMM_HEADER_CALIB		0xfc	// 252

// event flags
#define NB_FLAGS_UNKNOWN_TAG_ID				(1<<0)	// 0x01
#define NB_FLAGS_RECOGNIZED_CALIB_WEIGHT	(1<<1)	// 0x02
#define NB_FLAGS_CONTINUOUS_EVENT			(1<<2)	// 0x04
#define NB_FLAGS_INSUFFICIENT_STABILITY		(1<<3)	// 0x08
#define NB_FLAGS_CALIB_ID_SHIFT				4

// status flags (todo other status flags)
#define NB_STATUS_FLAGS_FIRMWARE_VERSION_2023_Q1	(1<<0)
#define NB_STATUS_FLAGS_FIRMWARE_VERSION_2023_Q3	(0b10<<0)

#define NB_STATUS_FLAGS_MAYOR_FIRMWARE_VERSION		NB_STATUS_FLAGS_FIRMWARE_VERSION_2023_Q3

#define NB_STATUS_FLAGS_CALIBRATION_UNFINISHED	(1<<4)
#define NB_STATUS_FLAGS_USB_CONNECTED		(1<<5)
#define NB_STATUS_FLAGS_USER_LED_ON			(1<<6)
#define NB_STATUS_FLAGS_SHUTDOWN			(1<<7) // 0x80

// offset flags	(TODO-OK)
#define NB_OFFSET_FLAGS_INSUFFICIENT_STABILITY		(1<<3)	// 0x04

// calibration flags (TODO-OK)
#define NB_CALIB_FLAGS_INSUFFICIENT_STABILITY		(1<<3)	// 0x04

typedef struct commEventStruct{
	uint8_t duration;		// number of seconds that owl is on perch (max. 240) --> this acts as message header [0 - 240]
	uint8_t flags;			// event flags (TBD)
	uint16_t weight;		// weight estimate in 0.01 grams (unsigned value)
	uint32_t uid;			// RFID UID lower 4 bytes

	uint16_t raw_adc_pos;	// raw ADC value from "positive excitation" (incl. offset)
	uint8_t stddev;			// weight measurement series standard deviation in 0.1 grams

	int8_t temperature;		// temperature in degrees Celsius (signed value)
} comm_event_t;

typedef struct commHeartbeatStruct{
	uint8_t header;			// always 255 (COMM_HEADER_HEARTBEAT)
	uint8_t events_with_id;		// number of events with successful RFID detection since last status update [0-255]. The value of 255 means >=255.
	uint8_t events_unknown_id;	// number of events with UN-successful RFID detection since last status update [0-255]. The value of 255 means >=255.
	uint8_t flags;			// status and error flags (TBD)
	uint32_t timestamp;		// Epoch timestamp (UTC+0): seconds since January 1, 1970 12:00:00 AM
	uint16_t vbat_mv;		// Battery voltage in mV
	uint8_t memory_used;	// Percentage of Flash memory used [0-100]
	uint8_t days_since_boot;// Number of days since last reboot [0-255]. The value of 255 means >=255.
} comm_heartbeat_t;

typedef struct commOffsetStruct{
	uint8_t header;			// always 250 (COMM_HEADER_OFFSET)
	uint8_t flags;			// offset flags (TBD)
	uint16_t weight;		// tare weight estimate in 0.01 grams (unsigned value)

	uint16_t raw_adc_pos;	// raw tare ADC value from "positive excitation"
	uint16_t raw_adc_neg;	// raw tare ADC value from "negative excitation"

	uint16_t padding;		// RFU, always 0
	uint8_t stddev;			// tare measurement series standard deviation in 0.1 grams

	int8_t temperature;		// temperature in degrees Celsius (signed value)
} comm_offset_t;

#define COMM_N_CALIB_IDS	16

typedef struct commCalibrationStruct{
	uint8_t header;			// always 252 (COMM_HEADER_CALIB)
	struct calib_info_struct{	// (1 byte:)
		uint8_t used_w1: 1;		// 1st weight (usually 100g) was taken into account for calculation
		uint8_t used_w2: 1;		// 2nd weight (usually 200g) was taken into account for calculation
		uint8_t used_w3: 1;		// 3rd weight (usually 500g) was taken into account for calculation
		uint8_t used_w4: 1;		// 4th weight (usually 1kg) was taken into account for calculation
		uint8_t id: 4; 	// non-unique calibration ID: 0 - 15; ID=0 is reserved for default values (= invalid calib)
	} info;
	uint16_t slope;			// linear regression slope: (raw ADC counts per 10mg)
	uint16_t intercept;		// linear regression intercept: (raw ADC value)
	int16_t temperature;	// signed temperature in 1/100 degC
	float r2;
} comm_calib_t;

#endif /* INC_COMM_TYPES_H_ */
