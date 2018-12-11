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
 * filename: sdi_sfp.h
 */


/*******************************************************************
* @file   sdi_sfp.h
* @brief  Declares the sfp private data structures and driver functions
*******************************************************************/

#ifndef _SDI_SFP_H_
#define _SDI_SFP_H_

#include "sdi_media.h"
#include "sdi_resource_internal.h"
#include "sdi_pin_group.h"

#define SDI_SFP_CHANNEL_NUM 0

#define SDI_SFP_LED_HIGH_VALUE 1


/* See SFF 8690 for all wavelength set related spec */
/* speed of light = frequency * wavelength */
/* since using THz (10^12) and nm (10 ^-9), speed * 0.001 = freq*wav  */
#define SPEED_OF_LIGHT                                                        299792458.0 /* in m/s */
#define WAVELENGTH_NM_TO_FREQ_THZ(wav)                                       (0.001 * SPEED_OF_LIGHT / (wav))
#define FREQ_THZ_TO_WAVELENGTH_NM(freq)                                      (0.001 * SPEED_OF_LIGHT / (freq))
#define SDI_TWO_BYTE_TO_UINT16(msb, lsb)                                     (uint16_t)(((msb) << 8) | (lsb))
#define SDI_GET_CHANNEL_NO_FROM_FREQ(target_freq, first_freq, grid_spacing)   lroundf(1 + ((target_freq) - (first_freq)) / (grid_spacing))
#define SDI_FREQ_FRACTIONAL_PART_DIVISION_FACTOR                              10000.0
#define SDI_FREQ_WAVELENGTH_DIVISION_FACTOR                                   0.05
#define SDI_FREQ_JOIN(integer_part, frac_part)                               ((float)(integer_part) + (float)(frac_part)/SDI_FREQ_FRACTIONAL_PART_DIVISION_FACTOR)

#define SDI_SFP_DEFAULT_PAGE                                      0
#define SDI_SFP_TUNABLE_PAGE                                      2

#define SDI_SFP_TUNABLE_FREQ_CAPABILITIES_LEN                     10
#define SDI_WAVELENGTH_SET_POLL_PERIOD_MS                         10
#define SDI_WAVELENGTH_SET_TIMEOUT_MS                             100

#define SDI_SFP_CAPABILITIES_MIN_FREQ_INT_PART_MSB_BYTE_POS       0
#define SDI_SFP_CAPABILITIES_MIN_FREQ_INT_PART_LSB_BYTE_POS       1
#define SDI_SFP_CAPABILITIES_MIN_FREQ_FRAC_PART_MSB_BYTE_POS      2
#define SDI_SFP_CAPABILITIES_MIN_FREQ_FRAC_PART_LSB_BYTE_POS      3
#define SDI_SFP_CAPABILITIES_MAX_FREQ_INT_PART_MSB_BYTE_POS       4
#define SDI_SFP_CAPABILITIES_MAX_FREQ_INT_PART_LSB_BYTE_POS       5
#define SDI_SFP_CAPABILITIES_MAX_FREQ_FRAC_PART_MSB_BYTE_POS      6
#define SDI_SFP_CAPABILITIES_MAX_FREQ_FRAC_PART_LSB_BYTE_POS      7
#define SDI_SFP_CAPABILITIES_GRID_SPACING_MSB_BYTE_POS            8
#define SDI_SFP_CAPABILITIES_GRID_SPACING_LSB_BYTE_POS            9

/* Unlateched current status register bitmasks */
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_TEC_FAULT_BITSMASK     (1<<6)
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_WAV_UNLOCKED_BITMASK   (1<<5)
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_TX_TUNING_BITSMASK     (1<<4)

#define TEST_LATCHED_STATUS(x) !(x & SDI_SFP_WAVELENGTH_CURRENT_STATUS_TEC_FAULT_BITSMASK \
                                    & SDI_SFP_WAVELENGTH_CURRENT_STATUS_WAV_UNLOCKED_BITMASK \
                                    & SDI_SFP_WAVELENGTH_CURRENT_STATUS_TX_TUNING_BITSMASK )

/* Latched status register bitmasks */
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_TEC_FAULT_BITSMASK              (1<<6)
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_WAV_UNLOCKED_BITMASK            (1<<5)
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_BAD_CHANNEL_BITMASK             (1<<4)
#define SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_NEW_CHANNEL_ACQUIRED_BITMASK    (1<<3)

#define TEST_UNLATCHED_STATUS(x) ! (x & SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_TEC_FAULT_BITSMASK \
                                       & SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_WAV_UNLOCKED_BITMASK \
                                       & SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_BAD_CHANNEL_BITMASK \
                                       & SDI_SFP_WAVELENGTH_CURRENT_STATUS_LATCHED_NEW_CHANNEL_ACQUIRED_BITMASK )



