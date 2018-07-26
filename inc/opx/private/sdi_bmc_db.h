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
 * filename: sdi_bmc_db.h 
 */

#ifndef __SDI_BMC_DB_H__
#define __SDI_BMC_DB_H__

#include "sdi_bmc_internal.h"
#include "std_type_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SDI_DB_KEY_LEN   (129)

typedef void (*sdi_bmc_register_entity_t)(sdi_bmc_entity_t *entity);
typedef void (*sdi_bmc_register_resource_t)(sdi_bmc_sensor_t *sensor, void *data);

/*
 * BMC entity database handling functions.
 */

/*
 * Add entity record to a entity map using entity id and instance.
 */
sdi_bmc_entity_t *sdi_bmc_db_entity_add (uint32_t id, uint32_t instance);

/*
 * Fetch entity record from entity map using entity id and instance.
 */
sdi_bmc_entity_t *sdi_bmc_db_entity_get (uint32_t id, uint32_t instance);

/* 
 * Delete entity record from entity map using entity id and instance.
 */
bool sdi_bmc_db_entity_remove (uint32_t id, uint32_t instance);

/*
 * sdi_bmc_db_for_each_entity is to call a specified method for each 
 * entity present in the map.
 */
void sdi_bmc_db_for_each_entity (sdi_bmc_register_entity_t callback_fn);

/*
 * Cleanup the entity map.
 */
void sdi_bmc_db_entity_cleanup(void);

/*
 * Dump all the entities in the map, its for debugging purpose.
 */
void sdi_bmc_db_entity_dump (void);

/*
 * Functions to handle sensor database.
 */

/*
 * Add sensor record to a sensor database using entity id, instance and sensor Id.
 */
sdi_bmc_sensor_t *sdi_bmc_db_sensor_add (uint32_t id, uint32_t instance, char *name);
/*
 * Add sensor record to a sensor database using sensor id.
 */
sdi_bmc_sensor_t *sdi_bmc_db_sensor_add_by_name (char *name);
/*
 * Fetch sensor record from a sensor database using entity id, instance and sensor Id.
 */
sdi_bmc_sensor_t *sdi_bmc_db_sensor_get (uint32_t id, uint32_t instance, char *name);

/*
 * Delete sensor record from a sensor database using entity id, instance and sensor Id.
 */
bool sdi_bmc_db_sensor_remove (uint32_t id, uint32_t instance, char *name);

/*
 * sdi_bmc_db_for_each_sensor is to call a specified method for each sensor
 * present in the sensor map.
 */
void sdi_bmc_db_for_each_sensor (uint32_t entity_id, uint32_t instance,
                                 sdi_bmc_register_resource_t callback_fn, void *data);

/*
 * Fetch sensor record from a sensor database using sensor Id.
 */
sdi_bmc_sensor_t *sdi_bmc_db_sensor_get_by_name (char *name);

/*
 * Sensor map cleanup function.
 */
void sdi_bmc_db_sensor_cleanup(void);

/*
 * Dump all sensor and sensor data present in the sensor map, its for debugging purpose.
 */
void sdi_bmc_db_sensor_dump (void);

#ifdef __cplusplus
}
#endif

#endif /* __SDI_BMC_DB_H__ */
