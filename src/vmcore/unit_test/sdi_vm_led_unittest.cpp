/*
 * Copyright (c) 2016 Dell Inc.
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

#include <stdio.h>
#include "gtest/gtest.h"

extern "C" {
#include "sdi_led.h"
#include "sdi_entity.h"
#include "sdi_sys_vm.h"
#include "sdi_db.h"
#include "std_error_codes.h"
}

static sdi_entity_hdl_t e_hdl;

/* Update the LED state */
static void sdi_led_state_test(
    const char *led_alias,
    bool state,
    const char *phy_state)
{
    sdi_resource_hdl_t led_hdl;
    sdi_resource_hdl_t phy_hdl;
    int phy_led;
    int db_state;
    char db_buffer[64];

    /* Lookup the logical LED */
    led_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_LED, led_alias);

    /* Lookup the physical LED associated with the logical LED */
    sdi_db_int_field_get(sdi_get_db_handle(), led_hdl, TABLE_LED_ON_OFF,
                         LED_PHYSICAL, &phy_led);
    phy_hdl = (sdi_resource_hdl_t)(uintptr_t)phy_led;

    if (state) {
        ASSERT_EQ(STD_ERR_OK, sdi_led_on(led_hdl));
    } else {
        ASSERT_EQ(STD_ERR_OK, sdi_led_off(led_hdl));
    }

    /* Retrieve the state from the database */
    sdi_db_int_field_get(sdi_get_db_handle(), led_hdl, TABLE_LED_ON_OFF,
                         LED_STATE, &db_state);

    ASSERT_EQ(state, db_state);

    /* If there's an associated physical LED, get and verify the state*/
    if (phy_hdl) {
        sdi_db_str_field_get(sdi_get_db_handle(), phy_hdl, TABLE_LED_PHYSICAL,
                             LED_PHY_STATE, db_buffer);
        ASSERT_STREQ(phy_state, db_buffer);
    }
}

/* TEST: test to set the LED ON */
/* PASS: if the LED is ON */
/* FAIL: if the LED is off */
TEST(sdi_vm_led_unittest, set_led_on)
{
    ASSERT_EQ(STD_ERR_OK, sdi_sys_init ());

    sdi_led_state_test("Alarm Major", true, "Blinking Amber");
    sdi_led_state_test("Alarm Minor", true, "Solid Amber");
    sdi_led_state_test("Beacon", true, "Blinking Blue");
    sdi_led_state_test("Stack Master", true, "Solid Green");

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close ());
}

/* TEST: test to set the LED OFF */
/* PASS: if the LED is off */
/* FAIL: if the LED is on */
TEST(sdi_vm_led_unittest, set_led_off)
{
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    sdi_led_state_test("Stack Master", false, "Off");
    sdi_led_state_test("Alarm Minor", false, "Solid Green");

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: test to set the string "Hello" on a digit display LED*/
/* PASS: if the retrieved string is "Hello" */
/* FAIL: if the retrieved string is anything other than "Hello" */
TEST (sdi_vm_led_unittest, digital_display_led_set)
{
    const char *display_string = "Hello";
    char buf[64];
    sdi_resource_hdl_t r_hdl;

    // initialize the buffer to enable correct compare
    memset (buf, 0, 10);

    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup display string to "Hello"
    r_hdl =
      sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_DIGIT_DISPLAY_LED,
                                 "Stack LED");

    sdi_db_str_field_set(sdi_get_db_handle(), r_hdl, TABLE_LED_DIGIT,
                         DIGIT_LED_DISPLAY, "");

    ASSERT_EQ (STD_ERR_OK, sdi_digital_display_led_set (r_hdl, display_string));

    //check for "Hello" string setup
    sdi_db_str_field_get(sdi_get_db_handle(), r_hdl, TABLE_LED_DIGIT,
                         DIGIT_LED_DISPLAY, buf);
    ASSERT_STREQ (display_string, buf);
    ASSERT_EQ (STD_ERR_OK, sdi_sys_close ());
}


/* TEST: test to turn on and off the digital display LED */
/* PASS: Expected values are stored in the database */
/* FAIL: Expected values are not stored in the database */
TEST(sdi_vm_led_unittest, digital_display_led_state)
{
    int state;
    sdi_resource_hdl_t r_hdl;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    // Find the resource handle
    r_hdl = 
        sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_DIGIT_DISPLAY_LED,
                                   "Stack LED");

    ASSERT_EQ(STD_ERR_OK, sdi_digital_display_led_on(r_hdl));
    sdi_db_int_field_get(sdi_get_db_handle(), r_hdl, TABLE_LED_DIGIT,
                         DIGIT_LED_STATE, &state);
    ASSERT_TRUE(state);

    ASSERT_EQ(STD_ERR_OK, sdi_digital_display_led_off(r_hdl));
    sdi_db_int_field_get(sdi_get_db_handle(), r_hdl, TABLE_LED_DIGIT,
                         DIGIT_LED_STATE, &state);
    ASSERT_FALSE(state);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    sdi_sys_init();
    e_hdl = sdi_entity_lookup(SDI_ENTITY_SYSTEM_BOARD, 1);

    return RUN_ALL_TESTS();
}

