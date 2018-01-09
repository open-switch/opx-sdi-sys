/*
 * Copyright (c) 2016 Dell Inc.
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
 * filename: sdi_driver_framework.c
 */


/**************************************************************************************
 *  SDI Driver Framework core functionality includes device registration, initialisation,
 *  attach and lookup.
 *  Drivers fall into one of the two broad categories
 *  a. Device-Drivers
 *  b. Bus-Drivers.
 *  Both drivers use similar model to register/load themselves.
 *
 *  Note:
 *  All the internal apis will not check the argument, whether it is valid. Its
 *  caller responsibility to validate it.
 ***************************************************************************************/

#include "sdi_driver_internal.h"
#include "std_llist.h"
#include "std_config_node.h"
#include "std_error_codes.h"
#include "sdi_sys_common.h"
#include "sdi_bus_framework.h"
#include "std_assert.h"
#include "dlfcn.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SDI_BUS_ENTRY_CALLBACK         "%s_entry_callbacks"

/* Master Device node list */
std_dll_head device_list;

/* SDI Device node structure which holds Driver specific datas */
typedef struct sdi_device_node {
    std_dll dll_node; /**< Node to the device*/
    sdi_device_hdl_t device; /**< Driver Specific data */
} sdi_device_node_t;

/* DLL handle for SDI Driver */
static void *dl_hdl = NULL;

/**
 * Get Device Driver's Symbol Address by looking-up sdi device driver
 * library based on symbol name
 * Parameters:
 * symbol_name[in] - Name to look-up in sdi device driver library
 * return symbol's address on success, NULL on failure
 */
static const sdi_driver_t *sdi_get_device_driver(const char *driver_name)
{
    const sdi_driver_t * driver_entry_callbacks = NULL;
    sdi_device_entry_callback query_api = NULL;
    char entry_str[SDI_MAX_NAME_LEN] = {0};

    STD_ASSERT(dl_hdl != NULL);
    STD_ASSERT(driver_name != NULL);
    snprintf(entry_str, sizeof(entry_str), "sdi_%s_entry_callbacks", driver_name);

    query_api = ((sdi_device_entry_callback)dlsym(dl_hdl, entry_str));

    if (query_api == NULL)
    {
        /* TODO : driver lookup by using "<driver>_entry" will be removed after
         * replacing "<diver>_entry" with driver_entry_callbacks for all drivers
         * */
        snprintf(entry_str, sizeof(entry_str), "%s_entry", driver_name);
        driver_entry_callbacks = (sdi_driver_t *)dlsym(dl_hdl, entry_str);
    } else {
        driver_entry_callbacks=query_api();
    }

    STD_ASSERT(driver_entry_callbacks != NULL);
    return driver_entry_callbacks;
}

/**
 * Get Bus Driver's Symbol Address by looking-up sdi device driver
 * library based on bus name
 * Parameters:
 * bus_driver_name[in] - Name to look-up in sdi device driver library
 * return symbol's address on success, NULL on failure
 */
sdi_bus_driver_t *sdi_get_bus_driver(const char *bus_driver_name)
{
    char entry_str[SDI_MAX_NAME_LEN] = {0};
    sdi_bus_entry_callback api_name = NULL;
    sdi_bus_driver_t *bus_entry_callbacks = NULL;

    STD_ASSERT(dl_hdl != NULL);
    STD_ASSERT(bus_driver_name != NULL);
    snprintf(entry_str, sizeof(entry_str), SDI_BUS_ENTRY_CALLBACK, bus_driver_name);

    api_name = ((sdi_bus_entry_callback)dlsym(dl_hdl, entry_str));
    if (api_name != NULL) {
        bus_entry_callbacks = api_name();
    }
    STD_ASSERT(bus_entry_callbacks != NULL);

    return bus_entry_callbacks;
}

/**
 * sdi_add_device
 * Add device to the device list
 * Parameters:
 * hdl[in] - device to add
 * return NULL
 */
static void sdi_add_device(sdi_device_hdl_t hdl)
{
    sdi_device_node_t *node = NULL;

    node = (sdi_device_node_t *)calloc(sizeof(sdi_device_node_t),1);
    STD_ASSERT(node != NULL);
    node->device = hdl;
    std_dll_insertatfront(&device_list, (std_dll *)node);

}

/**
 * Register the specified device.
 *
 * Parameters:
 * *node[in]      - pointer to the node configuration data.
 * *bus_hdl[in]   - Handle to the bus service provider.
 * *dev_hdl[out]  - Handle to registered device is filled in *dev_hdl if
 * dev_hdl is not NULL. Buses not interested in tracking device handle can pass
 * NULL.
 * returns - Error if node or bus is not valid
 */
t_std_error sdi_register_driver(std_config_node_t node, sdi_bus_hdl_t bus_hdl, sdi_device_hdl_t *device_hdl)
{
    t_std_error error = STD_ERR_OK;
    const char *driver_name = NULL;
    const sdi_driver_t *driver = NULL;
    sdi_device_hdl_t dev_hdl = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_hdl != NULL);

    driver_name = std_config_name_get(node);
    STD_ASSERT(driver_name != NULL);

    driver = sdi_get_device_driver(driver_name);
    STD_ASSERT(driver != NULL);

    error = driver->register_fn(node, bus_hdl, &dev_hdl);
    if (error == STD_ERR_OK) {
        sdi_add_device(dev_hdl);
        if (device_hdl != NULL) {
            *device_hdl = dev_hdl;
        }
    }
    return error;
}

/**
 * Register the specified devices.
 *
 * Parameters:
 * driver_cfg_file[in] - pointer to the node configuration data.
 * returns - Error if node or bus is not valid
 */
void sdi_register_drivers(const char * driver_cfg_file)
{
    std_config_hdl_t cfg_hdl = NULL;
    std_config_node_t root =  NULL;

    STD_ASSERT(driver_cfg_file != NULL);
    cfg_hdl = std_config_load(driver_cfg_file);
    STD_ASSERT(cfg_hdl != NULL);
    root =  std_config_get_root(cfg_hdl);
    STD_ASSERT(root != NULL);

    std_dll_init(&device_list);

    dl_hdl = dlopen(SDI_DRIVER_LIB, RTLD_LAZY);
    STD_ASSERT(dl_hdl != NULL);

    /**
     * bus registration will register device driver nodes
     */
    sdi_bus_register_node(root);
    /* Unload the config file handle since configuration
     * parsing is done */
    std_config_unload(cfg_hdl);
}

/**
 * sdi_device_init
 * initialize the specific device
 * Parameters:
 * device[in] -  device to init
 * return Error, if device is not initialized
 */
t_std_error sdi_device_init(sdi_device_hdl_t device)
{
    STD_ASSERT(device != NULL);
    return device->callbacks->init(device);
}
