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

/**
 * @file sdi_vm_entity.c
 * @brief Entity simulation functionality implements sdi-api headers
 */

#include "sdi_entity.h"
#include "sdi_entity_info.h"
#include "sdi_sys_vm.h"
#include "sdi_db.h"
#include "event_log.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Retrieve number of entities supported by system of given type.
 */
uint_t sdi_entity_count_get(sdi_entity_type_t etype)
{
    uint_t entity_count = 0;
    t_std_error rc;

    rc = sdi_db_get_entity_count(sdi_get_db_handle(), etype, &entity_count);
    if (rc != STD_ERR_OK) {
        return 0;
    }

    return entity_count;
}

/*
 * Retrieve the handle of the specified entity.
 */
sdi_entity_hdl_t sdi_entity_lookup(sdi_entity_type_t etype, uint_t instance)
{
    sdi_entity_hdl_t entity_hdl = 0;
    t_std_error rc;

    rc = sdi_db_get_entity_handle(sdi_get_db_handle(), etype, instance, &entity_hdl);
    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return entity_hdl;
}

/*
 * Retrieve the type of the entity
 */
sdi_entity_type_t sdi_entity_type_get(sdi_entity_hdl_t hdl)
{
    sdi_entity_type_t type;
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;

    rc = sdi_db_get_first_resource(sdi_get_db_handle(), hdl,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_int_field_get(sdi_get_db_handle(), info_hdl,
                              TABLE_INFO, TBL_ENTITY_TYPE, (int *)&type);
    if (rc != STD_ERR_OK) {
        return -1;
    }

    return type;
}

/*
 * Retrieve the name of the entity
 */
/* Since we can't return a value on the stack, we use a static variable
 * and return a pointer to that. This is not thread safe.
 */
static char sdi_entity_name_str[NAME_MAX];
const char *sdi_entity_name_get(sdi_entity_hdl_t hdl)
{
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;

    rc = sdi_db_get_first_resource(sdi_get_db_handle(), hdl,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return NULL;
    }

    rc = sdi_db_str_field_get(sdi_get_db_handle(), info_hdl,
                              TABLE_INFO, INFO_ENTITY_NAME, sdi_entity_name_str);
    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return sdi_entity_name_str;
}

/*
 * Retrieve the handle of the first entity.
 */
sdi_entity_hdl_t sdi_entity_find_first(void)
{
    /** retrieve the handle for the first entity **/
    sdi_entity_hdl_t hdl = NULL;
    t_std_error rc;

    rc = sdi_db_get_first_entity(sdi_get_db_handle(), &hdl);

    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return hdl;
}

/*
 * Retrieve the handle of the next entity.
 */
sdi_entity_hdl_t sdi_entity_find_next(sdi_entity_hdl_t hdl)
{
    t_std_error rc;

    rc = sdi_db_get_next_entity(sdi_get_db_handle(), &hdl);

    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return hdl;
}

/*
 * Apply the specified function for every entity in the system.
 * for every entity in the sytem, user-specified function "fn" will be called
 * with corresponding entity and user-specified data ar arguments.
 */
void sdi_entity_for_each(void (*fn)(sdi_entity_hdl_t hdl, void *user_data), void *user_data)
{
    sdi_entity_hdl_t hdl = sdi_entity_find_first();

    STD_ASSERT(fn != NULL);

    while (hdl != NULL) {
        fn(hdl, user_data);
        hdl = sdi_entity_find_next(hdl);
    };
}

/*
 * Reset the entity
 */
t_std_error sdi_entity_reset(sdi_entity_hdl_t hdl, sdi_reset_type_t type)
{
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;

    rc = sdi_db_get_first_resource(sdi_get_db_handle(), hdl,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_int_field_target_set(sdi_get_db_handle(), info_hdl,
                                TABLE_INFO, INFO_ENTITY_RESET, (int *)&type);
}

/*
 * Control the power status of the entity
 */
t_std_error sdi_entity_power_status_control(sdi_entity_hdl_t hdl, bool enable)
{
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;
    int db_enable = enable;

    rc = sdi_db_get_first_resource(sdi_get_db_handle(), hdl,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_int_field_target_set(sdi_get_db_handle(), info_hdl,
                                TABLE_INFO, INFO_ENTITY_POWER, &db_enable);
}

/*
 * Initialize the entity
 * This involves enabling the power and setting the reset type to COLD_RESET
 * in the database
 */
t_std_error sdi_entity_init(sdi_entity_hdl_t hdl)
{
    t_std_error rc;

    rc = sdi_entity_reset(hdl, COLD_RESET);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_entity_power_status_control(hdl, true);
}


/*
 * Retrieve presence status of given entity.
 */
t_std_error sdi_entity_presence_get(sdi_entity_hdl_t hdl, bool *pres)
{
    STD_ASSERT(pres != NULL);

    return sdi_db_entity_presence_get(sdi_get_db_handle(), hdl, pres);
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
    STD_ASSERT(lpc_test_status != NULL);

    *lpc_test_status = false;
    return STD_ERR_OK;
}



/*
 * Check if there are any faults in given entity.
 */
t_std_error sdi_entity_fault_status_get(sdi_entity_hdl_t hdl, bool *fault)
{
    STD_ASSERT(fault != NULL);

    return sdi_db_entity_fault_status_get(sdi_get_db_handle(), hdl, fault);
}

/*
 * Retrieve number of resources of given type within given entity.
 */
uint_t sdi_entity_resource_count_get(sdi_entity_hdl_t hdl,
        sdi_resource_type_t resource_type)
{
    uint_t resource_count = 0;
    t_std_error rc;

    rc = sdi_db_get_resource_count(sdi_get_db_handle(), hdl, resource_type, &resource_count);

    if (rc != STD_ERR_OK) {
        return 0;
    }

    return resource_count;
}

/*
 * Retrieve the handle of the resource whose name is known.
 */
sdi_resource_hdl_t sdi_entity_resource_lookup(sdi_entity_hdl_t hdl,
        sdi_resource_type_t type, const char *alias)
{
    sdi_resource_hdl_t resource_hdl = 0;
    t_std_error rc;

    STD_ASSERT(alias != NULL);

    rc = sdi_db_get_resource_handle_by_alias(sdi_get_db_handle(), hdl,
                                             type, alias, &resource_hdl);
    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return resource_hdl;
}


/*
 * Retrieve the handle of first resource of the specified type within the entity.
 */
sdi_resource_hdl_t sdi_entity_get_first_resource(sdi_entity_hdl_t hdl, sdi_resource_type_t type)
{
    sdi_resource_hdl_t resource_hdl = 0;
    t_std_error rc;

    rc = sdi_db_get_first_resource(sdi_get_db_handle(), hdl, type, &resource_hdl);

    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return resource_hdl;
}

/*
 * Retrieve the handle of next resource of the specified type within the entity.
 */
sdi_resource_hdl_t sdi_entity_get_next_resource(sdi_resource_hdl_t hdl, sdi_resource_type_t type)
{
    t_std_error rc;
    sdi_entity_hdl_t entity_hdl = NULL;

    rc = sdi_db_resource_get_parent_entity(sdi_get_db_handle(), hdl, type, &entity_hdl);

    if (rc != STD_ERR_OK) {
        return NULL;
    }

    rc = sdi_db_get_next_resource(sdi_get_db_handle(), entity_hdl, type, &hdl);

    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return hdl;
}

/*
 * Apply the specified function for every resource in the entity
 * For every entity in the sytem, user-specified function "fn" will be called
 * with corresponding entity and user-specified data ar arguments.
 */
void sdi_entity_for_each_resource(sdi_entity_hdl_t hdl,
        void (*fn)(sdi_resource_hdl_t, void *user_data), void *user_data)
{
    sdi_resource_hdl_t r_hdl;
    sdi_resource_type_t r_type;
    uint_t r_count;
    uint_t i;
    t_std_error rc;

    STD_ASSERT(fn != NULL);

    for (r_type = SDI_RESOURCE_MIN; r_type < SDI_RESOURCE_MAX; r_type++) {

        /* Walk all the resources of each type */
        rc = sdi_db_get_resource_count(sdi_get_db_handle(), hdl, r_type, &r_count);
        if (rc != STD_ERR_OK || r_count == 0) {
            /* If we got an error return from the DB API, or a zero count,
             * skip to the next resource type
             */
            continue;
        }

        /* Lookup each resource by instance */
        for (i = 1; i <= r_count; i++) {
            rc = sdi_db_get_resource_handle(sdi_get_db_handle(), hdl, r_type,
                                            i, &r_hdl);
            if (rc != STD_ERR_OK) {
                /* Skip this instance if we got an error return */
                continue;
            }

            /* Call the user defined function on that resource */
            fn(r_hdl, user_data);
        }
    }
}

static t_std_error sdi_entity_info_get(sdi_resource_hdl_t res_hdl, sdi_entity_info_t *info)
{
    t_std_error rc;
    db_sql_handle_t db_hdl = sdi_get_db_handle();
    char hw_rev_buffer[DB_SQL_OUTPUT_LEN];

    if (((rc = sdi_db_str_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_PRODUCT, info->prod_name)) != STD_ERR_OK) ||
        ((rc = sdi_db_str_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_PPID, info->ppid)) != STD_ERR_OK) ||
        ((rc = sdi_db_str_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_HW_REV, hw_rev_buffer)) != STD_ERR_OK) ||
        ((rc = sdi_db_str_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_PLATFORM, info->platform_name)) != STD_ERR_OK) ||
        ((rc = sdi_db_str_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_VENDOR, info->vendor_name)) != STD_ERR_OK) ||
        ((rc = sdi_db_str_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_SERVICE_TAG, info->service_tag)) != STD_ERR_OK) ||
        ((rc = sdi_db_int_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_NUM_MACS, &info->mac_size)) != STD_ERR_OK) ||
        ((rc = sdi_db_int_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_NUM_FANS, &info->num_fans)) != STD_ERR_OK) ||
        ((rc = sdi_db_int_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_FAN_MAX_SPEED, &info->max_speed)) != STD_ERR_OK) ||
        ((rc = sdi_db_int_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_FAN_AIRFLOW, (int*)&info->air_flow)) != STD_ERR_OK) ||
        ((rc = sdi_db_int_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_POWER_RATING, &info->power_rating)) != STD_ERR_OK) ||
        ((rc = sdi_db_int_field_get(db_hdl, res_hdl, TABLE_INFO, INFO_POWER_TYPE, (int*)&info->power_type)) != STD_ERR_OK)) {
        safestrncpy(info->hw_revision, hw_rev_buffer, SDI_HW_REV_LEN);
        return rc;
    }

    return STD_ERR_OK;
}