/**
 * @struct sdi_media_led_t
 * SFP LED related data
 */
typedef struct port_led {
    sdi_pin_group_bus_hdl_t bus_hdl; /*<< port led pin group bus handle */
    uint_t bitmask; /*<< bit number for port led */
    uint_t led_1g_mode_value; /*<< 1G mode value to be written on bus */
    uint_t led_10g_mode_value; /*<< 10G mode value to be written on bus */
} sdi_media_led_t;

/**
 * @struct sdi_sfp_get_tunable_capabilities_t
 * SFP tunable capabilities
 */
typedef struct sdi_sfp_tunable_capabilities {
    float min_freq;    /* Freq in THz*/
    float max_freq;    /* Freq in THz*/
    float grid_spacing;  /* in THz units */
    bool channel_set_support; /* can channel be set? */
    bool wavelength_set_support; /* can wavelenght be set? */
    uint_t channel_count; /* channel starts from 1 to N */
} sdi_sfp_tunable_capabilities_t;

/**
 * @struct sfp_device_t
 * SFP device private data
 */
typedef struct sfp_device {
    sdi_pin_group_bus_hdl_t mux_sel_hdl; /**<sfp device mux selection pin
                                           group bus handler*/
    uint_t mux_sel_value; /**<value which needs to be written on pin group bus for
                            selecting mux */
    /** sfp device module selection pin group bus handler */
    sdi_pin_group_bus_hdl_t mod_sel_hdl;
    /** value needs to be written on pin group bus for selecting module */
    uint_t mod_sel_value;
    /** sfp device module presence pin group bus handler */
    sdi_pin_group_bus_hdl_t mod_pres_hdl;
    /** sfp devie presence bit mask */
    uint_t mod_pres_bitmask;
    /** sfp device module transmitter control pin bus handler */
    sdi_pin_group_bus_hdl_t mod_tx_control_hdl;
    /** sfp devie tx control bit mask */
    uint_t mod_tx_control_bitmask;
    /** sfp device module rx los pin bus handler */
    sdi_pin_group_bus_hdl_t mod_sfp_rx_los_hdl;
    /** sfp devie rx los bit mask */
    uint_t mod_sfp_rx_los_bitmask;
    /** sfp device module tx fault pin bus handler */
    sdi_pin_group_bus_hdl_t mod_sfp_tx_fault_hdl;
    /** sfp devie tx fault bit mask */
    uint_t  mod_sfp_tx_fault_bitmask;
    /** flag for checking whether LED can be controlled by CPU */
    bool port_led_control_flag;
    /** port led related data */
    sdi_media_led_t port_led;
    /** Front panel port or media capability */
    sdi_media_speed_t  capability;

    sdi_media_port_info_t port_info;
} sfp_device_t;

/**
 * @enum sfp_calib_info_type_t
 * sfp calibration types which is used to interpret the data
 */
typedef enum {
    /** internal calibration */
    SFP_CALIB_TYPE_INTERNAL = 0,
    /** external calibration */
    SFP_CALIB_TYPE_EXTERNAL = 1,
} sfp_calib_info_type_t;

/**
 * @struct sfp_calib_info_t
 * Caliberation Information. The use of calibration information is expalined in
 * SFP standard(SFF-8472) under section "Diagnostics Overview"
 */
typedef struct {
    /** Type of calibration */
    sfp_calib_info_type_t type;
    /** External calibration constant offset, not applicable for internal
     * calibration */
    uint16_t offset;
    /** External calibration constant slope, not applicable for internal
     * calibration */
    uint8_t slope[2];
} sfp_calib_info_t;

/**
 * @def Attribute used to represent rx output power data length
 */
#define EXT_CAL_RX_POWER_DATA_LEN 4
/**
 * @def Attribute used to represent rx output power full length
 */
#define EXT_CAL_RX_POWER_LEN (EXT_CAL_RX_POWER_DATA_LEN * 5)

/**
 * @struct sfp_rx_power_calib_info_t
 * Calibration Information for Rx optical power. The use of calibration information
 * is explained in SFP standard(SFF-8472) under section "Diagnostics Overview"
 */
typedef struct {
    /** Type of calibration */
    sfp_calib_info_type_t type;
    /** Rx optical power constant - 4 */
    uint8_t rx_power_const_4[EXT_CAL_RX_POWER_DATA_LEN];
    /** Rx optical power constant - 3 */
    uint8_t rx_power_const_3[EXT_CAL_RX_POWER_DATA_LEN];
    /** Rx optical power constant - 2 */
    uint8_t rx_power_const_2[EXT_CAL_RX_POWER_DATA_LEN];
    /** Rx optical power constant - 1 */
    uint8_t rx_power_const_1[EXT_CAL_RX_POWER_DATA_LEN];
    /** Rx optical power constant - 0 */
    uint8_t rx_power_const_0[EXT_CAL_RX_POWER_DATA_LEN];
} sfp_rx_power_calib_info_t;

