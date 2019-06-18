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
 * filename: sdi_bmc_fan.c
 */


/******************************************************************************
 * Implements the driver for fan controller accessed through smart fusion.
 *
 ******************************************************************************/

#include "sdi_driver_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_fan_internal.h"
#include "sdi_bmc_internal.h"
#include "sdi_bmc_db.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_device_common.h"
#include "sdi_bus.h"
#include "std_bit_ops.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*Register and chip init function declarations for the sdi_bmc_fan driver*/
static t_std_error sdi_bmc_fan_register(std_config_node_t node, void *bus_handle,
                                sdi_device_hdl_t* device_hdl);

static t_std_error sdi_bmc_fan_chip_init(sdi_device_hdl_t device_hdl);

/*
 * Callback function to retrieve the speed of the fan referred by resource
 * resource_hdl[in] - chip instance is passed as a callback data
 * speed[out]       - pointer to a buffer to get the fan speed
 * Return           - STD_ERR_OK for success or error in case of failure
 */
static t_std_error sdi_bmc_fan_speed_get(sdi_resource_hdl_t real_resource_hdl, void *resource_hdl, uint_t *speed)
{
    sdi_device_hdl_t chip = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_bmc_dev_resource_info_t *tmp_res = NULL;
    sdi_bmc_sensor_t *sensor = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(speed != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;
    tmp_res = (sdi_bmc_dev_resource_info_t *) (chip->private_data);
    STD_ASSERT(tmp_res != NULL);
    if (tmp_res->data_sdr == NULL) {
        tmp_res->data_sdr = sdi_bmc_db_sensor_get_by_name(tmp_res->data_sdr_id);
        if (tmp_res->data_sdr == NULL) {
            return SDI_DEVICE_ERRCODE(EINVAL);
        }
    }
    sensor = tmp_res->data_sdr;
    *speed = sensor->res.reading.data;
    return rc;
}

/*
 * Callback function to retrieve the fault status of the fan referred by resource
 * it will check the stall, spin, drive fail status
 * Parameters:
 * resource_hdl[in] - callback data for this function
 * status[out]      - pointer to a buffer to get the fault status of the fan.
 *                    will be true, if any fault.otherwise false.
 * Return           - STD_ERR_OK for success or the error code in case of failure
 */
static t_std_error sdi_bmc_fan_status_get(void *resource_hdl, bool *status)
{
    sdi_device_hdl_t chip = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_bmc_dev_resource_info_t *tmp_res = NULL;
    sdi_bmc_sensor_t *sensor = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;
    tmp_res = (sdi_bmc_dev_resource_info_t *) (chip->private_data);
    STD_ASSERT(tmp_res != NULL);
    if (!strcmp(tmp_res->status_sdr_id, "")) { // Hack for MtEcho
        *status = false;
        return STD_ERR_OK;
    }
    if (tmp_res->status_sdr == NULL) {
        tmp_res->status_sdr = sdi_bmc_db_sensor_get_by_name(tmp_res->status_sdr_id);
        if (tmp_res->status_sdr == NULL) {
            /**
             * If fan status sensor is not implemented in BMC return 
             * status as good.
             */
            SDI_DEVICE_TRACEMSG_LOG("Fan status sensor not found(%s)", tmp_res->status_sdr_id);
            *status = false;
            return rc;
        }
    }
    sensor = tmp_res->status_sdr;
    if (tmp_res->status_bit == SDI_BMC_INVALID_BIT) {
        *status = ( (sensor->res.reading.discrete_state == 0) ? false : true );
    } else {
        uint32_t reading = sensor->res.reading.discrete_state;
        if (STD_BIT_TEST(reading, tmp_res->status_bit)) {
            *status = true;
        } else {
            *status = false;
        }
    }
    return rc;
}

static fan_ctrl_t sdi_bmc_fan_resource = {
    NULL, /* no fan specific init */
    sdi_bmc_fan_speed_get,
    NULL,
    sdi_bmc_fan_status_get
};

/*
 * Every driver must export function with name sdi_<driver_name>_query_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t *sdi_bmc_fan_entry_callbacks(void)
{
    /* Export Driver table */
    static const sdi_driver_t sdi_bmc_fan_entry = {
        sdi_bmc_fan_register,
        sdi_bmc_fan_chip_init
    };
    return &sdi_bmc_fan_entry;
};

 /*
 * This the call back function for the device registration
 * [in] node - Config node for the device
 * [in] bus_handle - Parent bus handle of the device
 * [out] device_hdl - pointer to the device handle which will be updated by this function
 * Return - STD_ERR_OK, this function is kept as non void as the driver table fp requires it
 */
static t_std_error sdi_bmc_fan_register(std_config_node_t node, void *bus_handle,
                                sdi_device_hdl_t* device_hdl)
{
    return STD_ERR_OK;
}


static t_std_error sdi_bmc_fan_chip_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

t_std_error sdi_bmc_fan_res_register (sdi_device_hdl_t bmc_dev_hdl, sdi_bmc_dev_resource_info_t *bmc_res)
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
    dev_hdl->callbacks = sdi_bmc_fan_entry_callbacks();
    dev_hdl->private_data = bmc_res;
    bmc_res->dev_hdl = dev_hdl;
    bmc_res->data_sdr = sdi_bmc_db_sensor_get_by_name(bmc_res->data_sdr_id);
    bmc_res->status_sdr = sdi_bmc_db_sensor_get_by_name(bmc_res->status_sdr_id);
    sdi_resource_add(bmc_res->resource_type, bmc_res->alias, dev_hdl, &sdi_bmc_fan_resource);
    return STD_ERR_OK;
}
