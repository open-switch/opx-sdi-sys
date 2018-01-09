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
#include "sdi_entity.h"
#include "sdi_sys_vm.h"
#include "sdi_db.h"
}

/* TEST: test to get the entity count from the SQL table */
/* PASS: if the entity count matches the correct number of entries in the SQL table */
/* FAIL: if the entity count does not match the number of entities in the SQL table */
TEST(sdi_vm_entity_info_unittest, getEntityCount)
{
    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    ASSERT_EQ(2, sdi_entity_count_get(SDI_ENTITY_PSU_TRAY));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: test to lookup the entity from the SQL table */
/* PASS: if the entity handle value matches the handle value corresponding to the instance and entity type specified */
/* FAIL: incase of an entity handle value mismatch*/
TEST(sdi_vm_entity_info_unittest, entityLookup)
{
    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());
    // Ensure that we return an entity handle of 4 for a lookup of
    // the first fan-tray entity
    ASSERT_EQ((sdi_entity_hdl_t) 4, sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1 ));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to check for entity presence from the SQL table */
/* PASS: if the entity is not present */
/* FAIL: incase of entity presence */
TEST(sdi_vm_entity_info_unittest, entityPresenceGet)
{
    bool presence;
    sdi_entity_hdl_t e_hdl;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());
    //setup to make the entity absent
    e_hdl = sdi_entity_lookup (SDI_ENTITY_PSU_TRAY, 2);

    sdi_db_entity_presence_set(sdi_get_db_handle(), e_hdl, false);

    //check for entity's absence
    ASSERT_EQ (STD_ERR_OK, sdi_entity_presence_get (e_hdl, &presence));
    ASSERT_FALSE (presence);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: test to check for entity fault status from the SQL table */
/* PASS: if the fault status is true */
/* FAIL: if the fault status is false */
TEST(sdi_vm_entity_info_unittest, faultStatusGet)
{
    bool fault;
    sdi_entity_hdl_t e_hdl;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    //setup fault status to true
    e_hdl = sdi_entity_lookup (SDI_ENTITY_PSU_TRAY, 2);
    sdi_db_entity_fault_status_set(sdi_get_db_handle(), e_hdl, true);
    ASSERT_EQ (STD_ERR_OK, sdi_entity_fault_status_get (e_hdl, &fault));

    //check for truth value of fault status
    ASSERT_EQ (true, fault);
    ASSERT_EQ (STD_ERR_OK, sdi_sys_close ());
}


/* TEST: test to count the number of resources from the SQL table */
/* PASS: if count value is consistent with the database entries */
/* FAIL: if count value is inconsistent with the database entries*/
TEST (sdi_vm_entity_info_unittest, resourceCountGet)
{
    sdi_entity_hdl_t e_hdl;
    ASSERT_EQ (STD_ERR_OK, sdi_sys_init ());

    /* Does the first fan tray have 2 fans? */
    e_hdl = sdi_entity_lookup (SDI_ENTITY_FAN_TRAY, 1);
    ASSERT_EQ (2, sdi_entity_resource_count_get (e_hdl, SDI_RESOURCE_FAN));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: test to lookup for resources with their ALIAS names */
/* PASS: if the returned resource handle is consistent with the database entries */
/* FAIL: if the returned resource handle not consistent with the database entries */
TEST(sdi_vm_entity_info_unittest, entityResourceLookup)
{
    sdi_entity_hdl_t e_hdl;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());
    /* Lookup the fan resource with the alias "PSU Fan" in the first PSU tray */
    e_hdl = sdi_entity_lookup(SDI_ENTITY_PSU_TRAY, 1);
    ASSERT_EQ((sdi_resource_hdl_t) 20,
        sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_FAN, "PSU Fan"));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