/**
 * @brief Converts a number from milliwatts to dbm
 * @param[in] power_mw - The power value to be converted.
 * @return - standard @ref float
 */
float sdi_convert_mw_to_dbm(float power_mw);

/**
 * @brief Get the required module alarm status of sfp
 * @param[in] resource_hdl - handle of the sfp resource
 * @param[in] flags - flags for status that are of interest.
 * @param[out] status - returns the set of status flags which are asserted.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                uint_t flags, uint_t *status);

/**
 * @brief Get the required channel alarm status of sfp
 * @param resource_hdl[in] - handle of the sfp resource
 * @param channel[in] - channel number
 * @param flags[in] - flags for channel monitoring status
 * @param status[out] - returns the set of status flags which are asserted
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                uint_t channel, uint_t flags, uint_t *status);

/**
 * @brief Get the required channel status of sfp
 * @param[in] resource_hdl - handle of the sfp resource
 * @param[in] channel - channel number
 * @param[in] flags - flags for channel status
 * @param[out] status - returns the set of status flags which are asserted
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_channel_status_get (sdi_resource_hdl_t resource_hdl,
                                        uint_t channel, uint_t flags, uint_t *status);

/**
 * @brief Disable/Enable the transmitter of sfp
 * @param[in] resource_hdl - handle of the resource
 * @param[in] channel - channel number
 * @param[in] enable - "false" to disable and "true" to enable
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_tx_control (sdi_resource_hdl_t resource_hdl,
                                uint_t channel, bool enable);

/**
 * @brief Get the transmitter status on a particular channel of sfp
 * @param[in] resource_hdl - handle of the resource
 * @param[in] channel - channel number
 * @param[out] status - "true" if transmitter enabled else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_tx_control_status_get(sdi_resource_hdl_t resource_hdl,
                                          uint_t channel, bool *status);

/**
 * @brief Read the speed of a sfp
 * @param[in] resource_hdl - handle of the resource
 * @param[out] speed - speed of sfp
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_speed_get(sdi_resource_hdl_t resource_hdl,
                              sdi_media_speed_t *speed);

/**
* @brief Reads the parameter value from eeprom
* @param[in] resource_hdl - handle of the sfp
* @param[in] param_type - parameter type.
* @param[out] value - value of the parameter read from eeprom.
* @return - standard @ref t_std_error
*/
t_std_error sdi_sfp_parameter_get(sdi_resource_hdl_t resource_hdl,
                                  sdi_media_param_type_t param, uint_t *value);

/**
 * @brief Reads the requested vendor information from eeprom
 * @param[in] resource_hdl - handle of the sfp
 * @param[in] vendor_info_type - vendor information that is of interest.
 * @param[out] vendor_info - vendor information which is read from eeprom.
 * @param[in] buf_size - size of the input buffer which is allocated by user(vendor_info)
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_vendor_info_get(sdi_resource_hdl_t resource_hdl,
                                    sdi_media_vendor_info_type_t vendor_info_type,
                                    char *vendor_info, size_t size);

/**
 * @brief Reads the transceiver compliance code
 * @param resource_hdl[in] - handle of the sfp
 * @param transceiver_info[out] - transceiver compliance code information
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_transceiver_code_get(sdi_resource_hdl_t resource_hdl,
                                         sdi_media_transceiver_descr_t *transceiver_info);

/**
 * @brief Get the alarm and warning thresholds for the given optic
 * @param resource_hdl[in] - Handle of the resource
 * @param threshold_type[in] - Threshold type. Refer @ref sdi_media_dell_product_info_t
 * for valid threshold types
 * @param value[out] - Threshold value
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_threshold_get(sdi_resource_hdl_t resource_hdl,
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
t_std_error sdi_sfp_module_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                 uint_t threshold_type, uint_t *value);

/**
 * @brief Get the threshold values for channel monitors like rx power and tx
 * bias
 * @param resource_hdl[in] - Handle of the resource
 * @param threshold_type[in] - Should be one of the flag @ref sdi_media_channel_status
 * @param value[out] - threshold value
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_channel_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                  uint_t threshold_type, uint_t *value);

/**
 * @brief retrieve module monitors assoicated with the specified media.
 * @param[in] resource_hdl - handle to sfp
 * @param[in] monitor - The monitor which needs to be retrieved.
 * @param[out] value - Value of the monitor
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                        sdi_media_module_monitor_t monitor, float *value);

/**
 * @brief retrieve channel monitors assoicated with the specified media.
 * @param[in] resource_hdl - handle to sfp
 * @param[in] channel - The channel number
 * @param[in] monitor - The monitor which needs to be retrieved.
 * @param[out] value - Value of the monitor
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_channel_monitor_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_channel_monitor_t monitor, float *value);

/**
 * @brief Get the optional feature support status for optics
 * @param resource_hdl[in] - handle to sfp
 * @param feature_support[out] - feature support flags. Flag will be set to
 * "true" if feature supported else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                sdi_media_supported_feature_t *feature_support);

/**
 * @brief Set the port LED based on the speed settings of the port
 * @param resource_hdl[in] - handle to sfp
 * @param channel[in] - Channel number. Should be 0 in sfp
 * @param speed[in] - LED mode setting is derived from speed
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_led_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                             sdi_media_speed_t speed);

/**
 * @brief Raw read api for media eeprom
 * @param resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * @param data[in] - Data to read
 * @param data_len[in] - length of the data to read
 * @return return - standard @ref t_std_error
*/
t_std_error sdi_sfp_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len);

