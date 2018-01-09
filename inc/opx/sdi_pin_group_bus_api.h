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
 * filename: sdi_pin_group_bus_api.h
 */


/******************************************************************************
 * @file sdi_pin_group_bus_api.h
 * @brief Defines SDI PIN GROUP Bus APIs exposed to SDI Drivers to operate on
 * Pin Group. These APIs hide the details of pin group.
 *****************************************************************************/

#ifndef __SDI_PIN_GROUP_BUS_API_H___
#define __SDI_PIN_GROUP_BUS_API_H___

#include "sdi_bus_framework.h"
#include "sdi_pin_group.h"

/**
 * @defgroup sdi_internal_pin_group_bus_api SDI Internal Pin Group Bus APIs
 * @ingroup sdi_internal_bus
 * @brief Implements APIs to read the status of pin group, modify the pin group
 * status, change the direction of pin group, invert the pin group logic level
 * (polarity)
 *
 *
 * @{
 */

/**
 * @brief Wrapper for std_mutex_lock for acquiring pin group bus
 * @param[in] bus_hdl - pin group bus handle
 * @return t_std_error
 */
t_std_error sdi_pin_group_acquire_bus(sdi_pin_group_bus_hdl_t bus_hdl);

/**
 * @brief  Wrapper for std_mutex_unlock for releasing bus
 * @param[in] bus_hdl - pin group bus handle
 */
void sdi_pin_group_release_bus(sdi_pin_group_bus_hdl_t bus_hdl);

/**
 * @brief sdi_pin_group_read_level
 * Read value of the Pin Group
 * @param[in] bus_hdl sdi pin group bus object
 * @param[out] value pointer to be set with configured pin group level
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_group_read_level(sdi_pin_group_bus_hdl_t bus_hdl,
                                     uint_t *value);


/**
 * @brief sdi_pin_group_write_level
 * Write value of the Pin Group
 * @param[in] bus_hdl sdi pin group bus object
 * @param[in] value pin group level to be set
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_group_write_level(sdi_pin_group_bus_hdl_t bus_hdl,
                                      uint_t value);


/**
 * @brief sdi_pin_group_set_direction
 * Configure the direction of pin Group
 * @param[in] bus_hdl sdi pin group bus object
 * @param[in] direction input/output direction to be configured
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_group_set_direction(sdi_pin_group_bus_hdl_t bus_hdl,
                                        sdi_pin_bus_direction_t direction);


/**
 * @brief sdi_pin_group_get_direction
 * Get the current configured direction of the pin group
 * @param[in] bus_hdl sdi pin group bus object
 * @param[out] direction pointer to be set with configured pin group direction
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_group_get_direction(sdi_pin_group_bus_hdl_t bus_hdl,
                                        sdi_pin_bus_direction_t *direction);


/**
 * @brief sdi_pin_group_set_polarity
 * Set the polairty of the pin group
 * @param[in] bus_hdl sdi pin group bus object
 * @param[in] polarity normal or inverted polarity to be set
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_group_set_polarity(sdi_pin_group_bus_hdl_t bus_hdl,
                                       sdi_pin_bus_polarity_t polarity);


/**
 * @brief sdi_pin_group_get_polarity
 * Get the current configured polarity of the pin group
 * @param[in] bus_hdl sdi pin group bus object
 * @param[out] polarity pointer to be set with configured pin group polarity
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_pin_group_get_polarity(sdi_pin_group_bus_hdl_t bus_hdl,
                                       sdi_pin_bus_polarity_t *polarity);

/**
 * @}
 */

#endif /* __SDI_PIN_GROUP_BUS_API_H___ */
