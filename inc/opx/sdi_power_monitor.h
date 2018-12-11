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
 * filename: sdi_power_monitor.h
 */



/**
 * @file sdi_power_monitor.h
 * @brief SDI Power Monitor API.
 *
 */

#ifndef __SDI_POWER_MONITOR_H_
#define __SDI_POWER_MONITOR_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"

/**
 * @defgroup sdi_power_monitor_api SDI Power Monitor API.
 * Power Monitor related API. All Power Monitor related API take an argument
 * of type sdi_resource_hdl_t.  Application should first identify the
 * right sdi_resource_hdl_t by using @ref sdi_entity_resource_lookup
 *
 * @ingroup sdi_sys
 * @{
 */

/**
 * @brief Retrieve the current in amps using the specified power monitor chip
 * @param[in] power_monitor_hdl - handle of the power monitor chip  that is of interest.
 * @param[out] *current_amp - the value of current in amps will be returned in this.
 *                            (after offsetting any PSU loss)
 * @return - standard @ref t_std_error
 */
t_std_error sdi_power_monitor_current_amp_get(sdi_resource_hdl_t power_monitor_hdl, float *current_amp);

/**
 * @brief Retrieve the voltage in volts using the specified power monitor chip
 * @param[in] power_monitor_hdl - handle of the power monitor chip  that is of interest.
 * @param[out] *voltage_volt - the value of voltage in volts will be returned in this.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_power_monitor_voltage_volt_get(sdi_resource_hdl_t power_monitor_hdl, float *voltage_volt);

/**
 * @brief Retrieve the current in amps using the specified power monitor chip
 * @param[in] power_monitor_hdl - handle of the power monitor chip  that is of interest.
 * @param[out] *power_watt - the value of power in watts will be returned in this 
 * @return - standard @ref t_std_error
 */
t_std_error sdi_power_monitor_power_watt_get(sdi_resource_hdl_t power_monitor_hdl, float *power_watt);


/**
 * @}
 */


#endif   /* __SDI_POWER_MONITOR_H_ */
