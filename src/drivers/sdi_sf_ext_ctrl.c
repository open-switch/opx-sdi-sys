/*
 * Copyright (c) 2018 Dell EMC..
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
 * filename: sdi_sf_ext_ctrl.c
 */

#include "sdi_driver_internal.h"
#include "sdi_entity.h"
#include "sdi_resource_internal.h"
#include "sdi_smartfusion_io_bus_api.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_device_common.h"
#include "sdi_ext_ctrl_internal.h"
#include "sdi_ext_ctrl_resource_attr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Smartfusion Temperature sensors device private data
 */
typedef struct sdi_sf_ext_ctrl_device
{
    uint16_t ctrl_value_offset;
    uint16_t width;
    uint16_t step;

    /* Default sensor limits */
    uint16_t default_ctrl_value_offset;
    uint16_t default_width;
    uint16_t default_step;

    bool ctrl_value_offset_valid;
    bool width_valid;
    bool step_valid;

    uint64_t io_bus_failure;
    uint64_t write_failure;
} sdi_sf_ext_ctrl_device_t;

#define SMF_EXT_CTRL_REGISTER_STEP_ALLOWED      (1)
#define SMF_EXT_CTRL_REGISTER_TWO_BYTE_WIDTH    (2)

/*Register and chip init function declarations for the sdi_sf_ext_ctrl driver*/
t_std_error sdi_sf_ext_ctrl_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl);
t_std_error sdi_sf_ext_ctrl_chip_init(sdi_device_hdl_t device_hdl);

static t_std_error sdi_sf_ext_ctrl_data_get(void *resource_hdl, uint16_t offset,
                                            uint16_t width, uint16_t step, int *data, int *size)
{
    uint8_t byte_ext_ctrl_value = 0;
    sdi_device_hdl_t chip = NULL;
    sdi_sf_ext_ctrl_device_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(data != NULL);
    STD_ASSERT(width <= SMF_EXT_CTRL_REGISTER_TWO_BYTE_WIDTH);
    STD_ASSERT(step  == SMF_EXT_CTRL_REGISTER_STEP_ALLOWED);

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_sf_ext_ctrl_device_t*)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    rc = sdi_sf_io_acquire_bus(chip->bus_hdl);
    if(rc != STD_ERR_OK)
    {
        ext_ctrl_data->io_bus_failure++;
        SDI_DEVICE_ERRMSG_LOG("io acquire bus failed with rc=0x%x\n", rc);
        return rc;
    }

    do {
    	rc = sdi_sf_io_bus_read_byte(chip->bus_hdl, offset, &byte_ext_ctrl_value);
    	if(rc != STD_ERR_OK)
    	{
    	    SDI_DEVICE_ERRMSG_LOG("io bus read first byte failed with rc=0x%x\n", rc);
    	    break;
    	}
    	*data = (int)byte_ext_ctrl_value;

    	if (width == SMF_EXT_CTRL_REGISTER_TWO_BYTE_WIDTH) {
    	    *data = (*data) << 8;

    	    rc = sdi_sf_io_bus_read_byte(chip->bus_hdl,(offset + step), &byte_ext_ctrl_value);
    	    if(rc != STD_ERR_OK)
    	    {
    	        SDI_DEVICE_ERRMSG_LOG("io bus read low byte failed with rc=0x%x\n", rc);
    	        break;
    	    }

    	    *data |= (int)byte_ext_ctrl_value;
    	}   
    } while (0);
    sdi_sf_io_release_bus(chip->bus_hdl);

    if (byte_ext_ctrl_value == 0xFF) {
        rc = SDI_DEVICE_ERRCODE(EINVAL);
        SDI_DEVICE_ERRMSG_LOG("io bus ext_ctrl data not ready, error %d\n", rc);
        return SDI_DEVICE_ERRCODE(EINVAL);
    }
    
    *data = *data / 10; /* Scale to degree in Celcius */
    *size = 1;

    return rc;
}

