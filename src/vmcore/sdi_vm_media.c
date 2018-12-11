/*
 * Copyright (c) 2018 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * @file sdi_vm_media.c
 * @brief Media simulation functionality implements sdi-api headers
 */

#include "sdi_sys_vm.h"
#include "sdi_entity.h"
#include "sdi_media.h"
#include "sdi_db.h"

/*
 * Get the media presence status
 */
t_std_error sdi_media_presence_get(sdi_resource_hdl_t resource_hdl, bool *presence)
{
    t_std_error rc;
    int present;

    STD_ASSERT(presence != NULL);

    rc = sdi_db_int_field_get(sdi_get_db_handle(), resource_hdl, TABLE_MEDIA,
                              MEDIA_PRESENCE, &present);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    *presence = (bool)present;
    return STD_ERR_OK;
}

/*
 * Get the required module monitor status of the specific media.
 */
t_std_error sdi_media_module_monitor_status_get (sdi_resource_hdl_t resource_hdl, uint_t flags, uint_t *status)
{
    return sdi_media_channel_monitor_status_get(resource_hdl, MEDIA_NO_CHANNEL,
                                                flags, status);
}

/*
 * Api to enable/disable Autoneg on media PHY
 */
t_std_error sdi_media_phy_autoneg_set (sdi_resource_hdl_t resource_hdl,
                                       uint_t channel, sdi_media_type_t type,
                                       bool enable)
{
    //TODO: To be implemented after discussing the required implementation. Stubbed out to allow build to succeed
    return STD_ERR_OK;
}

/*
 * Api to set mode on media PHY
 */
t_std_error sdi_media_phy_mode_set (sdi_resource_hdl_t resource_hdl,
                                    uint_t channel, sdi_media_type_t type,
                                    sdi_media_mode_t mode)
{
    //TODO: To be implemented after discussing the required implementation. Stubbed out to allow build to succeed
    return STD_ERR_OK;
}

/*
 * Api to set speed on media PHY
 */
t_std_error sdi_media_phy_speed_set (sdi_resource_hdl_t resource_hdl,
                                     uint_t channel, sdi_media_type_t type,
                                     sdi_media_speed_t *speed,uint_t count)
{
    //TODO: To be implemented after discussing the required implementation. Stubbed out to allow build to succeed
    return STD_ERR_OK;
}

/*
 * Set the LED status of the specific media based on speed.
 */
t_std_error sdi_media_led_set (sdi_resource_hdl_t resource_hdl, uint_t channel, sdi_media_speed_t speed)
{
    //TODO: To be implemented after discussing the required implementation. Stubbed out to allow build to succeed
    return STD_ERR_OK;
}

/*
 * Get the required channel monitor status of the specific media.
 */
t_std_error sdi_media_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel, uint_t flags, uint_t *status)
{
    t_std_error rc;
    uint_t local_status;

    STD_ASSERT(status != NULL);

    rc = sdi_db_media_channel_int_field_get(sdi_get_db_handle(), resource_hdl,
                                            channel, MEDIA_MONITOR_STATUS,
                                            (int *)&local_status);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Mask off and return the requested bits */
    *status = (local_status & flags);
    return STD_ERR_OK;
}

/*
 * Get the required channel status of the specific media.
 */
t_std_error sdi_media_channel_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel, uint_t flags, uint_t *status)
{
    t_std_error rc;
    uint_t local_status;

    STD_ASSERT(status != NULL);

    rc = sdi_db_media_channel_int_field_get(sdi_get_db_handle(), resource_hdl,
                                            channel, MEDIA_CHANNEL_STATUS,
                                            (int *)&local_status);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Mask off and return the requested bits */
    *status = (local_status & flags);
    return STD_ERR_OK;
}

/*
 * Disable/Enable the transmitter of the specific media.
 */
t_std_error sdi_media_tx_control(sdi_resource_hdl_t resource_hdl, uint_t channel, bool enable)
{
    return sdi_db_media_channel_int_field_target_set(sdi_get_db_handle(), resource_hdl,
                                              channel, MEDIA_TX_ENABLE, enable);
}

