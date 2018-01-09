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
 * filename: sdi_bus_framework.h
 */


/******************************************************************************
 * @file sdi_bus_framework.h
 * @brief SDI Bus Framework Data Structure and APIs
 *****************************************************************************/

#ifndef __SDI_BUS_FRAMEWORK_H___
#define __SDI_BUS_FRAMEWORK_H___

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "std_llist.h"
#include "std_mutex_lock.h"
#include "sdi_bus.h"
#include "sdi_driver_internal.h"

/**
 * @defgroup sdi_internal_bus_framework SDI Internal Bus Framework
 * @brief SDI Bus Framework Data Structure and APIs
 * SDI Bus framework maintains the list of registered buses and provides
 * APIs to
 * - register a bus created by bus drivers, with the framework
 * - iterate on bus list and execute a specified function on every bus
 * - get bus handle given bus identifier or bus name, used by debug utilties
 * like bus scan, peek and poke of devices attached to the bus.
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @typedef sdi_bus_node_t
 * SDI Bus Node in SDI Bus List
 */
typedef struct sdi_bus_node {
    /**
     * @brief dynamic linked list node for sdi bus
     */
    std_dll node;
    /**
     * @brief sdi bus handle to be pushed to sdi bus dynamic linked list
     */
    sdi_bus_t *bus;
} sdi_bus_node_t;

/**
 * @typedef sdi_dev_node_t
 * SDI Device Node in SDI Bus List
 */
typedef struct sdi_dev_node {
    /**
     * @brief dynamic linked list node for sdi device
     */
    std_dll node;

    /**
     * @brief sdi device handle to be pushed to sdi bus device list
     */
    sdi_device_hdl_t dev;
} sdi_dev_node_t;

/**
 * @typedef sdi_bus_search_type_t
 * Index type which is used to get handle from bus list
 */
typedef enum {
    SDI_COMPARE_BY_NAME,
    SDI_COMPARE_BY_ID
}sdi_bus_search_type_t;

/**
 * @brief sdi_bus_init_list
 * Initialize given bus list head and lock
 * @param[in] list - list bus list type
 * return none
 */
void sdi_bus_init_list(sdi_bus_list_t *list);

/**
 * @brief sdi_bus_framework_init
 * Initialize SDI Bus Registration Framework
 * @return none
 */
void sdi_bus_framework_init(void);

/**
 * @brief sdi_bus_enqueue_list
 * Add bus to be registered to the bus list.
 * Parameters:
 * @param[in] list  - bus list
 * @param[in] bus - bus handle to be added to bus list
 * return STD_ERR_OK on success
 */
t_std_error sdi_bus_enqueue_list(sdi_bus_list_t *list, sdi_bus_hdl_t bus);

/**
 * @brief SDI Bus Registration Function
 * Every SDI Bus device driver registers its bus handle with this framework
 * using this API
 * @param[in] bus_hdl Handle to SDI Bus
 * @return STD_ERR_OK on success
 */
t_std_error sdi_bus_register(sdi_bus_hdl_t bus_hdl);

/**
 * @brief sdi_bus_init
 * Initialize bus spcified by bus handle
 * @param[in] bus_handle - bus to be initialized
 * @return STD_ERR_OK on successful bus initialization
 */
t_std_error sdi_bus_init(sdi_bus_hdl_t bus_handle);

/**
 * @brief sdi_bus_get_handle_by_name
 * Get Bus Handle given its Bus Name and type
 * @param[in] bus_name Name of the Bus specified during bus registration
 * @param[in] bus_type Type of the Bus, I2C/PIN etc.
 * @return valid bus handle on success or Null on failure
 */
sdi_bus_hdl_t sdi_bus_get_handle_by_name(const char *bus_name,
    sdi_bus_type_t bus_type);

/**
 * @brief sdi_bus_get_handle_by_id
 * Get Bus Handle given its Bus Identifier and type
 * @param[in] bus_id Unique Bus Identifier specified during bus registration
 * @param[in] bus_type Type of the Bus, I2C/PIN etc.
 * @return valid bus handle on success or Null on failure
 */
sdi_bus_hdl_t sdi_bus_get_handle_by_id(sdi_bus_id_t bus_id,
    sdi_bus_type_t bus_type);

/**
 * @brief sdi_bus_get_symbol
 * Get driver of bus that matches given bus node
 * Parameters:
 * @param[in] bus_node - bus identifier
 * @return NULL if no matching bus found, otherwise returns bus driver symbol
 */
sdi_bus_driver_t *sdi_bus_get_symbol(std_config_node_t bus_node);

/**
 * @brief sdi_bus_register_node
 * Register every bus under given root_node
 * @param[in] root_node Configuration Node for root entry
 * @return none
 */
void sdi_bus_register_node(std_config_node_t root_node);

