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
 * filename: sdi_bus_framework.c
 */


/******************************************************************************
 * Implements SDI Bus framework API for Bus registration and initialisation of
 * the Bus Framework Functionality.
 *
 * Note:
 * All the internal apis will not check the argument, whether it is valid. Its
 * caller responsibility to validate it.
 *****************************************************************************/

#include "std_assert.h"
#include "sdi_bus_framework.h"
#include "sdi_sys_common.h"
#include "sdi_driver_internal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* List of SDI Bus List */
static sdi_bus_list_t sdi_bus_lists[SDI_MAX_BUS];

static sdi_bus_list_t *sdi_bus_get_list_by_bus_type(sdi_bus_type_t bus_type);

/**
 * sdi_bus_init_list
 * Initialize given bus list head and lock
 * list[in] - list bus list type
 * return none
 */
void sdi_bus_init_list(sdi_bus_list_t *list)
{
    std_dll_init(&list->head);
    std_mutex_lock_init_non_recursive(&list->lock);
}

/**
 * sdi_bus_framework_init
 * Initialize sdi bus framework. Needs to be invoked during sdi init.
 * return none
 */
void sdi_bus_framework_init(void)
{
    sdi_bus_type_t bus_type = SDI_I2C_BUS;

    /**
     * Initialize bus list and mutex
     */
    for (bus_type = SDI_I2C_BUS; bus_type < SDI_MAX_BUS; bus_type++) {
        sdi_bus_init_list(&sdi_bus_lists[bus_type]);
    }
}

/**
 * sdi_bus_enqueue_list
 * Add bus to be registered to the bus list.
 * Parameters:
 * list[in]  - bus list
 * bus[in]   - bus handle to be added to bus list
 * return STD_ERR_OK on success
 */
t_std_error sdi_bus_enqueue_list(sdi_bus_list_t *list, sdi_bus_hdl_t bus)
{
    t_std_error    ret = STD_ERR_OK;
    sdi_bus_node_t *bus_node = (sdi_bus_node_t *) calloc (sizeof(sdi_bus_list_t), 1);

    STD_ASSERT (bus_node != NULL);

    ret = std_mutex_lock(&list->lock);
    if (ret != STD_ERR_OK) {
        return ret;
    }
    bus_node->bus = bus;
    std_dll_insertatback(&(list->head), &(bus_node->node));
    ret = std_mutex_unlock(&list->lock);
    return ret;
}

/**
 * sdi_bus_dequeue_list
 * Remove bus to be unregistered from the bus list
 * Parameters:
 * bus_node[in] - bus node to be unregistered from bus list
 * return STD_ERR_OK on success
 */
static t_std_error sdi_bus_dequeue_list(sdi_bus_node_t *bus_node)
{
    t_std_error ret = STD_ERR_OK;
    sdi_bus_list_t *list = NULL;

    STD_ASSERT(bus_node->bus != NULL);
    list = sdi_bus_get_list_by_bus_type(bus_node->bus->bus_type);
    STD_ASSERT(list != NULL);

    ret = std_mutex_lock(&list->lock);
    if (ret != STD_ERR_OK) {
        return ret;
    }
    std_dll_remove(&(list->head), &(bus_node->node));
    free(bus_node);
    ret = std_mutex_unlock(&list->lock);
    return ret;
}

/**
 * sdi_bus_get_list_by_bus_type
 * Return the list based on the corresponding bus type
 * Parameters:
 * bus_type[in] - type of bus i2c/pin/pingroup
 * return list pointer if valid list exists, otherwise NULL
 */
static sdi_bus_list_t *sdi_bus_get_list_by_bus_type(sdi_bus_type_t bus_type)
{
    if (bus_type < SDI_MAX_BUS) {
        return (sdi_bus_list_t *)(&sdi_bus_lists[bus_type]);
    }
    return NULL;
}

/**
 * sdi_bus_register
 * Register bus to the bus framework.
 * Parameters:
 * bus_handle[in]  - Handle of the bus to be registered
 * return STD_ERR_OK on successful registration
 */