/* TEST: test to get the entity info */
/* PASS: if the entity info for each entity matches the expected values */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, entity_info_get)
{
    sdi_entity_hdl_t e_hdl;
    sdi_resource_hdl_t r_hdl;
    sdi_entity_info_t info;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Find the system board and retrieve it's entity info */
    e_hdl = sdi_entity_lookup(SDI_ENTITY_SYSTEM_BOARD, 1);
    r_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_ENTITY_INFO, "Info");

    ASSERT_EQ(STD_ERR_OK, sdi_entity_info_read(r_hdl, &info));
    /* Verify the individual fields */
    ASSERT_EQ(0, strcmp(info.prod_name, "S6000 System Board"));
    ASSERT_EQ(0, strcmp(info.hw_revision, "REV_1"));
    ASSERT_EQ(0, strcmp(info.ppid, "1")); // Test value in DB, not real PPID
    ASSERT_EQ(0, strcmp(info.platform_name, "S6000"));
    ASSERT_EQ(0, strcmp(info.vendor_name, "DELL"));
    ASSERT_EQ(0, strcmp(info.service_tag, "S6KSVC"));
    ASSERT_EQ(1, info.mac_size);
    ASSERT_EQ(0, info.num_fans);
    ASSERT_EQ(0, info.max_speed);
    ASSERT_EQ(0, info.power_rating);

    /* Find the first PSU and retrieve it's entity info */
    e_hdl = sdi_entity_lookup(SDI_ENTITY_PSU_TRAY, 1);
    r_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_ENTITY_INFO, "Info");

    ASSERT_EQ(STD_ERR_OK, sdi_entity_info_read(r_hdl, &info));
    /* Verify the individual fields */
    ASSERT_EQ(0, strcmp(info.prod_name, "Power Supply"));
    ASSERT_EQ(0, strcmp(info.hw_revision, "REV_1"));
    ASSERT_EQ(0, strcmp(info.ppid, "1")); // Test value in DB, not real PPID
    ASSERT_EQ(0, strcmp(info.platform_name, "S6000"));
    ASSERT_EQ(0, strcmp(info.vendor_name, "DELL"));
    ASSERT_EQ(0, strcmp(info.service_tag, "S6KPSU"));
    ASSERT_EQ(0, info.mac_size);
    ASSERT_EQ(1, info.num_fans);
    ASSERT_EQ(19000, info.max_speed);
    ASSERT_EQ(48, info.power_rating);
    ASSERT_FALSE(info.power_type.ac_power);
    ASSERT_TRUE(info.power_type.dc_power);

    /* Find the first fantray and retrieve it's entity info */
    e_hdl = sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1);
    r_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_ENTITY_INFO, "Info");

    ASSERT_EQ(STD_ERR_OK, sdi_entity_info_read(r_hdl, &info));
    /* Verify the individual fields */
    ASSERT_EQ(0, strcmp(info.prod_name, "Fan Tray"));
    ASSERT_EQ(0, strcmp(info.hw_revision, "REV_1"));
    ASSERT_EQ(0, strcmp(info.ppid, "1")); // Test value in DB, not real PPID
    ASSERT_EQ(0, strcmp(info.platform_name, "S6000"));
    ASSERT_EQ(0, strcmp(info.vendor_name, "DELL"));
    ASSERT_EQ(0, strcmp(info.service_tag, "S6KFTY"));
    ASSERT_EQ(0, info.mac_size);
    ASSERT_EQ(2, info.num_fans);
    ASSERT_EQ(18000, info.max_speed);
    ASSERT_EQ(0, info.power_rating);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to reset the entity */
/* PASS: if the db contains the reset type previously set by the SDI API */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, entity_reset)
{
    sdi_entity_hdl_t hdl = 0;
    sdi_resource_hdl_t info_hdl = 0;
    int value;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    hdl = sdi_entity_lookup(SDI_ENTITY_SYSTEM_BOARD, 1);

    info_hdl = sdi_entity_resource_lookup(hdl, SDI_RESOURCE_ENTITY_INFO, "Info");

    ASSERT_EQ(STD_ERR_OK, sdi_entity_reset(hdl, WARM_RESET));

    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_get(sdi_get_db_handle(),
                                               info_hdl,
                                               TABLE_INFO,
                                               INFO_ENTITY_RESET,
                                               &value));

    ASSERT_EQ(WARM_RESET, value);


    ASSERT_EQ(STD_ERR_OK, sdi_entity_reset(hdl, COLD_RESET));

    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_get(sdi_get_db_handle(),
                                               info_hdl,
                                               TABLE_INFO,
                                               INFO_ENTITY_RESET,
                                               &value));

    ASSERT_EQ(COLD_RESET, value);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to check the power control for the entity */