/**
 * @brief sdi_bus_for_each
 * Iterate on bus list for given bus type and run specified function on every
 * bus
 * @param[in] bus_type Type of bus i2c/pin/pingroup
 * @param[in] fn Function pointer to operate on every bus
 * @return STD_ERR_OK on success and standard error code on failure. This function return
 * error only when there is a failure in mutex acquisition while accessing linked list
 */
t_std_error sdi_bus_for_each(sdi_bus_type_t bus_type,
    void (*fn)(sdi_bus_hdl_t bus_hdl));

/**
 * @brief sdi_bus_for_each_bus_in_list
 * Iterate on every bus in the given list and run specified function on bus
 * @param[in] list  Bus list
 * @param[in] fn  Function pointer to operate on bus handle for each bus in list
 */
void sdi_bus_for_each_bus_in_list(sdi_bus_list_t *list,
        void (*fn) (sdi_bus_hdl_t bus_handle));

/**
 * @brief sdi_init_bus_for_each_bus_in_list
 * Iterate on every bus in the given list and run specified bus_init function on
 * bus, execute specified bus_init_failure_handler callback function in the
 * event of failure of bus initialization
 * @param[in] list - bus list
 * @param[in] bus_init - callback function to initialize bus (returns STD_ERR_OK
 * on successful bus initialization
 * @param[in] bus_init_failure_handler - callback function to run when bus init
 * fails
 */
void sdi_init_bus_for_each_bus_in_list(sdi_bus_list_t *list,
        t_std_error (*bus_init) (sdi_bus_hdl_t bus_handle),
        void (*bus_init_failure_handler) (sdi_bus_hdl_t bus_handle));

/**
 * @brief sdi_attach_device_to_bus
 * Add a device handle to device list of bus
 * @param[in] bus_handle - bus handle to which device is getting attached
 * @param[in] dev_handle - device handle to be attached to the bus
 * @return none
 */
void sdi_attach_device_to_bus(sdi_bus_hdl_t bus_handle,
                              sdi_device_hdl_t dev_handle);

/**
 * @brief sdi_bus_get_first_device
 * Return first device node entry in device list of specified bus handle
 * @param[in] bus_handle - bus handle for which the first device node needs
 * to be obtained
 * @return NULL on error, otherwise device node
 */
sdi_dev_node_t *sdi_bus_get_first_device(sdi_bus_hdl_t bus_handle);

/**
 * @brief sdi_bus_get_next_device
 * Return device node entry next to given device node entry in device
 * list of specified bus handle
 * @param[in] bus_handle - bus handle for which the first device node needs to
 * be obtained
 * @param[in] device_node - device node whose next node in bus device list is to
 * be obtained
 * @return NULL on error, otherwise device node
 */
sdi_dev_node_t *sdi_bus_get_next_device(sdi_bus_hdl_t bus_handle,
                                        sdi_dev_node_t *device_node);

/**
 * @brief sdi_bus_register_device_list
 * Registers every device attached to given bus node and add device handle to
 * device list of given bus handle
 * @param[in] bus_node - bus node
 * @param[in] bus_handle - bus handle
 * @return NULL
 */
void sdi_bus_register_device_list(std_config_node_t bus_node,
                                  sdi_bus_hdl_t bus_handle);

/**
 * @brief sdi_bus_init_device_list
 * Initalize every device attached to the given bus handle
 * @param[in] bus_handle - bus handle
 * @return NULL
 */
void sdi_bus_init_device_list(sdi_bus_hdl_t bus_handle);

/**
 * @brief sdi_is_i2c_bus
 * check the given bus handle is i2c bus handle
 * @param[in] bus_hdl bus handle
 * @return true if bus_hdl is i2c, false otherwise
 */
bool sdi_is_i2c_bus(sdi_bus_hdl_t bus_hdl);

/**
 * @brief sdi_is_io_bus
 * check the given bus handle is io bus handle
 * @param[in] bus_hdl bus handle
 * @return true if bus_hdl is io, false otherwise
 */
bool sdi_is_io_bus(sdi_bus_hdl_t bus_hdl);

/**
 * @brief sdi_is_pin_bus
 * check the given bus handle is pin bus handle
 * @param[in] bus_hdl bus handle
 * @return true if bus_hdl is pin, false otherwise
 */
bool sdi_is_pin_bus(sdi_bus_hdl_t bus_hdl);

/**
 * @brief sdi_is_pin_group_bus
 * check the given bus handle is pin group bus handle
 * @param[in] bus_hdl bus handle
 * @return true if bus_hdl is pin group, false otherwise
 */
bool sdi_is_pin_group_bus(sdi_bus_hdl_t bus_hdl);

/**
 * @}
 */
#endif /* __SDI_BUS_FRAMEWORK_H___ */