static t_std_error sdi_sf_ext_ctrl_data_set(void *resource_hdl, uint16_t offset,
                                            uint16_t width, uint16_t step, int *data, int size)
{
    uint8_t byte_ext_ctrl_value = 0;
    sdi_device_hdl_t chip = NULL;
    sdi_sf_ext_ctrl_device_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint16_t n_iter = width/step;

    STD_ASSERT(data != NULL);
    STD_ASSERT(width <= SMF_EXT_CTRL_REGISTER_TWO_BYTE_WIDTH);
    STD_ASSERT(size  == 1); /* expect all data fit into one int at a time */

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_sf_ext_ctrl_device_t*)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    rc = sdi_sf_io_acquire_bus(chip->bus_hdl);
    if(rc != STD_ERR_OK)
    {
        ext_ctrl_data->io_bus_failure++;
        SDI_DEVICE_ERRMSG_LOG("io acquire bus failed with rc=0x%x\n", rc);
        return rc;
    }

    do {
    	/* Smart Fusion only handle byte write at this moment
    	 */
    	if (n_iter > 2) {
    	    SDI_DEVICE_ERRMSG_LOG("invalid width %d or step %d", width, step);
    	    break;	    
    	}

    	/* SMF expects temperatures in tenths of degree Celsius, so multiply input value by 10 */
    	*data = *data * 10;

    	if (n_iter == 2) {
    	    byte_ext_ctrl_value = ((*data) >> 8) & 0xFF;
    	    rc = sdi_sf_io_bus_write_byte(chip->bus_hdl, offset, byte_ext_ctrl_value);
    	    if(rc != STD_ERR_OK) {
    	        ext_ctrl_data->write_failure++;
    	        SDI_DEVICE_ERRMSG_LOG("ext ctrl write high byte 0x%x failed with rc=0x%x\n",
    	                              byte_ext_ctrl_value, rc);
    	        break;
    	    }

    	    byte_ext_ctrl_value = (*data) & 0xFF;
    	    rc = sdi_sf_io_bus_write_byte(chip->bus_hdl, offset + step, byte_ext_ctrl_value);
    	    if(rc != STD_ERR_OK) {
    	        ext_ctrl_data->write_failure++;
    	        SDI_DEVICE_ERRMSG_LOG("ext ctrl write low byte 0x%x failed with rc=0x%x\n",
    	                                byte_ext_ctrl_value, rc);
    	        break;
    	    } 
    	} else { /* n_iter == 1 */
    	    byte_ext_ctrl_value = (*data) & 0xFF;
    	    rc = sdi_sf_io_bus_write_byte(chip->bus_hdl, offset, byte_ext_ctrl_value);
    	    if(rc != STD_ERR_OK) {
    	        ext_ctrl_data->write_failure++;
    	        SDI_DEVICE_ERRMSG_LOG("ext ctrl write low byte 0x%x failed with rc=0x%x\n",
    	                              byte_ext_ctrl_value, rc);
    	        break;
    	    } 
    	}   
    } while (0);
    sdi_sf_io_release_bus(chip->bus_hdl);
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
static t_std_error sdi_sf_ext_ctrl_get(void *resource_hdl, int *ext_ctrl, int *size)
{
    sdi_device_hdl_t chip = NULL;
    sdi_sf_ext_ctrl_device_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_sf_ext_ctrl_device_t*)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    rc = sdi_sf_ext_ctrl_data_get(resource_hdl, ext_ctrl_data->ctrl_value_offset,
                                  ext_ctrl_data->width, ext_ctrl_data->step, ext_ctrl, size);
    if(rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("get value failed with rc=0x%x\n", rc);
        return rc;
    }

    return rc;
}

static t_std_error sdi_sf_ext_ctrl_set(void *resource_hdl, int *ext_ctrl, int size)
{
    sdi_device_hdl_t chip = NULL;
    sdi_sf_ext_ctrl_device_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_sf_ext_ctrl_device_t*)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    rc = sdi_sf_ext_ctrl_data_set(resource_hdl, ext_ctrl_data->ctrl_value_offset,
                                  ext_ctrl_data->width, ext_ctrl_data->step, ext_ctrl, size);
    if(rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("set value failed with rc=0x%x\n", rc);
        return rc;
    }

    return rc;
}


static ext_ctrl_t sdi_sf_ext_ctrl = {
    NULL,
    sdi_sf_ext_ctrl_get,
    sdi_sf_ext_ctrl_set,
};
                    
const sdi_driver_t *sdi_sdi_sf_ext_ctrl_entry_callbacks (void)
{
    /* Export the Driver table */
    static const sdi_driver_t sdi_sf_ext_ctrl_entry = {
        sdi_sf_ext_ctrl_register,
	sdi_sf_ext_ctrl_chip_init
    };

    return &sdi_sf_ext_ctrl_entry;
};

t_std_error sdi_sf_ext_ctrl_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl)
{
    char *node_attr = NULL;
    sdi_device_hdl_t chip = NULL;
    sdi_sf_ext_ctrl_device_t *ext_ctrl_data = NULL;
    char name[SDI_MAX_NAME_LEN];

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t*)bus_handle)->bus_type == SDI_SF_IO_BUS);

    chip = calloc(sizeof(sdi_device_entry_t),1);
    STD_ASSERT(chip != NULL);

    ext_ctrl_data = calloc(sizeof(sdi_sf_ext_ctrl_device_t),1);
    STD_ASSERT(ext_ctrl_data != NULL);

    ext_ctrl_data->ctrl_value_offset_valid = 
      ext_ctrl_data->width_valid = 
      ext_ctrl_data->step_valid = false;

    ext_ctrl_data->default_ctrl_value_offset = 0;
    ext_ctrl_data->default_width = ext_ctrl_data->default_step = 1;

    ext_ctrl_data->ctrl_value_offset = ext_ctrl_data->default_ctrl_value_offset;
    ext_ctrl_data->width = ext_ctrl_data->default_width;
    ext_ctrl_data->step = ext_ctrl_data->default_step;

    chip->bus_hdl = bus_handle;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    if (node_attr != NULL) {
        chip->instance = (uint_t) strtoul(node_attr, NULL, 0);
    } else {
        chip->instance = 0;
    }

    chip->callbacks = sdi_sdi_sf_ext_ctrl_entry_callbacks();
    chip->private_data = (void*)ext_ctrl_data;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ALIAS);
    if (node_attr == NULL)
    {
        snprintf(chip->alias,SDI_MAX_NAME_LEN,"sdi_sf_ext_ctrl-%d", chip->instance );
    }
    else
    {
        safestrncpy(chip->alias,node_attr,SDI_MAX_NAME_LEN);
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

    // The name in the entity reference is the key to find the ext ctrl
    sdi_resource_add(SDI_RESOURCE_EXT_CONTROL, name, (void*)chip,
                     &sdi_sf_ext_ctrl);
    *device_hdl = chip;

    return STD_ERR_OK;
}

t_std_error sdi_sf_ext_ctrl_chip_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}