/*
 * Retrieve the entity info for the given resource handle
 */
t_std_error sdi_entity_info_read(sdi_resource_hdl_t res_hdl, sdi_entity_info_t *info)
{
    t_std_error       rc;
    uint_t            len;
    char              c[18];
    FILE              *fptr;
    sdi_entity_type_t entity_type;
    uint_t            base_mac[6];
    static bool       is_init = true;

    STD_ASSERT(info != NULL);

    /* Clear the info structure */
    memset(info, 0, sizeof(*info));

    /* Retrieve the fields one by one from the database */
    if ((rc = sdi_entity_info_get(res_hdl, info)) != STD_ERR_OK) {
        return rc;
    }

    sdi_db_get_entity_type(sdi_get_db_handle(), res_hdl, &entity_type);

    if(is_init && entity_type == SDI_ENTITY_SYSTEM_BOARD) {
        if(NULL != (fptr=fopen("/sys/class/net/eth0/address", "r"))) {

            if (fgets(c, sizeof(c), fptr) == NULL) {
                fclose(fptr);
                return STD_ERR(BOARD, PARAM, EINVAL);
            }
            sscanf(c,
                  "%02x:%02x:%02x:%02x:%02x:%02x",
                  &(base_mac[0]),
                  &(base_mac[1]),
                  &(base_mac[2]),
                  &(base_mac[3]),
                  &(base_mac[4]),
                  &(base_mac[5])
                  );
            fclose(fptr);
            info->base_mac[0] = (uint8_t) base_mac[0];
            info->base_mac[1] = (uint8_t) base_mac[1];
            info->base_mac[2] = (uint8_t) base_mac[2];
            info->base_mac[3] = (uint8_t) base_mac[3];
            info->base_mac[4] = (uint8_t) base_mac[4];
            info->base_mac[5] = (uint8_t) base_mac[5];
            len = sizeof(info->base_mac);
            rc = sdi_db_bin_field_set(sdi_get_db_handle(), res_hdl, TABLE_INFO,
                INFO_BASE_MAC, info->base_mac, len);
            if (rc != STD_ERR_OK) {
                return rc;
            }
            is_init = false;
            return STD_ERR_OK;
        }
    }

    /* Base MAC is a binary field, so can't use the above macro */
    len = sizeof(info->base_mac);
    return sdi_db_bin_field_get(sdi_get_db_handle(), res_hdl, TABLE_INFO,
            INFO_BASE_MAC, info->base_mac, &len);
}