t_std_error sdi_bus_register(sdi_bus_hdl_t bus_handle)
{
    sdi_bus_list_t *list = NULL;
    sdi_dev_list_t *dev_list = NULL;

    STD_ASSERT (bus_handle != NULL);

    list = sdi_bus_get_list_by_bus_type(bus_handle->bus_type);
    STD_ASSERT (list != NULL);

    dev_list = &(bus_handle->sdi_device_list);
    STD_ASSERT (dev_list != NULL);
    std_dll_init(&(dev_list->head));
    std_mutex_lock_init_non_recursive(&(dev_list->lock));

    return sdi_bus_enqueue_list(list, bus_handle);
}

/**
 * brief sdi_bus_init
 * Initialize bus spcified by bus handle
 * param[in] bus_handle - bus to be initialized
 * return STD_ERR_OK on successful bus initialization
 */
t_std_error sdi_bus_init(sdi_bus_hdl_t bus_handle)
{
    STD_ASSERT (bus_handle != NULL);
    return bus_handle->bus_init(bus_handle);
}

/**
 * sdi_bus_get_node
 * Return bus node object of bus from bus list
 * Parameters:
 * bus[in]  - bus for which bus node needs to be returned
 * return NULL on error, otherwise bus node
 */
static sdi_bus_node_t *sdi_bus_get_node(sdi_bus_t *bus)
{
    t_std_error    ret = STD_ERR_OK;
    sdi_bus_list_t *list = NULL;
    sdi_bus_node_t *dll_node = NULL;
    bool bus_node_found = false;

    STD_ASSERT (bus != NULL);

    list = sdi_bus_get_list_by_bus_type(bus->bus_type);
    STD_ASSERT (list != NULL);

    ret = std_mutex_lock(&list->lock);
    if (ret != STD_ERR_OK) {
        return NULL;
    }

    for ((dll_node = (sdi_bus_node_t *)std_dll_getfirst(&list->head));
         (dll_node != NULL);
         (dll_node = (sdi_bus_node_t *)std_dll_getnext(&list->head, &dll_node->node)))
    {
        if (bus == dll_node->bus) {
            bus_node_found = true;
            break;
        }
    }

    ret = std_mutex_unlock(&list->lock);
    if (ret != STD_ERR_OK) {
        return NULL;
    }

    if (!bus_node_found) {
        return NULL;
    }

    return dll_node;
}

/**
 * sdi_bus_unregister
 * Unregister a bus from bus framework list
 * Parameters:
 * bus[in] - bus to be unregistered
 * return STD_ERR_OK on success
 */
t_std_error sdi_bus_unregister(sdi_bus_hdl_t bus)
{
    sdi_bus_node_t *node = NULL;

    STD_ASSERT (bus != NULL);
    node = sdi_bus_get_node(bus);
    STD_ASSERT (node != NULL);
    return sdi_bus_dequeue_list(node);
}

/**
 * sdi_bus_for_each
 * Iterate on each bus of given type and run specified function on every bus
 * Parameters:
 * bus_type[in] - type of bus i2c/pin/pingroup
 * fn[in]       - function pointer to operate on every bus
 * return STD_ERR_OK on success and standard error on failure
 */
t_std_error sdi_bus_for_each(sdi_bus_type_t bus_type, void (*fn)(sdi_bus_hdl_t bus_hdl))
{
    t_std_error    ret = STD_ERR_OK;
    sdi_bus_list_t *list = NULL;
    sdi_bus_node_t *dll_node = NULL;

    STD_ASSERT(fn != NULL);

    list = sdi_bus_get_list_by_bus_type(bus_type);
    STD_ASSERT(list != NULL);

    ret = std_mutex_lock(&list->lock);
    if (ret != STD_ERR_OK) {
        return ret;
    }

    dll_node = (sdi_bus_node_t *)std_dll_getfirst(&list->head);
    for ((dll_node = (sdi_bus_node_t *)std_dll_getfirst(&list->head));
         (dll_node != NULL);
         (dll_node = (sdi_bus_node_t *)std_dll_getnext(&list->head, &dll_node->node)))
    {
        if ((dll_node->bus != NULL)) {
            fn(dll_node->bus);
        }
    }

    ret = std_mutex_unlock(&list->lock);
    return ret;
}

/**
 * sdi_bus_get_handle
 * Get bus handle for a bus type with matching bus identifier/name
 * @todo:
 * Move this to common-utils by implementing an api like #g-list-find-custom
 * http://developer.gimp.org/api/2.0/glib/glib-Doubly-Linked-Lists.html
 *
 * Parameters:
 * bus_type[in]       - type of bus i2c/pin/pingroup
 * compare_type[in]   - SDI_COMPARE_BY_ID or SDI_COMPARE_BY_NAME
 * return returns node that satisfies comparator, otherwise NULL
 */
