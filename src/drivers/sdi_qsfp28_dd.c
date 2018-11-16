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
 * filename: sdi_qsfp28_dd.c
 */


/******************************************************************************
 * sdi_qsfp28_dd.c
 * Implements the driver for Quad Small Form-factor Pluggable (QSFP)
 *
 *****************************************************************************/
#include "sdi_resource_internal.h"
#include "sdi_common_attr.h"
#include "sdi_device_common.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_media.h"
#include "sdi_qsfp28_dd.h"
#include "sdi_qsfp.h"
#include "sdi_media_internal.h"
#include "sdi_media_attr.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* qsfp28_dd driver init function */
static t_std_error sdi_qsfp28_dd_init (sdi_device_hdl_t device_hdl);

/* register function for qsfp28_dd driver */
static t_std_error sdi_qsfp28_dd_register (std_config_node_t node, void *bus_handle,
                                      sdi_device_hdl_t* device_hdl);

/**
 * Gets the presence status of qsfp28_dd module
 * For  QSFP28-DD, the presence state of port with a non-zero rank sub-port is dependent on the media inserted
 * Hence we need to check what kind of module is inserted (by reading the eeprom), and then publish the pseudo-state.
 * resource_hdl[in] - Handle of the qsfp28_dd resource
 * pres[out]      - presence status
 * return t_std_error
 */
static t_std_error sdi_qsfp28_dd_presence_get (sdi_resource_hdl_t resource_hdl, bool *pres)
{
    sdi_device_hdl_t qsfp28_dd_device = NULL;
    qsfp28_dd_device_t *qsfp28_dd_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t id = 0;


    STD_ASSERT(resource_hdl != NULL);

    qsfp28_dd_device = (sdi_device_hdl_t)resource_hdl;
    qsfp28_dd_priv_data = (qsfp28_dd_device_t *)qsfp28_dd_device->private_data;
    STD_ASSERT(qsfp28_dd_priv_data != NULL);

    rc = sdi_qsfp_presence_get(resource_hdl, pres);

    if (rc == STD_ERR_OK){
        if (pres && (qsfp28_dd_priv_data->port_info.sub_port_rank > 0)) {
            rc = sdi_qsfp28_dd_parameter_get(resource_hdl, SDI_MEDIA_IDENTIFIER, &id);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("Could not read EEPROM media id for pseudo-presence detection of module on %s",
                                   qsfp28_dd_device->alias);
                *pres = false;
            }
            /* If module is not double density, presence is false */
            if (id != SDI_MEDIA_DOUBLE_DENSITY_MODULE_ID) {
                *pres = false;
            }
        }
    }
    return rc;
}

/**
 * Enable/Disable the module control parameters like low power mode and reset
 * control
 * resource_hdl[in] - handle of the resource
 * ctrl_type[in]    - module control type(LP mode/reset)
 * enable[in]       - "true" to enable and "false" to disable
 * return           - standard t_std_error
 */
static t_std_error sdi_qsfp28_dd_module_control(sdi_resource_hdl_t resource_hdl,
                                           sdi_media_module_ctrl_type_t ctrl_type, bool enable)
{
    return sdi_qsfp_module_control(resource_hdl, ctrl_type, enable);
}

/**
 * Get the status of module control parameters like low power mode and reset
 * status
 * resource_hdl[in] - handle of the resource
 * ctrl_type[in]    - module control type(LP mode/reset)
 * status[out]      - "true" if enabled else "false"
 * return           - standard t_std_error
 */
static t_std_error sdi_qsfp28_dd_module_control_status_get(sdi_resource_hdl_t resource_hdl,
                                                      sdi_media_module_ctrl_type_t ctrl_type,
                                                      bool *status)
{
    return sdi_qsfp_module_control_status_get(resource_hdl, ctrl_type, status);
}

/* Not yet implemented */

t_std_error sdi_qsfp28_dd_module_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_module_info_t* module_info)
{
    return STD_ERR_OK;
}
t_std_error sdi_qsfp28_dd_port_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_port_info_t* port_info)
{
    return STD_ERR_OK;
}


