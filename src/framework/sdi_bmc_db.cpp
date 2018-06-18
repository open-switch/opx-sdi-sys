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
 * filename: sdi_bmc_db.cpp
 */

#include "sdi_bmc_db.h"
#include "std_utils.h"

#include <map>
#include <string>
#include <iostream>

typedef std::map<std::string, void *> sdi_data_map_t;
typedef std::pair<std::string, void *> sdi_data_pair_t;
typedef std::pair<sdi_data_map_t::iterator,bool> sdi_data_return_t;


static sdi_data_map_t entity_map;
static sdi_data_map_t sensor_map;

/**
 * Insert data record into a specified map
 */
static bool sdi_data_insert (sdi_data_map_t &data_map, const char *key, void *data_obj)
{
    sdi_data_map_t::iterator it;

    it = data_map.find(key);
    if (it != data_map.end()) {
        return false;
    }

    sdi_data_return_t ret = data_map.insert(sdi_data_pair_t(key, data_obj));
    if (ret.second == false) {
        return false;
    }

    return true;
}

/**
 * Remove record from a specified map.
 */
static void *sdi_data_remove (sdi_data_map_t &data_map, const char *key)
{
    sdi_data_map_t::iterator it;
    void                    *data_obj;

    it = data_map.find(key);
    if (it == data_map.end()) {
        return NULL;
    }
                    
    data_obj = it->second;
    data_map.erase(it);
    return data_obj;
}

/**
 * Fetch record from a specified map.
 */
static void *sdi_data_get (sdi_data_map_t &data_map, const char *key)
{
    sdi_data_map_t::iterator it = data_map.find(key);

    return (it == data_map.end() ? 0 : it->second);
}

/**
 * Entity key generation function.
 */
static inline const char *sdi_bmc_entity_key (char *key, uint32_t len, uint32_t id, uint32_t instance)
{
    snprintf(key, len, "entity.%u.%u", id, instance);
    return ((const char *) key);
}

/*
 * Add entity record to a entity map using entity id and instance.
 */

sdi_bmc_entity_t *sdi_bmc_db_entity_add (uint32_t id, uint32_t instance)
{
    char key[SDI_DB_KEY_LEN] = "";
    sdi_bmc_entity_t  *ent = NULL;

    sdi_bmc_entity_key(key, sizeof(key), id, instance);
    if ((ent = (sdi_bmc_entity_t *)sdi_data_get(entity_map, key)) == NULL) {
        ent = (sdi_bmc_entity_t *) calloc(1, sizeof(sdi_bmc_entity_t));
        if (ent == NULL) return NULL;
        ent->entity_id = id;
        ent->entity_instance = instance;
        ent->present = false;
        if (sdi_data_insert(entity_map, key, ent) == false) {
            SDI_ERRMSG_LOG("Adding entity in DB failed(%u.%u)", id, instance);
            free(ent);
            ent = NULL;
        }
    }
    return ent;
}

/**
 * Fetch entity record from entity map using entity id and instance.
 */

sdi_bmc_entity_t *sdi_bmc_db_entity_get (uint32_t id, uint32_t instance)
{
    char key[SDI_DB_KEY_LEN] = "";
    sdi_bmc_entity_t  *ent = NULL;

    sdi_bmc_entity_key(key, sizeof(key), id, instance);
    ent = (sdi_bmc_entity_t *) sdi_data_get(entity_map, key);
    return ent;
}

/**
 * Delete entity record from entity map using entity id and instance
 */

bool sdi_bmc_db_entity_remove (uint32_t id, uint32_t instance)
{
    char key[SDI_DB_KEY_LEN] = "";
    sdi_bmc_entity_t  *ent = NULL;

    sdi_bmc_entity_key(key, sizeof(key), id, instance);
    ent = (sdi_bmc_entity_t *) sdi_data_remove(entity_map, key);
    free(ent);
    return true;
}

/**
 * sdi_bmc_db_for_each_entity is to call a specified method for each
 * entity present in the map
 */

void sdi_bmc_db_for_each_entity (sdi_bmc_register_entity_t callback_fn)
{
    sdi_data_map_t::iterator it;

    if (callback_fn == NULL) return;
    for (it = entity_map.begin(); it != entity_map.end(); ++it) {
        sdi_bmc_entity_t  *ent = (sdi_bmc_entity_t *) it->second;
        if (ent->present == true) {
            callback_fn(ent);
        }
    }
    return;
}

/**
 * Cleanup the entity map
 */

void sdi_bmc_db_entity_cleanup(void)
{
    sdi_data_map_t::iterator it;
    for (it = entity_map.begin(); it != entity_map.end(); ++it) {
        sdi_bmc_entity_t  *ent = (sdi_bmc_entity_t *) it->second;
        if (ent != NULL) {
            free(ent);
            it->second = NULL;
        }
    }
    entity_map.clear();
}

/**
 * Dump all the entities in the map, its for debugging purpose.
 */

void sdi_bmc_db_entity_dump (void)
{
    std::cout << "BMC Entity DB:\n";
    sdi_data_map_t::iterator it;

    for (it = entity_map.begin(); it != entity_map.end(); ++it) {
        sdi_bmc_entity_t  *ent = (sdi_bmc_entity_t *) it->second;
        std::cout << it->first << "==>" << ent->entity_id << "." << ent->entity_instance << " present :" << ent->present 
            << " Type :" << ent->type << " Sdi type :" << ent->sdi_type << '\n';
    }
}

/**
 * Sensor key create function.
 */
