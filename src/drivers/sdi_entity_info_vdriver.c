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
 * filename: sdi_entity_info_vdriver.c
 *
 * Virtual Entity Info driver for Entities that do not have an eeprom
 */

/******************************************************************************
 ******************************************************************************/

#include "sdi_driver_internal.h"
#include "sdi_entity_info.h"
#include "sdi_entity_info_internal.h"
#include "sdi_device_common.h"
#include "sdi_resource_internal.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_bus_framework.h"
#include "sdi_entity_info_resource_attr.h"
#include "sdi_entity_info_vdriver.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_pin_bus_api.h"
#include "std_assert.h"
#include "std_config_node.h"
#include "std_error_codes.h"
#include "std_utils.h"
#include "sys/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static t_std_error sdi_entity_info_register(std_config_node_t node, 
                                            void *bus_handle,
                                            sdi_device_hdl_t* device_hdl);

static t_std_error sdi_entity_info_init(sdi_device_hdl_t device_hdl);

static entity_info_t entity_info_callbacks = {
    NULL, 
    sdi_entity_info_data_get,
};

/* Export the Driver table */
const sdi_driver_t entity_info_entry = {
    sdi_entity_info_register,
    sdi_entity_info_init
};

/**
 * FAN or PSU entity_info data get
 *
 * param[in] resource_hdl  - resource handler
 * param[out] entity_info  - entity_info structure to fill
 *
 * return STD_ERR_OK for success and the respective error code in case of failure.
 */
t_std_error sdi_entity_info_data_get(void *resource_hdl,
                                     sdi_entity_info_t *entity_info)
{
    sdi_device_hdl_t vchip = NULL;
    t_std_error rc = STD_ERR_OK;
    entity_info_data_t *entity_info_data = NULL;
    sdi_pin_group_bus_hdl_t airflow_dir_hdl = 0; 
    uint_t airflow_dir_value = 0;
    sdi_pin_bus_hdl_t psu_type_hdl = 0;
    uint_t powertype_value = 0;
    bool result = true;

    /** Validate arguments */
    vchip = (sdi_device_hdl_t)resource_hdl;
    STD_ASSERT(vchip != NULL);
    STD_ASSERT(entity_info != NULL);

    entity_info_data = (entity_info_data_t *)vchip->private_data;

    entity_info->max_speed = entity_info_data->max_fan_speed;
    entity_info->num_fans = entity_info_data->no_of_fans;
    entity_info->air_flow = SDI_PWR_AIR_FLOW_NOT_APPLICABLE;
    
    /* Retrieve the pin group bus hdl for airflow direction */
    airflow_dir_hdl = entity_info_data->airflow_dir_hdl;

    if (airflow_dir_hdl != 0) { 
        rc = sdi_pin_group_acquire_bus(airflow_dir_hdl); 
        if (rc != STD_ERR_OK){
            return rc;
        } 
        
        rc = sdi_pin_group_read_level(airflow_dir_hdl, &airflow_dir_value);
        
        sdi_pin_group_release_bus(airflow_dir_hdl); 

        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("airflow dir get failed for %s", 
                                  vchip->alias);
            return rc;
        } 
        
        if (entity_info_data->normal_airflow_val == airflow_dir_value) { 
            entity_info->air_flow = SDI_PWR_AIR_FLOW_NORMAL;
        } else if (entity_info_data->reverse_airflow_val == airflow_dir_value) { 
            entity_info->air_flow = SDI_PWR_AIR_FLOW_REVERSE; 
        } else { 
            SDI_DEVICE_ERRMSG_LOG("unknown airflow value %d for %s",
                                  airflow_dir_value, vchip->alias); 
            result = false; 
        } 
    } 

    /* Retrieve the pin bus hdl for psu_type */
    psu_type_hdl = entity_info_data->psu_type_hdl;

    if (psu_type_hdl !=0) {
        rc = sdi_pin_read_level(psu_type_hdl, &powertype_value);

        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("psu type get failed for %s", vchip->alias);
            return rc;
        } 
        
        if (entity_info_data->ac_power_val == powertype_value) {
            entity_info->power_type.ac_power = true;
        } else if (entity_info_data->dc_power_val == powertype_value) {
            entity_info->power_type.dc_power = true;
        } else { 
            SDI_DEVICE_ERRMSG_LOG("unknown psu_type %d for %s", 
                                  powertype_value, vchip->alias);
            result = false;
        }
    }

     /*
      * Mark the not-applicable fields in virtual entity_info as "NA"
      */
    safestrncpy(entity_info->prod_name, "NA", sizeof(entity_info->prod_name));
    safestrncpy(entity_info->ppid, "NA", sizeof(entity_info->ppid));
    safestrncpy(entity_info->hw_revision, "NA", sizeof(entity_info->hw_revision));
    safestrncpy(entity_info->platform_name, "NA", sizeof(entity_info->platform_name));
    safestrncpy(entity_info->vendor_name, "NA", sizeof(entity_info->vendor_name)); 
    safestrncpy(entity_info->part_number, "NA", sizeof(entity_info->part_number));

    if (false == result) {
        return SDI_DEVICE_ERRNO;
    }

    return rc;
}

