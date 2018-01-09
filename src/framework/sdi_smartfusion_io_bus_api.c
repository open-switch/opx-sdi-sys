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
 * filename: sdi_smartfusion_io_bus_api.c
 */


/******************************************************************************
 * Defines SDI SmartFusion IO Bus APIs
 *****************************************************************************/

#include "sdi_smartfusion_io_bus_api.h"
#include "std_assert.h"

/**
 * sdi_sf_io_acquire_bus
 * Acquire SmartFusion IO Bus lock
 */
t_std_error sdi_sf_io_acquire_bus(sdi_sf_io_bus_hdl_t bus_handle)
{
    STD_ASSERT(bus_handle != NULL);
    return (std_mutex_lock (&(bus_handle->lock)));
}

/**
 * sdi_sf_io_release_bus
 * Release acquired SmartFusion IO Bus lock
 */
void sdi_sf_io_release_bus(sdi_sf_io_bus_hdl_t bus_handle)
{
    STD_ASSERT(bus_handle != NULL);
    std_mutex_unlock (&(bus_handle->lock));
}

/**
 * sdi_sf_io_bus_read_byte
 * Read Byte from SmartFusion IO Bus
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_sf_io_bus_read_byte(sdi_sf_io_bus_hdl_t bus_handle, uint_t addr, uint8_t *value)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(value != NULL);

    error = bus_handle->ops->sdi_sf_io_bus_read_byte(bus_handle, addr, value);

    return error;
}

/**
 * sdi_sf_io_bus_write_byte
 * Write given byte data to SmartFusion IO Bus
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_sf_io_bus_write_byte(sdi_sf_io_bus_hdl_t bus_handle, uint_t addr, uint8_t value)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    error = bus_handle->ops->sdi_sf_io_bus_write_byte(bus_handle, addr, value);

    return error;
}