static sdi_bus_t * sdi_bus_get_handle(sdi_bus_type_t bus_type,
    sdi_bus_search_type_t compare_type, void *bus_arg)
{
    t_std_error    ret = STD_ERR_OK;
    sdi_bus_list_t *list = NULL;
    sdi_bus_node_t *dll_node = NULL;
    bool hdl_found = false;

    STD_ASSERT (bus_arg != NULL);

    list = sdi_bus_get_list_by_bus_type(bus_type);
    STD_ASSERT (list != NULL);

    ret = std_mutex_lock(&list->lock);
    if (ret != STD_ERR_OK) {
        return NULL;
    }

    for((dll_node = (sdi_bus_node_t *)std_dll_getfirst(&list->head));
        (dll_node != NULL);
        (dll_node = (sdi_bus_node_t *)std_dll_getnext(&list->head, &dll_node->node)))
    {
        if ((dll_node->bus != NULL)) {
            if (compare_type == SDI_COMPARE_BY_ID) {
                if (((*(uint_t *)bus_arg) == dll_node->bus->bus_id)) {
                    hdl_found = true;
                    break;
                }
            }else if(compare_type == SDI_COMPARE_BY_NAME) {
                if (!(strncmp((char *)bus_arg, dll_node->bus->bus_name, SDI_MAX_NAME_LEN))) {
                    hdl_found = true;
                    break;
                }
            }
        }
    }

    if((std_mutex_unlock(&list->lock)) != STD_ERR_OK){
        return NULL;
    }

    if(!hdl_found){
        return NULL;
    }
    return dll_node->bus;
}

/**
 * sdi_bus_get_handle_by_name
 * Get handle of bus that matches given bus name of specified bus type
 * Parameters:
 * bus_name [in] - bus name
 * bus_type[in]  - type of bus i2c/pin/pingroup
 * return NULL if no matching bus found, otherwise returns bus handle
 */
sdi_bus_hdl_t  sdi_bus_get_handle_by_name(const char *bus_name, sdi_bus_type_t bus_type)
{
    STD_ASSERT(bus_name != NULL);
    return sdi_bus_get_handle(bus_type, SDI_COMPARE_BY_NAME, (void *)bus_name);
}

/**
 * sdi_bus_get_handle_by_id_
 * Get handle of bus that matches given bus id of specified bus type
 * Parameters:
 * bus_id[in]   - bus identifier
 * bus_type[in] - type of bus i2c/pin/pingroup
 * return NULL if no matching bus found, otherwise returns bus handle
 */
sdi_bus_hdl_t sdi_bus_get_handle_by_id(sdi_bus_id_t bus_id, sdi_bus_type_t bus_type)
{
    return sdi_bus_get_handle(bus_type, SDI_COMPARE_BY_ID, (void *)&bus_id);
}

/**
 * sdi_bus_get_symbol
 * Get driver of bus that matches given bus node
 * Parameters:
 * bus_node[in] - bus identifier
 * return NULL if no matching bus found, otherwise returns bus driver symbol
 */
sdi_bus_driver_t *sdi_bus_get_symbol(std_config_node_t bus_node)
{
    const char *driver_name = NULL;

    driver_name = std_config_name_get(bus_node);
    STD_ASSERT(driver_name != NULL);

    return sdi_get_bus_driver(driver_name);
}

/**
 * sdi_bus_register_node
 * Registers and Initialize the buses of root node
 * Parameters:
 * node[in] - node of type bus
 * return none
 * @todo: needs to split bus register and init
 */