/* Callback handlers for QSFP */
static media_ctrl_t qsfp28_dd_media = {
    .presence_get = sdi_qsfp28_dd_presence_get,
    .module_init = sdi_qsfp28_dd_module_init,
    .module_monitor_status_get = sdi_qsfp28_dd_module_monitor_status_get,
    .channel_monitor_status_get = sdi_qsfp28_dd_channel_monitor_status_get,
    .channel_status_get = sdi_qsfp28_dd_channel_status_get,
    .tx_control = sdi_qsfp28_dd_tx_control,
    .tx_control_status_get = sdi_qsfp28_dd_tx_control_status_get,
    .cdr_status_set = sdi_qsfp28_dd_cdr_status_set,
    .cdr_status_get = sdi_qsfp28_dd_cdr_status_get,
    .speed_get = sdi_qsfp28_dd_speed_get,
    .parameter_get = sdi_qsfp28_dd_parameter_get,
    .vendor_info_get = sdi_qsfp28_dd_vendor_info_get,
    .transceiver_code_get = sdi_qsfp28_dd_transceiver_code_get,
    .threshold_get = sdi_qsfp28_dd_threshold_get,
    .module_monitor_threshold_get = sdi_qsfp28_dd_module_monitor_threshold_get,
    .channel_monitor_threshold_get = sdi_qsfp28_dd_channel_monitor_threshold_get,
    .module_control = sdi_qsfp28_dd_module_control,
    .module_control_status_get = sdi_qsfp28_dd_module_control_status_get,
    .module_monitor_get = sdi_qsfp28_dd_module_monitor_get,
    .channel_monitor_get = sdi_qsfp28_dd_channel_monitor_get,
    .feature_support_status_get = sdi_qsfp28_dd_feature_support_status_get,
    .read = sdi_qsfp28_dd_read,
    .write = sdi_qsfp28_dd_write,
    .read_generic = sdi_qsfp28_dd_read_generic,
    .write_generic = sdi_qsfp28_dd_write_generic,
    .media_phy_autoneg_set = sdi_qsfp28_dd_phy_autoneg_set,
    .media_phy_speed_set = sdi_qsfp28_dd_phy_speed_set,
    .media_phy_mode_set = sdi_qsfp28_dd_phy_mode_set,
    .wavelength_set = sdi_qsfp28_dd_wavelength_set,
    .media_phy_link_status_get = sdi_qsfp28_dd_phy_link_status_get,     /* Added for QSA support */
    .media_phy_power_down_enable = sdi_qsfp28_dd_phy_power_down_enable, /* Added for QSA support */
    .ext_rate_select = sdi_qsfp28_dd_ext_rate_select,
    .media_phy_serdes_control = sdi_qsfp28_dd_phy_serdes_control,         /* Added for QSA support */
    .media_qsa_adapter_type_get = sdi_qsfp28_dd_qsa_adapter_type_get,   /* QSA info get */
    .media_port_info_get = sdi_qsfp28_dd_port_info_get,
    .media_module_info_get = sdi_qsfp28_dd_module_info_get

};

/*
 * Every driver must export function with name sdi_<driver_name>_query_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t * sdi_qsfp28_dd_entry_callbacks(void)
{
    /*Export Driver table*/
    static const sdi_driver_t qsfp28_dd_entry = {
        sdi_qsfp28_dd_register,
        sdi_qsfp28_dd_init
    };

    return &qsfp28_dd_entry;
}

/**
 * initialize the device
 * device_hdl[in] - Handle to the device
 * return         - t_std_error
 */
static t_std_error sdi_qsfp28_dd_init (sdi_device_hdl_t device_hdl)
{
    /* Need to move out of reset as part of QSFP init and same is handled as
     * part of parent bus init in config file. Hence just return OK from here */
    return STD_ERR_OK;
}

/*
 * The configuration file format for QSFP(sff-8436) node is as follows
 *  qsfp28_dd instance="<port_number>"
 *  addr="<i2c address for qsfp28_dd>"
 *  mod_sel_bus="<pin group bus name for selecting module>"
 *  mod_sel_value="<pin value which needs to be wriiten on pin group bus for selecting module>"
 *  mod_pres_bus="<pin group bus name for knowing the presence status of qsfp28_dd>"
 *  mod_pres_bitmask="<presence check bit number for this instance of qsfp28_dd on mod_pres_bus>"
 *  mod_reset_bus="<pin group bus name for setting reset mode>"
 *  mod_reset_bitmask="<reset bit number for this instance of qsfp28_dd on mod_reset_bus>"
 *  mod_lpmode_bus="<pin group bus name for setting low power mode>"
 *  mod_lpmode_bitmask="<lp mode for this instance of qsfp28_dd on mod_lpmode_bus>"
 *  mod_sel_delay="<delay in milli seconds, time to be wait after selecting module" />
 */

/**
 * Register function for QSFP devices
 * node[in]         - Device node from configuration file
 * bus_handle[in]   - Parent bus handle of the device
 * device_hdl[out]  - Device handle which is filled by this function
 * return           - t_std_error
 */
