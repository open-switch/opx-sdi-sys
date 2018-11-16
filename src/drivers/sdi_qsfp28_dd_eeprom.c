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
 * filename: sdi_qsfp28_dd_eeprom.c
 */


/******************************************************************************
 * sdi_qsfp28_dd_eeprom.c
 * Implements the QSFP eeprom related APIs
 *****************************************************************************/
#include "sdi_resource_internal.h"
#include "sdi_device_common.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_i2c_bus_api.h"
#include "sdi_media.h"
#include "sdi_qsfp.h"
#include "sdi_qsfp28_dd.h"
#include "sdi_media_internal.h"
#include "sdi_media_phy_mgmt.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include "std_time_tools.h"
#include "sdi_platform_util.h"



static uint_t sdi_qsfp28_dd_channel_offset_get (sdi_resource_hdl_t resource_hdl)
{
    sdi_device_hdl_t qsfp28_dd_device = NULL;
    qsfp28_dd_device_t *qsfp28_dd_priv_data = NULL;

    STD_ASSERT(resource_hdl != NULL);

    qsfp28_dd_device = (sdi_device_hdl_t)resource_hdl;
    qsfp28_dd_priv_data = (qsfp_device_t *)qsfp28_dd_device->private_data;
    STD_ASSERT(qsfp28_dd_priv_data != NULL);


    return qsfp28_dd_priv_data->port_info.sub_port_channel_offset;
}


/**
 * Get the required module status of the specific qsfp28-dd
 * resource_hdl[in] - Handle of the resource
 * flags[in]        - flags for status that are of interest
 * status[out]    - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                uint_t flags, uint_t *status)
{
    return sdi_qsfp_module_monitor_status_get(resource_hdl, flags, status);
}

/**
 * Get the required channel monitoring(rx_power, tx_bias alarm) status of the specific QSFP28-DD
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest
 * flags[in]        - flags for channel status
 * status[out]      - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                                 uint_t flags, uint_t *status)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_channel_monitor_status_get(resource_hdl, channel, flags, status);
}

/**
 * Get the required channel status of the specific QSFP28-DD
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest
 * flags[in]        - flags for channel status
 * status[out]    - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_channel_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         uint_t flags, uint_t *status)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_channel_status_get(resource_hdl, channel, flags, status);
}

/**
 * Disable/Enable the transmitter of the specific QSFP28-DD
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * enable[in]       - "false" to disable and "true" to enable
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_tx_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                 bool enable)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_tx_control(resource_hdl, channel, enable);
}

/**
 * Gets the transmitter status on the specific channel of a QSFP28-DD
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * status[out]      - "true" if transmitter enabled, else "false"
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_tx_control_status_get(sdi_resource_hdl_t resource_hdl,
                                           uint_t channel, bool *status)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_tx_control_status_get (resource_hdl, channel, status);
}

/**
 * Disable/Enable the tx/rx cdr of the specific QSFP28-DD
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * enable[in]       - "false" to disable and "true" to enable
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_cdr_status_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                     bool enable)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_cdr_status_set (resource_hdl, channel, enable);
}

/**
 * Gets the transmitter status on the specific channel of a QSFP28-DD
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * status[out]      - "true" if transmitter enabled, else "false"
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_cdr_status_get(sdi_resource_hdl_t resource_hdl,
                                           uint_t channel, bool *status)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_cdr_status_get(resource_hdl, channel, status);
}

/**
 * Get the maximum speed that can be supported by a specific media resource
 * resource_hdl[in] - handle of the media resource
 * speed[out]     - speed of the media will be filled in this
 * return           - standard t_std_error
 */
t_std_error  sdi_qsfp28_dd_speed_get(sdi_resource_hdl_t resource_hdl,
                                sdi_media_speed_t *speed)
{
    return sdi_qsfp_speed_get (resource_hdl, speed);
}

/**
 * Reads the requested parameter value from eeprom
 * resource_hdl[in] - handle of the media resource
 * param[in]        - parametr type that is of interest(e.g wavelength, maximum
 *                    case temperature etc)
 * value[out]     - parameter value which is read from eeprom
 * return           - standard t_std_error
 */
