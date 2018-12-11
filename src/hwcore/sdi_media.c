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
 * filename: sdi_media.c
 */


/*******************************************************************************
 * Implementation of Media resource API.
 ******************************************************************************/
#include "sdi_media_internal.h"
#include "sdi_media.h"
#include "sdi_resource_internal.h"
#include "std_assert.h"

/**
 * Get the present status of the specific media
 * resource_hdl[in] - Handle of the resource
 * pres[out]        - "true" if module is present else "false"
 * return t_std_error
 */
t_std_error sdi_media_presence_get (sdi_resource_hdl_t resource_hdl, bool *pres)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(pres != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->presence_get(media_hdl->callback_hdl, pres);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to get the media present status for %s error code : %d(0x%x)",
                       media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * Gets the required module monitors(temperature and voltage) alarm status
 * resource_hdl[in] - Hnadle of the resource
 * flags[in]        - flags for status that are of interest
 * status[out]      - returns the set of status flags
 * return t_std_error
 */
t_std_error sdi_media_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                 uint_t flags, uint_t *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = 0;

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->module_monitor_status_get(media_hdl->callback_hdl,
                                                                              flags, status);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get module monitor status for %s error code : %d(0x%x)",
                            media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * Get the required channel monitoring(rx_power and tx_bias) alarm status of media.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest, it should be '0' if
 *                    only one channel is present
 * flags[in]        - flags for channel status
 * status[out]      - returns the set of status flags which are asserted.
 * return           - standard t_std_error
 */
t_std_error sdi_media_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                                  uint_t flags, uint_t *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = 0;

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->channel_monitor_status_get(media_hdl->callback_hdl,
                                                                               channel, flags, status);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get channel monitor status for %s"
                           "error code : %d(0x%x)", media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Get the required channel status of the specific media.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest, it should be '0' if
 *                    only one channel is present
 * flags[in]        - flags for channel status
 * status[out]      - returns the set of status flags which are asserted.
 * return           - standard t_std_error
 */
t_std_error sdi_media_channel_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          uint_t flags, uint_t *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = 0;

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->channel_status_get(media_hdl->callback_hdl,
                                                                       channel, flags, status);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get channel status for %s, error code : %d(0x%x)",
                            media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * Disable/Enable the transmitter of the specific media.
 * resource_hdl[in] - handle of the media resource
 * channel[in]      - channel number that is of interest and should be 0 only
 *                    one channel is present
 * enable[in]       - "false" to disable and "true" to enable
 * @return          - standard t_std_error
 */
t_std_error sdi_media_tx_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                  bool enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->tx_control (media_hdl->callback_hdl,
                                                                channel, enable);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to set the tx control for %s, error code : %d(0x%x)",
                       media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * For getting transmitter status(enabled/disabled) on the specified channel
 * resource_hdl[in] - handle of the media resource
 * channel[in]      - channel number
 * status[out]      -  transmitter status-> "true" if enabled, else "false"
 * return           - standard t_std_error
 */
t_std_error sdi_media_tx_control_status_get (sdi_resource_hdl_t resource_hdl,
                                             uint_t channel, bool *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->tx_control_status_get(media_hdl->callback_hdl,
                                                                          channel, status);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to set the tx control status for %s, error code : %d(0x%x)",
                       media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * Disable/Enable the cdr of the specific media.
 * resource_hdl[in] - handle of the media resource
 * channel[in]      - channel number that is of interest and should be 0 only
 *                    one channel is present
 * enable[in]       - "false" to disable and "true" to enable
 * @return          - standard t_std_error
 */
t_std_error sdi_media_cdr_status_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                  bool enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if(((media_ctrl_t *)media_hdl->callback_fns)->cdr_status_set == NULL) {
        return  SDI_ERRCODE(EOPNOTSUPP);
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->cdr_status_set (media_hdl->callback_hdl,
                                                                channel, enable);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to set the cdr status for %s, error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * For getting transmitter status(enabled/disabled) on the specified channel
 * resource_hdl[in] - handle of the media resource
 * channel[in]      - channel number
 * status[out]      -  transmitter status-> "true" if enabled, else "false"
 * return           - standard t_std_error
 */
t_std_error sdi_media_cdr_status_get (sdi_resource_hdl_t resource_hdl,
                                             uint_t channel, bool *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if(((media_ctrl_t *)media_hdl->callback_fns)->cdr_status_get == NULL) {
        return  SDI_ERRCODE(EOPNOTSUPP);
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->cdr_status_get (media_hdl->callback_hdl,
                                                                          channel, status);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get the cdr status for %s, error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * Get the maximum speed that can be supported by a specific media resource
 * resource_hdl[in] - handle of the media resource
 * speed[out]       - maximum speed that can be supported by media device
 * return           - standard t_std_error
 */
t_std_error sdi_media_speed_get (sdi_resource_hdl_t resource_hdl,
                                 sdi_media_speed_t *speed)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(speed != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->speed_get (media_hdl->callback_hdl,
                                                               speed);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to get the speed for %s, error code : %d(0x%x)",
                        media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * Reads the requested parameter value from eeprom
 * resource_hdl[in] - handle of the media resource
 * param[in]        - parametr type that is of interest(e.g wavelength, maximum
 *                    case temperature etc)
 * value[out]       - parameter value which is read from eeprom
 * return           - standard t_std_error
 */
t_std_error sdi_media_parameter_get (sdi_resource_hdl_t resource_hdl,
                                     sdi_media_param_type_t param, uint_t *value)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->parameter_get(media_hdl->callback_hdl,
                                                                  param, value);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get the requested parameter for %s, error code : %d(0x%x)",
                            media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Read the requested vendor information of a specific media resource
 * resource_hdl[in]     - handle of the media resource
 * vendor_info_type[in] - vendor information that is of interest.
 * vendor_info[out]     - vendor information which is read from eeprom
 * buf_size[in]         - size of the input buffer(vendor_info)
 * return               - standard t_std_error
 */
t_std_error sdi_media_vendor_info_get (sdi_resource_hdl_t resource_hdl,
                                       sdi_media_vendor_info_type_t vendor_info_type,
                                       char *vendor_info, size_t buf_size)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(vendor_info != NULL);
    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->vendor_info_get(media_hdl->callback_hdl,
                                                                    vendor_info_type,
                                                                    vendor_info, buf_size);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to get the vendor information for %s, error code : %d(0x%x)",
                        media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * Read the transceiver compliance code of a specific media resource
 * resource_hdl[in]     - handle of the media resource
 * transceiver_info[out]- transceiver compliance code which is read from eeprom
 * return               - standard t_std_error
 */
t_std_error sdi_media_transceiver_code_get (sdi_resource_hdl_t resource_hdl,
                                            sdi_media_transceiver_descr_t *transceiver_info)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(transceiver_info != NULL);
    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->transceiver_code_get(media_hdl->callback_hdl,
                                                                         transceiver_info);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to get the transceiver compliance information for %s"
                       "error code : %d(0x%x)", media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * Get the alarm and warning threshold values for a given optics
 * resource_hdl[in] - Handle of the resource
 * threshold_type[in] - type of threshold
 * value[out] - threshold value
 * return - standard t_std_error
 */
t_std_error sdi_media_threshold_get (sdi_resource_hdl_t resource_hdl,
                                     sdi_media_threshold_type_t threshold_type,
                                     float *value)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->threshold_get(media_hdl->callback_hdl,
                                                                  threshold_type, value);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get threshold value for %s error code : %d(0x%x)",
                            media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Read the threshold values for module monitors like temperature and voltage
 * resource_hdl[in] - Handle of the resource
 * threshold_type[in] - type of threshold
 * value[out] - threshold value
 * return - standard t_std_error
 * TODO: depricated API. Need to remove once upper layers adopted new api
 * sdi_media_threshold_get
 */
t_std_error sdi_media_module_monitor_threshold_get (sdi_resource_hdl_t resource_hdl,
                                                    uint_t threshold_type, uint_t *value)
{
    return STD_ERR_OK;
}

/**
 * Read the threshold values for channel monitors like rx-ower and tx-bias
 * resource_hdl[in] - Handle of the resource
 * threshold_type[in] - type of threshold
 * value[out] - threshold value
 * return - standard t_std_error
 * TODO: depricated API. Need to remove once upper layers adopted new api
 * sdi_media_threshold_get
 */
t_std_error sdi_media_channel_monitor_threshold_get (sdi_resource_hdl_t resource_hdl,
                                                     uint_t threshold_type, uint_t *value)
{
    return STD_ERR_OK;
}

/**
 * Enable/Disable the module control parameters like low power mode and reset
 * control
 * resource_hdl[in] - handle of the resource
 * ctrl_type[in]    - module control type(LP mode/reset)
 * enable[in]       - "true" to enable and "false" to disable
 * return           - standard t_std_error
 */
t_std_error sdi_media_module_control (sdi_resource_hdl_t resource_hdl,
                                      sdi_media_module_ctrl_type_t ctrl_type, bool enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if(((media_ctrl_t *)media_hdl->callback_fns)->module_control == NULL) {
        return  SDI_ERRCODE(EOPNOTSUPP);
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->module_control(media_hdl->callback_hdl,
                                                                   ctrl_type, enable);
    if (rc != STD_ERR_OK){
        if (STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to set module control parameters for %s, error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Enable/Disable Auto neg on SFP PHY
 * resource_hdl[in] - handle of the resource
 * enable[in]       - "true" to enable and "false" to disable
 * return           - standard t_std_error
 */

t_std_error sdi_media_phy_autoneg_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_type_t type, bool enable)
{

    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_phy_autoneg_set(media_hdl->callback_hdl,
            channel, type, enable);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to Set autoneg for media phy details for %s error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * set mode on  SFP PHY
 * resource_hdl[in] - handle of the resource
 * mode [in]       -  mode of interface (SGMII/MII/GMII..)
 * return           - standard t_std_error
 */


t_std_error sdi_media_phy_mode_set (sdi_resource_hdl_t resource_hdl, 
                                    uint_t channel, sdi_media_type_t type,
                                    sdi_media_mode_t mode)
{

    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_phy_mode_set(media_hdl->callback_hdl,
            channel, type, mode);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to Set mode for media phy details for %s error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * set speed on  SFP PHY
 * resource_hdl[in] - handle of the resource
 * speed [in]       - speed of interface (1G/100M/10M)
 * count[in]        - count for number of phy supported speed's 10/100/1000.
 * return           - standard t_std_error
 */

t_std_error sdi_media_phy_speed_set(sdi_resource_hdl_t resource_hdl,
                                    uint_t channel, sdi_media_type_t type,
                                    sdi_media_speed_t *speed, uint_t count)
{

    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    while(count != 0){
        rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_phy_speed_set(media_hdl->callback_hdl,
                channel, type, *speed);
        if (rc != STD_ERR_OK){
            if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
                SDI_ERRMSG_LOG("Failed to Set speed for media phy details for %s error code : %d(0x%x)",
                        media_hdl->name, rc, rc);
            }
        }
        speed++;
        count--;
    }

    return rc;
}

/**
 * get media phy link status 
 * resource_hdl[in] - handle of the resource
 * channel [in]     - channel number
 * type[in]         - Media type
 * status[out]       - link status
 * return           - standard t_std_error
 */

t_std_error sdi_media_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel, 
                                           sdi_media_type_t type, bool *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_phy_link_status_get(media_hdl->callback_hdl,
            channel, type, status);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to Set mode for media phy details for %s error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * Power down enable / disable 
 * resource_hdl[in] - handle of the resource
 * channel [in]     - channel number
 * type[in]         - Media type
 * enable[in]       - True - Power down, False - Power up
 * return           - standard t_std_error
 */

t_std_error sdi_media_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_type_t type, bool enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_phy_power_down_enable(media_hdl->callback_hdl,
            channel, type, enable);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to Set mode for media phy details for %s error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * Fiber/Serdes TX and RX  enable / disable 
 * resource_hdl[in] - handle of the resource
 * channel [in]     - channel number
 * type[in]         - Media type
 * enable[in]       - True - enable, False - disable
 * return           - standard t_std_error
 */

t_std_error sdi_media_phy_serdes_control(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_type_t type, bool enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_phy_serdes_control(media_hdl->callback_hdl,
            channel, type, enable);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to Set mode for media phy details for %s error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
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
t_std_error sdi_media_module_control_status_get (sdi_resource_hdl_t resource_hdl,
                                                 sdi_media_module_ctrl_type_t ctrl_type,
                                                 bool *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if(((media_ctrl_t *)media_hdl->callback_fns)->module_control_status_get == NULL) {
        return  SDI_ERRCODE(EOPNOTSUPP);
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->module_control_status_get(media_hdl->callback_hdl,
                                                                              ctrl_type, status);
    if (rc != STD_ERR_OK){
        if (STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to set module control parameters for %s, error code : %d(0x%x)",
                    media_hdl->name, rc, rc);
        }
    }
    return rc;
}
/**
 * Retrieve module monitors assoicated with the specified media
 * resource_hdl[in] - handle of the media resource
 * monitor[in]      - monitor which needs to be retrieved(TEMPERATURE/VOLTAGE)
 * value[out]       - Value of the monitor
 * return           - standard t_std_error
 */
t_std_error sdi_media_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                          sdi_media_module_monitor_t monitor, float *value)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(value != NULL);
    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->module_monitor_get(media_hdl->callback_hdl,
                                                                       monitor, value);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get module monitor  details for %s error code : %d(0x%x)",
                            media_hdl->name, rc, rc);
        }
    }

    return rc;
}

/**
 * Retrieve channel monitors assoicated with the specified media.
 * resource_hdl[in] - handle of the media resource
 * channel[in]      - channel whose monitor has to be retreived and should be 0,
 *                    if only one channel is present.
 * monitor[in]      - monitor which needs to be retrieved.
 * value[out]       - Value of the monitor
 * return           - standard t_std_error
 */
t_std_error sdi_media_channel_monitor_get (sdi_resource_hdl_t resource_hdl,
                                           uint_t channel, sdi_media_channel_monitor_t monitor, float *value)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(value != NULL);
    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->channel_monitor_get(media_hdl->callback_hdl,
                                                                        channel, monitor, value);
    if (rc != STD_ERR_OK){
        if( STD_ERR_EXT_PRIV(rc) != EOPNOTSUPP ) {
            SDI_ERRMSG_LOG("Failed to get channel monitor details for %s error code : %d(0x%x)",
                            media_hdl->name, rc, rc);
        }
    }

    return rc;
}



/**
 * Read data from media
 * resource_hdl[in] - handle of the media resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * data[out]        - buffer for data to be read
 * data_len[in]     - length of the data to be read
 * return           - standard t_std_error
 */
t_std_error sdi_media_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                      uint8_t *data, size_t data_len)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(data != NULL);
    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(addr != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if (addr == NULL){
        SDI_ERRMSG_LOG("Invalid argument passed when trying to read EEPROM");
        return EINVAL;
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->read_generic(media_hdl->callback_hdl, addr,
                        data, data_len);
    if (rc != STD_ERR_OK){
        if( rc == STD_ERR_UNIMPLEMENTED) {
            SDI_ERRMSG_LOG("Raw read from optic eeprom is not implemented for %s"
                           "error code : %d(0x%x)", media_hdl->name, rc, rc);
        } else {
            SDI_ERRMSG_LOG("Failed to read from offset %u for %s error code : %d(0x%x)",
                           addr->offset, media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Write data to media
 * resource_hdl[in] - handle of the media resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * data[in]         - buffer holding data to be written
 * data_len[in]     - length of the data to write
 * return           - standard t_std_error
 */
t_std_error sdi_media_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                     uint8_t *data, size_t data_len)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(data != NULL);
    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(addr != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if (addr == NULL){
        SDI_ERRMSG_LOG("Invalid argument passed when trying to write to EEPROM");
        return EINVAL;
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->write_generic(media_hdl->callback_hdl, addr,
                        data, data_len);
    if (rc != STD_ERR_OK){
        if( rc == STD_ERR_UNIMPLEMENTED) {
            SDI_ERRMSG_LOG("Raw read from optic eeprom is not implemented for %s"
                           "error code : %d(0x%x)", media_hdl->name, rc, rc);
        } else {
            SDI_ERRMSG_LOG("Failed to write to offset %u for %s error code : %d(0x%x)",
                           addr->offset, media_hdl->name, rc, rc);
        }
    }
    return rc;
}


/**
 * Read data from media
 * resource_hdl[in] - handle of the media resource
 * offset[in]       - offset from which data to be read
 * data[out]        - buffer for data to be read
 * data_len[in]     - length of the data to be read
 * return           - standard t_std_error
 */
t_std_error sdi_media_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                            uint8_t *data, size_t data_len)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(data != NULL);
    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->read(media_hdl->callback_hdl,
                                                         offset, data, data_len);
    if (rc != STD_ERR_OK){
        if( rc == STD_ERR_UNIMPLEMENTED) {
            SDI_ERRMSG_LOG("Raw read from optic eeprom is not implemented for %s"
                           "error code : %d(0x%x)", media_hdl->name, rc, rc);
        } else {
            SDI_ERRMSG_LOG("Failed to read from offset %u for %s error code : %d(0x%x)",
                           offset, media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Write data to media
 * resource_hdl[in] - handle of the media resource
 * offset[in]       - offset to which data to be write
 * data[in]         - input buffer which contains data to be written
 * data_len[in]     - length of the data to be write
 * return standard t_std_error
 */
t_std_error sdi_media_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                             uint8_t *data, size_t data_len)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->write(media_hdl->callback_hdl,
                                                          offset, data, data_len);
    if (rc != STD_ERR_OK){
        if( rc == STD_ERR_UNIMPLEMENTED) {
            SDI_ERRMSG_LOG("Raw read from optic eeprom is not implemented for %s"
                           "error code : %d(0x%x)", media_hdl->name, rc, rc);
        } else {
            SDI_ERRMSG_LOG("Failed to write from offset %u for %s error code : %d(0x%x)",
                           offset, media_hdl->name, rc, rc);
        }
    }
    return rc;
}

/**
 * Get the optional feature support status on a given optics
 * resource_hdl[in] - handle of the media resource
 * feature_support[out] - feature support flags. Flag will be set to "true" if
 * feature is supported else "false"
 * return - standard t_std_error
 */
t_std_error sdi_media_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                  sdi_media_supported_feature_t *feature_support)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(feature_support != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->feature_support_status_get(media_hdl->callback_hdl,
                                                                               feature_support);
    if (rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("Failed to get optional fields support status for %s with error code 0x%x",
                       media_hdl->name, rc);
    }
    return rc;
}

/**
 * Set the port LED based on the speed settings of the port.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - Channel number. Should be 0, if only one channel is present
 * speed[in] - LED mode setting is derived from speed
 * return - standard t_std_error
 */
t_std_error sdi_media_led_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                               sdi_media_speed_t speed)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->led_set(media_hdl->callback_hdl,
                                                            channel, speed);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to set the led for %s, error code : %d(0x%x)",
                        media_hdl->name, rc, rc);
    }

    return rc;
}

/*
 * @brief initialize plugedin  module
 * @param[in] resource_hdl - handle to the qsfp
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */

t_std_error sdi_media_module_init (sdi_resource_hdl_t resource_hdl, bool pres)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->module_init(media_hdl->callback_hdl,
                                                                pres);

    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to initialize the module for %s, error code : %d(0x%x)",
                        media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * @brief Api to set the rate select on optics.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number that is of interest.
 * rev[in]     -  Firmware revision
 * cdr_enable[in]  -  true to enable CDR, false to disable CDR via rate select bits.
 * return           - t_std_error
 */

t_std_error sdi_media_ext_rate_select (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_fw_rev_t rev, bool cdr_enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    if(((media_ctrl_t *)media_hdl->callback_fns)->ext_rate_select == NULL) {
        return  SDI_ERRCODE(EOPNOTSUPP);
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->ext_rate_select(media_hdl->callback_hdl,
                                                                    channel, rev, cdr_enable);
    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to initialize the module for %s, error code : %d(0x%x)",
                media_hdl->name, rc, rc);
    }
    return rc;
}

/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */

t_std_error sdi_media_wavelength_set (sdi_resource_hdl_t resource_hdl, float value)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->wavelength_set(media_hdl->callback_hdl,
                                                                   value);

    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to set wavelength for %s, error code : %d (0x%x)",
                media_hdl->name, rc, rc);
    }

    return rc;
}

/**
 * @brief API to get QSA adapter type
 * resource_hdl[in] - Handle of the resource
 * sdi_qsa_adapter_type_t*[out] - adapter type obtained
 * return           - t_std_error
 */

t_std_error sdi_media_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t media_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    media_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (media_hdl->type != SDI_RESOURCE_MEDIA){
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((media_ctrl_t *)media_hdl->callback_fns)->media_qsa_adapter_type_get(media_hdl->callback_hdl,
                                                   qsa_adapter);

    if (rc != STD_ERR_OK){
        SDI_ERRMSG_LOG("Failed to get QSA adapter type for the module for %s, error code : %d(0x%x)",
                media_hdl->name, rc, rc);
    }
    return rc;

}
