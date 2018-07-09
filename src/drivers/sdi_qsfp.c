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
 * filename: sdi_qsfp.c
 */


/******************************************************************************
 * sdi_qsfp.c
 * Implements the driver for Quad Small Form-factor Pluggable (QSFP)
 *
 *****************************************************************************/
#include "sdi_resource_internal.h"
#include "sdi_common_attr.h"
#include "sdi_device_common.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_media.h"
#include "sdi_qsfp.h"
#include "sdi_media_internal.h"
#include "sdi_media_attr.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include "std_bit_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* qsfp driver init function */
static t_std_error sdi_qsfp_init (sdi_device_hdl_t device_hdl);

/* register function for qsfp driver */
static t_std_error sdi_qsfp_register (std_config_node_t node, void *bus_handle,
                                      sdi_device_hdl_t* device_hdl);

/**
 * Gets the presence status of qsfp module
 * resource_hdl[in] - Handle of the qsfp resource
 * pres[out]      - presence status
 * return t_std_error
 */
static t_std_error sdi_qsfp_presence_get (sdi_resource_hdl_t resource_hdl, bool *pres)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t value = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(pres != NULL);

    *pres = false;

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_pres_hdl);
    if (rc != STD_ERR_OK){
        return rc;
    }

    rc = sdi_pin_group_read_level(qsfp_priv_data->mod_pres_hdl, &value);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("presence status get failed for %s",
                qsfp_device->alias);
    }

    sdi_pin_group_release_bus(qsfp_priv_data->mod_pres_hdl);

    if (rc == STD_ERR_OK){
        if( ( (STD_BIT_TEST(value, qsfp_priv_data->mod_pres_bitmask)) == 0) ) {
            *pres = false;
        } else {
            *pres = true;
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
static t_std_error sdi_qsfp_module_control(sdi_resource_hdl_t resource_hdl,
                                           sdi_media_module_ctrl_type_t ctrl_type, bool enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t value = 0;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return SDI_ERRCODE(EOPNOTSUPP);
    }

    switch(ctrl_type)
    {
        case SDI_MEDIA_LP_MODE:


            if ((qsfp_priv_data->module_info.max_module_power_mw > qsfp_priv_data->port_info.max_port_power_mw)
                                                & (!enable)){
                SDI_DEVICE_ERRMSG_LOG("FATAL: %s media max power (%umW) exceeds port max power (%umW). Cannot enable high power on media.",
                            qsfp_device->alias, qsfp_priv_data->module_info.max_module_power_mw, qsfp_priv_data->port_info.max_port_power_mw);
                return SDI_DEVICE_ERRCODE(EOPNOTSUPP);

            }
            if (qsfp_priv_data->module_info.software_controlled_power_mode) {
                rc = sdi_qsfp_media_force_power_mode_set(resource_hdl, !enable);
                if (rc == STD_ERR_OK){
                    SDI_DEVICE_TRACEMSG_LOG("Power class set on module %s %umW, rc %u", qsfp_device->alias,
                        qsfp_priv_data->module_info.max_module_power_mw, rc);

                } else if (rc != SDI_DEVICE_ERRCODE(EOPNOTSUPP)) {
                    SDI_DEVICE_ERRMSG_LOG("Error when powering up module %s . Module may not work as expected", qsfp_device->alias);
                }
            }
            rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_lpmode_hdl);
            if (rc != STD_ERR_OK){
                return rc;
            }
            do {
                rc = sdi_pin_group_read_level(qsfp_priv_data->mod_lpmode_hdl, &value);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("lp mode status get failed for %s",
                            qsfp_device->alias);
                    break;
                }

                if(enable == true) {
                    STD_BIT_SET(value, qsfp_priv_data->mod_lpmode_bitmask);
                } else {
                    STD_BIT_CLEAR(value, qsfp_priv_data->mod_lpmode_bitmask);
                }

                rc = sdi_pin_group_write_level(qsfp_priv_data->mod_lpmode_hdl, value);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("lp mode status set failed for %s",
                            qsfp_device->alias);
                }
            } while(0);
            sdi_pin_group_release_bus(qsfp_priv_data->mod_lpmode_hdl);
            break;

        case SDI_MEDIA_RESET:
            rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_reset_hdl);
            if (rc != STD_ERR_OK){
                return rc;
            }
            do {
                rc = sdi_pin_group_read_level(qsfp_priv_data->mod_reset_hdl, &value);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("lp mode status get failed for %s",
                            qsfp_device->alias);
                    break;
                }

                if(enable == true) {
                    STD_BIT_SET(value, qsfp_priv_data->mod_reset_bitmask);
                } else {
                    STD_BIT_CLEAR(value, qsfp_priv_data->mod_reset_bitmask);
                }

                rc = sdi_pin_group_write_level(qsfp_priv_data->mod_reset_hdl, value);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("lp mode status set failed for %s",
                            qsfp_device->alias);
                }
            } while(0);
            sdi_pin_group_release_bus(qsfp_priv_data->mod_reset_hdl);
            break;

        default:
            SDI_DEVICE_ERRMSG_LOG("Invalid control type for %s",
                                  qsfp_device->alias);
            rc = SDI_DEVICE_ERRCODE(EINVAL);
           break;
    }
    return rc;
}

