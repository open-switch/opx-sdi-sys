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
 * filename: sdi_bmc_io_bus.c
 */

#include "sdi_device_common.h"
#include "sdi_bmc_io_bus.h"
#include "sdi_bus_attr.h"
#include "sdi_common_attr.h"
#include "std_assert.h"
#include "std_utils.h"

#include <stdlib.h>

/**
 * BMC IO bus driver init function to initialize devices associated with this
 * IO bus.
 */
static t_std_error bmc_io_bus_driver_init(sdi_bus_hdl_t bus_hdl)
{
    t_std_error rc = STD_ERR_OK;
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(bus_hdl->bus_type == SDI_BMC_IO_BUS);

    sdi_bus_init_device_list(bus_hdl);
    SDI_DEVICE_TRACEMSG_LOG("bmc_io_bus_driver_init bus_hdl : 0x%x", bus_hdl);
    return rc;
}

/**
 * BMC IO bus driver registration function to use register with sdi io bus
 * infrastructure.
 */
static t_std_error bmc_io_bus_driver_register(std_config_node_t node,
                                             sdi_bus_hdl_t *bus_hdl)
{
    char *node_attr = NULL;
    t_std_error error = STD_ERR_OK;
    sdi_bmc_io_bus_hdl_t bmc_bus_hdl = NULL;
    sdi_bmc_io_bus_t *bmc_bus = (sdi_bmc_io_bus_t *)calloc(sizeof(sdi_bmc_io_bus_t), 1);

    STD_ASSERT(bmc_bus != NULL);
    bmc_bus_hdl = bmc_bus;
    bmc_bus_hdl->bus.bus_init = bmc_io_bus_driver_init;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_BUS_NAME);
    STD_ASSERT(node_attr != NULL);
    safestrncpy(bmc_bus_hdl->bus.bus_name, node_attr, SDI_MAX_NAME_LEN);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(node_attr != NULL);
    bmc_bus_hdl->bus.bus_id = (uint_t) strtoul (node_attr, NULL, 0);

    std_mutex_lock_init_non_recursive(&(bmc_bus_hdl->lock));
    bmc_bus_hdl->bus.bus_type = SDI_BMC_IO_BUS;
    error = sdi_bus_register((sdi_bus_hdl_t )bmc_bus);
    if (error != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("%s:%d registration failed %s with error %d",
                              __FUNCTION__, __LINE__, bmc_bus_hdl->bus.bus_name, error);
        free(bmc_bus);
        return error;
    }
    *bus_hdl = (sdi_bus_hdl_t ) bmc_bus;
    sdi_bus_register_device_list(node, (sdi_bus_hdl_t) bmc_bus);

    return error;
}

/**
 * BMC IO bus entry callback info
 */
const sdi_bus_driver_t *bmc_io_bus_entry_callbacks(void)
{
    /* Export bus driver table */
    static const sdi_bus_driver_t bmc_io_bus_entry = {
        .bus_register = bmc_io_bus_driver_register,
        .bus_init = bmc_io_bus_driver_init
    };
    return &bmc_io_bus_entry;
}
