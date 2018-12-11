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
 * filename: sdi_resource_framework.c
 */


/**************************************************************************************
 * Core SDI framework which provides core api that work on resource.
 ***************************************************************************************/
#include "sdi_resource_internal.h"
#include "sdi_fan_internal.h"
#include "sdi_thermal_internal.h"
#include "sdi_ext_ctrl_internal.h"
#include "sdi_nvram_internal.h"
#include "sdi_power_monitor_internal.h"
#include "sdi_entity_info_internal.h"
#include "std_assert.h"
#include "std_llist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static std_dll_head resource_list;

/**
 * sdi_resource_node_t - holds resource specific data
 */
typedef struct sdi_resource_node {
    std_dll node; /**< node to a resource*/
    sdi_resource_priv_hdl_t resource_hdl; /**< resource specific data */
}sdi_resource_node_t;

/**
 * Wrapper function for allocation memory for the resource
 */
void sdi_resource_alloc(sdi_resource_node_t **ptr)
{
    *ptr = (sdi_resource_node_t *)calloc(1, sizeof(sdi_resource_node_t));
    STD_ASSERT(ptr != NULL);
}

/**
 * Returns the name of resource from resource handler
 */
const char *sdi_resource_name_get(sdi_resource_hdl_t hdl)
{
    return ((sdi_resource_priv_hdl_t)hdl)->name;
}

/**
 * Returns the type of resource from resource handler
 */
sdi_resource_type_t sdi_internal_resource_type_get(sdi_resource_hdl_t hdl)
{
    return ((sdi_resource_priv_hdl_t)hdl)->type;
}

/**
 * Initilizes the resource list database
 */
void sdi_resource_mgr_init(void)
{
    std_dll_init(&resource_list);
}

/**
 * Creates handler for resource and add this resource handler to resource list
 *
 * type[in] - type of the resource
 * name[in] - name of the resource
 * callback_hdl[in] - Call back handlers for the resource
 * callback_fns[in] - Call back functions for the resource
 */
void sdi_resource_add(sdi_resource_type_t type, const char *name, void *callback_hdl, void *callback_fns)
{
    sdi_resource_node_t *newnode = NULL;
    sdi_resource_alloc(&newnode);

    newnode->resource_hdl = (struct sdi_resource *)calloc(1, sizeof(struct sdi_resource));
    STD_ASSERT(newnode->resource_hdl != NULL);

    strncpy(newnode->resource_hdl->name, name, SDI_MAX_NAME_LEN);
    newnode->resource_hdl->type = type;
    newnode->resource_hdl->callback_hdl = callback_hdl;
    newnode->resource_hdl->callback_fns = callback_fns;

    std_dll_insertatback(&resource_list, (std_dll *)newnode);
}

/**
 * Finds the resource handler by suing resource name
 *
 * name[in] - Name of the resource
 *
 * returns resource handler if name matches with resource name in resource list else NULL.
 */
sdi_resource_hdl_t sdi_find_resource_by_name(const char *name)
{
    sdi_resource_node_t *node = NULL;

    STD_ASSERT(name != NULL);
    node = (sdi_resource_node_t *)std_dll_getfirst(&resource_list);
    while(node)
    {
        if (strncmp(sdi_resource_name_get(node->resource_hdl), name, SDI_MAX_NAME_LEN) == 0)
        {
            return (sdi_resource_hdl_t) (node->resource_hdl);
        }
        node=(sdi_resource_node_t *)std_dll_getnext(&resource_list, (std_dll *)node);
    }
    return NULL;
}

/**
 * sdi_dump_resources can be used for debugging purpose to list all the
 * resources in the systsem.
 */
void sdi_dump_resources (void)
{
    sdi_resource_node_t *node = NULL;
    node = (sdi_resource_node_t *)std_dll_getfirst(&resource_list);
    while(node)
    {
        sdi_resource_priv_hdl_t res_hdl = (sdi_resource_priv_hdl_t)(node->resource_hdl);
        fprintf(stdout, "\nName : %s, type = %d, fns(%p), hdl(%p), alias: %s\n",
                res_hdl->name, res_hdl->type, res_hdl->callback_fns, res_hdl->callback_hdl,
                res_hdl->alias);
        node=(sdi_resource_node_t *)std_dll_getnext(&resource_list, (std_dll *)node);
    }
    return;
}

/**
 * Initialize the resource.
 * param[in] hdl - handle to the resource whose information has to be initialised.
 * param[in] data - data to initialise
 * return STD_ERR_OK on success and standard error on failure
 */
t_std_error sdi_resource_init(sdi_resource_hdl_t hdl, void *data)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t resource_hdl = (sdi_resource_priv_hdl_t)hdl;

    STD_ASSERT(resource_hdl != NULL);

    switch(sdi_internal_resource_type_get(resource_hdl)) {
       case SDI_RESOURCE_FAN:
           STD_ASSERT((uint_t *)data != NULL);
           if ((((fan_ctrl_t *)resource_hdl->callback_fns)->init) != NULL) {
               rc = ((fan_ctrl_t *)resource_hdl->callback_fns)->init(resource_hdl->callback_hdl,
                                                                *((uint_t *)data));
           }
           break;
       case SDI_RESOURCE_ENTITY_INFO:
           if ((((entity_info_t *)resource_hdl->callback_fns)->init) != NULL) {
               rc = ((entity_info_t *)resource_hdl->callback_fns)->init(resource_hdl->callback_hdl);
           }
           break;
       case SDI_RESOURCE_TEMPERATURE:
           if ((((temperature_sensor_t *)resource_hdl->callback_fns)->init) != NULL) {
               rc = ((temperature_sensor_t *)resource_hdl->callback_fns)->init(resource_hdl->callback_hdl);
           }
           break;
       case SDI_RESOURCE_NVRAM:
           if ((((nvram_t *) resource_hdl->callback_fns)->init) != NULL) {
               rc = (*((nvram_t *) resource_hdl->callback_fns)->init)(resource_hdl->callback_hdl);
           }
           break;
       case SDI_RESOURCE_POWER_MONITOR:
           if ((((power_monitor_t *) resource_hdl->callback_fns)->init) != NULL) {
               rc = (*((power_monitor_t *) resource_hdl->callback_fns)->init)(resource_hdl->callback_hdl);
           }
           break;
       case SDI_RESOURCE_EXT_CONTROL:
           if ((((ext_ctrl_t *)resource_hdl->callback_fns)->init) != NULL) {
               rc = ((ext_ctrl_t *)resource_hdl->callback_fns)->init(resource_hdl->callback_hdl);
           }
           break;
       case SDI_RESOURCE_LED:
       case SDI_RESOURCE_MEDIA:
       case SDI_RESOURCE_COMM_DEV:
       case SDI_RESOURCE_HOST_SYSTEM:
       case SDI_RESOURCE_DIGIT_DISPLAY_LED:
       case SDI_RESOURCE_UPGRADABLE_PLD:
           rc = STD_ERR_OK;
           break;
       default:
           break;
    }

    if(rc != STD_ERR_OK)  {
       SDI_ERRMSG_LOG("Failed to init resource=%s rc=%d \n", resource_hdl->name, rc);
    }

    return rc;
}
