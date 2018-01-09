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
 * filename: sdi_i2c_bus_framework.h
 */


/******************************************************************************
 * @file sdi_i2c_bus_framework.h
 * @brief Defines SDI I2C Bus Framework APIs
 *****************************************************************************/

#ifndef __SDI_I2C_BUS_FRAMEWORK_H___
#define __SDI_I2C_BUS_FRAMEWORK_H___

#include "sdi_bus_framework.h"
#include "sdi_i2c.h"

/**
 * @defgroup sdi_internal_i2c_bus_framework_api SDI Internal I2C Bus Framework API
 * @brief SDI I2C Bus Framework APIs
 * This framework implements APIs to
 * - Register i2c bus handle with bus framework, used by i2c bus driver
 * - Get i2c bus handle given i2c bus-id or i2c bus name
 * This framework internally uses @ref sdi_internal_bus_framework apis.
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @brief sdi_i2c_bus_register
 * Every SDI I2C Bus device driver registers its bus handle with bus framework
 * using this API
 * @param[in] bus_hdl I2C Bus Handle getting registed with bus framework
 * @return STD_ERR_OK on success
 */
static inline t_std_error sdi_i2c_bus_register(sdi_i2c_bus_hdl_t bus_hdl)
{
    return sdi_bus_register((sdi_bus_hdl_t)bus_hdl);
}

/**
 * @brief sdi_get_i2c_bus_handle_by_name
 * Get I2C Bus Handle given its bus name specified during bus registration
 * @param[in] bus_name I2C Bus Name
 * @return valid i2c bus handle on success, NULL on failure
 */
static inline sdi_i2c_bus_hdl_t sdi_get_i2c_bus_handle_by_name(
        const char *bus_name)
{
    return (sdi_i2c_bus_hdl_t) sdi_bus_get_handle_by_name(bus_name, SDI_I2C_BUS);
}

/**
 * @brief sdi_get_i2c_bus_handle_by_id
 * Get I2C Bus Handle given its unique bus id specified during bus registration
 * @param[in] bus_id Unique I2C Bus Identifier
 * @return valid i2c bus handle on success, NULL on failure
 */
static inline sdi_i2c_bus_hdl_t sdi_get_i2c_bus_handle_by_id(sdi_bus_id_t bus_id)
{
    return (sdi_i2c_bus_hdl_t) sdi_bus_get_handle_by_id(bus_id, SDI_I2C_BUS);
}

/**
 * @}
 */
#endif /* __SDI_I2C_BUS_FRAMEWORK_H___ */
