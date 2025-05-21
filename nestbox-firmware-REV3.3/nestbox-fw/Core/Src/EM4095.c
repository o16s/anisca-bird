#include <rfid_tags.h>
#include "EM4095.h"
#include "rfid.h"
#include <stdint.h>
#include <string.h>

#define RFID_OUTPUT_BUF_LEN 20
uint8_t outbuffer[RFID_OUTPUT_BUF_LEN];


int16_t em4095_read(em4095_t *dev, uint8_t input_bit)
{
	if((dev->counter_header==9))
	{
	    // only execute once after header was fully detected:
	    // reset all counter variables!
        dev->counter = 0;
        dev->nibble_counter = 0;
        dev->id_counter = 0;
        dev->vertical_crc[0] = 0;
        dev->vertical_crc[1] = 0;
        dev->vertical_crc[2] = 0;
        dev->vertical_crc[3] = 0;
        dev->counter_header=10; //increase once more to not come back to this code.
	}

	if(dev->counter_header==10)
	{
		dev->data[dev->counter] = input_bit;

		if(dev->id_counter<10)
		{
            if(dev->nibble_counter==4)
            {
                //horizontal crc (row parity bit)
                dev->nibble_counter = 0;
                dev->tagId[dev->id_counter] = dev->data[dev->counter-1] + //
                                                ((dev->data[dev->counter-2])<<1) +//
                                                ((dev->data[dev->counter-3])<<2) +//
                                                ((dev->data[dev->counter-4])<<3);
                uint8_t crc = dev->data[dev->counter-1] +//
                                    dev->data[dev->counter-2] +//
                                    dev->data[dev->counter-3] +//
                                    dev->data[dev->counter-4];


                if((crc&0x01) == (dev->data[dev->counter]&0x01))
                {
                    // CRC OK!
                    dev->nibble_counter = 0;
                    dev->id_counter++;
                }
                else
                {
                    // start over
                    dev->counter_header=0;
                    dev->counter = 0;
                    dev->nibble_counter = 0;
                    return EM4095_CRC_NOT_OK;
                }

            }
            else
            {
                // summing up bits of each column for vertical parity bits:
               dev->vertical_crc[dev->nibble_counter] += input_bit;

               dev->nibble_counter++;
            }
		}
		else if(dev->nibble_counter<4)
		{
		    //check vertical crc:
            /* "Then, the last group consists of 4 event column parity bits without
             * row parity bit. S0 is a stop bit which is written to "0"" */

		    if(((dev->vertical_crc[dev->nibble_counter]) & 0x01) == (input_bit & 0x01))
		    {
		        //crc OK!
	            dev->nibble_counter++;
		    }
		    else
		    {
		        // start over
                dev->counter_header=0;
                dev->counter = 0;
                dev->nibble_counter = 0;
                return EM4095_CRC_NOT_OK;
		    }
		}

		dev->counter++;
	}
	else if(dev->counter_header<9)
	{
		// Detect Header (111111111 	 9bit Header (following a stop-0)
		if (input_bit == 0)
		{// 0's
			dev->counter_header=0;
		}
		else
		{//1's
			dev->counter_header++;
		}
	}
	// Data complete after 64 bit incl header
	if (dev->counter > 63-9 && (dev->counter_header==10))
	{
		dev->counter = 0;
		dev->counter_header = 0;

		return EM4095_DATA_OK;
	}
	else
	{
		return EM4095_OK;
	}
}