void sdi_bus_register_node(std_config_node_t root_node)
{
    std_config_node_t bus_node = NULL;
    sdi_bus_hdl_t bus = NULL;

    STD_ASSERT(root_node != NULL);
    for (bus_node = std_config_get_child(root_node);
         bus_node != NULL;
         bus_node = std_config_next_node(bus_node))
    {
        sdi_bus_driver_t *bus_driver = NULL;

        bus_driver = sdi_bus_get_symbol(bus_node);
        STD_ASSERT (bus_driver != NULL);

        SDI_TRACEMSG_LOG("%s:%d childnodes %s\n", __FUNCTION__, __LINE__,
            std_config_name_get(bus_node));

        if (bus_driver->bus_register(bus_node, &bus) == STD_ERR_OK) {
            if (bus_driver->bus_init(bus) != STD_ERR_OK) {
                SDI_ERRMSG_LOG("%s:%d bus %s initialization failed\n",
                    __FUNCTION__, __LINE__, std_config_name_get(bus_node));
            }
        } else {
            SDI_ERRMSG_LOG("%s:%d bus %s registration failed\n",
                __FUNCTION__, __LINE__, std_config_name_get(bus_node));
        }
        bus = NULL;
        bus_driver = NULL;
    }
}

/**
 * brief sdi_attach_device_to_bus
 * Add a device handle to device list of bus
 * param[in] bus_handle - bus handle to which device is getting attached
 * param[in] dev_handle - device handle to be attached to the bus
 * return none
 */
void sdi_attach_device_to_bus(sdi_bus_hdl_t bus_handle, sdi_device_hdl_t
            dev_handle)
{
    sdi_dev_node_t *dev_node = NULL;
    sdi_dev_list_t *dev_list = &(bus_handle->sdi_device_list);

    dev_node=(sdi_dev_node_t *)calloc(sizeof(sdi_dev_node_t), 1);
    STD_ASSERT (dev_node != NULL);

    dev_node->dev = dev_handle;
    std_mutex_lock(&dev_list->lock);
    std_dll_insertatback(&(dev_list->head), &(dev_node->node));
    std_mutex_unlock(&dev_list->lock);
}

/**
 * brief sdi_bus_get_first_device
 * Return first device node entry in device list of specified bus handle
 * param[in] bus_handle - bus handle for which the first device node needs
 * to be obtained
 * return NULL on error, otherwise device node
 */
sdi_dev_node_t *sdi_bus_get_first_device(sdi_bus_hdl_t bus_handle)
{
    sdi_dev_node_t *dev_node = NULL;
    sdi_dev_list_t *dev_list = &(bus_handle->sdi_device_list);

    STD_ASSERT (dev_list != NULL);

    std_mutex_lock(&dev_list->lock);
    dev_node = (sdi_dev_node_t *) std_dll_getfirst(&dev_list->head) ;
    std_mutex_unlock(&dev_list->lock);
    return dev_node;
}

/**
 * brief sdi_bus_get_next_device
 * Return device node entry next to given device node entry in device
 * list of specified bus handle
 * param[in] bus_handle - bus handle for which the first device node needs to
 * be obtained
 * param[in] device_node - device node whose next node in bus device list is to
 * be obtained
 * return NULL on error, otherwise device node
 */
sdi_dev_node_t *sdi_bus_get_next_device(sdi_bus_hdl_t bus_handle,
    sdi_dev_node_t *device_node)
{
    sdi_dev_node_t *dev_node = NULL;
    sdi_dev_list_t *dev_list = &(bus_handle->sdi_device_list);

    STD_ASSERT (dev_list != NULL);

    std_mutex_lock(&dev_list->lock);
    dev_node = (sdi_dev_node_t *)std_dll_getnext(&dev_list->head,
            (std_dll *)device_node);
    std_mutex_unlock(&dev_list->lock);
    return dev_node;
}

/**
 * brief sdi_bus_register_device_list
 * Registers every device attached to given bus node and add device handle to
 * device list of given bus handle
 * param[in] bus_node - bus node
 * param[in] bus_handle - bus handle
 * return NULL
 */
void sdi_bus_register_device_list(std_config_node_t bus_node,
    sdi_bus_hdl_t bus_handle)
{
    std_config_node_t cur_node = NULL;
    sdi_device_hdl_t device_hdl = NULL;
    for (cur_node = std_config_get_child(bus_node); cur_node != NULL;
            cur_node = std_config_next_node(cur_node)) {
        sdi_register_driver(cur_node, bus_handle, &device_hdl);
        sdi_attach_device_to_bus(bus_handle, device_hdl);
    }
}

/**
 * brief sdi_bus_init_device_list
 * Initalize every device attached to the given bus handle
 * param[in] bus_handle - bus handle
 * return NULL
 */
