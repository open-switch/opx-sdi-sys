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
 * filename: sdi_bmc_tmp.c
 */


/******************************************************************************
 * Implements the driver for temperature sensor chip accessed through BMC.
 * This chip has only one sensor, so it exports only one resource to the
 * resource framework.The chip provides register interface for setting high
 * and low temperature threshold limits
 *
 ******************************************************************************/

#include "sdi_driver_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_bmc_internal.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_device_common.h"
#include "sdi_thermal_internal.h"
#include "sdi_bmc_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*Register and chip init function declarations for the sdi_bmc_tmp driver*/
t_std_error sdi_bmc_tmp_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl);
t_std_error sdi_bmc_tmp_chip_init(sdi_device_hdl_t device_hdl);

/*
 * Retrieve temperature of the chip refered by resource.
 * This is a callback function for temperature sensor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [out] temperature - pointer to a buffer to ge the temperature of the chip.
 * Return - STD_ERR_OK for success and the respective error code from i2c api in case of failure
 */
static t_std_error sdi_bmc_tmp_temperature_get(void *resource_hdl, int *temperature)
{
    sdi_device_hdl_t chip = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_bmc_dev_resource_info_t *tmp_res = NULL;
    sdi_bmc_sensor_t *sensor = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(temperature != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;
    tmp_res = (sdi_bmc_dev_resource_info_t *) chip->private_data;
    STD_ASSERT(tmp_res != NULL);
    if (tmp_res->data_sdr == NULL) {
        tmp_res->data_sdr = sdi_bmc_db_sensor_get_by_name(tmp_res->data_sdr_id);
        if (tmp_res->data_sdr == NULL) {
            return SDI_DEVICE_ERRCODE(EINVAL);
        }
    }
    sensor = tmp_res->data_sdr;
    *temperature = sensor->res.reading.data;
    return rc;
}

/*
 * Retrieve threshold values of the chip refered by resource.
 * This is a callback function for temperature sensor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [in] type - type of threshold (low or high)
 * [out] temperature - pointer to a buffer to get the temperature threshold value of the chip.
 * Return - STD_ERR_OK for success or EOPNOTSUPP in case of not supported threshold types
 */
static t_std_error sdi_bmc_tmp_threshold_get(void *resource_hdl, sdi_threshold_t type, int *temperature)
{
    sdi_device_hdl_t chip = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_bmc_dev_resource_info_t *tmp_res = NULL;
    sdi_bmc_sensor_t *sensor = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(temperature != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;
    tmp_res = (sdi_bmc_dev_resource_info_t *) chip->private_data;
    STD_ASSERT(tmp_res != NULL);
    if (tmp_res->data_sdr == NULL) {
        tmp_res->data_sdr = sdi_bmc_db_sensor_get_by_name(tmp_res->data_sdr_id);
        if (tmp_res->data_sdr == NULL) {
            return SDI_DEVICE_ERRCODE(EINVAL);
        }
    }
    sensor = tmp_res->data_sdr;

    switch(type)
    {
        case SDI_LOW_THRESHOLD:
            *temperature = sensor->threshold[IPMI_LOWER_NON_RECOVERABLE];
            break;
        case SDI_HIGH_THRESHOLD:
            *temperature = sensor->threshold[IPMI_UPPER_NON_RECOVERABLE];
            break;

        default:
            return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }
    return rc;

}

/*
 * Set the threshold values for a chip refered by resource.
 * This is also a callback function for temperature sensor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [in] type - type of threshold (low or high)
 * [in] temperature - threshold value to be set for a given chip.
 * Return - STD_ERR_OK for success or the respective error code from i2c API in case of failure
 */
static t_std_error sdi_bmc_tmp_threshold_set(void *resource_hdl, sdi_threshold_t type, int temperature)
{
    if((temperature > 255) || (temperature < -256))
    {
        return SDI_DEVICE_ERRCODE(EINVAL);
    }

    return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
}

/*
 * Get the fault status of the chip refered by resource.
 * This is also a callback function for temperature sensor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [in] status - pointer to a buffer to get the status of the chip
 * Return - STD_ERR_OK for success and the respective error code from i2c API in case of failure
 */
static t_std_error sdi_bmc_tmp_status_get(void *resource_hdl, bool *status)
{
    t_std_error rc = STD_ERR_OK;

    *status = false;

    return rc;
}

static temperature_sensor_t sdi_bmc_tmp_sensor={
        NULL, /*As the init is done as part of chip init, resource init is not required*/
        sdi_bmc_tmp_temperature_get,
        sdi_bmc_tmp_threshold_get,
        sdi_bmc_tmp_threshold_set,
        sdi_bmc_tmp_status_get
};

/* Export the Driver table */
static sdi_driver_t sdi_bmc_tmp_entry={
        sdi_bmc_tmp_register,
        sdi_bmc_tmp_chip_init
};


t_std_error sdi_bmc_tmp_register(std_config_node_t node, void *bus_handle,sdi_device_hdl_t* device_hdl)
{
    return STD_ERR_OK;
}


t_std_error sdi_bmc_tmp_chip_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

t_std_error sdi_bmc_tmp_res_register (sdi_device_hdl_t bmc_dev_hdl, sdi_bmc_dev_resource_info_t *bmc_res)
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
    dev_hdl->callbacks = &sdi_bmc_tmp_entry;
    dev_hdl->private_data = bmc_res;
    bmc_res->dev_hdl = dev_hdl;
    bmc_res->data_sdr = sdi_bmc_db_sensor_get_by_name(bmc_res->data_sdr_id);
    sdi_resource_add(bmc_res->resource_type, bmc_res->alias, dev_hdl, &sdi_bmc_tmp_sensor);
    return STD_ERR_OK;
}
