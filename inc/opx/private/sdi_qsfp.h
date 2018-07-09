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
 * filename: sdi_qsfp.h
 */


/*******************************************************************
* @file   sdi_qsfp.h
* @brief  Declares the qsfp private data structures and driver functions
*******************************************************************/

#ifndef __SDI_QSFP_H_
#define __SDI_QSFP_H_
#include "sdi_resource_internal.h"
#include "sdi_media.h"

/**
 * @media qsfp category
 */

typedef enum qsfp_category {
    SDI_CATEGORY_QSFP,
    SDI_CATEGORY_QSFPPLUS,
    SDI_CATEGORY_QSFP28,
    SDI_CATEGORY_QSFPDD,
} qsfp_category_t;

/**
 * @struct qsfp_device_t
 * QSFP device private data
 */
typedef struct qsfp_device {
    sdi_pin_group_bus_hdl_t mux_sel_hdl; /**<qsfp device mux selection pin
                                           group bus handler*/
    uint_t mux_sel_value; /**<value which needs to be written on pin group bus for
                            selecting mux */
    sdi_pin_group_bus_hdl_t mod_sel_hdl; /**<qsfp device module selection pin
                                           group bus handler*/
    uint_t mod_sel_value; /**<value which needs to be written on pin group bus for
                            selecting module */
    sdi_pin_group_bus_hdl_t mod_pres_hdl; /**<qsfp device module presence pin
                                            group bus handler*/
    uint_t mod_pres_bitmask; /**<qsfp devie presence bit mask*/
    sdi_pin_group_bus_hdl_t mod_reset_hdl; /**<qsfp device module reset pin
                                             group bus handler*/
    uint_t mod_reset_bitmask; /**<qsfp devie reset bit mask*/
    sdi_pin_group_bus_hdl_t mod_lpmode_hdl; /**<qsfp device module lpmode pin
                                              group bus handler*/
    uint_t mod_lpmode_bitmask; /**<qsfp devie lpmode bitmask*/
    uint_t delay; /**<delay in milli seconds*/
    
    sdi_media_type_t  mod_type; /**<media module type which is pluged in using
                                    QSA adapter in qsfp port */
    sdi_device_hdl_t  sfp_device;/**<sfp device callback data when QSA adapter is
                                   used to plugin sfp/sfp+ media*/
    qsfp_category_t  mod_category; /**<media category to distinguish between
                                     QSFP/QSFPPLUS/QSFP28/QSFPDD */
    sdi_media_speed_t  capability; /**<Front panel port or media capability */


    sdi_media_module_info_t module_info;

    sdi_media_port_info_t port_info;

    uint_t eeprom_version; /* Used for QSFP28-DD EEPROM version */
} qsfp_device_t;

/* This function overrides the LP_MODE hardware pin. Use carefully */
/* If this function is used to set power HIGH, one must also use it to set power LOW */
t_std_error sdi_qsfp_media_force_power_mode_set(sdi_resource_hdl_t resource_hdl, bool state);

