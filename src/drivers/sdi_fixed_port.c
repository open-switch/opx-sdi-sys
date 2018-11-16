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
 * filename: sdi_fixed_port_c
 */


/******************************************************************************
 * sdi_fixed_port.c
 * Implements the driver for Fixed Ports. This file contains the APIs for Driver
 * registration and init functions. It also contains the functions which are
 * used to control fixed_ports using CPLD pins.
 *
 *****************************************************************************/
#include "sdi_fixed_ports.h"
#include "sdi_media.h"
#include "sdi_media_internal.h"
#include "sdi_media_attr.h"
#include "sdi_resource_internal.h"
#include "sdi_common_attr.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* fixed_port driver init function */
static t_std_error sdi_fixed_port_init (sdi_device_hdl_t device_hdl);

/* register function for fixed_port driver */
static t_std_error sdi_fixed_port_register (std_config_node_t node, void *bus_handle,
                                      sdi_device_hdl_t* device_hdl);

/**
 * Gets the presence status of fixed_port module
 * resource_hdl[in] - Handle of the fixed_port resource
 * pres[out]      - presence status
 * return t_std_error
 */
static t_std_error sdi_fixed_port_presence_get (sdi_resource_hdl_t resource_hdl, bool *pres)
{
    /* fixed ports are always present */
    STD_ASSERT(pres != NULL);
    *pres = true;

    return STD_ERR_OK;
}

/**
 * Set the port LED based on the speed settings of the port
 * resource_hdl[in] - handle to fixed_port
 * channel[in] - Channel number. Should be 0 in fixed_port
 * speed[in] - LED mode setting is derived from speed
 * return - standard t_std_error
 */
t_std_error sdi_fixed_port_led_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                             sdi_media_speed_t speed)
{
    /* Nothing to do */
    return STD_ERR_OK;
}

/* Not yet implemented */
t_std_error sdi_fixed_port_module_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_module_info_t* module_info)
{
    return STD_ERR_OK;
}
t_std_error sdi_fixed_port_port_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_port_info_t* port_info)
{
    return STD_ERR_OK;
}

t_std_error sdi_fixed_port_module_control (sdi_resource_hdl_t resource_hdl,
                                           sdi_media_module_ctrl_type_t ctrl_type, bool enable)
{
    return STD_ERR_OK;
}

t_std_error sdi_fixed_port_control_status_get (sdi_resource_hdl_t resource_hdl,
                                                      sdi_media_module_ctrl_type_t ctrl_type,
                                                      bool *status)
{
    return STD_ERR_OK;
}

/* Callback handlers for fixed_port */
static media_ctrl_t fixed_port_media = {
    .presence_get = sdi_fixed_port_presence_get,
    .module_init = sdi_fixed_port_module_init,
    .module_monitor_status_get = sdi_fixed_port_module_monitor_status_get,
    .channel_monitor_status_get = sdi_fixed_port_channel_monitor_status_get,
    .channel_status_get = sdi_fixed_port_channel_status_get,
    .tx_control = sdi_fixed_port_tx_control,
    .tx_control_status_get = sdi_fixed_port_tx_control_status_get,
    .cdr_status_set = sdi_fixed_port_cdr_status_set,
    .cdr_status_get = sdi_fixed_port_cdr_status_get,
    .speed_get = sdi_fixed_port_speed_get,
    .parameter_get = sdi_fixed_port_parameter_get,
    .vendor_info_get = sdi_fixed_port_vendor_info_get,
    .transceiver_code_get = sdi_fixed_port_transceiver_code_get,
    .threshold_get = sdi_fixed_port_threshold_get,
    .module_monitor_threshold_get = sdi_fixed_port_module_monitor_threshold_get,
    .channel_monitor_threshold_get = sdi_fixed_port_channel_monitor_threshold_get,
    .module_control = sdi_fixed_port_module_control,
    .module_control_status_get = sdi_fixed_port_control_status_get,
    .module_monitor_get = sdi_fixed_port_module_monitor_get,
    .channel_monitor_get = sdi_fixed_port_channel_monitor_get,
    .feature_support_status_get = sdi_fixed_port_feature_support_status_get,
    .led_set = sdi_fixed_port_led_set,
    .read = sdi_fixed_port_read,
    .write = sdi_fixed_port_write,
    .read_generic = sdi_fixed_port_read_generic,
    .write_generic = sdi_fixed_port_write_generic,
    .media_phy_autoneg_set = sdi_fixed_port_phy_autoneg_set,
    .media_phy_speed_set = sdi_fixed_port_phy_speed_set,
    .media_phy_mode_set = sdi_fixed_port_phy_mode_set,
    .wavelength_set = sdi_fixed_port_wavelength_set,
    .media_phy_link_status_get = sdi_fixed_port_phy_link_status_get,
    .media_phy_power_down_enable = sdi_fixed_port_phy_power_down_enable,
    .ext_rate_select = sdi_fixed_port_ext_rate_select,
    .media_phy_serdes_control = sdi_fixed_port_phy_serdes_control,
    .media_qsa_adapter_type_get = sdi_fixed_port_qsa_adapter_type_get,
    .media_port_info_get = sdi_fixed_port_port_info_get,
    .media_module_info_get = sdi_fixed_port_module_info_get

};

