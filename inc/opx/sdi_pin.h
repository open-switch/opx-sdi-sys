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
 * filename: sdi_pin.h
 */


/******************************************************************************
 * @file sdi_pin.h
 * @brief Defines SDI PIN Bus Data Structures and Pin bus Operator functions
 *****************************************************************************/

#ifndef __SDI_PIN_H___
#define __SDI_PIN_H___

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_bus.h"
#include "sdi_bus_framework.h"


/**
 * @defgroup sdi_internal_pin_bus_data_types SDI Internal Pin Bus Data Types
 * @brief  SDI PIN Bus Data Structures and Pin Bus Operator functions
 * Platform Hardware consists of several components that exposes pins to be
 * accessed/controlled by Platform Software for desired functionality like:
 * SOC
 * CPU exposes GPIO pins and could also have other pins. GPIO pins could be used
 * to read CPU ID, Control pins that are used to upgrade Programmable Logic
 * Devices, LEDs, Boot status indication to PLD etc
 * Programmable Logic Devices
 * CPLD provides pins/pingroups to
 *         indicate presence status of components like fan tray, psu, media/optics
 *         and sensors etc,
 *         reset CPU, NIC, NPU, USB and above listed components,
 *         control ON/OFF LED, Digit LED of system and port LED functionality,
 *         interrupts/alarm from HW components mentioned above to CPU,
 *         configure media/optic mode selection, mux selection of media, TX
 *         enable/disable and fault status, RX los configuration
 *         Special GPIO chips
 * @ref sdi_internal_pin_bus_framework_apis provides mechanism to register every pin
 * with their functionality. Devices that expose pins, register every pin with
 * pin bus framework.
 * Device Drivers can access/consume the pin using the pinhandle via pin bus api
 * @ref sdi_internal_pin_bus_api
 * Device Drivers doesn't need to be aware of the underlying device that exports
 * the pin and this framework simplifies writing drivers.
 * Pin handle
 *         encapsulates sdi_bus_t object to identify pin bus.
 *         pin bus operations like get/set pin level (high/low), direction
 *         (input/ouput), polarity(normal/inverted)
 *         default configuration of pin
 *         lock to syncrhonize access to pin
 *
 * @todo
 * Add interrupt related functionality when a pin can receive interrupt/generate
 * interrupt
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @enum sdi_pin_bus_direction_t
 * SDI PIN Direction
 */
typedef enum {
    /**
     * SDI PIN Bus is INPUT
     */
    SDI_PIN_BUS_INPUT,
    /**
     * SDI PIN Bus is OUTPUT
     */
    SDI_PIN_BUS_OUTPUT,
} sdi_pin_bus_direction_t;

/**
 * @enum sdi_pin_bus_polarity_t
 * SDI PIN Polarity
 */
typedef enum {
    /**
     * SDI PIN BUS Polarity Normal
     */
    SDI_PIN_POLARITY_NORMAL,
    /**
     * SDI PIN BUS Polarity INVERTED
     */
    SDI_PIN_POLARITY_INVERTED,
} sdi_pin_bus_polarity_t;

/**
 * @enum sdi_pin_bus_level_t
 * SDI PIN Level
 */
typedef enum {
    /**
     * SDI PIN Bus Level Driven LOW
      */
    SDI_PIN_LEVEL_LOW,
    /**
     * SDI PIN Bus Level Driven HIGH
      */
    SDI_PIN_LEVEL_HIGH,
} sdi_pin_bus_level_t;

/**
 * @typedef sdi_pin_bus_t
 * SDI PIN BUS type
 */
typedef struct sdi_pin_bus sdi_pin_bus_t;

/**
 * @typedef sdi_pin_bus_hdl_t
 * SDI PIN Bus Handle
 */
typedef sdi_pin_bus_t *sdi_pin_bus_hdl_t;

/**
 * @struct sdi_pin_bus_ops
 * SDI PIN Bus Operations defined by every PIN Bus Registered
 */
typedef struct sdi_pin_bus_ops {
    /**
     * @brief sdi_pin_bus_read_level
     * Read value of the Pin
     */
    t_std_error (*sdi_pin_bus_read_level) (sdi_pin_bus_hdl_t bus_handle,
        sdi_pin_bus_level_t *value);
    /**
     * @brief sdi_pin_bus_write_level
     * Write value of the Pin
     */
    t_std_error (*sdi_pin_bus_write_level) (sdi_pin_bus_hdl_t bus_handle,
        sdi_pin_bus_level_t value);
    /**
     * @brief sdi_pin_bus_set_direction
     * Configure pin direction to Input/Output
     */
    t_std_error (*sdi_pin_bus_set_direction) (sdi_pin_bus_hdl_t bus_handle,
            sdi_pin_bus_direction_t direction);
    /**
     * @brief sdi_pin_bus_get_direction
     * Get configured pin direction
     */
    t_std_error (*sdi_pin_bus_get_direction) (sdi_pin_bus_hdl_t bus_handle,
            sdi_pin_bus_direction_t *direction);
    /**
     * @brief sdi_pin_bus_set_polarity
     * Set the polarity of the pin
     */
    t_std_error (*sdi_pin_bus_set_polarity) (sdi_pin_bus_hdl_t bus_handle,
            sdi_pin_bus_polarity_t polarity);
    /**
     * @brief sdi_pin_bus_get_polarity
     * Get current configured pin polarity
     */
    t_std_error (*sdi_pin_bus_get_polarity) (sdi_pin_bus_hdl_t bus_handle,
            sdi_pin_bus_polarity_t *polarity);
} sdi_pin_bus_ops_t;

/**
 * @struct sdi_pin_bus
 * SDI PIN Bus Structure Registered by every PIN Bus
 */
typedef struct sdi_pin_bus {
    /**
     * @brief bus
     * SDI BUS object, to store bus_type, bus_id and bus_name
     */
    sdi_bus_t bus;
    /**
     * @brief ops
     * SDI PIN Bus Operations to read/write status, change/get direction,
     * polarity.
     */
    sdi_pin_bus_ops_t *ops;
    /**
     * @brief default_direction
     * SDI PIN Bus Direction
     * Set to SDI_PIN_BUS_INPUT for pins with no default configuration
     */
    sdi_pin_bus_direction_t default_direction;
    /**
     * @brief default_level
     * SDI PIN Bus Default Level to be set during initialization
     * Set to SDI_PIN_LEVEL_LOW for pins with no default configuration
     */
    sdi_pin_bus_level_t default_level;
    /**
     * @brief default_polarity
     * SDI PIN Bus Default Polarity to be set during initialization
     * Set to SDI_PIN_POLARITY_NORMAL for pins with no default configuration
     */
    sdi_pin_bus_polarity_t default_polarity;
    /**
     * @brief lock
     * To serialize access to pin
     */
    pthread_mutex_t lock;
} sdi_pin_bus_t;

/**
 * @}
 */
#endif /* __SDI_PIN_H___ */