/**
 * @brief Get the required module alarm status of qsfp
 * @param[in] resource_hdl - handle of the qsfp resource
 * @param[in] flags - flags for status that are of interest.
 * @param[out] status - returns the set of status flags which are asserted.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                uint_t flags, uint_t *status);

/**
 * @brief Get the required channel alarm status of qsfp
 * @param resource_hdl[in] - handle of the qsfp resource
 * @param channel[in] - channel number
 * @param flags[in] - flags for channel monitoring status
 * @param status[out] - returns the set of status flags which are asserted
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                 uint_t channel, uint_t flags, uint_t *status);

/**
 * @brief Get the required channel status of qsfp
 * @param[in] resource_hdl - handle of the qsfp resource
 * @param[in] channel - channel number
 * @param[in] flags - flags for channel status
 * @param[out] status - returns the set of status flags which are asserted
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_channel_status_get (sdi_resource_hdl_t resource_hdl,
                                         uint_t channel, uint_t flags, uint_t *status);

/**
 * @brief Disable/Enable the transmitter of qsfp
 * @param[in] resource_hdl - handle of the resource
 * @param[in] channel - channel number
 * @param[in] enable - "false" to disable and "true" to enable
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_tx_control (sdi_resource_hdl_t resource_hdl,
                                 uint_t channel, bool enable);

/**
 * @brief Get the transmitter status on a particular channel of qsfp
 * @param[in] resource_hdl - handle of the resource
 * @param[in] channel - channel number
 * @param[out] status - "true" if transmitter enabled else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_tx_control_status_get(sdi_resource_hdl_t resource_hdl,
                                           uint_t channel, bool *status);
/**
 * @brief Disable/Enable the clock and data recovery function of qsfp
 * @param[in] resource_hdl - handle of the resource
 * @param[in] channel - channel number
 * @param[in] enable - "false" to disable and "true" to enable
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_cdr_status_set(sdi_resource_hdl_t resource_hdl,
                                     uint_t channel, bool enable);

/**
 * @brief Get the clock and data recovery status on a particular channel of qsfp
 * @param[in] resource_hdl - handle of the resource
 * @param[in] channel - channel number
 * @param[out] status - "true" if transmitter enabled else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_cdr_status_get(sdi_resource_hdl_t resource_hdl,
                                    uint_t channel, bool *status);
/**
 * @brief Read the speed of a qsfp
 * @param[in] resource_hdl - handle of the resource
 * @param[out] speed - speed of qsfp
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_speed_get(sdi_resource_hdl_t resource_hdl,
                               sdi_media_speed_t *speed);

/**
* @brief Reads the parameter value from eeprom
* @param[in] resource_hdl - handle of the qsfp
* @param[in] param_type - parameter type.
* @param[out] value - value of the parameter read from eeprom.
* @return - standard @ref t_std_error
*/
t_std_error sdi_qsfp_parameter_get(sdi_resource_hdl_t resource_hdl,
                                   sdi_media_param_type_t param, uint_t *value);

/**
 * @brief Reads the requested vendor information from eeprom
 * @param[in] resource_hdl - handle of the qsfp
 * @param[in] vendor_info_type - vendor information that is of interest.
 * @param[out] vendor_info - vendor information which is read from eeprom.
 * @param[in] buf_size - size of the input buffer which is allocated by user(vendor_info)
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_vendor_info_get(sdi_resource_hdl_t resource_hdl,
                                     sdi_media_vendor_info_type_t vendor_info_type,
                                     char *vendor_info, size_t size);

/**
 * @brief Reads the transceiver compliance code
 * @param resource_hdl[in] - handle of the qsfp
 * @param transceiver_info[out] - transceiver compliance code information
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_transceiver_code_get(sdi_resource_hdl_t resource_hdl,
                                          sdi_media_transceiver_descr_t *transceiver_info);

/**
 * @brief Get the alarm and warning threshold values
 * @param resource_hdl[in] - Handle of the resource
 * @param threshold_type[in] - type of the threshold refer @ref sdi_media_threshold_type_t
 * for valid threshold types
 * @param value[out] - threshold value
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_threshold_get(sdi_resource_hdl_t resource_hdl,
                                   sdi_media_threshold_type_t threshold_type,
                                   float *value);

/**
 * @brief Get the threshold values for module monitors like temperature and
 * voltage
 * @param resource_hdl[in] - Handle of the resource
 * @param threshold_type[in] - Should be one of the flag @ref sdi_media_status
 * @param value[out] - threshold value
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_module_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                  uint_t threshold_type, uint_t *value);

/**
 * @brief Get the threshold values for channel monitors like rx power and tx
 * bias
 * @param resource_hdl[in] - Handle of the resource
 * @param threshold_type[in] - Should be one of the flag @ref sdi_media_channel_status
 * @param value[out] - threshold value
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_channel_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                   uint_t threshold_type, uint_t *value);

/**
 * @brief retrieve module monitors assoicated with the specified media.
 * @param[in] resource_hdl - handle to qsfp
 * @param[in] monitor - The monitor which needs to be retrieved.
 * @param[out] value - Value of the monitor
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                         sdi_media_module_monitor_t monitor, float *value);

/**
 * @brief retrieve channel monitors assoicated with the specified media.
 * @param[in] resource_hdl - handle to qsfp
 * @param[in] channel - The channel number
 * @param[in] monitor - The monitor which needs to be retrieved.
 * @param[out] value - Value of the monitor
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_channel_monitor_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_channel_monitor_t monitor, float *value);

/**
 * @brief Get the optional feature support status for optics
 * @param resource_hdl[in] - handle to qsfp
 * @param feature_support[out] - feature support flags. Flag will be set to
 * "true" if feature supported else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                 sdi_media_supported_feature_t *feature_support);

/**
 * @brief Raw read api for media eeprom
 * @param resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * @param data[in] - Data to read
 * @param data_len[in] - length of the data to read
 * @return return - standard @ref t_std_error
*/
t_std_error sdi_qsfp_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len);

