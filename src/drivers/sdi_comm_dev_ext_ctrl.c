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
 * filename: sdi_comm_dev_ext_ctrl.c
 */

#include "sdi_comm_dev.h"
#include "sdi_comm_dev_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_comm_dev_attr.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_device_common.h"
#include "sdi_i2c_bus_api.h"
#include "std_error_codes.h"
#include "std_assert.h"

#include "sdi_ext_ctrl_internal.h"
#include "sdi_ext_ctrl_resource_attr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Register and chip init function declarations for the sdi_comm_dev_ext_ctrl driver*/
t_std_error sdi_comm_dev_ext_ctrl_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl);
t_std_error sdi_comm_dev_ext_ctrl_chip_init(sdi_device_hdl_t device_hdl);

static t_std_error sdi_comm_dev_ext_ctrl_data_get(sdi_resource_hdl_t comm_dev, uint16_t offset,
                                                  uint16_t width, uint16_t step, int *data, int *size)
{
    t_std_error rc = STD_ERR_OK;
    STD_ASSERT(data != NULL);
    STD_ASSERT(width == SDI_COMM_DEV_MAX_CTRL_DATA_LEN);
    uint8_t buffer[SDI_COMM_DEV_MAX_CTRL_DATA_LEN];
    uint16_t n_byte = width;

    rc = sdi_comm_dev_read(comm_dev, COMM_DEV_I2C_ADDR, offset, n_byte, &buffer[0]);
    if (STD_ERR_OK != rc) {
        SDI_DEVICE_ERRMSG_LOG("Error reading ext ctrl rc=0x%x\n", rc);
    }
    
    *data = (int)buffer[1];
    *data <<= 8;
    *data |= (int)buffer[0];

    *data /= 10; /* Scale to degree in Celcius */

    *size = 1;

    return rc;
}

static t_std_error sdi_comm_dev_ext_ctrl_data_set(sdi_resource_hdl_t comm_dev, uint16_t offset,
                                                  uint16_t width, uint16_t step, int *data, int size,
                                                  bool refresh)
{
    t_std_error rc = STD_ERR_OK;
    STD_ASSERT(width == SDI_COMM_DEV_MAX_CTRL_DATA_LEN);
    STD_ASSERT(data != NULL);
    STD_ASSERT(comm_dev != NULL);
    STD_ASSERT(size == 1);
    uint8_t buffer[SDI_COMM_DEV_MAX_CTRL_DATA_LEN];
    uint16_t* p_temperature = (uint16_t*) &buffer[0];
    uint16_t n_byte = width;

    /* Comm_Dev expects temperatures in tenths of degree Celsius, so multiply input value by 10 */
    *p_temperature = (uint16_t) ((*data) * 10);

    if (refresh) {
        /* refresh comm dev register values if there is a dynamic reset */
        rc = sdi_comm_dev_reset_refresh(comm_dev);
        if (STD_ERR_OK != rc) {
            SDI_DEVICE_ERRMSG_LOG("Error refeshing comm dev reset refresh status rc=0x%x\n", rc);
            return rc;
        }  
    }

    rc = sdi_comm_dev_write(comm_dev, COMM_DEV_I2C_ADDR, offset, n_byte, &buffer[0]);
    if (STD_ERR_OK != rc) {
        SDI_DEVICE_ERRMSG_LOG("Error writing ext ctrl rc=0x%x refresh %d\n",
                              rc, refresh);
    }

    return rc;
}

/*
 * Retrieve ext ctrl of the chip refered by resource.
 * This is a callback function for ext ctrl resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [out] ext_ctrl - pointer to a buffer to ge the ext ctrl from the chip.
 * [out] size - len of the output in unit of sizeof(int)
 * Return - STD_ERR_OK for success and the respective error code from i2c api in case of failure
 */
static t_std_error sdi_comm_dev_ext_ctrl_get(void *resource_hdl, int *ext_ctrl, int *size)
{
    sdi_device_hdl_t chip = NULL;
    sdi_comm_dev_ext_ctrl_device_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_resource_hdl_t comm_dev = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_comm_dev_ext_ctrl_device_t*)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    comm_dev = ext_ctrl_data->comm_dev;
    STD_ASSERT(comm_dev != NULL);

    rc = sdi_comm_dev_ext_ctrl_data_get(comm_dev, ext_ctrl_data->ctrl_value_offset,
                                  ext_ctrl_data->width, ext_ctrl_data->step, ext_ctrl, size);
    if(rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("get value at offset 0x%x failed with rc=0x%x\n",
                              ext_ctrl_data->ctrl_value_offset, rc);
        return rc;
    }

    return rc;
}

