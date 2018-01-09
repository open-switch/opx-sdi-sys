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
 * filename: sdi_pin_bus_api.h
 */


/******************************************************************************
 * @file sdi_pin_bus_api.h
 * @brief Defines SDI PIN Bus APIs exposed to SDI Drivers to operate on pins.
 * These APIs hide the details of pin.
 *****************************************************************************/

#ifndef __SDI_PIN_BUS_API_H___
#define __SDI_PIN_BUS_API_H___

#include "sdi_bus_framework.h"
#include "sdi_pin.h"

/**
 * @defgroup sdi_internal_pin_bus_api SDI Internal Pin Bus API
 * @brief SDI PIN Bus APIs
 * Implements APIs to read the status of pin, modify the pin status, change the
 * direction of pin, invert the pin logic level (polarity)
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @brief sdi_pin_read_level
 * Read value of the Pin
 * @param[in] bus sdi pin bus object
 * @param[out] value sdi_pin_bus_level_t data pointer to store pin value
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_read_level(sdi_pin_bus_hdl_t bus, sdi_pin_bus_level_t *value);

/**
 * @brief sdi_pin_write_level
 * Write value of the Pin
 * @param[in] bus sdi pin bus object
 * @param[in] value sdi_pin_bus_level_t value of pin to be written to
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_write_level(sdi_pin_bus_hdl_t bus, sdi_pin_bus_level_t value);

/**
 * @brief sdi_pin_set_direction
 * Configure the direction of pin
 * @param[in] bus sdi pin bus object
 * @param[in] direction configure given direction to the specified sdi pin
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_set_direction(sdi_pin_bus_hdl_t bus,
                                  sdi_pin_bus_direction_t direction);

/**
 * @brief sdi_pin_get_direction
 * Get the current configured direction of the pin
 * @param[in] bus sdi pin bus object
 * @param[out] value sdi_pin_bus_direction_t data pointer to read direction
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_get_direction(sdi_pin_bus_hdl_t bus,
                                  sdi_pin_bus_direction_t *value);

/**
 * @brief sdi_pin_set_polarity
 * Set the polairty of the pin
 * @param[in] bus sdi pin bus object
 * @param[in] polarity sdi_pin_bus_polarity_t normal or inverted
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_set_polarity(sdi_pin_bus_hdl_t bus,
                     sdi_pin_bus_polarity_t polarity);

/**
 * @brief sdi_pin_get_polarity
 * Get the current configured polarity of the pin
 * @param[in] bus sdi pin bus object
 * @param[out] polarity sdi_pin_bus_polarity_t pointer normal or inverted
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_get_polarity(sdi_pin_bus_hdl_t bus,
                                 sdi_pin_bus_polarity_t *polarity);

/**
 * @}
 */

#endif /* __SDI_PIN_BUS_API_H___ */
