/*
 * Copyright (c) 2019 Dell EMC..
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
 * filename: sdi_bmc_oem_ext_ctrl.c
 */

#include "sdi_driver_internal.h"
#include "sdi_entity.h"
#include "sdi_resource_internal.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_device_common.h"
#include "sdi_ext_ctrl_internal.h"
#include "sdi_bmc_internal.h"
#include "sdi_bmc.h"
#include "sdi_common_attr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const sdi_driver_t *sdi_bmc_oem_ext_ctrl_entry_callbacks (void);

static t_std_error bmc_oem_ext_ctrl_set(void *resource_hdl, int *ext_ctrl, int size)
{
    sdi_device_hdl_t chip = NULL;
    sdi_bmc_oem_cmd_info_t *ext_ctrl_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ext_ctrl_data = (sdi_bmc_oem_cmd_info_t *)chip->private_data;
    STD_ASSERT(ext_ctrl_data != NULL);

    rc = sdi_bmc_oem_cmd_execute(ext_ctrl_data, (uint8_t *)ext_ctrl);
    if(rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("bmc_oem_ext_ctrl : set value failed with rc=0x%x\n", rc);
        return rc;
    }

    return rc;
}

static ext_ctrl_t bmc_oem_ext_ctrl = {
    NULL,
    NULL,
    bmc_oem_ext_ctrl_set,
};

t_std_error bmc_oem_ext_ctrl_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl)
{
    char *node_attr = NULL;
    sdi_device_hdl_t chip = NULL;
    sdi_bmc_oem_cmd_info_t *ext_ctrl_data = NULL;
    char name[SDI_MAX_NAME_LEN];

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t*)bus_handle)->bus_type == SDI_BMC_IO_BUS);

    chip = calloc(sizeof(sdi_device_entry_t),1);
    STD_ASSERT(chip != NULL);

    ext_ctrl_data = calloc(sizeof(sdi_bmc_oem_cmd_info_t),1);
    STD_ASSERT(ext_ctrl_data != NULL);


    chip->bus_hdl = bus_handle;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    if (node_attr != NULL) {
        chip->instance = (uint_t) strtoul(node_attr, NULL, 0);
    } else {
        chip->instance = 0;
    }

    chip->callbacks = sdi_bmc_oem_ext_ctrl_entry_callbacks();
    chip->private_data = (void*)ext_ctrl_data;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ALIAS);
    if (node_attr == NULL)
    {
        snprintf(chip->alias,SDI_MAX_NAME_LEN,"bmc_oem_ext_ctrl-%d", chip->instance );
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


    sdi_bmc_populate_oem_cmd_info(node, ext_ctrl_data);
    
    // The name in the entity reference is the key to find the ext ctrl
    sdi_resource_add(SDI_RESOURCE_EXT_CONTROL, name, (void*)chip,
                     &bmc_oem_ext_ctrl);
    *device_hdl = chip;

    return STD_ERR_OK;
}   

t_std_error bmc_oem_ext_ctrl_chip_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

const sdi_driver_t *sdi_bmc_oem_ext_ctrl_entry_callbacks (void)
{
    /* Export the Driver table */
    static const sdi_driver_t bmc_oem_ext_ctrl_entry = {
        bmc_oem_ext_ctrl_register,
        bmc_oem_ext_ctrl_chip_init
    };

    return &bmc_oem_ext_ctrl_entry;
};