void sdi_bus_init_device_list(sdi_bus_hdl_t bus_handle)
{
    sdi_device_hdl_t dev_handle = NULL;
    sdi_dev_node_t *dev_node = NULL;

    for (dev_node = sdi_bus_get_first_device(bus_handle); dev_node != NULL;
            dev_node = sdi_bus_get_next_device(bus_handle, dev_node)) {
        dev_handle = dev_node->dev;
        sdi_device_init(dev_handle);
    }
}

/**
 * sdi_bus_for_each_bus_in_list
 * Iterate on every bus in the given list and run specified function on bus
 * @param[in] list  bus list
 * @param[in] fn  function pointer to operate on bus handle for each bus in list
 */
void sdi_bus_for_each_bus_in_list(sdi_bus_list_t *list,
        void (*fn) (sdi_bus_hdl_t bus_handle))
{
    sdi_bus_node_t *node = NULL;

    STD_ASSERT (list != NULL);

    std_mutex_lock(&list->lock);
    node = (sdi_bus_node_t *)std_dll_getfirst(&list->head);
    for ((node = (sdi_bus_node_t *)std_dll_getfirst(&list->head));
            (node != NULL);
                (node = (sdi_bus_node_t *)std_dll_getnext(&list->head,
                &node->node))) {
        if ((node->bus != NULL)) {
            fn(node->bus);
        }
    }
    std_mutex_unlock(&list->lock);
}

/**
 * sdi_init_bus_for_each_bus_in_list
 * Iterate on every bus in the given list and run specified bus_init function on
 * bus, execute specified bus_init_failure_handler callback function in the
 * event of failure of bus initialization
 * param[in] list - bus list
 * param[in] bus_init - callback function to initialize bus (returns STD_ERR_OK
 * on successful bus initialization
 * param[in] bus_init_failure_handle - callback function to run when bus init
 * fails
 */
void sdi_init_bus_for_each_bus_in_list(sdi_bus_list_t *list,
        t_std_error (*bus_init) (sdi_bus_hdl_t bus_handle),
        void (*bus_init_failure_handler) (sdi_bus_hdl_t bus_handle))
{
    sdi_bus_node_t *node = NULL;
    t_std_error error = STD_ERR_OK;

    STD_ASSERT (list != NULL);

    std_mutex_lock(&list->lock);
    node = (sdi_bus_node_t *)std_dll_getfirst(&list->head);
    for ((node = (sdi_bus_node_t *)std_dll_getfirst(&list->head));
            (node != NULL);
                (node = (sdi_bus_node_t *)std_dll_getnext(&list->head,
                &node->node))) {
        if ((node->bus != NULL)) {
            if (bus_init != NULL) {
                error = bus_init(node->bus);
                if (error != STD_ERR_OK) {
                    if (bus_init_failure_handler != NULL) {
                        bus_init_failure_handler(node->bus);
                    }
                }
            }
        }
    }
    std_mutex_unlock(&list->lock);
}

/**
 * brief sdi_is_i2c_bus
 * check the given bus handle is i2c bus handle
 * param[in] bus_hdl bus handle
 * return true if bus_hdl is i2c, false otherwise
 */
bool sdi_is_i2c_bus(sdi_bus_hdl_t bus_hdl)
{
    if (bus_hdl->bus_type == SDI_I2C_BUS) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * brief sdi_is_pin_bus
 * check the given bus handle is pin bus handle
 * param[in] bus_hdl bus handle
 * return true if bus_hdl is pin, false otherwise
 */
bool sdi_is_pin_bus(sdi_bus_hdl_t bus_hdl)
{
    if (bus_hdl->bus_type == SDI_PIN_BUS) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * brief sdi_is_pin_group_bus
 * check the given bus handle is pin group bus handle
 * param[in] bus_hdl bus handle
 * return true if bus_hdl is pin group, false otherwise
 */
bool sdi_is_pin_group_bus(sdi_bus_hdl_t bus_hdl)
{
    if (bus_hdl->bus_type == SDI_PIN_GROUP_BUS) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * brief sdi_is_io_bus
 * check the given bus handle is io bus handle
 * param[in] bus_hdl bus handle
 * return true if bus_hdl is io, false otherwise
 */
bool sdi_is_io_bus(sdi_bus_hdl_t bus_hdl)
{
    if (bus_hdl->bus_type == SDI_IO_BUS) {
        return true;
    } else {
        return false;
    }
}
