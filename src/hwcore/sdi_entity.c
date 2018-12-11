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
 * filename: sdi_entity.c
 */


/**************************************************************************************
 * Implementation of generic entity  and resource API.
 ***************************************************************************************/

#include <string.h>

#include "sdi_entity.h"
#include "sdi_entity_info.h"
#include "sdi_entity_info_internal.h"
#include "sdi_fan.h"
#include "sdi_sys_common.h"
#include "sdi_pin_bus_api.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_bmc_internal.h"
#include "private/sdi_entity_internal.h"
#include "private/sdi_bmc_bus_api.h"
#include "std_assert.h"
#include "std_bit_ops.h"

/* Action to take when an entity is inserted */
static t_std_error sdi_entity_inserted(sdi_entity_priv_hdl_t entity_priv_hdl)
{
    /* Read entity info EEPROM, cache contents */
    memset(&entity_priv_hdl->entity_info, 0, sizeof(entity_priv_hdl->entity_info));
    sdi_resource_priv_hdl_t entity_info_hdl = entity_priv_hdl->entity_info_hdl;
    t_std_error rc = ((entity_info_t *) entity_info_hdl->callback_fns)->entity_info_data_get(
                                            entity_info_hdl->callback_hdl, &entity_priv_hdl->entity_info);
    if (rc == STD_ERR_OK) {
        entity_priv_hdl->entity_info_valid = true;
    } else {
        SDI_TRACEMSG_LOG("Failed to get the entity content of %s ",
                       entity_info_hdl->name);
    }

    return rc;
}

/* Action to take when an entity is removed */
static t_std_error sdi_entity_removed(sdi_entity_priv_hdl_t entity_priv_hdl)
{
    /* Mark entity info cache as invalid */
    entity_priv_hdl->entity_info_valid = false;

    return (STD_ERR_OK);
}

/**
 * Retrieve presence status of given entity.
 *
 * entity_hdl[in] - handle to the entity whose information has to be retrieved.
 * presence[out]    - true if entity is present, false otherwise
 *
 * return STD_ERR_OK on success and standard error on failure
 */
t_std_error sdi_entity_presence_get(sdi_entity_hdl_t entity_hdl, bool *presence)
{
    sdi_entity_priv_hdl_t entity_priv_hdl = NULL;
    sdi_pin_bus_level_t bus_val = SDI_PIN_LEVEL_LOW;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(entity_hdl != NULL);
    STD_ASSERT(presence != NULL);

    entity_priv_hdl = (sdi_entity_priv_hdl_t)entity_hdl;
    if (STD_BIT_TEST(entity_priv_hdl->oper_support_flag, SDI_HOTSWAPPABLE)) {
        if (entity_priv_hdl->access_type == SDI_ENT_ACCESS_BMC) {
            uint32_t reading = 0;
            rc = sdi_bmc_dc_sensor_reading_get_by_name(entity_priv_hdl->pres_attr, &reading);
            if (rc == STD_ERR_OK) {
                if (entity_priv_hdl->type == SDI_ENTITY_FAN_TRAY) {
                    *presence = ((reading == SDI_BMC_ENTITY_PRESENT) ? true : false);
                } else if (entity_priv_hdl->type == SDI_ENTITY_PSU_TRAY) {
                    if (STD_BIT_TEST(reading, SDI_BMC_PSU_STATUS_PRSNT)) {
                        *presence = true;
                    } else {
                        *presence = false;
                    }
                }
            }
        } else {
            STD_ASSERT(entity_priv_hdl->pres_pin_hdl != NULL);
            rc = sdi_pin_read_level(entity_priv_hdl->pres_pin_hdl,
                                    &bus_val);
            if(rc != STD_ERR_OK){
                return rc;
            }
            *presence = ( (bus_val == SDI_PIN_LEVEL_HIGH) ? true : false );
        }
    } else {
        *presence = true;
    }

    bool power_good = true;
    if (entity_priv_hdl->type == SDI_ENTITY_PSU_TRAY) {
        power_good = false;
        bus_val = SDI_PIN_LEVEL_LOW;
        if (entity_priv_hdl->power_output_status_pin_hdl != NULL) {
            rc = sdi_pin_read_level(entity_priv_hdl->power_output_status_pin_hdl,
                    &bus_val);
            if(rc != STD_ERR_OK){
                SDI_ERRMSG_LOG("Error in getting psu output power status, rc=%d",rc);
            } else {
                power_good = ( (bus_val == SDI_PIN_LEVEL_HIGH) ? true : false );
            }
        } else if (entity_priv_hdl->access_type == SDI_ENT_ACCESS_BMC) {
            uint32_t reading = 0;
            rc = sdi_bmc_dc_sensor_reading_get_by_name(entity_priv_hdl->power_output_status_attr,
                    &reading);
            if (rc == STD_ERR_OK) {
                if ((STD_BIT_TEST(reading, SDI_BMC_PSU_STATUS_FAILURE))
                        || (STD_BIT_TEST(reading, SDI_BMC_PSU_STATUS_INPUT_LOST))) {
                    power_good = false;
                } else {
                    power_good = true;
                }
            }
        }
    }
    
    /* If presence state changed, take action */
    bool old = entity_priv_hdl->present;
    entity_priv_hdl->present = *presence;
    if ((entity_priv_hdl->present && !old)
            || ((entity_priv_hdl->entity_info_valid == false)
                && (power_good == true))) {
        sdi_entity_inserted(entity_priv_hdl);
    } else if (!entity_priv_hdl->present && old) {
        sdi_entity_removed(entity_priv_hdl);
    }

    return rc;
}