/**
 * Get the status of module control parameters like low power mode and reset
 * status
 * resource_hdl[in] - handle of the resource
 * ctrl_type[in]    - module control type(LP mode/reset)
 * status[out]      - "true" if enabled else "false"
 * return           - standard t_std_error
 */
static t_std_error sdi_qsfp_module_control_status_get(sdi_resource_hdl_t resource_hdl,
                                                      sdi_media_module_ctrl_type_t ctrl_type,
                                                      bool *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t value = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return SDI_ERRCODE(EOPNOTSUPP);
    }

    switch(ctrl_type)
    {
        case SDI_MEDIA_LP_MODE:
            rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_lpmode_hdl);
            if (rc != STD_ERR_OK){
                return rc;
            }

            rc = sdi_pin_group_read_level(qsfp_priv_data->mod_lpmode_hdl, &value);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("lp mode status get failed for %s",
                        qsfp_device->alias);
            }

            sdi_pin_group_release_bus(qsfp_priv_data->mod_lpmode_hdl);

            if (rc == STD_ERR_OK){
                if((STD_BIT_TEST(value, qsfp_priv_data->mod_lpmode_bitmask)) == 0) {
                    *status = false;
                } else {
                    *status = true;
                }
            }

            break;

        case SDI_MEDIA_RESET:
            rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_reset_hdl);
            if (rc != STD_ERR_OK){
                return rc;
            }

            rc = sdi_pin_group_read_level(qsfp_priv_data->mod_reset_hdl, &value);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("lp mode status get failed for %s",
                        qsfp_device->alias);
            }

            sdi_pin_group_release_bus(qsfp_priv_data->mod_reset_hdl);

            if (rc == STD_ERR_OK){
                if((STD_BIT_TEST(value, qsfp_priv_data->mod_reset_bitmask)) == 0) {
                    *status = false;
                } else {
                    *status = true;
                }
            }
            break;

        default:
            SDI_DEVICE_ERRMSG_LOG("Invalid control type for %s",
                                  qsfp_device->alias);
            rc = SDI_DEVICE_ERRCODE(EINVAL);
           break;
    }
    return rc;
}

/* Not yet implemented */

t_std_error sdi_qsfp_module_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_module_info_t* module_info)
{
    return STD_ERR_OK;
}
t_std_error sdi_qsfp_port_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_port_info_t* port_info)
{
    return STD_ERR_OK;
}