/*
 * Get the transmitter status on a particular channel of the specific media
 */
t_std_error sdi_media_tx_control_status_get(sdi_resource_hdl_t resource_hdl,
                                            uint_t channel, bool *status)
{
    t_std_error rc;
    int db_status;

    STD_ASSERT(status != NULL);

    rc = sdi_db_media_channel_int_field_get(sdi_get_db_handle(), resource_hdl,
                                            channel, MEDIA_TX_ENABLE, &db_status);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    *status = (bool)db_status;
    return STD_ERR_OK;
}

/*
 * Read the speed of a specific media resource.
 */
t_std_error sdi_media_speed_get(sdi_resource_hdl_t resource_hdl, sdi_media_speed_t *speed)
{
    STD_ASSERT(speed != NULL);

    return sdi_db_int_field_get(sdi_get_db_handle(), resource_hdl, TABLE_MEDIA,
                                MEDIA_OPTIC_SPEED, (int *)speed);
}

/*
 * Read the parameter value from the specific media
 */
t_std_error sdi_media_parameter_get(sdi_resource_hdl_t resource_hdl,
                                    sdi_media_param_type_t param_type,
                                    uint_t *value)
{
    STD_ASSERT(value != NULL);

    return sdi_db_media_param_get(sdi_get_db_handle(), resource_hdl,
                                  param_type, value);
}

/*
 * Read the requested vendor information from the specific media
 */
t_std_error sdi_media_vendor_info_get(sdi_resource_hdl_t resource_hdl,
                                      sdi_media_vendor_info_type_t info_type,
                                      char *info, size_t buf_size)
{
    uint_t len;

    STD_ASSERT(info != NULL);
    switch (info_type) {
    case SDI_MEDIA_VENDOR_OUI:
        /*
         * Because OUI is the sole non-ASCII field, we are saving it in the
         * main media table as a blob field
         */
        len = SDI_MEDIA_MAX_VENDOR_OUI_LEN;
        /* Ensure we have sufficient space in the buffer */
        if (buf_size < len) {
            return STD_ERR(BOARD, PARAM, EINVAL);
        }
        return sdi_db_bin_field_get(sdi_get_db_handle(), resource_hdl,
                                    TABLE_MEDIA, MEDIA_VENDOR_OUI,
                                    (uint8_t *)info, &len);

    default:
        /* All string fields are retrieved through the vendor_info API */
        return sdi_db_media_vendor_info_get(sdi_get_db_handle(), resource_hdl,
                                            info_type, buf_size, info);
    }
}

/*
 * Read the transceiver code from the media resource
 */
t_std_error sdi_media_transceiver_code_get(sdi_resource_hdl_t resource_hdl,
                                           sdi_media_transceiver_descr_t *code)
{
    uint_t len;
    STD_ASSERT(code != NULL);

    len = sizeof(*code);
    return sdi_db_bin_field_get(sdi_get_db_handle(), resource_hdl,
                                TABLE_MEDIA, MEDIA_TRANSCEIVER_CODE,
                                (uint8_t *)code, &len);
}

/*
 * Get the Media Supported Feature information
 */
t_std_error sdi_media_feature_support_status_get(sdi_resource_hdl_t resource_hdl,
                                                 sdi_media_supported_feature_t *feature_support)
{
    uint_t size = sizeof(*feature_support);
    STD_ASSERT(feature_support != NULL);
    memset(feature_support, 0, size);

    return sdi_db_bin_field_get(sdi_get_db_handle(), resource_hdl, TABLE_MEDIA,
                                MEDIA_SUPPORTED_FEATURES, (uint8_t *)feature_support, &size);
}

static char * media_module_ctrl_map(sdi_media_module_ctrl_type_t ctrl_type)
{
    switch (ctrl_type) {
    case SDI_MEDIA_LP_MODE:
        return MEDIA_LP_MODE;

    case SDI_MEDIA_RESET:
        return MEDIA_RESET;

    default:
        return NULL;
    }
}

