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
#include "sdi_db.h"
#include "sdi_thermal.h"
#include "sdi_sys_vm.h"
#include "sdi_entity.h"
}

static sdi_entity_hdl_t e_hdl;
static sdi_resource_hdl_t r_hdl;

/* TEST: test to retrieve a temperature sensor value from the temperature SQL table */
/* PASS: if the retrieved temperature matches the value set by the test driver */
/* FAIL: if the retrieved temperature does not match the value set by the test driver */
TEST(sdi_vm_thermal_unittest, TemperatureSensorValueGet)
{
    int setup_temperature = 65;
    int temperature;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup temperature sensor value
    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_TEMPERATURE, &setup_temperature);

    //checking for corresponding temperature sensor value
    ASSERT_EQ (STD_ERR_OK, sdi_temperature_get (r_hdl, &temperature));
    ASSERT_EQ (setup_temperature, temperature);
    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: to set low, high and critical temperature sensor threshold values from the temperature SQL table */
/* PASS: if the temperature threshold retrieved by the test driver matches the value set in the test */
/* FAIL: if the threshold temperature retrieved by the test driver does not match the value set in the test */
TEST(sdi_vm_thermal_unittest, TemperatureSensorThresholdValueSet)
{
    int val = 0;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup low, high and critical threshold values
    ASSERT_EQ (STD_ERR_OK,
               sdi_temperature_threshold_set (r_hdl, SDI_LOW_THRESHOLD, 35));
    ASSERT_EQ (STD_ERR_OK,
               sdi_temperature_threshold_set (r_hdl, SDI_HIGH_THRESHOLD, 55));
    ASSERT_EQ (STD_ERR_OK,
               sdi_temperature_threshold_set (r_hdl, SDI_CRITICAL_THRESHOLD,
                                              65));

    //checking threshold values
    sdi_db_int_field_get(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_THRESHOLD_LOW, &val);
    ASSERT_EQ (35, val);

    sdi_db_int_field_get(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_THRESHOLD_HIGH, &val);
    ASSERT_EQ (55, val);

    sdi_db_int_field_get(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_THRESHOLD_CRITICAL, &val);
    ASSERT_EQ (65, val);

    ASSERT_EQ (STD_ERR_OK, sdi_sys_close ());
}

/* TEST: to retrieve a temperature sensor threshold value from the temperature SQL table */
/* PASS: if the temperature threshold retrieved by the test matches the value set in the test driver */
/* FAIL: if the threshold temperature retrieved by the test does not match the value set in the test driver*/
TEST(sdi_vm_thermal_unittest, TemperatureSensorThresholdValueGet)
{
    int temperature_threshold;
    int setup_low_temperature = 45;
    int setup_high_temperature = 55;
    int setup_critical_temperature = 65;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup low, high and critical threshold values
    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_THRESHOLD_LOW, &setup_low_temperature);

    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_THRESHOLD_HIGH, &setup_high_temperature);

    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_THRESHOLD_CRITICAL, &setup_critical_temperature);

    //checking threshold values
    ASSERT_EQ (STD_ERR_OK,
               sdi_temperature_threshold_get (r_hdl, SDI_LOW_THRESHOLD,
                                              &temperature_threshold));
    ASSERT_EQ (45, temperature_threshold);

    ASSERT_EQ(STD_ERR_OK, sdi_temperature_threshold_get(r_hdl, SDI_HIGH_THRESHOLD, &temperature_threshold));
    ASSERT_EQ(55, temperature_threshold);

    ASSERT_EQ(STD_ERR_OK, sdi_temperature_threshold_get(r_hdl, SDI_CRITICAL_THRESHOLD, &temperature_threshold));
    ASSERT_EQ(65, temperature_threshold);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: to retrieve a temperature sensor status value from the temperature SQL table */
/* PASS: if the temperature sensor status retrieved by the test matches the value set in the test driver */
/* FAIL: if the temperature sensor status retrieved by the test does not match the value set in the test driver*/
TEST(sdi_vm_thermal_unittest, TemperatureSensorStatusValueGet)
{
    bool alert_on = true;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup temperature sensor status
    sdi_db_int_field_set(sdi_get_db_handle(), r_hdl, TABLE_THERMAL_SENSOR,
                         THERMAL_FAULT, (int *)&alert_on);

    //checking for truth value in the temperature sensor status
    ASSERT_EQ (STD_ERR_OK, sdi_temperature_status_get (r_hdl, &alert_on));
    ASSERT_EQ (true, alert_on);
    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    sdi_sys_init();
    e_hdl = sdi_entity_lookup(SDI_ENTITY_SYSTEM_BOARD, 1);
    r_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_TEMPERATURE, "NIC Thermal Sensor");
    sdi_sys_close();

    return RUN_ALL_TESTS();
}

