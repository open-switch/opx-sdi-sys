/*
 * Copyright (c) 2019 Dell Inc.
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
 * filename: sdi_vm_power_monitor.c
 */


/**************************************************************************************
 * sdi_vm_power_monitor.c
 * API implementation for power monitoring related functionalities for VM
***************************************************************************************/

#include "sdi_power_monitor_internal.h"

/*
 * API implementation to retrieve the current (Amps) of the chip refered by resource.
 * [in] monitor_hdl - resource handle of the chip
 * [out] current_amp - current value in Amps is returned in this
 */
t_std_error sdi_power_monitor_current_amp_get(sdi_resource_hdl_t monitor_hdl, float *current_amp)
{
    return (STD_ERR_OK);
}

/*
 * API implementation to retrieve the voltate (volts) of the chip refered by resource.
 * [in] monitor_hdl - resource handle of the chip
 * [out] voltage_volt - voltage value in Volts is returned in this
 */
t_std_error sdi_power_monitor_voltage_volt_get(sdi_resource_hdl_t monitor_hdl, float *voltage_volt)
{
    return (STD_ERR_OK);
}

/*
 * API implementation to retrieve the power (watts) of the chip refered by resource.
 * [in] monitor_hdl - resource handle of the chip
 * [out] power_watt - voltage value in Volts is returned in this
 */
t_std_error sdi_power_monitor_power_watt_get(sdi_resource_hdl_t monitor_hdl, float *power_watt)
{
    return (STD_ERR_OK);
}