/*
 * Enable/Disable the LP mode/reset control on a specific media
 */
t_std_error sdi_media_module_control(sdi_resource_hdl_t resource_hdl,
                                     sdi_media_module_ctrl_type_t ctrl_type,
                                     bool enable)
{
    char *ctrl = media_module_ctrl_map(ctrl_type);
    int db_enable = (int)enable;

    return sdi_db_int_field_target_set(sdi_get_db_handle(), resource_hdl, TABLE_MEDIA,
                                ctrl, &db_enable);
}

/*
 * Get the LP mode/reset control status on a specific media
 */
t_std_error sdi_media_module_control_status_get(sdi_resource_hdl_t resource_hdl,
                                                sdi_media_module_ctrl_type_t ctrl_type,
                                                bool *status)
{
    char *ctrl = media_module_ctrl_map(ctrl_type);
    int db_status;
    t_std_error rc;

    STD_ASSERT(status != NULL);

    rc = sdi_db_int_field_get(sdi_get_db_handle(), resource_hdl, TABLE_MEDIA,
                              ctrl, &db_status);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    *status = (bool)db_status;
    return STD_ERR_OK;
}

/* Get the module monitor thresholds */
t_std_error sdi_media_module_monitor_threshold_get(sdi_resource_hdl_t media_hdl,
                                                   uint_t monitor_type,
                                                   uint_t *threshold)
{
    STD_ASSERT(threshold != NULL);

    return sdi_db_media_monitor_threshold_get(sdi_get_db_handle(), media_hdl,
                                              MEDIA_NO_CHANNEL, monitor_type,
                                              threshold);
}

/* Get the channel monitor thresholds */
t_std_error sdi_media_channel_monitor_threshold_get(sdi_resource_hdl_t media_hdl,
                                                    uint_t monitor_type,
                                                    uint_t *threshold)
{
    STD_ASSERT(threshold != NULL);

    return sdi_db_media_monitor_threshold_get(sdi_get_db_handle(), media_hdl,
                                              MEDIA_DEFAULT_CHANNEL, monitor_type,
                                              threshold);
}

/* Get the media thresholds */
t_std_error sdi_media_threshold_get(sdi_resource_hdl_t media_hdl,
                                    sdi_media_threshold_type_t threshold_type,
                                    float *value)
{
    STD_ASSERT(value != NULL);

    return sdi_db_media_threshold_get(sdi_get_db_handle(), media_hdl,
                                      threshold_type, value);
}

static char *media_monitor_map(sdi_media_module_monitor_t monitor)
{
    switch(monitor) {
    case SDI_MEDIA_TEMP:
        return MEDIA_MONITOR_TEMP;
    case SDI_MEDIA_VOLT:
        return MEDIA_MONITOR_VOLT;
    default:
        return NULL;
    }
}

static char *channel_monitor_map(sdi_media_channel_monitor_t monitor)
{
    switch(monitor) {
    case SDI_MEDIA_INTERNAL_RX_POWER_MONITOR:
        return MEDIA_MONITOR_RX_POWER;
    case SDI_MEDIA_INTERNAL_TX_BIAS_CURRENT:
        return MEDIA_MONITOR_TX_BIAS;
    case SDI_MEDIA_INTERNAL_TX_OUTPUT_POWER:
        /* We don't actually have a field for this, for now, stick with
         * the RX power
         */
        return MEDIA_MONITOR_RX_POWER;
    default:
        return NULL;
    }
}

static t_std_error sdi_media_module_or_channel_monitor_get(sdi_resource_hdl_t resource_hdl,
                                                           uint_t channel,
                                                           char *monitor_field,
                                                           float *value)
{
    STD_ASSERT(monitor_field != NULL);
    STD_ASSERT(value != NULL);

    return sdi_db_media_channel_float_field_get(sdi_get_db_handle(),
                                                resource_hdl, channel,
                                                monitor_field, value);
}

