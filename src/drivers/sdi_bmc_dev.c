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
 * filename: sdi_bmc_dev.c
 */

#include "sdi_resource_internal.h"
#include "sdi_driver_internal.h"
#include "sdi_device_common.h"
#include "sdi_bmc_internal.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_bmc.h"
#include "sdi_entity.h"
#include "sdi_entity_info_resource_attr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static t_std_error sdi_bmc_dev_register(std_config_node_t node, void *bus_handle,
                                        sdi_device_hdl_t *device_hdl);
static t_std_error sdi_bmc_dev_init(sdi_device_hdl_t dev_hdl);

/*
 * Every driver must export function with name sdi_<driver_name>_query_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t* sdi_bmc_dev_entry_callbacks(void) {
    /*Export Driver table*/
    static const sdi_driver_t bmc_dev_entry = {
        sdi_bmc_dev_register,
        sdi_bmc_dev_init
    };

    return &bmc_dev_entry;
}

/**
 * Dump BMC device and resource config, its for debugging only.
 */
void sdi_dump_bmc_config_list (sdi_device_hdl_t dev_hdl)
{
    sdi_bmc_dev_t      *bmc_dev = NULL;
    sdi_bmc_dev_list_t *dev_list = NULL;
    uint32_t count;

    if (dev_hdl == NULL) return;

    bmc_dev = (sdi_bmc_dev_t *) dev_hdl->private_data;
    if (bmc_dev == NULL) return;

    dev_list = bmc_dev->dev_list;
    for (count = 0; count < dev_list->count; count++) {
        SDI_DEVICE_ERRMSG_LOG("Resouce type : %u, instance : %u, alias : %s, "
                "data_sdr_id : %s, status_sdr_id : %s, fan_count : %u, "
                "max_fan_speed : %u", dev_list->data[count].resource_type,
                dev_list->data[count].instance, dev_list->data[count].alias,
                dev_list->data[count].data_sdr_id, (dev_list->data[count].status_sdr_id != NULL) ?
                                                    dev_list->data[count].status_sdr_id : "NULL",
                dev_list->data[count].fan_count, dev_list->data[count].max_fan_speed);
    }
}

sdi_bmc_dev_resource_info_t * sdi_bmc_dev_get_by_data_sdr (sdi_device_hdl_t dev_hdl, char *sdr_id)
{
    sdi_bmc_dev_t      *bmc_dev = NULL;
    sdi_bmc_dev_list_t *dev_list = NULL;
    sdi_bmc_dev_resource_info_t *ret = NULL;
    uint32_t count = 0;

    do {
        if ((sdr_id == NULL) || (dev_hdl == NULL)) break;
        bmc_dev = (sdi_bmc_dev_t *) dev_hdl->private_data;
        if (bmc_dev == NULL) break;

        dev_list = bmc_dev->dev_list;
        for (count = 0; count < dev_list->count; count++) {
            if (strcmp(dev_list->data[count].data_sdr_id, sdr_id) == 0) {
                ret = &dev_list->data[count];
                break;
            }
        }
    } while (0);

    return ret;
}

/**
 * sdi_bmc_populate_dev_list will walk through device config and populates
 * device list under BMC device.
 */
sdi_bmc_dev_list_t * sdi_bmc_populate_dev_list(std_config_node_t node)
{
    sdi_bmc_dev_list_t  *dev_list = NULL;
    std_config_node_t   cur_node = NULL;
    uint_t count = 0;

    if (node == NULL) {
        return NULL;
    }

    for (cur_node = std_config_get_child(node); cur_node != NULL;
            cur_node = std_config_next_node(cur_node)) {
        count++;
    }
    if (count == 0) {
        return NULL;
    }

    dev_list = calloc(sizeof(sdi_bmc_dev_list_t) + (count * sizeof(sdi_bmc_dev_resource_info_t)), 1);
    if (dev_list == NULL) {
        return NULL;
    }
    dev_list->count = count;
    const char  *attr = NULL;
    const char  *node_attr = NULL;

    for (count = 0, cur_node = std_config_get_child(node); cur_node != NULL;
            cur_node = std_config_next_node(cur_node)) {
        if ((node_attr = std_config_name_get(cur_node)) == NULL) continue;

        attr = std_config_attr_get(cur_node, SDI_DEV_ATTR_INSTANCE);
        STD_ASSERT(attr != NULL);
        dev_list->data[count].instance = (uint_t) strtoul(attr, NULL, 0);

        attr = std_config_attr_get(cur_node, SDI_DEV_ATTR_ALIAS);
        STD_ASSERT(attr != NULL);
        snprintf(dev_list->data[count].alias,
                sizeof(dev_list->data[count].alias) - 1, "%s", attr);

        attr = std_config_attr_get(cur_node, SDI_BMC_DEV_ATTR_DATA);
        STD_ASSERT(attr != NULL);
        snprintf(dev_list->data[count].data_sdr_id,
                 sizeof(dev_list->data[count].data_sdr_id) - 1, "%s", attr);

        if (strcmp(node_attr, SDI_BMC_DEV_ATTR_FAN) == 0) {
            dev_list->data[count].resource_type = SDI_RESOURCE_FAN;

            attr = std_config_attr_get(cur_node, SDI_BMC_DEV_ATTR_STATUS);
            STD_ASSERT(attr != NULL);
            snprintf(dev_list->data[count].status_sdr_id,
                    sizeof(dev_list->data[count].status_sdr_id) - 1, "%s", attr);
            
        } else if (strcmp(node_attr, SDI_BMC_DEV_ATTR_TEMP) == 0) {
            dev_list->data[count].resource_type = SDI_RESOURCE_TEMPERATURE;

        } else if (strcmp(node_attr, SDI_BMC_DEV_ATTR_EEPROM) == 0) {
            dev_list->data[count].resource_type = SDI_RESOURCE_ENTITY_INFO;

            attr = std_config_attr_get(cur_node, SDI_DEV_ATTR_NO_OF_FANS);
            STD_ASSERT(attr != NULL);
            dev_list->data[count].fan_count = (uint_t) strtoul(attr, NULL, 0);

            attr = std_config_attr_get(cur_node, SDI_DEV_ATTR_MAX_SPEED);
            STD_ASSERT(attr != NULL);
            dev_list->data[count].max_fan_speed = (uint_t) strtoul(attr, NULL, 0);

            attr = std_config_attr_get(cur_node, SDI_BMC_INT_USE_ELM_OFFSET);
            if (attr != NULL) {
                dev_list->data[count].int_use_elm_offset = (uint_t) strtoul(attr, NULL, 0x10);
            } else {
                dev_list->data[count].int_use_elm_offset = SDI_BMC_INVALID_OFFSET;
            }

            attr = std_config_attr_get(cur_node, SDI_BMC_AIRFLOW_OFFSET);
            if (attr != NULL) {
                dev_list->data[count].airflow_offset = (uint_t) strtoul(attr, NULL, 0x10);
            } else {
                dev_list->data[count].airflow_offset = SDI_BMC_INVALID_OFFSET;
            }

            attr = std_config_attr_get(cur_node, SDI_BMC_PSU_TYPE_OFFSET);
            if (attr != NULL) {
                dev_list->data[count].psu_type_offset = (uint_t) strtoul(attr, NULL, 0x10);
            } else {
                dev_list->data[count].psu_type_offset = SDI_BMC_INVALID_OFFSET;
            }
        } else {
            continue;
        }
        ++count;
    }
    return dev_list;
}
/**
 * Register devices which are managed by BMC. Will loop through
 * populated device list and call appropriate resource register functions.
 */

