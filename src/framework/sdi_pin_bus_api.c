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
 * filename: sdi_pin_bus_api.c
 */


/******************************************************************************
 * Defines SDI PIN Bus APIs exposed to SDI Drivers to operate on pins.
 * These APIs hide the details of pin.
 *****************************************************************************/


#include "sdi_pin_bus_api.h"
#include "std_assert.h"

/**
 * sdi_validate_pin_bus_handle
 * Validate pin bus handle.
 * param[in] bus_handle - sdi pin bus handle
 */
static inline void sdi_validate_pin_bus_handle(sdi_pin_bus_hdl_t bus_handle)
{
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(bus_handle->bus.bus_type == SDI_PIN_BUS);
}

/**
 * sdi_pin_read_level
 * Read value of the Pin
 * param[in] bus - sdi pin bus object
 * param[out] value - sdi_pin_bus_level_t data pointer to store pin value
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_read_level(sdi_pin_bus_hdl_t bus,
                               sdi_pin_bus_level_t *value)
{
    t_std_error error = STD_ERR_OK;

    sdi_validate_pin_bus_handle(bus);

    STD_ASSERT(value != NULL);

    error = std_mutex_lock (&(bus->lock));
    if (error != STD_ERR_OK) {
        return error;
    }

    error = bus->ops->sdi_pin_bus_read_level(bus, value);

    std_mutex_unlock (&(bus->lock));

    return error;
}

/**
 * sdi_pin_write_level
 * Write value of the Pin
 * param[in] bus - sdi pin bus object
 * param[in] value - sdi_pin_bus_level_t value of pin to be written to
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_write_level(sdi_pin_bus_hdl_t bus,
                                sdi_pin_bus_level_t value)
{
    t_std_error error = STD_ERR_OK;

    sdi_validate_pin_bus_handle(bus);

    error = std_mutex_lock (&(bus->lock));
    if (error != STD_ERR_OK) {
        return error;
    }

    error = bus->ops->sdi_pin_bus_write_level(bus, value);

    std_mutex_unlock (&(bus->lock));

    return error;
}

/**
 * sdi_pin_set_direction
 * Configure the direction of pin
 * param[in] bus - sdi pin bus object
 * param[in] direction - INPUT/OUTPUT direction
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_set_direction(sdi_pin_bus_hdl_t bus,
                                  sdi_pin_bus_direction_t direction)
{
    t_std_error error = STD_ERR_OK;

    sdi_validate_pin_bus_handle(bus);

    error = std_mutex_lock (&(bus->lock));
    if (error != STD_ERR_OK) {
        return error;
    }

    error = bus->ops->sdi_pin_bus_set_direction(bus, direction);

    std_mutex_unlock (&(bus->lock));

    return error;

}

/**
 * sdi_pin_get_direction
 * Get the current configured direction of the pin
 * param[in] bus - sdi pin bus object
 * param[in] direction - sdi_pin_bus_direction_t data pointer to read direction
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_get_direction(sdi_pin_bus_hdl_t bus,
                                  sdi_pin_bus_direction_t *direction)
{
    t_std_error error = STD_ERR_OK;

    sdi_validate_pin_bus_handle(bus);

    STD_ASSERT(direction != NULL);

    error = std_mutex_lock (&(bus->lock));
    if (error != STD_ERR_OK) {
        return error;
    }

    error = bus->ops->sdi_pin_bus_get_direction(bus, direction);

    std_mutex_unlock (&(bus->lock));

    return error;

}

/**
 * sdi_pin_set_polarity
 * Set the polairty of the pin
 * param[in] bus - sdi pin bus object
 * param[in] polarity - sdi_pin_bus_polarity_t normal or inverted
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_set_polarity(sdi_pin_bus_hdl_t bus,
                                 sdi_pin_bus_polarity_t polarity)
{
    t_std_error error = STD_ERR_OK;

    sdi_validate_pin_bus_handle(bus);

    error = std_mutex_lock (&(bus->lock));
    if (error != STD_ERR_OK) {
        return error;
    }

    error = bus->ops->sdi_pin_bus_set_polarity(bus, polarity);

    std_mutex_unlock (&(bus->lock));

    return error;

}

/**
 * sdi_pin_get_polarity
 * Get the current configured polarity of the pin
 * param[in] bus - sdi pin bus object
 * param[in] polarity - sdi_pin_bus_polarity_t pointer  normal or inverted
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_get_polarity(sdi_pin_bus_hdl_t bus,
                                 sdi_pin_bus_polarity_t *polarity)
{
    t_std_error error = STD_ERR_OK;

    sdi_validate_pin_bus_handle(bus);

    STD_ASSERT(polarity != NULL);

    error = std_mutex_lock (&(bus->lock));
    if (error != STD_ERR_OK) {
        return error;
    }

    error = bus->ops->sdi_pin_bus_get_polarity(bus, polarity);

    std_mutex_unlock (&(bus->lock));

    return error;

}
