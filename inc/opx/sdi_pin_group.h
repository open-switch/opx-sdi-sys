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
 * filename: sdi_pin_group.h
 */


/******************************************************************************
 * @file sdi_pin_group.h
 * @brief Defines SDI PIN Group Bus Data Structures and Pin Goup Bus Operator
 * functions
 *****************************************************************************/

#ifndef __SDI_PIN_GROUP_H___
#define __SDI_PIN_GROUP_H___

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_pin.h"


/**
 * @defgroup sdi_internal_pin_group_bus_data_types SDI Internal Pin Group Bus Data Types
 * @brief SDI PIN Group Bus Data Structures and Pin Group Bus Operator functions
 * Platform Hardware consists of several components that exposes pin groups to
 * be accessed/controlled by Platform Software for desired functionality like:
 * - SOC
 * CPU exposes group of GPIO pins to read CPU-ID, indicate system status via
 * LEDs etc
 * CPLD provides pingroups to
 *    - multiplex media (sfp/qsfp etc) using group of CPLD pins
 *    - indicate system status via LED
 * @ref sdi_internal_pin_group_bus_framework_apis provides mechanism to register
 * every pin group with their functionality. Devices that exposes pins groups
 * registers every pin group with pin group bus framework.
 * Device Drivers can access/consume the pin group using the pin group handle
 * via pin group bus api @ref sdi_internal_pin_group_bus_api
 * Device Drivers doesn't need to be aware of the underlying device that exports
 * the pin group and this framework simplifies writing drivers.
 * Pin group handle
 * - encapsulates sdi_bus_t object to identify pin group bus.
 * - pin group bus operations like get/set pin group level, direction
 * (input/ouput), polarity(normal/inverted)
 * - default configuration for pin group
 * - lock to syncrhonize access to pin group
 *
 * @note
 * 1. Pin group can be created only with pins produced by same pin producer.
 * 2. A pin can't be part of both registered pin-group and registered pin.
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @typedef sdi_pin_group_bus_t
 * SDI PIN GROUP BUS type
 */
typedef struct sdi_pin_group_bus sdi_pin_group_bus_t;

/**
 * @typedef sdi_pin_group_bus_hdl_t
 * SDI PIN GROUP Bus Handle
 */
typedef sdi_pin_group_bus_t *sdi_pin_group_bus_hdl_t;

/**
 * @struct sdi_pin_group_bus_ops
 * SDI PIN Group Bus Operations defined by every PIN Bus Registered
 */
typedef struct sdi_pin_group_bus_ops {
    /**
     * @brief sdi_pin_group_bus_read_level
     * Read value of the Pin Group
     */
    t_std_error (*sdi_pin_group_bus_read_level) (sdi_pin_group_bus_hdl_t bus_hdl,
        uint_t *value);
    /**
     * @brief sdi_pin_group_bus_write_level
     * Write value of the Pin Group
     */
    t_std_error (*sdi_pin_group_bus_write_level) (sdi_pin_group_bus_hdl_t bus_hdl,
        uint_t value);
    /**
     * @brief sdi_pin_group_bus_set_direction
     * Configure pin group direction to Input/Output
     */
    t_std_error (*sdi_pin_group_bus_set_direction) (sdi_pin_group_bus_hdl_t bus_hdl,
            sdi_pin_bus_direction_t direction);
    /**
     * @brief sdi_pin_group_bus_get_direction
     * Get configured pin group direction
     */
    t_std_error (*sdi_pin_group_bus_get_direction) (sdi_pin_group_bus_hdl_t bus_hdl,
            sdi_pin_bus_direction_t *direction);
    /**
     * @brief sdi_pin_group_bus_set_polarity
     * Set the polarity of the pin group
     */
    t_std_error (*sdi_pin_group_bus_set_polarity) (sdi_pin_group_bus_hdl_t bus_hdl,
            sdi_pin_bus_polarity_t polarity);
    /**
     * @brief sdi_pin_group_bus_get_polarity
     * Get current configured pin group polarity
     */
    t_std_error (*sdi_pin_group_bus_get_polarity) (sdi_pin_group_bus_hdl_t bus_hdl,
            sdi_pin_bus_polarity_t *polarity);
} sdi_pin_group_bus_ops_t;

/**
 * @struct sdi_pin_group_bus
 * SDI PIN Bus Structure Registered by every PIN Bus
 */
typedef struct sdi_pin_group_bus {
    /**
     * @brief bus
     * SDI BUS object, to store bus_type, bus_id and bus_name
     */
    sdi_bus_t bus;
    /**
     * @brief ops
     * SDI PIN Group Bus Operations to read/write status, change/get direction,
     * polarity.
     */
    sdi_pin_group_bus_ops_t *ops;
    /**
     * @brief default_direction
     * SDI PIN Group Bus Direction
     * Set to SDI_PIN_BUS_INPUT for pins with no default configuration
     */
    sdi_pin_bus_direction_t default_direction;
    /**
     * @brief default_level
     * SDI PIN Group Bus Default Level to be set during initialization
     */
    uint_t default_level;
    /**
     * @brief default_polarity
     * SDI PIN Group Bus Default Polarity to be set during initialization
     * Set to SDI_PIN_POLARITY_NORMAL for pins with no default configuration
     */
    sdi_pin_bus_polarity_t default_polarity;
    /**
     * @brief lock
     * To serialize access to pin
     */
    pthread_mutex_t lock;
} sdi_pin_group_bus_t;

/**
 * @}
 */
#endif /* __SDI_PIN_GROUP_H___ */