void sdi_bmc_dev_res_register (sdi_device_hdl_t dev_hdl)
{
    sdi_bmc_dev_t      *bmc_dev = NULL;
    sdi_bmc_dev_list_t *dev_list = NULL;
    uint32_t           count = 0;

    STD_ASSERT(dev_hdl != NULL);
    bmc_dev = (sdi_bmc_dev_t *) dev_hdl->private_data;
    dev_list = bmc_dev->dev_list;

    for (count = 0; count < dev_list->count; ++count) {
        if (dev_list->data[count].resource_type == SDI_RESOURCE_FAN) {
            sdi_bmc_fan_res_register(dev_hdl, &dev_list->data[count]);
        } else if (dev_list->data[count].resource_type == SDI_RESOURCE_TEMPERATURE) {
            sdi_bmc_tmp_res_register(dev_hdl, &dev_list->data[count]);
        } else if (dev_list->data[count].resource_type == SDI_RESOURCE_ENTITY_INFO) {
            sdi_bmc_entity_info_res_register(dev_hdl, &dev_list->data[count]);
        }
    }
}

/*
 * Register function for bmc_Dev
 * node[in]         - Device node from configuration file
 * bus_handle[in]   - Parent bus handle of the device
 * device_hdl[out]  - Device handle which is filled by this function
 * return           - t_std_error
 */
static t_std_error sdi_bmc_dev_register(std_config_node_t node, void *bus_handle,
                                        sdi_device_hdl_t *device_hdl) {
    sdi_device_hdl_t    dev_hdl = NULL;
    sdi_bmc_dev_t       *bmc_dev = NULL;
    const char          *attr = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t *)bus_handle)->bus_type == SDI_BMC_IO_BUS);

    dev_hdl = calloc(sizeof(sdi_device_entry_t), 1);
    STD_ASSERT(dev_hdl != NULL);
    dev_hdl->bus_hdl = bus_handle;
    dev_hdl->callbacks = sdi_bmc_dev_entry_callbacks();

    bmc_dev = calloc(sizeof(sdi_bmc_dev_t), 1);
    STD_ASSERT(bmc_dev != NULL);
    attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    if (attr != NULL) {
        bmc_dev->instance = (uint_t) strtoul(attr, NULL, 0);
    }
    attr = std_config_attr_get(node, SDI_DEV_ATTR_ALIAS);
    if (attr != NULL) {
        safestrncpy(bmc_dev->alias, attr, sizeof(bmc_dev->alias)-1);
    }
    bmc_dev->polling_interval = BMC_DEFAULT_POLLING;
    attr = std_config_attr_get(node, SDI_BMC_DEV_ATR_POLLING_INT);
    if (attr != NULL) {
        bmc_dev->polling_interval = (uint_t) strtoul(attr, NULL, 0);
    }

    sdi_resource_add(SDI_RESOURCE_BMC_DEV, "bmc_dev", (void *)dev_hdl, NULL);
    bmc_dev->dev_list = sdi_bmc_populate_dev_list(node);
    dev_hdl->private_data = bmc_dev;
    *device_hdl = dev_hdl;
    sdi_bmc_device_driver_init(dev_hdl);
    sdi_bmc_dev_res_register(dev_hdl);

    return STD_ERR_OK;
}

/*
 * Initialize the device
 * device_hdl[in] - Handle to the device
 * return         - t_std_error
 */
static t_std_error sdi_bmc_dev_init(sdi_device_hdl_t dev_hdl) {

    sdi_bus_hdl_t bus_hdl = (sdi_bus_t *)(dev_hdl->bus_hdl);
    SDI_DEVICE_ERRMSG_LOG("sdi_bmc_dev_init 0x%x, dev_hdl 0x%x", bus_hdl, dev_hdl);
    return STD_ERR_OK;
}




