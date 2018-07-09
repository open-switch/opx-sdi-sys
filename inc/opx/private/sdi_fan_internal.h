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
 * filename: sdi_fan_internal.h
 */



/**
 * @file sdi_fan_internal.h
 * @brief Internal fan resource related declarations.
 * This file will be used by the fan controller drivers
 *
 */

#ifndef __SDI_FAN_INTERNAL_H_
#define __SDI_FAN_INTERNAL_H_

#include <sys/types.h>
#include <regex.h>

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_fan.h"

/* One mapping entry in map, below; given RPM <=> given % */
struct sdi_fan_speed_map_entry {
    struct sdi_fan_speed_map_entry *next;
    uint_t  rpm;
    uint_t  pct;
};

/* Mapping between fan speed RPM and percent */
struct sdi_fan_speed_ppid_map {
    struct sdi_fan_speed_ppid_map *next;
    regex_t ppid_pat[1]; /* Map applies for entities with PPID matching this RE */
    struct sdi_fan_speed_map_entry *speeds;
};

/**
 * Each Fan resource provides the following callbacks.so the following api have
 * to be provided by the driver when registering a fan resource with the sdi
 * framework
 */
typedef struct {
    /**
     * callback function for resource init
     */
    t_std_error (*init)(void *resource_hdl, uint_t max_rpm);
    /**
     * callback function for getting the speed(in RPM) of the Fan
     */
    t_std_error (*speed_get)(sdi_resource_hdl_t real_resource_hdl,
                             void *resource_hdl,
                             uint_t *speed
                             );
    /**
     * callback function for setting the speed for the Fan
     */
    t_std_error (*speed_set)(sdi_resource_hdl_t real_resource_hdl,
                             void *resource_hdl,
                             uint_t speed
                             );
    /**
     * callback function to get the status of the Fan
     */
    t_std_error (*status_get)(void *resource_hdl, bool *status);

    /* Callback to convert fan speed RPM to percent;
       NIL => Fall back to (rpm / max_speed) * 100 to calculate %
     */
    uint_t (*speed_rpm_to_pct)(sdi_resource_hdl_t real_resource_hdl,
                               void *resource_hdl,
                               uint_t rpm
                               );

    /* Callback to convert fan speed percent to RPM;
       NIL => Fall back to (pct / 100) * max_speed to calculate RPM
     */
    uint_t (*speed_pct_to_rpm)(sdi_resource_hdl_t real_resource_hdl,
                               void *resource_hdl,
                               uint_t pct
                               );

    /* Fan speed RPM-to-percent map for fan; NIL => none */
    struct sdi_fan_speed_ppid_map *speed_ppid_map;
} fan_ctrl_t;
#endif