/**
 * @brief Raw write api for media eeprom
 * @param resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * @param data[in] - Data to write
 * @param data_len[in] - length of the data to be written
 * @return return - standard @ref t_std_error
*/
t_std_error sdi_qsfp_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len);

/**
 * @brief raw read from qsfp eeprom
 * @param[in] resource_hdl - handle to the qsfp
 * @param[in] offset - offset from which to read
 * @param[out] data - buffer for read data
 * @param[in] data_len - length of the data to be read
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len);

/**
 * @brief Debug api to write data in to qsfp eeprom
 * @param[in] resource_hdl - handle to the qsfp
 * @param[in] offset - offset from which to read
 * @param[in] data - input buffer which contains the data to be written
 * @param[in] data_len - length of the data to be written
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                            uint8_t *data, size_t data_len);

/**
 * Set the autoneg config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * enable[in] - autoneg enable (True/Flase)
 * return standard t_std_error
 */
t_std_error sdi_qsfp_phy_autoneg_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                     sdi_media_type_t type, bool enable);

/**
 * Set the interface type config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * mode[in] - Interface type (GMII/SMII/XFI)
 * return standard t_std_error
 */
t_std_error sdi_qsfp_phy_mode_set (sdi_resource_hdl_t resource_hdl,uint_t channel,
                                  sdi_media_type_t type, sdi_media_mode_t mode);

/**
 * Set the speed config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * speed[in] - Speed (10MBPS/100MBPS/1G/10G/40G)
 * return standard t_std_error
 */
t_std_error sdi_qsfp_phy_speed_set(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                  sdi_media_type_t type, sdi_media_speed_t speed);

/*
 * @brief initialize the module
 * @param[in] resource_hdl - handle to the qsfp
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */
t_std_error sdi_qsfp_module_init (sdi_resource_hdl_t resource_hdl, bool pres);

/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */
t_std_error sdi_qsfp_wavelength_set (sdi_resource_hdl_t resource_hdl, float value);

/**
 * @brief Api to get link status from media PHY.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number, 0 is valid
 * @param[in] type - Media type.
 * param[out] status - true when link is up, false when link is down
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_qsfp_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_type_t type, bool *status);

/**
 * @brief Api to power down enable/disable on media PHY.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number, 0 is valid 
 * @param[in] type - Media type.
 * @param[in] enable - true power down media PHY, false - power up media PHY.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_qsfp_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool enable);

/**
 * @brief Api to set the rate select on optics.
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] channel  - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] rev -  Firmware revision
 * @param[in] cdr_enable -  true to enable CDR, false to disable CDR via rate select bits.
 * return           - t_std_error
 */
t_std_error sdi_qsfp_ext_rate_select (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                      sdi_media_fw_rev_t rev, bool cdr_enable);

/**
 * @brief Api to enable/disable Fiber/Serdes TX and RX on media PHY.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number, 0 is valid
 * @param[in] type - Media type.
 * @param[in] enable - true enable Serdes, false - Disable Serdes.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_qsfp_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                        sdi_media_type_t type, bool enable);

/**
 * @brief API to get QSA adapter type
 * resource_hdl[in] - Handle of the resource
 * sdi_qsa_adapter_type_t*[out] - adapter type obtained
 * return           - t_std_error
 */

t_std_error sdi_qsfp_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter);

/**
 * @brief API to get the info of the port in which the device is connected
 * resource_hdl[in] - Handle of the resource
 * sdi_media_port_info_t*[out] - port info type obtained
 * return           - t_std_error
 */

t_std_error sdi_qsfp_port_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_port_info_t* port_info);

/**
 * @brief API to get the module
 * resource_hdl[in] - Handle of the resource
 * sdi_media_module_info_t*[out] - module info
 * return           - t_std_error
 */

t_std_error sdi_qsfp_module_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_module_info_t* module_info);


#endif
