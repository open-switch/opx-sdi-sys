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
 * filename: sdi_thermal_internal.h
 */



/**
 * @file sdi_thermal_internal.h
 * @brief Internal thermal resource related declarations.
 *
 */

#ifndef __SDI_THERMAL_INTERNAL_H_
#define __SDI_THERMAL_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_thermal.h"

/**
 * Each temperature resource provides the following callbacks
 * - init -  callback function for resource init.
 * - temperature_get - callback function for getting the temperature.
 * - threshold_get - callback function for getting the threshold value
 * - threshold_set - callback function for setting the threshold value
 * - status_get - callback function to get the alarm status of the sensor
 *
 */
typedef struct {
    t_std_error (*init)(void *resource_hdl);
    /* Temperature Specific extensions start from here. */
    t_std_error (*temperature_get)(void *resource_hdl, int *temperature);
    t_std_error (*threshold_get)(void *resource_hdl, sdi_threshold_t type, int *threshold);
    t_std_error (*threshold_set)(void *resource_hdl, sdi_threshold_t type, int threshold);
    t_std_error (*status_get)(void *resource_hdl, bool  *status);
} temperature_sensor_t;
#endif
