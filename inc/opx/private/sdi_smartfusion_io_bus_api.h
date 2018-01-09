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
 * filename: sdi_smartfusion_io_bus_api.h
 */


/******************************************************************************
 * @file sdi_smartfusion_io_bus_api.h
 * @brief Defines SDI SmartFusion IO Bus APIs
 *****************************************************************************/

#ifndef __SDI_SMARTFUSION_IO_BUS_API_H__
#define __SDI_SMARTFUSION_IO_BUS_API_H__

#include "sdi_bus_framework.h"
#include "sdi_pin.h"
#include "sdi_smartfusion_io.h"

/**
 * @defgroup sdi_internal_sf_io_bus_api SDI Internal SmartFusion IO Bus API
 * @brief SDI SmartFusion IO Bus APIs
 * Implements APIs to read the status of SmartFusion IO Bus
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @brief sdi_sf_io_acquire_bus
 * Acquire SmartFusion IO Bus lock
 * @param[in] bus sdi smartfusion io bus object
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_sf_io_acquire_bus(sdi_sf_io_bus_hdl_t bus);

/**
 * @brief sdi_sf_io_release_bus
 * Release acquired SmartFusion IO Bus lock
 * @param[in] bus sdi smartfusion io bus object
 * @return none
 */
void sdi_sf_io_release_bus(sdi_sf_io_bus_hdl_t bus);

/**
 * @brief sdi_sf_io_bus_read_byte
 * Read Byte from SmartFusion IO Bus
 * @param[in] bus sdi smartfusion io bus object
 * @param[in] addr sdi smartfusion io bus address
 * @param[out] value data pointer to store byte read from BUS
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_sf_io_bus_read_byte(sdi_sf_io_bus_hdl_t bus, uint_t addr, uint8_t *value);

/**
 * @brief sdi_sf_io_bus_write_byte
 * Write given byte data to SmartFusion IO Bus
 * @param[in] bus sdi smartfusion io bus object
 * @param[in] addr sdi smartfusion io bus address
 * @param[out] value data to be written to smartfusion io bus
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_sf_io_bus_write_byte(sdi_sf_io_bus_hdl_t bus, uint_t addr, uint8_t value);

/**
 * @}
 */

#endif /* __SDI_SMARTFUSION_IO_BUS_API_H__ */