t_std_error sdi_qsfp28_dd_parameter_get(sdi_resource_hdl_t resource_hdl,
                                   sdi_media_param_type_t param, uint_t *value)
{
    return sdi_qsfp_parameter_get(resource_hdl, param, value);
}

/**
 * Read the requested vendor information of a specific media resource
 * resource_hdl[in]     - handle of the media resource
 * vendor_info_type[in] - vendor information that is of interest.
 * vendor_info[out]   - vendor information which is read from eeprom
 * buf_size[in]         - size of the input buffer(vendor_info)
 * return               - standard t_std_error
 */
t_std_error sdi_qsfp28_dd_vendor_info_get(sdi_resource_hdl_t resource_hdl,
                                     sdi_media_vendor_info_type_t vendor_info_type,
                                     char *vendor_info, size_t size)
{
    return sdi_qsfp_vendor_info_get(resource_hdl, vendor_info_type, vendor_info, size);
}

/**
 * Read the transceiver compliance code information for a specific media resource
 * resource_hdl[in]         - handle of the media resource
 * transceiver_info[out]    - transceiver information which is read from eeprom
 * return                   - standard t_std_error
 */
t_std_error sdi_qsfp28_dd_transceiver_code_get(sdi_resource_hdl_t resource_hdl,
                                          sdi_media_transceiver_descr_t *transceiver_info)
{
    return sdi_qsfp_transceiver_code_get(resource_hdl, transceiver_info);
}

/**
 * Get the alarm and warning thresholds for the given optic
 * resource_hdl[in] - Handle of the resource
 * threshold_type[in] - Type of the threshold
 * value[out] - Threshold value
 * return - standard t_std_error
 */
t_std_error sdi_qsfp28_dd_threshold_get (sdi_resource_hdl_t resource_hdl,
                                    sdi_media_threshold_type_t threshold_type,
                                    float *value)
{
    return sdi_qsfp_threshold_get(resource_hdl, threshold_type, value);
}
/**
 * Read the threshold values for module monitors like temperature and voltage
 * resource_hdl[in]     - Handle of the resource
 * threshold_type[in]   - type of threshold
 * value[out]           - threshold value
 * return               - standard t_std_error
 */
t_std_error sdi_qsfp28_dd_module_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                  uint_t threshold_type, uint_t *value)
{
    return sdi_qsfp_module_monitor_threshold_get(resource_hdl, threshold_type, value);
}

/**
 * Read the threshold values for channel monitors like rx-ower and tx-bias
 * resource_hdl[in]     - Handle of the resource
 * threshold_type[in]   - type of threshold
 * value[out]           - threshold value
 * return               - standard t_std_error
 */
t_std_error sdi_qsfp28_dd_channel_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                   uint_t threshold_type, uint_t *value)
{
    return sdi_qsfp_channel_monitor_threshold_get(resource_hdl, threshold_type, value);
}
/**
 * Debug api to retrieve module monitors assoicated with the specified QSFP28-DD
 * resource_hdl[in] - Handle of the resource
 * monitor[in]      - monitor which needs to be retrieved
 * value[out]     - Value of the monitor
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                         sdi_media_module_monitor_t monitor, float *value)
{
    return sdi_qsfp_module_monitor_get(resource_hdl, monitor, value);
}

/**
 * Retrieve channel monitors assoicated with the specified QSFP28-DD
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel whose monitor has to be retreived
 * monitor[in]      - monitor which needs to be retrieved
 * value[out]     - Value of the monitor
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_channel_monitor_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_channel_monitor_t monitor, float *value)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_channel_monitor_get(resource_hdl, channel, monitor, value);
}
/**
 * Get the inforamtion of whether optional features supported or not on a given
 * module
 * resource_hdl[in] - Handle of the resource
 * feature_support[out] - feature support flags. Flag will be set to "true" if
 * feature is supported else "false"
 * return standard t_std_error
 */
t_std_error sdi_qsfp28_dd_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                 sdi_media_supported_feature_t *feature_support)
{
    return sdi_qsfp_feature_support_status_get(resource_hdl, feature_support);
}

