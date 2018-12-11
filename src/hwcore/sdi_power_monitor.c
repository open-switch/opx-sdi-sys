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
 * filename: sdi_power_monitor.c
 */


/**************************************************************************************
 * sdi_power_monitor.c
 * API implementation for power monitoring related functionalities
***************************************************************************************/

#include "sdi_power_monitor_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_sys_common.h"
#include "std_assert.h"

/*
 * API implementation to retrieve the current (Amps) of the chip refered by resource.
 * [in] monitor_hdl - resource handle of the chip
 * [out] current_amp - current value in Amps is returned in this
 */
t_std_error sdi_power_monitor_current_amp_get(sdi_resource_hdl_t monitor_hdl, float *current_amp)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t power_monitor_hdl = (sdi_resource_priv_hdl_t)monitor_hdl;

    STD_ASSERT(power_monitor_hdl != NULL);
    STD_ASSERT(is_sdi_inited());

    if(power_monitor_hdl->type != SDI_RESOURCE_POWER_MONITOR)
    {
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((power_monitor_t *)power_monitor_hdl->callback_fns)->
        current_amp_get(power_monitor_hdl->callback_hdl, current_amp);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to get the current amp from %s power monitor chip",
                       power_monitor_hdl->name);
    }

    return rc;
}

/*
 * API implementation to retrieve the voltate (volts) of the chip refered by resource.
 * [in] monitor_hdl - resource handle of the chip
 * [out] voltage_volt - voltage value in Volts is returned in this
 */
t_std_error sdi_power_monitor_voltage_volt_get(sdi_resource_hdl_t monitor_hdl, float *voltage_volt)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t power_monitor_hdl = (sdi_resource_priv_hdl_t)monitor_hdl;

    STD_ASSERT(power_monitor_hdl != NULL);
    STD_ASSERT(is_sdi_inited());

    if(power_monitor_hdl->type != SDI_RESOURCE_POWER_MONITOR)
    {
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((power_monitor_t *)power_monitor_hdl->callback_fns)->
        voltage_volt_get(power_monitor_hdl->callback_hdl, voltage_volt);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to get the voltage volt from %s power monitor chip",
                       power_monitor_hdl->name);
    }

    return rc;
}

/*
 * API implementation to retrieve the power (watts) of the chip refered by resource.
 * [in] monitor_hdl - resource handle of the chip
 * [out] power_watt - voltage value in Volts is returned in this
 */
t_std_error sdi_power_monitor_power_watt_get(sdi_resource_hdl_t monitor_hdl, float *power_watt)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t power_monitor_hdl = (sdi_resource_priv_hdl_t)monitor_hdl;

    STD_ASSERT(power_monitor_hdl != NULL);
    STD_ASSERT(is_sdi_inited());

    if(power_monitor_hdl->type != SDI_RESOURCE_POWER_MONITOR)
    {
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((power_monitor_t *)power_monitor_hdl->callback_fns)->
                    power_watt_get(power_monitor_hdl->callback_hdl, power_watt);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to get the power watt from %s power monitor chip",
                       power_monitor_hdl->name);
    }

    return rc;
}