static inline const char *sdi_bmc_sensor_key (char *key, uint32_t len, uint32_t id, uint32_t instance, char *name)
{
    snprintf(key, len, "sensor.%u.%u.%s", id, instance,name);
    return ((const char *) key);
}

/**
 * Add sensor record to a sensor database using entity id, instance and sensor Id.
 */

sdi_bmc_sensor_t *sdi_bmc_db_sensor_add (uint32_t id, uint32_t instance, char *name)
{
    char key[SDI_DB_KEY_LEN] = "";
    sdi_bmc_sensor_t  *sensor = NULL;

    sdi_bmc_sensor_key(key, sizeof(key), id, instance, name);
    if ((sensor = (sdi_bmc_sensor_t *) sdi_data_get(sensor_map, key)) == NULL) {
        sensor = (sdi_bmc_sensor_t *) calloc(1, sizeof(sdi_bmc_sensor_t));
        if (sensor == NULL) return NULL;
        sensor->entity_id = id;
        sensor->entity_instance = instance;
        safestrncpy(sensor->name, name, IPMI_MAX_NAME_LEN);
        if (sdi_data_insert(sensor_map, key, sensor) == false) {
            SDI_ERRMSG_LOG("Adding sensor in DB failed(%u.%u.%s)", id, instance, name);
            free(sensor);
            sensor = NULL;
        }
    }
    return sensor;
}

/**
 * Fetch sensor record from a sensor database using entity id, instance and sensor Id.
 */

sdi_bmc_sensor_t *sdi_bmc_db_sensor_get (uint32_t id, uint32_t instance, char *name)
{
    char key[SDI_DB_KEY_LEN] = "";
    sdi_bmc_sensor_t  *sensor = NULL;

    sdi_bmc_sensor_key(key, sizeof(key), id, instance, name);
    sensor = (sdi_bmc_sensor_t *) sdi_data_get(sensor_map, key);
    return sensor;
}

/**
 * Fetch sensor record from a sensor database using sensor Id.
 */

sdi_bmc_sensor_t *sdi_bmc_db_sensor_get_by_name (char *name)
{
    sdi_data_map_t::iterator it;
    sdi_bmc_sensor_t *ret = NULL;

    for (it = sensor_map.begin(); it != sensor_map.end(); ++it) {
        sdi_bmc_sensor_t *sensor = (sdi_bmc_sensor_t *) it->second;
        if (strcmp(sensor->name, name) == 0) {
            ret = sensor;
            break;
        }
    }
    return ret;
}

/**
 * Delete sensor record from a sensor database using entity id, instance and sensor Id.
 */

bool sdi_bmc_db_sensor_remove (uint32_t id, uint32_t instance, char *name)
{
    char key[SDI_DB_KEY_LEN] = "";
    sdi_bmc_sensor_t  *sensor = NULL;

    sdi_bmc_sensor_key(key, sizeof(key), id, instance, name);
    sensor = (sdi_bmc_sensor_t *) sdi_data_remove(sensor_map, key);

    if (sensor->ev_state != NULL) {
        free(sensor->ev_state);
        sensor->ev_state = NULL;
    }
    if (sensor->thresholds != NULL) {
        free(sensor->thresholds);
        sensor->thresholds = NULL;
    }
    free(sensor);
    return true;
}

/**
 * sdi_bmc_db_for_each_sensor is to call a specified method for each sensor
 * present in the sensor map.
 */

void sdi_bmc_db_for_each_sensor (uint32_t entity_id, uint32_t instance, 
                                 sdi_bmc_register_resource_t callback_fn, void *data)
{
    sdi_data_map_t::iterator it;

    if (callback_fn == NULL) return;

    for (it = sensor_map.begin(); it != sensor_map.end(); ++it) {
        sdi_bmc_sensor_t *sensor = (sdi_bmc_sensor_t *) it->second;
        if ((sensor->entity_id == entity_id)
                && (sensor->entity_instance == instance)) {
            callback_fn(sensor, data);
        }
    }
    return;
}

/**
 * Sensor map cleanup function.
 */

void sdi_bmc_db_sensor_cleanup(void)
{
    sdi_data_map_t::iterator it;
    for (it = sensor_map.begin(); it != sensor_map.end(); ++it) {
        sdi_bmc_sensor_t  *sensor = (sdi_bmc_sensor_t *) it->second;
        if (sensor != NULL) {
            if (sensor->ev_state != NULL) {
                free(sensor->ev_state);
                sensor->ev_state = NULL;
            }
            if (sensor->thresholds != NULL) {
                free(sensor->thresholds);
                sensor->thresholds = NULL;
            }
            free(sensor);
            it->second = NULL;
        }
    }
    sensor_map.clear();
}

/**
 * Dump all sensor and sensor data present in the sensor map, its for debugging purpose.
 */

void sdi_bmc_db_sensor_dump (void)
{
    std::cout << "BMC Sensor DB:\n";
    sdi_data_map_t::iterator it;

    for (it = sensor_map.begin(); it != sensor_map.end(); ++it) {
        sdi_bmc_sensor_t  *sensor = (sdi_bmc_sensor_t *) it->second;
        std::cout << it->first << "==>" << sensor->entity_id << "." << sensor->entity_instance << "sensor name :" 
            << sensor->name << " type :" <<  sensor->type;

        if ((sensor->reading_type == SDI_SDR_READING_THRESHOLD)
                | (sensor->reading_type == SDI_SDR_READING_DISCRETE)) {
            std::cout << "-->reading : " << sensor->res.reading.data << "raw value : " << sensor->res.reading.raw_data
                << " discrete_state :" << sensor->res.reading.discrete_state;
        }
        std::cout << '\n' << '\n';
    }
}