/* Callback handlers for QSFP */
static media_ctrl_t qsfp_media = {
    .presence_get = sdi_qsfp_presence_get,
    .module_init = sdi_qsfp_module_init,
    .module_monitor_status_get = sdi_qsfp_module_monitor_status_get,
    .channel_monitor_status_get = sdi_qsfp_channel_monitor_status_get,
    .channel_status_get = sdi_qsfp_channel_status_get,
    .tx_control = sdi_qsfp_tx_control,
    .tx_control_status_get = sdi_qsfp_tx_control_status_get,
    .cdr_status_set = sdi_qsfp_cdr_status_set,
    .cdr_status_get = sdi_qsfp_cdr_status_get,
    .speed_get = sdi_qsfp_speed_get,
    .parameter_get = sdi_qsfp_parameter_get,
    .vendor_info_get = sdi_qsfp_vendor_info_get,
    .transceiver_code_get = sdi_qsfp_transceiver_code_get,
    .threshold_get = sdi_qsfp_threshold_get,
    .module_monitor_threshold_get = sdi_qsfp_module_monitor_threshold_get,
    .channel_monitor_threshold_get = sdi_qsfp_channel_monitor_threshold_get,
    .module_control = sdi_qsfp_module_control,
    .module_control_status_get = sdi_qsfp_module_control_status_get,
    .module_monitor_get = sdi_qsfp_module_monitor_get,
    .channel_monitor_get = sdi_qsfp_channel_monitor_get,
    .feature_support_status_get = sdi_qsfp_feature_support_status_get,
    .read = sdi_qsfp_read,
    .write = sdi_qsfp_write,
    .read_generic = sdi_qsfp_read_generic,
    .write_generic = sdi_qsfp_write_generic,
    .media_phy_autoneg_set = sdi_qsfp_phy_autoneg_set,
    .media_phy_speed_set = sdi_qsfp_phy_speed_set,
    .media_phy_mode_set = sdi_qsfp_phy_mode_set,
    .wavelength_set = sdi_qsfp_wavelength_set,
    .media_phy_link_status_get = sdi_qsfp_phy_link_status_get,     /* Added for QSA support */
    .media_phy_power_down_enable = sdi_qsfp_phy_power_down_enable, /* Added for QSA support */
    .ext_rate_select = sdi_qsfp_ext_rate_select,
    .media_phy_serdes_control = sdi_qsfp_phy_serdes_control,         /* Added for QSA support */
    .media_qsa_adapter_type_get = sdi_qsfp_qsa_adapter_type_get,   /* QSA info get */
    .media_port_info_get = sdi_qsfp_port_info_get,
    .media_module_info_get = sdi_qsfp_module_info_get

};

/*
 * Every driver must export function with name sdi_<driver_name>_query_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t * sdi_qsfp_entry_callbacks(void)
{
    /*Export Driver table*/
    static const sdi_driver_t qsfp_entry = {
        sdi_qsfp_register,
        sdi_qsfp_init
    };

    return &qsfp_entry;
}

/**
 * initialize the device
 * device_hdl[in] - Handle to the device
 * return         - t_std_error
 */
static t_std_error sdi_qsfp_init (sdi_device_hdl_t device_hdl)
{
    /* Need to move out of reset as part of QSFP init and same is handled as
     * part of parent bus init in config file. Hence just return OK from here */
    return STD_ERR_OK;
}

/*
 * The configuration file format for QSFP(sff-8436) node is as follows
 *  qsfp instance="<port_number>"
 *  addr="<i2c address for qsfp>"
 *  mod_sel_bus="<pin group bus name for selecting module>"
 *  mod_sel_value="<pin value which needs to be wriiten on pin group bus for selecting module>"
 *  mod_pres_bus="<pin group bus name for knowing the presence status of qsfp>"
 *  mod_pres_bitmask="<presence check bit number for this instance of qsfp on mod_pres_bus>"
 *  mod_reset_bus="<pin group bus name for setting reset mode>"
 *  mod_reset_bitmask="<reset bit number for this instance of qsfp on mod_reset_bus>"
 *  mod_lpmode_bus="<pin group bus name for setting low power mode>"
 *  mod_lpmode_bitmask="<lp mode for this instance of qsfp on mod_lpmode_bus>"
 *  mod_sel_delay="<delay in milli seconds, time to be wait after selecting module" />
 */

/**
 * Register function for QSFP devices
 * node[in]         - Device node from configuration file
 * bus_handle[in]   - Parent bus handle of the device
 * device_hdl[out]  - Device handle which is filled by this function
 * return           - t_std_error
 */
