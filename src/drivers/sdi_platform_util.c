
/*
 * Copyright (c) 2018 Dell EMC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 *  LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * filename: sdi_platform_util.c
 */


/******************************************************************************
  * sdi_platform_util.c
  * Implements the api to support architecture and endian independent nature of sdi drivers.
  ******************************************************************************/

#include "sdi_platform_util.h"


/* API to convert uint8 byte array to a uint16 value */

uint16_t sdi_platform_util_convert_le_to_uint16(uint8_t * data){

	if(data != NULL){
		return ( data[0] | (data[1] << 8) );
	} else {
		return 0;
	}
}

/* API to convert uint8 byte array to a uint32 value */

uint32_t sdi_platform_util_convert_le_to_uint32(uint8_t * data){

	if(data != NULL){
		return ( data[0] | (data[1] << 8 )  | (data[2] << 16) | (data[3] << 24));
	} else {
		return 0;
	}
}
/* API to convert uint8 byte array to a uint16 value */

uint16_t sdi_platform_util_convert_be_to_uint16(uint8_t * data){

	if(data != NULL){
		return ( (data[0] << 8) | (data[1]) );
	} else {
		return 0;
	}
}


/* API to write 16 bit value to a byte array in le */


void sdi_platform_util_write_16bit_to_bytearray_le(uint8_t* byte_array, uint16_t value) {

	if(byte_array != NULL){
		byte_array[0] = (uint8_t)value;
		byte_array[1] = (uint8_t)(value >> 8);
	} else {
		return ;
	}
}