/**
 * This function is required to support the fault status for the entities
 * which does not have a fault status pin. The entity fault is determined by checking
 * all the resource fault status within that entity and determining the fault status
 * of the entity.This function will be called from sdi_entity_fault_status_get for each
 * entity.As of now only fan is supported and may be extend in future.
 */
static void sdi_check_fault_each_resource(sdi_resource_hdl_t hdl, void *data)
{
    t_std_error rc = STD_ERR_OK;
    bool fault = false;

    if (sdi_resource_type_get(hdl) == SDI_RESOURCE_FAN)
    {
        rc = sdi_fan_status_get(hdl, &fault);
        if (rc != STD_ERR_OK)
        {
            SDI_ERRMSG_LOG("Error in getting fault status for %s %d", sdi_resource_name_get(hdl),rc);
        }
        else
        {
            *((bool *)data) |= fault;
        }
    }
}

/**
 * Checks the fault status for a given entity
 *
 * entity_hdl[in] - handle to the entity whose information has to be retrieved.
 * fault[out] - true if entity has any fault, false otherwise.
 *
 * return STD_ERR_OK on success and standard error on failure
 */
t_std_error sdi_entity_fault_status_get(sdi_entity_hdl_t entity_hdl, bool *fault)
{
    sdi_entity_priv_hdl_t entity_priv_hdl = NULL;
    sdi_pin_bus_level_t bus_val = SDI_PIN_LEVEL_LOW;
    bool pres = false;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(entity_hdl != NULL);
    STD_ASSERT(fault != NULL);
    *fault = false;

    entity_priv_hdl = (sdi_entity_priv_hdl_t)entity_hdl;

    if (entity_priv_hdl->fault_status_pin_hdl != NULL) {
        rc = sdi_pin_read_level(entity_priv_hdl->fault_status_pin_hdl,
                                &bus_val);
        if(rc != STD_ERR_OK){
            SDI_ERRMSG_LOG("Error in getting fault status,rc = %d",rc);
        } else {
            *fault = ( (bus_val == SDI_PIN_LEVEL_HIGH) ? true : false );
        }
        return rc;

    } else {
        /*
         * For those entities which does not have fault status pin, fault is determined by checking
         * all the resource fault status within that entity and determining the fault status
         * of the entity
         */
        rc = sdi_entity_presence_get(entity_hdl, &pres);
        if(rc != STD_ERR_OK){
            return rc;
        }
        if(pres != false){
            sdi_entity_for_each_resource(entity_hdl, sdi_check_fault_each_resource, fault);
            return rc;
        }
        else{
            return SDI_ERRCODE(ENXIO);
        }
    }
}

/**
 * Checks the psu output power status for a given psu
 *
 * entity_hdl[in] - handle to the psu entity whose information has to be retrieved.
 * status[out] - true if psu output status is good , false otherwise.
 *
 * return STD_ERR_OK on success , standard error on failure
 */