static t_std_error sdi_comm_dev_ext_ctrl_set(void *resource_hdl, int *ext_ctrl, int size)
{
    sdi_device_hdl_t chip = NULL;
    sdi_comm_dev_ext_ctrl_device_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_resource_hdl_t comm_dev = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_comm_dev_ext_ctrl_device_t*)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    comm_dev = ext_ctrl_data->comm_dev;
    STD_ASSERT(comm_dev != NULL);

    rc = sdi_comm_dev_ext_ctrl_data_set(comm_dev, ext_ctrl_data->ctrl_value_offset,
                                        ext_ctrl_data->width, ext_ctrl_data->step,
                                        ext_ctrl, size, ext_ctrl_data->refresh);

    if(rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("set offs 0x%x value 0x%x failed with rc=0x%x\n",
                              ext_ctrl_data->ctrl_value_offset, *ext_ctrl, rc);
        return rc;
    }

    return rc;
}

static ext_ctrl_t sdi_comm_dev_ext_ctrl = {
    NULL,
    sdi_comm_dev_ext_ctrl_get,
    sdi_comm_dev_ext_ctrl_set,
};

                    
const sdi_driver_t *sdi_comm_dev_ext_ctrl_entry_callbacks (void)
{
    /* Export the Driver table */
    static const sdi_driver_t sdi_comm_dev_ext_ctrl_entry = {
        sdi_comm_dev_ext_ctrl_register,
        sdi_comm_dev_ext_ctrl_chip_init
    };

    return &sdi_comm_dev_ext_ctrl_entry;
};

t_std_error sdi_comm_dev_ext_ctrl_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl)
{
    char *node_attr = NULL;
    sdi_device_hdl_t chip = NULL;
    sdi_comm_dev_ext_ctrl_device_t *ext_ctrl_data = NULL;
    char name[SDI_MAX_NAME_LEN];

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t *)bus_handle)->bus_type == SDI_I2C_BUS);

    chip = calloc(sizeof(sdi_device_entry_t),1);
    STD_ASSERT(chip != NULL);

    ext_ctrl_data = calloc(sizeof(sdi_comm_dev_ext_ctrl_device_t),1);
    STD_ASSERT(ext_ctrl_data != NULL);

    ext_ctrl_data->ctrl_value_offset_valid = 
      ext_ctrl_data->width_valid = 
      ext_ctrl_data->refresh_valid =
      ext_ctrl_data->step_valid = false;

    ext_ctrl_data->default_ctrl_value_offset = ext_ctrl_data->default_refresh = 0;
    ext_ctrl_data->default_width = ext_ctrl_data->default_step = 1;

    ext_ctrl_data->ctrl_value_offset = ext_ctrl_data->default_ctrl_value_offset;
    ext_ctrl_data->width = ext_ctrl_data->default_width;
    ext_ctrl_data->step = ext_ctrl_data->default_step;
    ext_ctrl_data->refresh = ext_ctrl_data->default_refresh;

    chip->bus_hdl = bus_handle;

    ext_ctrl_data->comm_dev = sdi_find_resource_by_name("comm_dev");
    STD_ASSERT(ext_ctrl_data->comm_dev != NULL);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    if (node_attr != NULL) {
        chip->instance = (uint_t) strtoul(node_attr, NULL, 0);
    } else {
        chip->instance = 0;
    }

    chip->callbacks = sdi_comm_dev_ext_ctrl_entry_callbacks();
    chip->private_data = (void*)ext_ctrl_data;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ALIAS);
    if (node_attr == NULL)
    {
        snprintf(chip->alias,SDI_MAX_NAME_LEN,"comm_dev_ext_ctrl-%d", chip->instance);
    }
    else
    {
        safestrncpy(chip->alias,node_attr, SDI_MAX_NAME_LEN);
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_NAME);
    if (node_attr != NULL)
    {
        safestrncpy(name, node_attr, SDI_MAX_NAME_LEN);
    } else {
        safestrncpy(name, chip->alias, SDI_MAX_NAME_LEN);
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_EXT_CTRL_VAL_OFFSET);
    if(node_attr != NULL)
    {
        ext_ctrl_data->ctrl_value_offset = (uint16_t) strtol(node_attr, NULL, 0);
        ext_ctrl_data->ctrl_value_offset_valid = true;
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_EXT_CTRL_WIDTH);
    if(node_attr != NULL)
    {
        ext_ctrl_data->width = (uint16_t) strtol(node_attr, NULL, 0);
        ext_ctrl_data->width_valid = true;
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_EXT_CTRL_STEP);
    if(node_attr != NULL)
    {
        ext_ctrl_data->step = (uint16_t) strtol(node_attr, NULL, 0);
        ext_ctrl_data->step_valid = true;
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_EXT_CTRL_REFRESH);
    if(node_attr != NULL)
    {
        ext_ctrl_data->refresh = (uint16_t) strtol(node_attr, NULL, 0);
        ext_ctrl_data->refresh_valid = true;
    }

    sdi_resource_add(SDI_RESOURCE_EXT_CONTROL, name, (void*)chip,
                     &sdi_comm_dev_ext_ctrl);

    *device_hdl = chip;

    return STD_ERR_OK;
}

t_std_error sdi_comm_dev_ext_ctrl_chip_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}
