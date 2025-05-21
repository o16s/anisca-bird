/*
 * Copyright (C) 2014 Gautier Hattenberger, Alexandre Bustico
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file peripherals/ads1220.c
 *
 * Driver for the ADS1220
 * 24-bits ADC from TI
 * SPI communication
 *
 */

#include "ads1220.h"
#include "load_cell.h"
#include <string.h>

// Commands
#define ADS1220_WREG(_reg, _nb) ((1<<6)|(_reg<<2)|(_nb-1))
#define ADS1220_RREG(_reg, _nb) ((1<<5)|(_reg<<2)|(_nb-1))
#define ADS1220_RESET 0x06
#define ADS1220_START_SYNC 0x08
#define ADS1220_POWERDOWN 0x02
#define ADS1220_RDATA 0x10

// Conf registers
#define ADS1220_CONF0 0x0
#define ADS1220_CONF1 0x1
#define ADS1220_CONF2 0x2
#define ADS1220_CONF3 0x3

// Tare variables
#define ADS_TARE_TOLERANCE 1000 	// ADC counts --> 50 miligrams!

int32_t ADS_OFFSET = 0;	// used for tare weight
//float ADS_SCALE = 1000;	// used to return weight in grams //

// Init function
void ads1220_init(struct Ads1220 *ads)
{
  /* configure spi transaction */

  ads->spi_trans.output_length = 0;
  ads->spi_trans.input_length = 0;
  ads->spi_trans.before_cb = NULL;
  ads->spi_trans.after_cb = NULL;
  ads->spi_trans.input_buf = &(ads->rx_buf[0]);
  ads->spi_trans.output_buf = &(ads->tx_buf[0]);

  /* set inital status: Success or Done */
  ads->spi_trans.status = SPITransDone;

  ads->data = 0;
  ads->data_available = false;
  ads->config.status = ADS1220_UNINIT;
}


// Configuration function called once before normal use
void ads1220_send_config(struct Ads1220 *ads)
{
	ads->spi_trans.output_length = 5;
	ads->spi_trans.input_length = 0;
	ads->tx_buf[0] = ADS1220_WREG(ADS1220_CONF0, 4);
	ads->tx_buf[1] = (
					 (ads->config.pga_bypass << 0) |
					 (ads->config.gain << 1) |
					 (ads->config.mux << 4));
	ads->tx_buf[2] = (
					(ads->config.temp_sensor << 1) |
					 (ads->config.conv << 2) |
					 (ads->config.rate << 5));
	ads->tx_buf[3] = (
					 (ads->config.idac << 0) |
					(ads->config.low_switch << 3) |
					 (ads->config.vref << 6));
	ads->tx_buf[4] = (
					 (ads->config.i2mux << 2) |
					 (ads->config.i1mux << 5));
	spi_submit(&(ads->spi_trans));

	// read back registers:
	ads->spi_trans.output_length = 0;
	ads->spi_trans.input_length = 5;
	ads->tx_buf[0] = ADS1220_RREG(ADS1220_CONF0, 4);

	spi_submit(&(ads->spi_trans));
	if (ads->spi_trans.status == SPITransSuccess || ads->spi_trans.status == SPITransDone) {
		ads->spi_trans.status = SPITransDone;

		if(memcmp(&(ads->tx_buf[1]), &(ads->rx_buf[1]), 4)==0)
		{
			ads->config.status = ADS1220_INITIALIZED;
		}
	}

  //  //start continuous readout mode:
//  //TI recommends always sending a START/SYNC command immediately after the CM (continuous mode) bit is set to 1.
//  ads1220_start_conversion(ads);
}