/*
 * Debug api to retrieve module monitors assoicated with the specified media.
 */
t_std_error sdi_media_module_monitor_get(sdi_resource_hdl_t resource_hdl, sdi_media_module_monitor_t monitor, float *value)
{
    char *monitor_field = media_monitor_map(monitor);
    return sdi_media_module_or_channel_monitor_get(resource_hdl, MEDIA_NO_CHANNEL,
                                                   monitor_field, value);
}

/*
 * Debug api to retrieve channel monitors assoicated with the specified media.
 */
t_std_error sdi_media_channel_monitor_get(sdi_resource_hdl_t resource_hdl, uint_t channel, sdi_media_channel_monitor_t monitor, float *value)
{
    char *monitor_field = channel_monitor_map(monitor);
    return sdi_media_module_or_channel_monitor_get(resource_hdl, channel,
                                                   monitor_field, value);
}

/* The read and write API are not implemented for the VM but are simply stub
 * functions to prevent failure at link time. These APIs are used by
 * serviceability, but only on the real hardware.
 */

/*
 * Debug api to read data from media, with paging support
 */
t_std_error sdi_media_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t *addr,
                     uint8_t *data, size_t data_len)
{
    return STD_ERR_OK;
}

/*
 * Debug api to write data in to media, with paging support
 */
t_std_error sdi_media_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t *addr,
                     uint8_t *data, size_t data_len)
{
    return STD_ERR_OK;
}

/*
 * Debug api to read data from media
 */
t_std_error sdi_media_read(sdi_resource_hdl_t resource_hdl, uint_t offset, uint8_t *data ,size_t data_len)
{
    return STD_ERR_OK;
}

/*
 * Debug api to write data in to media
 */
t_std_error sdi_media_write(sdi_resource_hdl_t resource_hdl, uint_t offset, uint8_t *data, size_t data_len)
{
    return STD_ERR_OK;
}

/*
 * initialize pluged in module
 */
t_std_error sdi_media_module_init (sdi_resource_hdl_t resource_hdl, bool pres)
{
    return STD_ERR_OK;
}

/*
 * Set wavelength for tunable media
 */

t_std_error sdi_media_wavelength_set (sdi_resource_hdl_t resource_hdl, float value)
{
    return STD_ERR_OK;
}

/*
 * Disable/Enable the clock and data recovery function of qsfp per channel.
 */

t_std_error sdi_media_cdr_status_set (sdi_resource_hdl_t resource_hdl,
                                     uint_t channel, bool enable)
{
    return STD_ERR_OK;
}

/*
 * Get the clock and data recovery status on a particular channel of qsfp
 */

t_std_error sdi_media_cdr_status_get (sdi_resource_hdl_t resource_hdl,
                                     uint_t channel, bool *status)
{
    *status = true;
    return STD_ERR_OK;
}

/*
 * Api to get phy link status .
 *
 */
t_std_error sdi_media_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool *status)
{
    *status = true;
    return STD_ERR_OK;
}

/*
 * Api to control media phy and MAC interfaces power down enable/disable .
 */
t_std_error sdi_media_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                             sdi_media_type_t type, bool enable)
{
    return STD_ERR_OK;
}

/*
 * API to select the desired channel rate on special media
 */
t_std_error sdi_media_ext_rate_select (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_fw_rev_t rev, bool cdr_enable)
{
    return STD_ERR_OK;
}

/*
 * Api to enable/disable Fiber/Serdes transmitter and receiver .
 */
t_std_error sdi_media_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_type_t type, bool enable)
{
    return STD_ERR_OK;
}

/*
 * API to get QSA adapter type
 */
t_std_error sdi_media_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl, sdi_qsa_adapter_type_t* qsa_adapter)
{
    *qsa_adapter = SDI_QSA_ADAPTER_NONE;
    return STD_ERR_OK;
}

