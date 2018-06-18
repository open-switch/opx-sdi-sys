/*
 * Copyright (c) 2018 Dell EMC.
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
 * filename: sdi_bmc_bus_api.c
 */



#include "sdi_bmc_bus_api.h"
#include "std_assert.h"
#include "sdi_bmc_internal.h"
#include "sdi_entity_internal.h"
#include "sdi_bmc_db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARRAY_SIZE(a)         (sizeof(a)/sizeof(a[0]))

/**
 * bmc_sdi_entity_map_t is to hold BMC to SDI entity mapping.
 */
typedef struct bmc_sdi_entity_map_s {
    uint32_t           bmc_ent_type; /** BMC entity type */
    sdi_entity_type_t  sdi_ent_type; /** SDI entity type */
} bmc_sdi_entity_map_t;

/**
 * BMC to SDI entity mapping
 */
static bmc_sdi_entity_map_t entity_map[] =
{
    {IPMI_ENTITY_ID_SYSTEM_BOARD, SDI_ENTITY_SYSTEM_BOARD},
    {IPMI_ENTITY_ID_SYSTEM_CHASSIS, SDI_ENTITY_SYSTEM_BOARD},
    {IPMI_ENTITY_ID_POWER_SUPPLY, SDI_ENTITY_PSU_TRAY},
    {IPMI_ENTITY_ID_FAN_COOLING, SDI_ENTITY_FAN_TRAY}
};


/**
 * sdi_bmc_dc_sensor_reading_get_by_name is to get the discrete sensor
 * reading by using sensor name.
 */
t_std_error sdi_bmc_dc_sensor_reading_get_by_name (char *sensor_id, uint32_t *data)
{
    sdi_bmc_sensor_t *sensor = NULL;

    STD_ASSERT(sensor_id != NULL);
    STD_ASSERT(data != NULL);
    sensor = sdi_bmc_db_sensor_get_by_name(sensor_id);
    if (sensor == NULL) {
        return SDI_ERRCODE(EINVAL);
    }
    if (sensor->reading_type != SDI_SDR_READING_DISCRETE) {
        return SDI_ERRCODE(ENOTSUP);
    }
    *data = sensor->res.reading.discrete_state;
    return STD_ERR_OK;
}

/**
 * sdi_bmc_th_sensor_reading_get_by_name is to get the threshold sensor
 * reading by using sensor name.
 */

t_std_error sdi_bmc_th_sensor_reading_get_by_name (char *sensor_id, double *data)
{
    sdi_bmc_sensor_t *sensor = NULL;

    STD_ASSERT(sensor_id != NULL);
    STD_ASSERT(data != NULL);
    sensor = sdi_bmc_db_sensor_get_by_name(sensor_id);
    if (sensor == NULL) {
        return SDI_ERRCODE(EINVAL);
    }
    if (sensor->reading_type != SDI_SDR_READING_THRESHOLD) {
        return SDI_ERRCODE(ENOTSUP);
    }
    *data = sensor->res.reading.data;
    return STD_ERR_OK;
}

/**
 * sdi_bmc_get_entity_name is to get entity name using entity type and entity instance.
 */

char *sdi_bmc_get_entity_name (uint32_t id, uint32_t instance, char *name, uint32_t len)
{

    if ((name == NULL) || (len == 0)) return NULL;

    switch (id) {
        case SDI_ENTITY_SYSTEM_BOARD:
            snprintf(name, len, "%s", "System Board");
            break;
        case SDI_ENTITY_PSU_TRAY:
            snprintf(name, len, "%s-%u", "PSU Tray", instance);
            break;
        case SDI_ENTITY_FAN_TRAY:
            snprintf(name, len, "%s-%u", "Fan Tray", instance);
            break;
        default:
            snprintf(name, len, "%s-%u", "Unknown", instance);
            break;
    }
    return name;

}

/**
 * sdi_bmc_sdi_entity_type_get provides mapping betwee bmc entity type
 */

sdi_entity_type_t sdi_bmc_sdi_entity_type_get (uint32_t bmc_type)
{
    uint32_t id;
    sdi_entity_type_t ent_type = SDI_ENTITY_SYSTEM_BOARD;

    for (id = 0; id < ARRAY_SIZE(entity_map); id++) {
        if (entity_map[id].bmc_ent_type == bmc_type) {
            ent_type = entity_map[id].sdi_ent_type;
            break;
        }
    }
    return ent_type;
}