/**
 * Raw read api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * data[out]      - Data will be filled after read
 * data_len[in]     - length of the data to be read
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    return sdi_qsfp_read_generic(resource_hdl, addr, data, data_len);
}

/**
 * Raw write api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * data[in]      - Data to write
 * data_len[in]     - length of the data to be written
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    return sdi_qsfp_write_generic (resource_hdl, addr, data, data_len);
}

/**
 * Raw read api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * offset[in]       - offset from which to read
 * data[out]      - Data will be filled after read
 * data_len[in]     - length of the data to be read
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len)
{
    return sdi_qsfp_read(resource_hdl, offset, data, data_len);
}

/**
 * Raw write api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * offset[in]       - offset from which to write
 * data[in]         - input buffer which contains the data to be written
 * data_len[in]     - length of the data to be written
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len)
{
    return sdi_qsfp_write(resource_hdl, offset, data, data_len);
}

/**
 * Set the autoneg config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * enable[in] - autoneg enable (True/Flase)
 * return standard t_std_error
 */
t_std_error sdi_qsfp28_dd_phy_autoneg_set (sdi_resource_hdl_t resource_hdl,
                                      uint_t channel, sdi_media_type_t type,
                                      bool enable)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_phy_autoneg_set(resource_hdl, channel, type, enable);
}


/**
 * Set the interface type config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * mode[in] - Interface type (GMII/SMII/XFI)
 * return standard t_std_error
 */
t_std_error sdi_qsfp28_dd_phy_mode_set (sdi_resource_hdl_t resource_hdl,
                                   uint_t channel, sdi_media_type_t type,
                                   sdi_media_mode_t mode)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return  sdi_qsfp_phy_mode_set( resource_hdl, channel, type, mode);
}


/**
 * Set the speed config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * speed[in] - Speed (10MBPS/100MBPS/1G/10G/40G)
 * return standard t_std_error
 */
t_std_error sdi_qsfp28_dd_phy_speed_set (sdi_resource_hdl_t resource_hdl,
                                   uint_t channel, sdi_media_type_t type,
                                   sdi_media_speed_t speed)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_phy_speed_set(resource_hdl, channel, type, speed);
}


/*
 * @brief initialize the module
 * @param[in] resource_hdl - handle to the qsfp28-dd
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */

t_std_error sdi_qsfp28_dd_module_init (sdi_resource_hdl_t resource_hdl, bool pres)
{
    return sdi_qsfp_module_init(resource_hdl, pres);
}


/**
 * Get media PHY link status.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * status           - true - link up, false - link down
 * return           - t_std_error
 */
t_std_error sdi_qsfp28_dd_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_type_t type, bool *status)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_phy_link_status_get(resource_hdl, channel, type, status);
}

/**
 * Set power down state (enable/disable) on media PHY.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * enable           - true - power down, false - power up
 * return           - t_std_error
 */

t_std_error sdi_qsfp28_dd_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool enable)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_phy_power_down_enable(resource_hdl, channel, type, enable);
}


/**
 * Control (enable/disable) Fiber/Serdes tx and RX on media PHY.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * enable           - true - Enable Serdes, false - Disable Serdes
 * return           - t_std_error
 */

t_std_error sdi_qsfp28_dd_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                        sdi_media_type_t type, bool enable)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_phy_serdes_control (resource_hdl, channel, type, enable);
}


/**
 * @brief Api to set the rate select on optics.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number that is of interest.
 * rev[in]     -  Firmware revision
 * cdr_enable[in]  -  true to enable CDR, false to disable CDR via rate select bits.
 * return           - t_std_error
 */

t_std_error sdi_qsfp28_dd_ext_rate_select (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                      sdi_media_fw_rev_t rev, bool cdr_enable)
{
    channel += sdi_qsfp28_dd_channel_offset_get (resource_hdl);
    return sdi_qsfp_ext_rate_select(resource_hdl, channel, rev, cdr_enable);
}


/**
 * @brief API to get QSA adapter type
 * resource_hdl[in] - Handle of the resource
 * sdi_qsa_adapter_type_t*[out] - adapter type obtained
 * return           - t_std_error
 */

t_std_error sdi_qsfp28_dd_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter)
{
    return sdi_qsfp_qsa_adapter_type_get(resource_hdl, qsa_adapter);
}

/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */

t_std_error sdi_qsfp28_dd_wavelength_set (sdi_resource_hdl_t resource_hdl, float value)
{
    return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
}