// Configuration function called to start fast conversion; single-shot mode
void ads1220_change_mode(struct Ads1220 *ads, enum Ads1220SampleRate rate, enum Ads1220ConvMode mode, enum Ads1220TempSensorMode temp)
{
	ads->config.rate = rate; //for presence detection, set to fast=inexact & single shot mode
	ads->config.conv = mode;
	ads->config.temp_sensor = temp;

	ads->spi_trans.output_length = 2;
	ads->spi_trans.input_length = 0;
	ads->tx_buf[0] = ADS1220_WREG(ADS1220_CONF1, 1);
	ads->tx_buf[1] = (
					(ads->config.temp_sensor << 1) |
					 (ads->config.conv << 2) |
					 (ads->config.rate << 5));

	spi_submit(&(ads->spi_trans));


//	// read back register
//	ads->tx_buf[0] = ADS1220_RREG(ADS1220_CONF1, 1);
//
//	spi_submit(&(ads->spi_trans));
//	if (ads->spi_trans.status == SPITransSuccess || ads->spi_trans.status == SPITransDone) {
//		ads->spi_trans.status = SPITransDone;
//
//		if(memcmp(&(ads->tx_buf[1]), &(ads->rx_buf[1]), 1)==0)
//		{
//			ads->config.status = ADS1220_INITIALIZED;
//		}
//	}

	//    HAL_Delay(100);



}

void ads1120_change_polarity(struct Ads1220 *ads, enum Ads1220VRef vref)
{
	ads->config.vref = vref;

	ads->spi_trans.output_length = 2;
	ads->spi_trans.input_length = 0;
	ads->tx_buf[0] = ADS1220_WREG(ADS1220_CONF2, 1);
	ads->tx_buf[1] = (
					 (ads->config.idac << 0) |
					(ads->config.low_switch << 3) |
					 (ads->config.vref << 6));

	spi_submit(&(ads->spi_trans));

	// read back register
	ads->tx_buf[0] = ADS1220_RREG(ADS1220_CONF2, 1);

	spi_submit(&(ads->spi_trans));
	if (ads->spi_trans.status == SPITransSuccess || ads->spi_trans.status == SPITransDone) {
		ads->spi_trans.status = SPITransDone;

		if(memcmp(&(ads->tx_buf[1]), &(ads->rx_buf[1]), 1)==0)
		{
			ads->config.status = ADS1220_INITIALIZED;
		}
	}
}

// Configuration function called before normal use
void ads1220_configure(struct Ads1220 *ads)
{
  if (ads->config.status == ADS1220_UNINIT) {
    if (ads->spi_trans.status == SPITransSuccess || ads->spi_trans.status == SPITransDone) {
      ads->spi_trans.output_length = 1;
      ads->spi_trans.input_length = 0;
      ads->tx_buf[0] = ADS1220_RESET;
      spi_submit(&(ads->spi_trans));
      ads->config.status = ADS1220_SEND_RESET;}
      if (ads->spi_trans.status == SPITransSuccess || ads->spi_trans.status == SPITransDone) {
    	  ads1220_send_config(ads); // do config
	}
  }
}

// Read next data
void ads1220_read(struct Ads1220 *ads)
{
  if (ads->config.status == ADS1220_INITIALIZED && ads->spi_trans.status == SPITransDone) {
    ads->spi_trans.output_length = 0;
    ads->spi_trans.input_length = 3;
    //NOTE: SENDING THE RDATA COMMAND MAKES THAT THE DATA RE-STARTS ON THE NEXT BYTE --> SHOULD HAVE A READ LENGTH OF 4 IN THIS CASE!
    ads->tx_buf[0] = 0; // (todo-prod: test effect of this code change (used to start at index 1)
    ads->tx_buf[1] = 0;
    ads->tx_buf[2] = 0;
    spi_submit(&(ads->spi_trans));
  }
}

float ads1220_convert_temperature(struct Ads1220 *ads)
{
	ads->data = (ads->data)>>10;
	if(ads->rx_buf[0] & 0x80) //negative number!
	{
		ads->data = ads->data - 1;
		ads->data = ~(ads->data);
		return (float)(ads->data) * (-0.03125);
	}
	else
	{
		return (float)(ads->data) * (0.03125);
	}
}

