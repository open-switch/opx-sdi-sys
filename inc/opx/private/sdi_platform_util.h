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
 * filename: sdi_platform_util.h
 */


#ifndef __SDI_PLATFORM_UTIL_H__
#define __SDI_PLATFORM_UTIL_H__


#include "std_assert.h"
#include "std_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* API to convert uint8 byte array to a uint16 value */

uint16_t convert_le_to_uint16(uint8_t* data);

/* API to convert uint8 byte array to a uint32 value */

uint32_t convert_le_to_uint32(uint8_t* data);

/* API to convert uint8 byte array to a uint16 value */

uint16_t convert_be_to_uint16(uint8_t* data);


/* API to write 16 bit value to byte array in le format*/
void write_16bit_to_bytearray_le(uint8_t* byte_array, uint16_t value); 



#endif