static t_std_error sdi_qsfp_register (std_config_node_t node, void *bus_handle,
                                      sdi_device_hdl_t* device_hdl)
{
    sdi_device_hdl_t dev_hdl = NULL;
    qsfp_device_t *qsfp_data = NULL;
    char *node_attr = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t*)bus_handle)->bus_type == SDI_I2C_BUS);

    dev_hdl = calloc(sizeof(sdi_device_entry_t), 1);
    STD_ASSERT(dev_hdl != NULL);

    qsfp_data = calloc(sizeof(qsfp_device_t), 1);
    STD_ASSERT(qsfp_data != NULL);

    dev_hdl->bus_hdl = bus_handle;
    dev_hdl->callbacks = sdi_qsfp_entry_callbacks();

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ADDRESS);
    STD_ASSERT(node_attr != NULL);
    dev_hdl->addr.i2c_addr.i2c_addr = (i2c_addr_t) strtoul(node_attr, NULL, 16);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(node_attr != NULL);
    dev_hdl->instance = strtoul(node_attr, NULL, 0);
    snprintf(dev_hdl->alias, SDI_MAX_NAME_LEN, "qsfp-%u", dev_hdl->instance);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MUX_SELECTION_BUS);
    if (node_attr != NULL) {
        qsfp_data->mux_sel_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MUX_SELECTION_VALUE);
    if (node_attr != NULL) {
        qsfp_data->mux_sel_value = strtoul(node_attr, NULL, 0);
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_SELECTION_BUS);
    STD_ASSERT(node_attr != NULL);
    if(strncmp(node_attr, SDI_MEDIA_MODULE_SEL_ALWAYS_ENABLED,
                SDI_MEDIA_MODULE_SEL_ALWAYS_ENABLED_STRLEN) == 0 ) {
        /* Only one module present on the channel */
        qsfp_data->mod_sel_hdl = NULL;
        qsfp_data->mod_sel_value = 0;
    } else {
        qsfp_data->mod_sel_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);
        node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_SELECTION_VALUE);
        STD_ASSERT(node_attr != NULL);
        qsfp_data->mod_sel_value = strtoul(node_attr, NULL, 0);
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_PORT_TYPE);
    if (node_attr != NULL) {
         if (strcmp(node_attr, SDI_PORT_TYPE_QSFP28) == 0) {
             qsfp_data->capability = SDI_MEDIA_SPEED_100G;
         } else if (strcmp(node_attr, SDI_PORT_TYPE_QSFP28_DD) == 0) {
             qsfp_data->capability = SDI_MEDIA_SPEED_200G;
         } else {
             qsfp_data->capability = SDI_MEDIA_SPEED_40G;
         }
    } else {
        qsfp_data->capability = SDI_MEDIA_SPEED_40G;
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_PRESENCE_BUS);
    STD_ASSERT(node_attr != NULL);
    qsfp_data->mod_pres_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_PRESENCE_BITMASK);
    STD_ASSERT(node_attr != NULL);
    qsfp_data->mod_pres_bitmask = strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_RESET_BUS);
    STD_ASSERT(node_attr != NULL);
    qsfp_data->mod_reset_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_RESET_BITMASK);
    STD_ASSERT(node_attr != NULL);
    qsfp_data->mod_reset_bitmask = strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_LPMODE_BUS);
    STD_ASSERT(node_attr != NULL);
    qsfp_data->mod_lpmode_hdl = sdi_get_pin_group_bus_handle_by_name(node_attr);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_LPMODE_BITMASK);
    STD_ASSERT(node_attr != NULL);
    qsfp_data->mod_lpmode_bitmask = strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_MEDIA_MODULE_SELECTION_DELAY_IN_MILLI_SECONDS);
    if (node_attr != NULL){
        qsfp_data->delay = strtoul(node_attr, NULL, 0);
    } else {
        qsfp_data->delay = SDI_MEDIA_NO_DELAY;
    }

    node_attr = std_config_attr_get(node, SDI_MEDIA_MAX_PORT_POWER_MILLIWATTS);

    if (node_attr == NULL){
        switch (qsfp_data->capability){
            case SDI_MEDIA_SPEED_40G:
                qsfp_data->port_info.max_port_power_mw = SDI_MEDIA_DEFAULT_QSFP_MAX_PORT_POWER_MILLIWATTS;
                break;
            case SDI_MEDIA_SPEED_100G:
                qsfp_data->port_info.max_port_power_mw = SDI_MEDIA_DEFAULT_QSFP28_MAX_PORT_POWER_MILLIWATTS;
                break;
            case SDI_MEDIA_SPEED_200G:
                qsfp_data->port_info.max_port_power_mw = SDI_MEDIA_DEFAULT_QSFP28_DD_MAX_PORT_POWER_MILLIWATTS;
                break;
            default:
                qsfp_data->port_info.max_port_power_mw = SDI_MEDIA_DEFAULT_QSFP_MAX_PORT_POWER_MILLIWATTS;
                break;
        }

        SDI_DEVICE_TRACEMSG_LOG("Could not find max port power in config file for  %s"
            "Defaulting to %u mW max port power", dev_hdl->alias,
                 qsfp_data->port_info.max_port_power_mw);

    } else {
        qsfp_data->port_info.max_port_power_mw = strtoul(node_attr, NULL, 0);
    }

    dev_hdl->private_data = (void *)qsfp_data;

    sdi_resource_add(SDI_RESOURCE_MEDIA, dev_hdl->alias, (void *)dev_hdl,
                     &qsfp_media);

    *device_hdl = dev_hdl;

    return STD_ERR_OK;
}