/**
 * The config file format will be as below for entity_info virtual devices
 *
 *  <entity_info instance="<vchip_instance>"
 *    alias="<Alias name for the particular device>"
 *    entity_type="<Entity Type for fan-tray or PSU>"
 *    airflow_dir="<To specify normal/reverse dir>"
 *    no_of_fans="<Maximum number of fans in the entity (fan-tray or PSU)>"
 *    max_fan_speed="<Maximum fan speed in this entity (fan-tray or PSU)>"
 *    psu_type="<power type of the PSU (ac or dc)>"
 *  </entity_info>
 */
static t_std_error sdi_entity_info_register (std_config_node_t node, 
                                             void *bus_handle,
                                             sdi_device_hdl_t* device_hdl)
{
    char *attr_value = NULL;
    sdi_device_hdl_t vchip = NULL;
    entity_info_data_t *entity_info_data = NULL;
    t_std_error rc = STD_ERR_OK;

    /** Validate arguments */
    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);

    vchip = calloc(sizeof(sdi_device_entry_t),1);
    STD_ASSERT(vchip != NULL);
    entity_info_data = calloc(sizeof(entity_info_data_t),1);
    STD_ASSERT(entity_info_data != NULL);

    vchip->bus_hdl = bus_handle;

    /* Get all config attributes */
    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(attr_value != NULL);
    vchip->instance = strtoul(attr_value, NULL, 0);

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_ALIAS);
    if (attr_value == NULL) {
        snprintf(vchip->alias, SDI_MAX_NAME_LEN, "virt-entity-info-%d", 
                 vchip->instance );
    } else {
        safestrncpy(vchip->alias, attr_value, sizeof(vchip->alias));
    }

    vchip->callbacks = &entity_info_entry;
    vchip->private_data = (void*)entity_info_data;

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_NO_OF_FANS);
    if(attr_value != NULL) {
        entity_info_data->no_of_fans = strtoul(attr_value, NULL, 0);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_MAX_SPEED);
    if(attr_value != NULL) {
        entity_info_data->max_fan_speed = strtoul(attr_value, NULL, 0);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_AIRFLOW_DIR_BUS);
    if (attr_value != NULL) {
        entity_info_data->airflow_dir_hdl = 
                        sdi_get_pin_group_bus_handle_by_name(attr_value);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_NORMAL_AIRFLOW_VAL);
    if(attr_value != NULL) {
        entity_info_data->normal_airflow_val = strtoul(attr_value, NULL, 0);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_REVERSE_AIRFLOW_VAL);
    if(attr_value != NULL) {
        entity_info_data->reverse_airflow_val = strtoul(attr_value, NULL, 0);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_PSU_TYPE_BUS);
    if (attr_value != NULL) {
        entity_info_data->psu_type_hdl = 
                        sdi_get_pin_bus_handle_by_name(attr_value);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_AC_POWER_VAL);
    if(attr_value != NULL) {
        entity_info_data->ac_power_val = strtoul(attr_value, NULL, 0);
    }

    attr_value = std_config_attr_get(node, SDI_DEV_ATTR_DC_POWER_VAL);
    if(attr_value != NULL) {
        entity_info_data->dc_power_val = strtoul(attr_value, NULL, 0);
    }

    sdi_resource_add(SDI_RESOURCE_ENTITY_INFO, vchip->alias,(void*)vchip, 
                         &entity_info_callbacks);

    *device_hdl = vchip;

    return rc;
}

/**
 * Dummy function to initialize there is no real device to initialize
 * param[in] device_hdl - device handle of the specific device
 * return: STD_ERR_OK
 */
static t_std_error sdi_entity_info_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

