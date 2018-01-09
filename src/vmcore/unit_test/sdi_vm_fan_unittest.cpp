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
#include "sdi_fan.h"
#include "sdi_sys_vm.h"
#include "sdi_db.h"
}

static sdi_entity_hdl_t e_hdl;
static sdi_resource_hdl_t r_hdl;
static sdi_resource_hdl_t r_next_hdl;

static uint_t fan_speed_rpm(uint_t max_speed, uint_t percent)
{
    return ((max_speed * percent) / 100);
}

/* TEST: test to write fan speed to SQL table */
/* PASS: if the set speed matches the value read by the test driver */
/* FAIL: if the set speed does not match the value read by the test driver */
TEST(sdi_vm_fan_unittest, fanSpeedWrite)
{
    sdi_resource_hdl_t info_hdl;
    uint_t fan_speed1;
    uint_t fan_speed2;
    uint_t max_speed;
    int val = 0;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    sdi_db_resource_get_associated_info(sdi_get_db_handle(), r_hdl,
                                        SDI_RESOURCE_FAN, &info_hdl);

    sdi_db_int_field_get(sdi_get_db_handle(), info_hdl, TABLE_INFO,
                         INFO_FAN_MAX_SPEED, (int *)&max_speed);

    
    //setup fan speed
    fan_speed1 = fan_speed_rpm(max_speed, 80);
    fan_speed2 = fan_speed_rpm(max_speed, 100);
    ASSERT_EQ (STD_ERR_OK, sdi_fan_speed_set (r_hdl, fan_speed1));
    ASSERT_EQ (STD_ERR_OK, sdi_fan_speed_set (r_next_hdl, fan_speed2));

    //check for fan speed
    sdi_db_int_field_get(sdi_get_db_handle(), r_hdl, TABLE_FAN,
                         FAN_SPEED, &val);
    ASSERT_EQ (fan_speed1, val);

    sdi_db_int_field_get(sdi_get_db_handle(), r_next_hdl, TABLE_FAN,
                         FAN_SPEED, &val);
    ASSERT_EQ (fan_speed2, val);

    // Test cases to check for setting speeds in excess of 100%
    fan_speed1 = fan_speed_rpm(max_speed, 101);
    fan_speed2 = fan_speed_rpm(max_speed, 102);
    ASSERT_EQ (STD_ERR(BOARD, PARAM, EINVAL), sdi_fan_speed_set(r_hdl, fan_speed1));
    ASSERT_EQ (STD_ERR(BOARD, PARAM, EINVAL), sdi_fan_speed_set(r_hdl, fan_speed2));
    ASSERT_EQ (STD_ERR_OK, sdi_sys_close ());
}

/* TEST: test to read fan speed from SQL table */
/* PASS: if the read speed matches the value set by the test driver */
/* FAIL: if the read speed does not match the value set by the test driver */
TEST(sdi_vm_fan_unittest, fanSpeedRead)
{
    uint_t fan_speed;
    int setup_speed = 80;

    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup fan speed
    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_FAN,
                         FAN_SPEED, &setup_speed);

    //check for fan speed
    ASSERT_EQ (STD_ERR_OK, sdi_fan_speed_get (r_hdl, &fan_speed));
    ASSERT_EQ (setup_speed, fan_speed);

    ASSERT_EQ (STD_ERR_OK, sdi_sys_close ());
}

/* TEST: test to read fan status from SQL table */
/* PASS: if the read status matches the value set by the test driver */
/* FAIL: if the read status does not match the value set by the test driver */
TEST(sdi_vm_fan_unittest, fanStatusGet)
{
    bool alert_on;
    bool setup_status = true;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup fan status
    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_FAN, FAN_FAULT,
                         (int *)&setup_status);

    //check for fan status
    ASSERT_EQ (STD_ERR_OK, sdi_fan_status_get (r_hdl, &alert_on));
    ASSERT_EQ (true, alert_on);

    ASSERT_EQ (STD_ERR_OK, sdi_sys_close ());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    sdi_sys_init();
    e_hdl = sdi_entity_lookup (SDI_ENTITY_FAN_TRAY, 1);
    r_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_FAN, "Fantray Fan 1");
    r_next_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_FAN, "Fantray Fan 2");

    return RUN_ALL_TESTS();
}

