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
#include <math.h>
#include "gtest/gtest.h"

extern "C" {
#include "sdi_media.h"
#include "sdi_sys_vm.h"
#include "sdi_entity.h"
#include "sdi_db.h"
}

static sdi_resource_hdl_t media_hdl;

TEST(sdi_vm_media_unittest, mediaPresenceGet)
{
    bool presence;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Setup the presence field in the DB */
    presence = true;
    ASSERT_EQ(STD_ERR_OK, sdi_db_int_field_set(sdi_get_db_handle(), media_hdl,
                                TABLE_MEDIA, MEDIA_PRESENCE, (int *)&presence));

    /* Verify that the API returns that the media is present */
    ASSERT_EQ(STD_ERR_OK, sdi_media_presence_get(media_hdl, &presence));
    ASSERT_EQ(true, presence);
    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

TEST(sdi_vm_media_unittest, monitor_status_get)
{
    uint_t status;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Check that our module query returns all flags */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_monitor_status_get(media_hdl, 0xFF, &status));
    ASSERT_EQ(0xFF, status);

    /* Check that we return only the requested flag when queried */
    ASSERT_EQ(STD_ERR_OK, sdi_media_module_monitor_status_get(media_hdl,
            SDI_MEDIA_STATUS_TEMP_HIGH_ALARM, &status));
    ASSERT_EQ(SDI_MEDIA_STATUS_TEMP_HIGH_ALARM, status);

    /* Check that our channel query returns the correct value */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_channel_monitor_status_get(media_hdl, 0, 0xFF, &status));
    ASSERT_EQ(0xFF, status);
}