// Check end of transaction
void ads1220_event(struct Ads1220 *ads)
{
  if (ads->config.status == ADS1220_INITIALIZED) {
    if (ads->spi_trans.status == SPITransFailed) {
      ads->spi_trans.status = SPITransDone;
    } else if (ads->spi_trans.status == SPITransSuccess) {
		// Successful reading of 24bits adc
		ads->data = (int32_t)(((uint32_t)(ads->rx_buf[0]) << 16) | ((uint32_t)(ads->rx_buf[1]) << 8) | (ads->rx_buf[2]));
   		if(ads->rx_buf[0] & 0x80) //negative number!
    		{
   			ads->data = ads->data | 0xFF000000; //account for the two's complement minus sign.
    		}
		ads->data_available = true;
		ads->spi_trans.status = SPITransDone;
    }
  } else if (ads->config.status == ADS1220_SEND_RESET) { // Reset ads1220 before configuring
    if (ads->spi_trans.status == SPITransFailed) {
      ads->spi_trans.status = SPITransDone;
      ads->config.status = ADS1220_UNINIT; // config failed
    } else if (ads->spi_trans.status == SPITransSuccess) {
      ads->spi_trans.status = SPITransDone;
      ads->config.status = ADS1220_INITIALIZING;
      // do config at next call of ads1220_configure() (or ads1220_periodic())
    }
  } else if (ads->config.status == ADS1220_INITIALIZING) { // Configuring but not yet initialized
    if (ads->spi_trans.status == SPITransFailed) {
      ads->spi_trans.status = SPITransDone;
      ads->config.status = ADS1220_UNINIT; // config failed
    } else if (ads->spi_trans.status == SPITransSuccess) {
      ads->spi_trans.status = SPITransDone;
      ads->config.status = ADS1220_INITIALIZED; // config done
    }
  }
}

void ads1220_start_conversion(struct Ads1220 *ads)
{
	//clear RX buffer: (todo-prod: test effect of this code change)
	ads->rx_buf[0] = 0;
	ads->rx_buf[1] = 0;
	ads->rx_buf[2] = 0;
	ads->rx_buf[3] = 0;

	//start continuous readout mode:
	ads->spi_trans.output_length = 1;
    ads->spi_trans.input_length = 0;
	ads->tx_buf[0] = ADS1220_START_SYNC;
	spi_submit(&(ads->spi_trans));
    ads->spi_trans.status = SPITransDone;
}

//int32_t ads1220_read_average(uint8_t times, int32_t* max_deviation, struct Ads1220 *ads)
//{
//    int32_t max = 0;
//
//	if(ads->config.conv != ADS1220_CONTINIOUS_CONVERSION)
//	{
//		ads1220_change_mode(ads, ADS1220_RATE_20_HZ, ADS1220_CONTINIOUS_CONVERSION, ADS1220_TEMPERATURE_DISABLED);
//	}
//
//	load_cell_reset_ready();
//    load_cell_enable_drdy_int();
//    load_cell_wait_for_ready(100); // timeout 100 ms in case DRDY pin is not connected
//
//
//    load_cell_reset_ready();
//    ads1220_start_conversion(ads);
//    load_cell_wait_for_ready(100); // timeout 100 ms in case DRDY pin is not connected
//
//    ads1220_periodic(ads);
//    ads1220_event(ads);
//
//	int32_t value = ads->data;
//	int32_t sum = ads->data;
//	int32_t min = ads->data;
//
//
//	uint8_t i;
//
//	max = ads->data;
//	for (i = 1; i < times; i++)
//	{
//	    load_cell_reset_ready();
//	    load_cell_wait_for_ready(100); // timeout 100 ms in case DRDY pin is not connected
//
//	    ads1220_periodic(ads);
//	    ads1220_event(ads);
//
//		value = ads->data;
//		sum += value;
//		if(value>max)
//			max = value;
//		if(value<min)
//			min = value;
//	}
//    load_cell_disable_drdy_int();
//
//	*max_deviation = (max-min);
//	return sum/times;
//}

//float ads1220_get_units(uint8_t times, float* max_deviation, struct Ads1220 *ads)
//{
//	int32_t max_dev_int = 0;
//	int32_t value = ads1220_read_average(times, &max_dev_int, ads) - ADS_OFFSET;
//
//	*max_deviation = (float)max_dev_int/ADS_SCALE;
//
//	return (float)value/ADS_SCALE;
//}

//float ads1220_convert_units(struct Ads1220 *ads)
//{
//	return (float)(ads->data - ADS_OFFSET)/ADS_SCALE;
//}




void ads1220_powerdown(struct Ads1220 *ads)
{
  if (ads->config.status == ADS1220_INITIALIZED) {
    ads->spi_trans.output_length = 1;
    ads->spi_trans.input_length = 0;
    ads->tx_buf[0] = ADS1220_POWERDOWN;
    spi_submit(&(ads->spi_trans));
    ads->spi_trans.status = SPITransDone;
  }
}