/** Get the psu output power status for a given psu. */
t_std_error sdi_entity_psu_output_power_status_get(sdi_entity_hdl_t entity_hdl, bool *status)
{
    STD_ASSERT(status != NULL);

    *status = true;
    return STD_ERR_OK;
}

/*
 * Retrieve the resource type for the given resource handle
 */
sdi_resource_type_t sdi_resource_type_get(sdi_resource_hdl_t hdl)
{
    t_std_error rc;
    int type;

    rc = sdi_db_int_field_get(sdi_get_db_handle(), hdl, TABLE_RESOURCES,
                              TBL_RESOURCE_TYPE, &type);

    if (rc != STD_ERR_OK) {
        return 0;
    }

    return (sdi_resource_type_t)type;
}

/*
 * Retrieve the alias for the specified resource
 */
/* Since we can't return a value on the stack, we use a static variable
 * and return a pointer to that. This is not thread safe.
 */
static char sdi_resource_alias_str[NAME_MAX];
const char * sdi_resource_alias_get(sdi_resource_hdl_t hdl)
{
    t_std_error rc;

    rc = sdi_db_str_field_get(sdi_get_db_handle(), hdl, TABLE_RESOURCES,
                              TBL_RESOURCE_ALIAS, sdi_resource_alias_str);

    if (rc != STD_ERR_OK) {
        return NULL;
    }

    return sdi_resource_alias_str;
}
