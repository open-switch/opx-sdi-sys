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
 * filename: sdi_power_monitor_internal.h
 */



/**
 * @file sdi_power_monitor_internal.h
 * @brief Internal power monitor resource related declarations.
 *
 */

#ifndef __SDI_POWER_MONITOR_INTERNAL_H_
#define __SDI_POWER_MONITOR_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_power_monitor.h"

/**
 * Each power monitor resource provides the following callbacks
 * - init             - callback function for resource init.
 * - current_amp_get  - callback function for getting the current measurement in amps.
 * - voltage_volt_get - callback function for getting the voltage measurement in volts 
 * - power_watt_get   - callback function for getting the power value in watts
 *
 */
typedef struct {
    t_std_error (*init)(void *resource_hdl);
    /* Power monitor Specific extensions start from here. */
    t_std_error (*current_amp_get)(void *resource_hdl, float *current_amp);
    t_std_error (*voltage_volt_get)(void *resource_hdl, float *voltage_volt);
    t_std_error (*power_watt_get)(void *resource_hdl, float *power_watt);
} power_monitor_t;

#endif /* __SDI_POWER_MONITOR_INTERNAL_H_ */
