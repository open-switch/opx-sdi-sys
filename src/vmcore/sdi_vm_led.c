/*
 * Copyright (c) 2018 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file sdi_vm_led.c
 * @brief SDI LED API VM implementation
 */

#include "sdi_led.h"
#include "sdi_sys_vm.h"
#include "sdi_entity.h"

/* Generic function to turn an LED on or off */
static t_std_error sdi_led_state(sdi_resource_hdl_t led_hdl, int state)
{
    char phy_state[LED_STATE_SZ];
    int phy_led;
    sdi_resource_hdl_t phy_hdl;
    t_std_error rc;

    /* Retrieve the physical LED for the corresponding LED handle */
    rc = sdi_db_int_field_get(sdi_get_db_handle(), led_hdl,
                              TABLE_LED_ON_OFF, LED_PHYSICAL, &phy_led);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Set the resource handle of the physical LED to the ID */
    phy_hdl = (sdi_resource_hdl_t)(uintptr_t)phy_led;

    /* Set the state of the logical LED */
    rc = sdi_db_int_field_target_set(sdi_get_db_handle(), led_hdl,
                                TABLE_LED_ON_OFF, LED_STATE, &state);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* We may have a case where we don't have an associated physical LED,
     * where the phy_hdl value is zero. In that case, simply return with
     * no error.
     */
    if (phy_led == 0) {
        return STD_ERR_OK;
    }

    /* Retrieve the state to be set for the LED */
    if (state) {
        rc = sdi_db_str_field_get(sdi_get_db_handle(), led_hdl,
                                  TABLE_LED_ON_OFF, LED_PHY_STATE_ON, phy_state);
        if (rc != STD_ERR_OK) {
            return rc;
        }
    } else {
        rc = sdi_db_str_field_get(sdi_get_db_handle(), led_hdl,
                                  TABLE_LED_ON_OFF, LED_PHY_STATE_OFF, phy_state);
        if (rc != STD_ERR_OK) {
            return rc;
        }
    }

    return sdi_db_str_field_set(sdi_get_db_handle(), phy_hdl,
                                TABLE_LED_PHYSICAL, LED_PHY_STATE, phy_state);
}

/*
 * Turn on the specified LED
 */
t_std_error sdi_led_on(sdi_resource_hdl_t led_hdl)
{
    return sdi_led_state(led_hdl, 1);
}

/*
 * Turn off the specified LED
 */
t_std_error sdi_led_off(sdi_resource_hdl_t led_hdl)
{
    return sdi_led_state(led_hdl, 0);
}

/*
 * Set the specified value in the digital_display_led.
 */
t_std_error sdi_digital_display_led_set(sdi_resource_hdl_t led_hdl, const char *display_string)
{
    STD_ASSERT(display_string != NULL);

    return sdi_db_str_field_target_set(sdi_get_db_handle(), led_hdl,
                                TABLE_LED_DIGIT, DIGIT_LED_DISPLAY,
                                display_string);
}

/* Generic function to turn a digital display LED on or off */
static t_std_error sdi_digital_display_led_state(sdi_resource_hdl_t led_hdl, int state)
{
    return sdi_db_int_field_target_set(sdi_get_db_handle(), led_hdl,
                                TABLE_LED_DIGIT, DIGIT_LED_STATE, &state);
}

/*
 * Turn on the specified digital display LED
 */
t_std_error sdi_digital_display_led_on(sdi_resource_hdl_t led_hdl)
{
    return sdi_digital_display_led_state(led_hdl, 1);
}

/*
 * Turn off the specified digital display LED
 */
t_std_error sdi_digital_display_led_off(sdi_resource_hdl_t led_hdl)
{
    return sdi_digital_display_led_state(led_hdl, 0);
}

/*
 * Get the digital display LED power state
 */
t_std_error sdi_digital_display_led_get_state (sdi_resource_hdl_t led_hdl, bool *state)
{
    int temp=0;
    t_std_error rc = sdi_db_int_field_get(sdi_get_db_handle(), led_hdl,
                            TABLE_LED_DIGIT, DIGIT_LED_STATE, &temp);
    *state = (bool)temp;
    return rc;
}

/*
 * Get the value on the digital display
 */
t_std_error sdi_digital_display_led_get (sdi_resource_hdl_t led_hdl,
            char *display_string, size_t buf_size)
{
    return sdi_db_bin_field_get(sdi_get_db_handle(), led_hdl,
                    TABLE_LED_DIGIT, DIGIT_LED_DISPLAY, (uint8_t*)display_string, (uint_t*)&buf_size);
}