TEST(sdi_vm_media_unittest, channel_status_get)
{
    uint_t module_status;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Check that our channel query returns RX_LOSS when we query all flags */
    ASSERT_EQ(STD_ERR_OK, sdi_media_channel_status_get(media_hdl, 0, 0xFF, &module_status));
    ASSERT_EQ(SDI_MEDIA_STATUS_RXLOSS, module_status);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, tx_control)
{
    int tx_status;
    bool status;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());
    ASSERT_EQ(STD_ERR_OK, sdi_media_tx_control(media_hdl, 0, false));

    /* Expect that 0 is written into the database */
    sdi_db_media_channel_int_field_get(sdi_get_db_handle(), media_hdl, 0,
                                       MEDIA_TX_ENABLE, &tx_status);
    ASSERT_EQ(0, tx_status);

    ASSERT_EQ(STD_ERR_OK, sdi_media_tx_control(media_hdl, 0, true));

    /* Expect that 1 is written into the database */
    sdi_db_media_channel_int_field_get(sdi_get_db_handle(), media_hdl, 0,
                                       MEDIA_TX_ENABLE, &tx_status);
    ASSERT_EQ(1, tx_status);

    /* Read via the get API and see if we get the same result */
    ASSERT_EQ(STD_ERR_OK, sdi_media_tx_control_status_get(media_hdl, 0, &status));
    ASSERT_TRUE(status);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, opticSpeedGet)
{
    sdi_media_speed_t optic_speed;


    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());
    ASSERT_EQ(STD_ERR_OK, sdi_media_speed_get(media_hdl, &optic_speed));
    ASSERT_EQ(3, optic_speed);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, isDellQualified)
{
    bool dell_qualified;
    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    ASSERT_EQ(STD_ERR_OK, sdi_media_is_dell_qualified(media_hdl, &dell_qualified));
    ASSERT_EQ(true, dell_qualified);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, media_parameter_get)
{
    uint_t value;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Wavelength */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_WAVELENGTH, &value));
    ASSERT_EQ(850, value);

    /* Wavelength tolerance */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_WAVELENGTH_TOLERANCE, &value));
    ASSERT_EQ(2, value);

    /* Max case temp */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_MAX_CASE_TEMP, &value));
    ASSERT_EQ(60, value);

    /* Check code base */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_CC_BASE, &value));
    ASSERT_EQ(123, value);

    /* Check code ext */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_CC_EXT, &value));
    ASSERT_EQ(231, value);

    /* Connector */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_CONNECTOR, &value));
    ASSERT_EQ(1, value);

    /* Encoding type */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_ENCODING_TYPE, &value));
    ASSERT_EQ(0, value);

    /* Nominal bitrate */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_NM_BITRATE, &value));
    ASSERT_EQ(400, value);

    /* Identifier */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_IDENTIFIER, &value));
    ASSERT_EQ(3, value);

    /* Extended Identifier */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_EXT_IDENTIFIER, &value));
    ASSERT_EQ(4, value);

    /* SMF (km) */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_LENGTH_SMF_KM, &value));
    ASSERT_EQ(1, value);

    /* OM1 */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_LENGTH_OM1, &value));
    ASSERT_EQ(1024, value);

    /* OM2 */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_LENGTH_OM2, &value));
    ASSERT_EQ(512, value);

    /* OM3 */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_LENGTH_OM3, &value));
    ASSERT_EQ(128, value);

    /* Cable assembly */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_LENGTH_CABLE_ASSEMBLY, &value));
    ASSERT_EQ(0, value);

    /* SMF */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_LENGTH_SMF, &value));
    ASSERT_EQ(0, value);

    /* Options */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_OPTIONS, &value));
    ASSERT_EQ(0, value);

    /* Enhanced Options */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_ENHANCED_OPTIONS, &value));
    ASSERT_EQ(0, value);

    /* Diag monitoring type */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_DIAG_MON_TYPE, &value));
    ASSERT_EQ(7, value);

    /* Device tech */
    ASSERT_EQ(STD_ERR_OK,
              sdi_media_parameter_get(media_hdl, SDI_MEDIA_DEVICE_TECH, &value));
    ASSERT_EQ(9, value);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, media_vendor_info_get)
{
    char buffer[32];

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Vendor name */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_NAME, buffer, sizeof(buffer)));
    ASSERT_STREQ("Finisar", buffer);

    /* Vendor serial number */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_SN, buffer, sizeof(buffer)));
    ASSERT_STREQ("123456", buffer);

    /* Vendor date code */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_DATE, buffer, sizeof(buffer)));
    ASSERT_STREQ("20150512", buffer);

    /* Vendor part num */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_PN, buffer, sizeof(buffer)));
    ASSERT_STREQ("FTL410QE3C-FC", buffer);

    /* Vendor revision */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_REVISION, buffer, sizeof(buffer)));
    ASSERT_STREQ("4", buffer);

    /* Vendor OUI */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_OUI, buffer, sizeof(buffer)));
    ASSERT_EQ('1', buffer[0]);
    ASSERT_EQ('2', buffer[1]);
    ASSERT_EQ('3', buffer[2]);

    /* Expect failure for a buffer that's too short */
    ASSERT_EQ(STD_ERR(BOARD, PARAM, EINVAL),
        sdi_media_vendor_info_get(media_hdl, SDI_MEDIA_VENDOR_NAME, buffer, 1));

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, transceiver_code)
{
    sdi_media_transceiver_descr_t code;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());
    ASSERT_EQ(STD_ERR_OK, sdi_media_transceiver_code_get(media_hdl, &code));

    ASSERT_EQ(0x01, code.qsfp_descr.sdi_qsfp_eth_1040g_code);
    ASSERT_EQ(0x23, code.qsfp_descr.sdi_qsfp_sonet_code);
    ASSERT_EQ(0x45, code.qsfp_descr.sdi_qsfp_sas_code);
    ASSERT_EQ(0x67, code.qsfp_descr.sdi_qsfp_eth_1g_code);
    ASSERT_EQ(0x09, code.qsfp_descr.sdi_qsfp_fc_distance);
    ASSERT_EQ(0x2AE, code.qsfp_descr.sdi_qsfp_fc_technology);
    ASSERT_EQ(0xCD, code.qsfp_descr.sdi_qsfp_fc_media);
    ASSERT_EQ(0xEF, code.qsfp_descr.sdi_qsfp_fc_speed);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, dell_product_info)
{
    sdi_media_dell_product_info_t dpinfo;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());
    ASSERT_EQ(STD_ERR_OK, sdi_media_dell_product_info_get(media_hdl, &dpinfo));

    /* Check magic key */
    ASSERT_EQ(0x0F, dpinfo.magic_key0);
    ASSERT_EQ(0x10, dpinfo.magic_key1);

    /* Check revision */
    ASSERT_EQ(0x00, dpinfo.revision);

    /* Check product ID */
    ASSERT_EQ(0x11, dpinfo.product_id[0]);
    ASSERT_EQ(0x11, dpinfo.product_id[1]);

    /* Check reserved */
    ASSERT_EQ(0x40, dpinfo.reserved[0]);
    ASSERT_EQ(0x00, dpinfo.reserved[1]);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, module_control)
{
    int en_state;
    bool status;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    en_state = 1;
    sdi_db_int_field_set(sdi_get_db_handle(), media_hdl, TABLE_MEDIA,
                         MEDIA_LP_MODE, &en_state);

    /* Check if the low power state is disabled */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_control_status_get(media_hdl, SDI_MEDIA_LP_MODE, &status));
    ASSERT_TRUE(status);

    sdi_db_int_field_set(sdi_get_db_handle(), media_hdl, TABLE_MEDIA,
                         MEDIA_RESET, &en_state);

    /* Check if the reset state is disabled */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_control_status_get(media_hdl, SDI_MEDIA_RESET, &status));
    ASSERT_TRUE(status);

    /* Write the updated state through the SDI API */
    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_control(media_hdl, SDI_MEDIA_LP_MODE, false));

    sdi_db_int_field_get(sdi_get_db_handle(), media_hdl, TABLE_MEDIA,
                         MEDIA_LP_MODE, &en_state);
    ASSERT_EQ(0, en_state);

    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_control(media_hdl, SDI_MEDIA_RESET, false));

    sdi_db_int_field_get(sdi_get_db_handle(), media_hdl, TABLE_MEDIA,
                         MEDIA_RESET, &en_state);
    ASSERT_EQ(0, en_state);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, module_thresholds)
{
    uint_t threshold;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_monitor_threshold_get(media_hdl,
            SDI_MEDIA_STATUS_TEMP_HIGH_ALARM, &threshold));
    ASSERT_EQ(120, threshold);


    ASSERT_EQ(STD_ERR_OK,
        sdi_media_module_monitor_threshold_get(media_hdl,
            SDI_MEDIA_STATUS_VOLT_LOW_WARNING, &threshold));
    ASSERT_EQ(2, threshold);


    ASSERT_EQ(STD_ERR_OK,
        sdi_media_channel_monitor_threshold_get(media_hdl, 
            SDI_MEDIA_RX_PWR_HIGH_WARNING, &threshold));
    ASSERT_EQ(18, threshold);


    ASSERT_EQ(STD_ERR_OK,
        sdi_media_channel_monitor_threshold_get(media_hdl,
            SDI_MEDIA_TX_BIAS_HIGH_ALARM, &threshold));
    ASSERT_EQ(20, threshold);

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}


TEST(sdi_vm_media_unittest, mediaMonitorGet)
{
    float monitor;
    float diff;

    ASSERT_EQ(STD_ERR_OK, sdi_sys_init());

    /* Check module temperature */
    ASSERT_EQ(STD_ERR_OK, sdi_media_module_monitor_get(media_hdl, SDI_MEDIA_TEMP, &monitor));
    diff = fabs(98.4 - monitor);
    ASSERT_LE(diff, 1e-5); // Floating point comparision (good enough within 0.00001)

    /* Check module voltage */
    ASSERT_EQ(STD_ERR_OK, sdi_media_module_monitor_get(media_hdl, SDI_MEDIA_VOLT, &monitor));
    diff = fabs(6.023 - monitor);
    ASSERT_LE(diff, 1e-5); // Floating point comparision (good enough within 0.00001)

    ASSERT_EQ(STD_ERR_OK, sdi_sys_close());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    sdi_entity_hdl_t e_hdl;

    sdi_sys_init();
    e_hdl = sdi_entity_lookup(SDI_ENTITY_SYSTEM_BOARD, 1);
    media_hdl = sdi_entity_resource_lookup(e_hdl, SDI_RESOURCE_MEDIA, "QSFP 1");

    return RUN_ALL_TESTS();
}