/**
 * @brief Raw write api for media eeprom
 * @param resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * @param data[in] - Data to write
 * @param data_len[in] - length of the data to be written
 * @return return - standard @ref t_std_error
*/
t_std_error sdi_sfp_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len);

/**
 * @brief raw read from sfp eeprom
 * @param[in] resource_hdl - handle to the sfp
 * @param[in] offset - offset from which to read
 * @param[out] data - buffer for read data
 * @param[in] data_len - length of the data to be read
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                          uint8_t *data, size_t data_len);

/**
 * @brief Debug api to write data in to sfp eeprom
 * @param[in] resource_hdl - handle to the sfp
 * @param[in] offset - offset from which to read
 * @param[in] data - input buffer which contains the data to be written
 * @param[in] data_len - length of the data to be written
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len);

/**
 * @brief Api to enable/disable Autoneg on media PHY .
 * @param[in] resource_hdl - handle to media
 * @param[in] speed - speed of the interface. Should be of type @ref sdi_media_speed_t .
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_sfp_phy_autoneg_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                     sdi_media_type_t type, bool enable);

/**
 * @brief Api to set mode on media PHY .
 * @param[in] resource_hdl - handle to media
 * @param[in] mode - SGMII/MII/GMII, Should be of type Refer @ref sdi_media_mode_t.
 * @return - standard @ref t_std_error
 *
 */

t_std_error sdi_sfp_phy_mode_set (sdi_resource_hdl_t resource_hdl,uint_t channel,
                                  sdi_media_type_t type, sdi_media_mode_t mode);

/**
 * @brief Api to  set speed  on media PHY .
 * @param[in] resource_hdl - handle to media
 * @param[in] speed - speed of the interface. Should be of type @ref sdi_media_speed_t .
 * @return - standard @ref t_std_error
 *
 */

t_std_error sdi_sfp_phy_speed_set(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                  sdi_media_type_t type, sdi_media_speed_t speed);


/*
 * @brief initialize pluged in module
 * @param[in] resource_hdl - handle to the sfp
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */
t_std_error sdi_sfp_module_init (sdi_resource_hdl_t resource_hdl, bool pres);

/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */
t_std_error sdi_sfp_wavelength_set (sdi_resource_hdl_t resource_hdl, float value);

/**
 * @brief Api to get link status from media PHY.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number, 0 is valid for SFP
 * @param[in] type - Media type.
 * param[out] status - true when link is up, false when link is down
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_sfp_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_type_t type, bool *status);

/**
 * @brief Api to power down enable/disable on media PHY.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number, 0 is valid for SFP
 * @param[in] type - Media type.
 * @param[in] enable - true power down media PHY, false - power up media PHY.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_sfp_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool enable);

/**
 * @brief Api to enable/disable Fiber/Serdes TX and RX on media PHY.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number, 0 is valid for SFP
 * @param[in] type - Media type.
 * @param[in] enable - true enable Serdes, false - Disable Serdes.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_sfp_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                        sdi_media_type_t type, bool enable);

/**
 * @brief API to get QSA adapter type
 * resource_hdl[in] - Handle of the resource
 * sdi_qsa_adapter_type_t*[out] - adapter type obtained
 * return           - t_std_error
 */

t_std_error sdi_sfp_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter);

/**
 * @brief API to get the info of the port in which the device is connected
 * resource_hdl[in] - Handle of the resource
 * sdi_media_port_info_t*[out] - port info type obtained
 * return           - t_std_error
 */

t_std_error sdi_sfp_port_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_port_info_t* port_info);

/**
 * @brief API to get the module
 * resource_hdl[in] - Handle of the resource
 * sdi_media_module_info_t*[out] - module info
 * return           - t_std_error
 */

t_std_error sdi_sfp_module_info_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_module_info_t* module_info);

#endif