/* PASS: if the db contains the power state previously set by the SDI API */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, entity_power)
{
    sdi_entity_hdl_t hdl = 0;
    sdi_resource_hdl_t info_hdl = 0;
    int value;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    hdl = sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1);
    info_hdl = sdi_entity_resource_lookup(hdl, SDI_RESOURCE_ENTITY_INFO, "Info");

    ASSERT_EQ(STD_ERR_OK, sdi_entity_power_status_control(hdl, false));
    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_get(sdi_get_db_handle(),
                                               info_hdl,
                                               TABLE_INFO,
                                               INFO_ENTITY_POWER,
                                               &value));
    ASSERT_FALSE(value);

    ASSERT_EQ(STD_ERR_OK, sdi_entity_power_status_control(hdl, true));
    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_get(sdi_get_db_handle(),
                                               info_hdl,
                                               TABLE_INFO,
                                               INFO_ENTITY_POWER,
                                               &value));
    ASSERT_TRUE(value);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to check the initialization for the entity */
/* PASS: if the db contains the expected values */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, entity_init)
{
    sdi_entity_hdl_t hdl = 0;
    sdi_resource_hdl_t info_hdl = 0;
    int value;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    hdl = sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1);
    info_hdl = sdi_entity_resource_lookup(hdl, SDI_RESOURCE_ENTITY_INFO, "Info");

    ASSERT_EQ(STD_ERR_OK, sdi_entity_init(hdl));

    /* Make sure that the power value is true */
    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_get(sdi_get_db_handle(),
                                               info_hdl,
                                               TABLE_INFO,
                                               INFO_ENTITY_POWER,
                                               &value));
    ASSERT_TRUE(value);

    /* Make sure the reset type is COLD_RESET */
    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_get(sdi_get_db_handle(),
                                               info_hdl,
                                               TABLE_INFO,
                                               INFO_ENTITY_RESET,
                                               &value));
    ASSERT_EQ(COLD_RESET, value);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to check the entity name */
/* PASS: if the db contains the expected values */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, entity_name)
{
    sdi_entity_hdl_t hdl = 0;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    hdl = sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1);
    ASSERT_STREQ("S6000 Fan Tray", sdi_entity_name_get(hdl));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to check the entity type */
/* PASS: if the db contains the expected values */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, entity_type)
{
    sdi_entity_hdl_t hdl = 0;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    hdl = sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1);
    ASSERT_EQ(SDI_ENTITY_FAN_TRAY, sdi_entity_type_get(hdl));

    hdl = sdi_entity_lookup(SDI_ENTITY_PSU_TRAY, 1);
    ASSERT_EQ(SDI_ENTITY_PSU_TRAY, sdi_entity_type_get(hdl));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


/* TEST: test to check the resource type and alias */
/* PASS: if the returned value matches the expected type and alias */
/* FAIL: Any mismatch from the expected values */
TEST(sdi_vm_entity_info_unittest, resource_type_alias)
{
    sdi_entity_hdl_t e_hdl;
    sdi_resource_hdl_t r_hdl;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    e_hdl = sdi_entity_lookup(SDI_ENTITY_FAN_TRAY, 1);
    r_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_FAN, "Fantray Fan 1");

    ASSERT_EQ(SDI_RESOURCE_FAN, sdi_resource_type_get(r_hdl));

    ASSERT_STREQ("Fantray Fan 1", sdi_resource_alias_get(r_hdl));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

