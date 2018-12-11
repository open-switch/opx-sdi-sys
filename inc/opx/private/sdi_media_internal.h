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
 * filename: sdi_media_internal.h
 */



/**
 * @file sdi_media_internal.h
 * @brief Internal media resource related declarations.
 * This file will be used by the media drivers(sfp/qsfp)
 *
 */

#ifndef __SDI_MEDIA_INTERNAL_H_
#define __SDI_MEDIA_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_media.h"

/**
 * Each media resource provides the following callbacks.
 */

typedef struct {
    /* For getting presence status of a media resource */
    t_std_error (*presence_get)(sdi_resource_hdl_t resource_hdl, bool *pres);

    t_std_error (*module_init) (sdi_resource_hdl_t resource_hdl, bool pres);

    /* For getting status of a alarm related to temperature and voltage of a
     * media resource */
    t_std_error (*module_monitor_status_get)(sdi_resource_hdl_t resource_hdl,
                                             uint_t flags, uint_t *status);

    /* For getting status of channel monitor alarm(rx_power and tx_bias) of a
     * media resource */
    t_std_error (*channel_monitor_status_get)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                              uint_t flags, uint_t *status);

    /* For getting channel status like tx_loss, tx_fault, tx_enable and rx_loss
     * of a specific media resource */
    t_std_error (*channel_status_get)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                      uint_t flags, uint_t *status);

    /* For enabling/disabling the transmitter of a media resource */
    t_std_error (*tx_control)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                              bool enable);

    /* For getting transmitter status(enabled/disabled) on a specified channel  */
    t_std_error (*tx_control_status_get)(sdi_resource_hdl_t resource_hdl,
                                         uint_t channel, bool *status);

    /* For enabling/disabling the cdr of a media resource */
    t_std_error (*cdr_status_set)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                              bool enable);

    /* For getting cdr status(enabled/disabled) on a specified channel  */
    t_std_error (*cdr_status_get)(sdi_resource_hdl_t resource_hdl,
                                         uint_t channel, bool *status);

    /* For getting the speed of the media resource */
    t_std_error (*speed_get)(sdi_resource_hdl_t resource_hdl,
                             sdi_media_speed_t *speed);

    /* Get the requested parameter value from eeprom */
    t_std_error (*parameter_get)(sdi_resource_hdl_t resource_hdl,
                                 sdi_media_param_type_t param, uint_t *value);

    /* Get the requested vendor information from eeprom */
    t_std_error (*vendor_info_get)(sdi_resource_hdl_t resource_hdl,
                                   sdi_media_vendor_info_type_t vendor_info_type,
                                   char *vendor_info, size_t size);

    /* Get the transceiver compliance code information from eeprom */
    t_std_error (*transceiver_code_get)(sdi_resource_hdl_t resource_hdl,
                                        sdi_media_transceiver_descr_t *transceiver_info);

    /* Get the alarm and warning threshold values */
    t_std_error (*threshold_get)(sdi_resource_hdl_t resource_hdl,
                                 sdi_media_threshold_type_t threshold_type, float *value);

    /* Get the threshold values for module monitors  like temperature and voltage*/
    t_std_error (*module_monitor_threshold_get)(sdi_resource_hdl_t resource_hdl,
                                                uint_t threshold_type, uint_t *value);

    /* Get the threshold values for channel monitors like rx power and tx bias */
    t_std_error (*channel_monitor_threshold_get)(sdi_resource_hdl_t resource_hdl,
                                                 uint_t threshold_type, uint_t *value);

    /* To Enable/Disable the LP mode/Reset control */
    t_std_error (*module_control)(sdi_resource_hdl_t resource_hdl,
                                   sdi_media_module_ctrl_type_t ctrl_type, bool enable);

    /* For getting the status of LP mode or Reset status of the specific media */
    t_std_error (*module_control_status_get)(sdi_resource_hdl_t resource_hdl,
                                             sdi_media_module_ctrl_type_t ctrl_type, bool *status);

    /* For getting the module monitor values like temperature and voltage of a
     * media resource */
    t_std_error (*module_monitor_get)(sdi_resource_hdl_t resource_hdl,
                                      sdi_media_module_monitor_t monitor, float *value);

    /* For getting the channel monitor values like rx_power and tx_power_bias of
     * a channel on a specified media resource */
    t_std_error (*channel_monitor_get)(sdi_resource_hdl_t resource_hdl,
                                       uint_t channel, sdi_media_channel_monitor_t monitor, float *value);

    /* for getting optional feature support status */
    t_std_error (*feature_support_status_get)(sdi_resource_hdl_t resource_hdl,
                                              sdi_media_supported_feature_t *feature_support);

    /* for setting port led based on speed settings */
    t_std_error (*led_set)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                           sdi_media_speed_t speed);

    /* Debug api to read data from media */
    t_std_error (*read)(sdi_resource_hdl_t resource_hdl, uint_t offset,
                        uint8_t *data, size_t data_len);

    /* Debug api to write data in to media */
    t_std_error (*write)(sdi_resource_hdl_t resource_hdl, uint_t offset,
                         uint8_t *data, size_t data_len);

    /* To read from media */
    t_std_error (*read_generic) (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                     uint8_t *data, size_t data_len);

    /* To write from media */
    t_std_error (*write_generic) (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                     uint8_t *data, size_t data_len);

    /* For enabling/disabling phy media Auto negotiation */
    t_std_error (*media_phy_autoneg_set)(sdi_resource_hdl_t resource_hdl,
                                         uint_t channel, sdi_media_type_t type,
                                         bool enable);

    /* For setting media phy speed */
    t_std_error (*media_phy_speed_set)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_type_t type, sdi_media_speed_t speed);

    /* For setting media phy mode */
    t_std_error  (*media_phy_mode_set)(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_type_t type, sdi_media_mode_t mode);

    /* For setting wavelength for tunable media */
    t_std_error (*wavelength_set) (sdi_resource_hdl_t resource_hdl, float value);

    /* For getting media phy link status */
    t_std_error (*media_phy_link_status_get)(sdi_resource_hdl_t resource_hdl,
            uint_t channel,sdi_media_type_t type, bool *status);

    /* For setting media phy power down enable/disable */
    t_std_error (*media_phy_power_down_enable)(sdi_resource_hdl_t resource_hdl,
            uint_t channel, sdi_media_type_t type, bool enable);

    /* For setting rate select on media based on speed settings.*/
    t_std_error (*ext_rate_select) (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                    sdi_media_fw_rev_t rev, bool cdr_enable);

    /* For enable/disable Fiber/Serdes Transmitter and Receiver*/
    t_std_error (*media_phy_serdes_control)(sdi_resource_hdl_t resource_hdl,
            uint_t channel, sdi_media_type_t type, bool enable);

    /* For obtaining the QSA adapter type */
    t_std_error (*media_qsa_adapter_type_get)(sdi_resource_hdl_t resource_hdl,
            sdi_qsa_adapter_type_t* qsa_adapter);

    /* For obtaining the port info */
    t_std_error (*media_port_info_get)(sdi_resource_hdl_t resource_hdl,
            sdi_media_port_info_t* port_info);

    /* For obtaining the module info */
    t_std_error (*media_module_info_get)(sdi_resource_hdl_t resource_hdl,
            sdi_media_module_info_t* module_info);

} media_ctrl_t;

#endif