static t_std_error sdi_qsfp28_dd_register (std_config_node_t node, void *bus_handle,
                                      sdi_device_hdl_t* device_hdl)
{
    sdi_device_hdl_t dev_hdl = NULL;
    qsfp28_dd_device_t *qsfp28_dd_data = NULL;
    char *node_attr = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t*)bus_handle)->bus_type == SDI_I2C_BUS);

    dev_hdl = calloc(sizeof(sdi_device_entry_t), 1);
    STD_ASSERT(dev_hdl != NULL);

    qsfp28_dd_data = calloc(sizeof(qsfp28_dd_device_t), 1);
    STD_ASSERT(qsfp28_dd_data != NULL);

    dev_hdl->bus_hdl = bus_handle;
    dev_hdl->callbacks = sdi_qsfp28_dd_entry_callbacks();

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ADDRESS);
    STD_ASSERT(node_attr != NULL);
    dev_hdl->addr.i2c_addr.i2c_addr = (i2c_addr_t) strtoul(node_attr, NULL, 16);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(node_attr != NULL);
    dev_hdl->instance = strtoul(node_attr, NULL, 0);
    snprintf(dev_hdl->alias, SDI_MAX_NAME_LEN, "qsfp28_dd-%u", dev_hdl->instance);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MUX_SELECTION_BUS);
    if (node_attr != NULL) {
        qsfp28_dd_data->mux_sel_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MUX_SELECTION_VALUE);
    if (node_attr != NULL) {
        qsfp28_dd_data->mux_sel_value = strtoul(node_attr, NULL, 0);
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_SELECTION_BUS);
    STD_ASSERT(node_attr != NULL);
    if(strncmp(node_attr, SDI_MEDIA_MODULE_SEL_ALWAYS_ENABLED,
                SDI_MEDIA_MODULE_SEL_ALWAYS_ENABLED_STRLEN) == 0 ) {
        /* Only one module present on the channel */
        qsfp28_dd_data->mod_sel_hdl = NULL;
        qsfp28_dd_data->mod_sel_value = 0;
    } else {
        qsfp28_dd_data->mod_sel_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);
        node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_SELECTION_VALUE);
        STD_ASSERT(node_attr != NULL);
        qsfp28_dd_data->mod_sel_value = strtoul(node_attr, NULL, 0);
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_PORT_TYPE);
    qsfp28_dd_data->capability = SDI_MEDIA_SPEED_200G;
    qsfp28_dd_data->port_info.port_type = SDI_MEDIA_PORT_TYPE_QSFP28_DD_1;
    qsfp28_dd_data->port_info.port_density = 2;
    qsfp28_dd_data->port_info.sub_port_rank = 0;

    if (node_attr != NULL) {
        if (strcmp(node_attr, SDI_PORT_TYPE_QSFP28_DD_1) == 0) {
             qsfp28_dd_data->capability = SDI_MEDIA_SPEED_200G; /* need to change to 100G */
             qsfp28_dd_data->port_info.port_type = SDI_MEDIA_PORT_TYPE_QSFP28_DD_1;
             qsfp28_dd_data->port_info.port_density = 2;
             qsfp28_dd_data->port_info.sub_port_rank = 0;
        } else if (strcmp(node_attr, SDI_PORT_TYPE_QSFP28_DD_2) == 0) {
             qsfp28_dd_data->capability = SDI_MEDIA_SPEED_200G;  /* need to change to 100G */
             qsfp28_dd_data->port_info.port_type = SDI_MEDIA_PORT_TYPE_QSFP28_DD_2;
             qsfp28_dd_data->port_info.port_density = 2;
             qsfp28_dd_data->port_info.sub_port_rank = 1;
        }

    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_SUB_PORT_CHANNEL_OFFSET);
    qsfp28_dd_data->port_info.sub_port_channel_offset
                                           = (node_attr == NULL)
                                           ? 0
                                           : strtoul(node_attr, NULL, 0);


    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_PRESENCE_BUS);
    STD_ASSERT(node_attr != NULL);
    qsfp28_dd_data->mod_pres_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_PRESENCE_BITMASK);
    STD_ASSERT(node_attr != NULL);
    qsfp28_dd_data->mod_pres_bitmask = strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_RESET_BUS);
    STD_ASSERT(node_attr != NULL);
    qsfp28_dd_data->mod_reset_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_RESET_BITMASK);
    STD_ASSERT(node_attr != NULL);
    qsfp28_dd_data->mod_reset_bitmask = strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_LPMODE_BUS);
    STD_ASSERT(node_attr != NULL);
    qsfp28_dd_data->mod_lpmode_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_LPMODE_BITMASK);
    STD_ASSERT(node_attr != NULL);
    qsfp28_dd_data->mod_lpmode_bitmask = strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_SELECTION_DELAY_IN_MILLI_SECONDS);
    if (node_attr != NULL){
        qsfp28_dd_data->delay = strtoul(node_attr, NULL, 0);
    } else {
        qsfp28_dd_data->delay = SDI_MEDIA_NO_DELAY;
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MAX_PORT_POWER_MILLIWATTS);

    if (node_attr == NULL){
        qsfp28_dd_data->port_info.max_port_power_mw = SDI_MEDIA_DEFAULT_QSFP28_DD_MAX_PORT_POWER_MILLIWATTS;
        SDI_DEVICE_TRACEMSG_LOG("Could not find max port power in config file for  %s"
            "Defaulting to %u mW max port power", dev_hdl->alias,
                 qsfp28_dd_data->port_info.max_port_power_mw);

    } else {
        qsfp28_dd_data->port_info.max_port_power_mw = strtoul(node_attr, NULL, 0);
    }

    dev_hdl->private_data = (void *)qsfp28_dd_data;

    sdi_resource_add(SDI_RESOURCE_MEDIA, dev_hdl->alias, (void *)dev_hdl,
                     &qsfp28_dd_media);

    *device_hdl = dev_hdl;

    return STD_ERR_OK;
}

