/*
 * Copyright (c) 2016 Dell Inc.
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
 * filename: sdi_led_internal.h
 */



/**
 * @file sdi_led_internal.h
 * @brief Internal LED resource related declarations.
 *
 */

#ifndef __SDI_LED_INTERNAL_H_
#define __SDI_LED_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_led.h"

/**
 * @struct sdi_led_sensor_t
 * @brief SDI LED Sensor Object for Callback Functions
 * Each LED resource provides the following callbacks
 * - led_on - callback function for turning on led
 * - led_off - callback function for turning off led
 */
typedef struct {
    /* LED Resource specific extensions start from here. */
    t_std_error (*led_on)(void *resource_hdl); /**< callback function for turning
    on led */
    t_std_error (*led_off)(void *resource_hdl); /**< callback function for
    turning off led */
} sdi_led_sensor_t;

/**
 * @struct sdi_digital_display_led_t
 * @brief SDI seven segment LED object for callback functions
 * Each LED resource provides the following callbacks
 *  - led_on - callback function for turning on digit led
 *  - led_off - callback function for turning off digit led
 *  - digit_led_set - callback function to set specified value in digit led
 */
typedef struct {
    /** digital_display_led_on : callback function to turn-on digital LED */
    t_std_error (*digital_display_led_on)(void *resource_hdl);

    /** digital_display_led_off : callback function to turn-off digital LED */
    t_std_error (*digital_display_led_off)(void *resource_hdl);

    /** digital_display_led_set : callback function to set specified value on LED */
    t_std_error (*digital_display_led_set)(void *resource_hdl, const char *display_string);

    /** digital_display_led_get : callback function to get the value on digital LED */
    t_std_error (*digital_display_led_get)(void *resource_hdl, char *display_string, size_t buf_size);

    /** digital_display_led_get_state : callback function to get on/off value of LED */
    t_std_error (*digital_display_led_get_state)(void *resource_hdl, bool *state);
} sdi_digital_display_led_t;

#endif /* __SDI_LED_INTERNAL_H_ */