t_std_error sdi_entity_psu_output_power_status_get(sdi_entity_hdl_t entity_hdl,
                                                   bool *status)
{
    sdi_entity_priv_hdl_t entity_priv_hdl = NULL;
    sdi_pin_bus_level_t bus_val = SDI_PIN_LEVEL_LOW;
    bool pres = false;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(entity_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = false;

    entity_priv_hdl = (sdi_entity_priv_hdl_t)entity_hdl;

    if (entity_priv_hdl->type != SDI_ENTITY_PSU_TRAY) {
        SDI_ERRMSG_LOG("%s : Not a PSU entity", entity_priv_hdl->name);
        return SDI_ERRCODE(ENOTSUP);
    }

    rc = sdi_entity_presence_get(entity_hdl, &pres);
    if (rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("%s Unable to get Present status:rc=%d",
                       entity_priv_hdl->name, rc);
        return rc;
    }
    if (pres == false) {
        SDI_ERRMSG_LOG("%s entity is not present", entity_priv_hdl->name);
        return(SDI_ERRCODE(EPERM));
    }

    if (entity_priv_hdl->power_output_status_pin_hdl != NULL) {
        rc = sdi_pin_read_level(entity_priv_hdl->power_output_status_pin_hdl,
                                &bus_val);
        if(rc != STD_ERR_OK){
           SDI_ERRMSG_LOG("Error in getting psu output power status, rc=%d",rc);
        } else {
           *status = ( (bus_val == SDI_PIN_LEVEL_HIGH) ? true : false );
        }
    } else if (entity_priv_hdl->access_type == SDI_ENT_ACCESS_BMC) {
        uint32_t reading = 0;
        rc = sdi_bmc_dc_sensor_reading_get_by_name(entity_priv_hdl->power_output_status_attr,
                                                   &reading);
        if (rc == STD_ERR_OK) {
            if ((STD_BIT_TEST(reading, SDI_BMC_PSU_STATUS_FAILURE))
                    || (STD_BIT_TEST(reading, SDI_BMC_PSU_STATUS_INPUT_LOST))) {
                *status = false;
            } else {
                *status = true;
            }
        }
    }
    return rc;
}

/**
 * Checks the LPC bus access by performing pattern write/read.
 *
 * entity_hdl[in] - handle to the CPU lpc bus
 * lpc_test_status[out] - true if test fails , false otherwise.
 *
 * return STD_ERR_OK on success , standard error on failure
 */
t_std_error sdi_entity_lpc_bus_check(sdi_entity_hdl_t entity_hdl,bool *lpc_test_status)
{
    sdi_entity_priv_hdl_t entity_priv_hdl = NULL;
    bool pres = false;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(entity_hdl != NULL);
    STD_ASSERT(lpc_test_status != NULL);
    *lpc_test_status = false;

    entity_priv_hdl = (sdi_entity_priv_hdl_t)entity_hdl;

    if (entity_priv_hdl->type != SDI_ENTITY_SYSTEM_BOARD) {
        SDI_ERRMSG_LOG("%s : Not a entity", entity_priv_hdl->name);
        return SDI_ERRCODE(ENOTSUP);
    }

    rc = sdi_entity_presence_get(entity_hdl, &pres);
    if (rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("%s Unable to get Present status:rc=%d",
                entity_priv_hdl->name, rc);
        return rc;
    }

    if (pres == false) {
        SDI_ERRMSG_LOG("%s entity is not present", entity_priv_hdl->name);
        return(SDI_ERRCODE(EPERM));
    }

    if (entity_priv_hdl->gpr_pin_grp_hdl != NULL) {

        rc = sdi_pin_group_acquire_bus(entity_priv_hdl->gpr_pin_grp_hdl);
        if (rc != STD_ERR_OK) {
            return rc;
        }

        uint_t write_value =  0x55;

        rc = sdi_pin_group_write_level(entity_priv_hdl->gpr_pin_grp_hdl,
                write_value);

        sdi_pin_group_release_bus(entity_priv_hdl->gpr_pin_grp_hdl);

        if (rc != STD_ERR_OK){
            SDI_ERRMSG_LOG("%s LPC bus write failure", entity_priv_hdl->name);
            return rc;
        }

        rc = sdi_pin_group_acquire_bus(entity_priv_hdl->gpr_pin_grp_hdl);
        if (rc != STD_ERR_OK) {
            return rc;
        }

        uint_t read_value =  0x00;
        rc = sdi_pin_group_read_level(entity_priv_hdl->gpr_pin_grp_hdl,
                &read_value);

        sdi_pin_group_release_bus(entity_priv_hdl->gpr_pin_grp_hdl);

        if (rc != STD_ERR_OK){
            SDI_ERRMSG_LOG("%s LPC bus read failure", entity_priv_hdl->name);
            return rc;
        }

        if(write_value != read_value){
            SDI_ERRMSG_LOG("%s CRITICAL : LPC BUS FAILURE",entity_priv_hdl->name);
            *lpc_test_status = true;
        }
    }
    return rc;
}
