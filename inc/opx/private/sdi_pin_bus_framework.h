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
 * filename: sdi_pin_bus_framework.h
 */


/******************************************************************************
 * @file sdi_pin_bus_framework.h
 * @brief Defines SDI PIN Bus Framework APIs
 *****************************************************************************/

#ifndef __SDI_PIN_BUS_FRAMEWORK_H___
#define __SDI_PIN_BUS_FRAMEWORK_H___

#include "sdi_bus_framework.h"
#include "sdi_pin.h"

/**
 * @defgroup sdi_internal_pin_bus_framework_apis SDI Internal Pin Bus Framework APIs
 * @brief SDI PIN Bus Framework api
 * This framework provides APIs to
 * - pin producers to register a pin. GPIO, CPLD are examples of pin producers.
 * - get pin bus handle given pin bus name or pin bus identifier
 * These APIs are built on top of @ref sdi_internal_bus_framework apis.
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @brief sdi_pin_bus_register
 * Every SDI PIN Bus device driver registers its bus handle with this framework
 * using this API
 * @param[in] bus_hdl Pin bus handle
 * @return STD_ERR_OK on success
 */
static inline t_std_error sdi_pin_bus_register(sdi_pin_bus_hdl_t bus_hdl)
{
    return sdi_bus_register((sdi_bus_hdl_t)bus_hdl);
}

/**
 * @brief sdi_get_pin_bus_handle_by_name
 * Get Pin Bus Handle given its Bus Name specified during pin bus registration
 * @param[in] bus_name Pin Bus Name for which pin bus handle needs to be fetched
 * @return valid sdi pin bus handle on success, NULL on failure
 */
static inline sdi_pin_bus_hdl_t sdi_get_pin_bus_handle_by_name(
    const char *bus_name)
{
    return (sdi_pin_bus_hdl_t )
            sdi_bus_get_handle_by_name(bus_name, SDI_PIN_BUS);
}

/**
 * @brief sdi_get_pin_bus_handle_by_id
 * Get Pin Bus Handle given its Bus Id specified during pin bus registration
 * @param[in] bus_id Pin Bus Id for which pin bus handle needs to be fetched
 * @return valid sdi pin bus handle on success, NULL on failure
 */
static inline sdi_pin_bus_hdl_t sdi_get_pin_bus_handle_by_id(
    sdi_bus_id_t bus_id)
{
    return (sdi_pin_bus_hdl_t ) sdi_bus_get_handle_by_id(bus_id, SDI_PIN_BUS);
}

/**
 * @}
 */

#endif /* __SDI_PIN_BUS_FRAMEWORK_H___ */
