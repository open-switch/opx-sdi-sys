/*
 * Copyright (c) 2018 Dell Inc.
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
 * filename: sdi_bmc_entity_info.c
 */


/******************************************************************************
 *  Driver implementation for EEPROM (PSU eeprom, FAN eeprom) accessible through
 *  SmartFusion Mailbox
 ******************************************************************************/

#include "sdi_driver_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_device_common.h"
#include "sdi_eeprom.h"
#include "std_assert.h"
#include "sdi_bmc_db.h"
#include "sdi_bus_framework.h"
#include "sdi_bmc_internal.h"
#include "std_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static t_std_error sdi_bmc_entity_info_register(std_config_node_t node,
                                               void *bus_handle,
                                               sdi_device_hdl_t* device_hdl);
static t_std_error sdi_bmc_entity_info_init(sdi_device_hdl_t device_hdl);

/*
 * Every driver must export function with name sdi_<driver_name>_query_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t *sdi_bmc_entity_info_entry_callbacks(void)
{
    /* Export Driver table */
    static const sdi_driver_t sdi_bmc_entity_info_entry = {
        sdi_bmc_entity_info_register,
        sdi_bmc_entity_info_init
    };
    return &sdi_bmc_entity_info_entry;
};

t_std_error sdi_bmc_entity_info_data_get(void *resource_hdl,
                                        sdi_entity_info_t *entity_info)
{
    t_std_error rc = STD_ERR_OK;
    sdi_device_hdl_t chip = NULL;
    sdi_bmc_dev_resource_info_t *tmp_res = NULL;
    sdi_bmc_sensor_t *sensor = NULL;

    chip = (sdi_device_hdl_t) resource_hdl;
    STD_ASSERT(chip != NULL);
    STD_ASSERT(entity_info != NULL);
    tmp_res = (sdi_bmc_dev_resource_info_t *) (chip->private_data);
    STD_ASSERT(tmp_res != NULL);
    if (tmp_res->data_sdr == NULL) {
        tmp_res->data_sdr = sdi_bmc_db_sensor_get_by_name(tmp_res->data_sdr_id);
        if (tmp_res->data_sdr == NULL) {
            return SDI_DEVICE_ERRCODE(EINVAL);
        }
    }
    sensor = tmp_res->data_sdr;
    safestrncpy(entity_info->vendor_name, sensor->res.entity_info.board_manufacturer,
            sizeof(entity_info->vendor_name));
    safestrncpy(entity_info->prod_name, sensor->res.entity_info.board_product_name,
            sizeof(entity_info->prod_name));
    safestrncpy(entity_info->part_number, sensor->res.entity_info.board_part_number,
            sizeof(entity_info->part_number));
    entity_info->part_number[SDI_DELL_PN_SIZE] = '\0';

    safestrncpy(entity_info->hw_revision, &sensor->res.entity_info.board_part_number[SDI_DELL_PN_SIZE],
            sizeof(entity_info->hw_revision));
    safestrncpy(entity_info->ppid, sensor->res.entity_info.board_serial_number,
            sizeof(entity_info->ppid));

    entity_info->num_fans = sensor->res.entity_info.num_fans;
    entity_info->max_speed = sensor->res.entity_info.max_speed;

    if (sensor->entity_id == IPMI_ENTITY_ID_POWER_SUPPLY) {
        safestrncpy(entity_info->ppid + SDI_DELL_CN_SIZE, entity_info->part_number,
                sizeof(entity_info->ppid) - SDI_DELL_CN_SIZE);
        safestrncpy(entity_info->ppid + SDI_MGID_OFFSET, &sensor->res.entity_info.board_serial_number[SDI_DELL_CN_SIZE],
                sizeof(entity_info->ppid) - SDI_DELL_CN_SIZE - SDI_DELL_PN_SIZE);
    }
    return rc;
}

static entity_info_t bmc_entity_info_callback = {
    NULL,
    sdi_bmc_entity_info_data_get
};

static t_std_error sdi_bmc_entity_info_register(std_config_node_t node,
                                               void *bus_handle,
                                               sdi_device_hdl_t* device_hdl)
{
    return STD_ERR_OK;
}

/**
 * Does the initialization for the eeprom device
 * param[in] device_hdl - device handle of the specific device
 * return: STD_ERR_OK
 */
static t_std_error sdi_bmc_entity_info_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

t_std_error sdi_bmc_entity_info_res_register (sdi_device_hdl_t bmc_dev_hdl, sdi_bmc_dev_resource_info_t *bmc_res)
{
    sdi_device_hdl_t dev_hdl = NULL;
    void *bus_handle = NULL;

    STD_ASSERT(bmc_dev_hdl != NULL);
    STD_ASSERT(bmc_res != NULL);

    bus_handle = bmc_dev_hdl->bus_hdl;
    STD_ASSERT(((sdi_bus_t *)bus_handle)->bus_type == SDI_BMC_IO_BUS);

    dev_hdl = calloc(sizeof(sdi_device_entry_t), 1);
    STD_ASSERT(dev_hdl != NULL);
    dev_hdl->bus_hdl = bus_handle;
    dev_hdl->callbacks = sdi_bmc_entity_info_entry_callbacks();
    dev_hdl->private_data = bmc_res;
    bmc_res->dev_hdl = dev_hdl;
    bmc_res->data_sdr = sdi_bmc_db_sensor_get_by_name(bmc_res->data_sdr_id);
    bmc_res->data_sdr->res.entity_info.num_fans = bmc_res->fan_count;
    bmc_res->data_sdr->res.entity_info.max_speed = bmc_res->max_fan_speed;
    sdi_resource_add(bmc_res->resource_type, bmc_res->alias, dev_hdl, &bmc_entity_info_callback);
    return STD_ERR_OK;
}