/*
 * Every driver must export function with name sdi_<driver_name>_query_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t * sdi_fixed_port_entry_callbacks(void)
{
    /*Export Driver table*/
    static const sdi_driver_t fixed_port_entry = {
        sdi_fixed_port_register,
        sdi_fixed_port_init
    };

    return &fixed_port_entry;
}

/**
 * initialize the device
 * device_hdl[in] - Handle to the device
 * return         - t_std_error
 */
static t_std_error sdi_fixed_port_init (sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

/*
 * The configuration file format for fixed_port node is as follows
 *  fixed_port instance="<port_number>"
 *  addr="<i2c address for fixed_port>"
 *  mod_sel_bus="<pin group bus name for selecting module>"
 *  mod_sel_value="<pin value which needs to be wriiten on pin group bus for selecting module>"
 *  mod_pres_bus="<pin group bus name for knowing the presence status of fixed_port>"
 *  mod_pres_bitmask="<presence check bit number for this instance of fixed_port on mod_pres_bus>"
 *  mod_tx_control_bus="<pin group bus name for tx control>"
 *  mod_tx_control_bitmask="<tx control bit number for this instance of fixed_port on mod_tx_control_bus>"
 *  mod_rx_los_bus="<pin group bus name for getting rx los(loss of signal)>"
 *  mod_rx_los_bitmask="<rx los bit number for this instance of fixed_port on mod_rx_los_bus>"
 *  mod_tx_fault_bus="<pin group bus name for getting tx fault>"
 *  mod_tx_fault_bitmak="<tx fault bit number for this instance of fixed_port on mod_tx_fault_bus>"
 */

/**
 * Register function for fixed_port devices
 * node[in]         - Device node from configuration file
 * bus_handle[in]   - Parent bus handle of the device
 * device_hdl[out]  - Device handle which is filled by this function
 * return           - t_std_error
 */
static t_std_error sdi_fixed_port_register (std_config_node_t node, void *bus_handle,
                                     sdi_device_hdl_t* device_hdl)
{
    sdi_device_hdl_t dev_hdl = NULL;
    sdi_fixed_port_device_t *fixed_port_data = NULL;
    char *node_attr = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t*)bus_handle)->bus_type == SDI_I2C_BUS); /* Need to add fake bus alternative to i2c */

    dev_hdl = calloc(sizeof(sdi_device_entry_t), 1);
    STD_ASSERT(dev_hdl != NULL);

    fixed_port_data = calloc(sizeof(sdi_fixed_port_device_t), 1);
    STD_ASSERT(fixed_port_data != NULL);

    dev_hdl->bus_hdl = bus_handle;
    dev_hdl->callbacks = sdi_fixed_port_entry_callbacks();

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(node_attr != NULL);
    dev_hdl->instance = strtoul(node_attr, NULL, 0);
    snprintf(dev_hdl->alias, SDI_MAX_NAME_LEN, "fixed_port-%u", dev_hdl->instance);

    node_attr = std_config_attr_get(node, SDI_MEDIA_PORT_DEFAULT_SPEED_MBPS);
    STD_ASSERT(node_attr != NULL);
    fixed_port_data->capability = strtoul(node_attr, NULL, 0);
    fixed_port_data->port_info.port_density = 1;
    fixed_port_data->port_info.max_port_speed_mbps = fixed_port_data->capability;

    node_attr = std_config_attr_get(node, SDI_MEDIA_PORT_TYPE);
    fixed_port_data->port_info.port_type = SDI_MEDIA_PORT_TYPE_RJ45;
    if (node_attr != NULL) {
        if (strcmp(node_attr, SDI_PORT_TYPE_BACKPLANE) == 0) {
            fixed_port_data->port_info.port_type =  SDI_MEDIA_PORT_TYPE_BACKPLANE;
        } else if (strcmp(node_attr, SDI_PORT_TYPE_RJ45) == 0) {
            fixed_port_data->port_info.port_type = SDI_MEDIA_PORT_TYPE_RJ45;
        }
    }

    dev_hdl->private_data = (void *)fixed_port_data;

    sdi_resource_add(SDI_RESOURCE_MEDIA, dev_hdl->alias, (void *)dev_hdl,
                     &fixed_port_media);

    *device_hdl = dev_hdl;

    return STD_ERR_OK;
}

