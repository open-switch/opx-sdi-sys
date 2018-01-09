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
 * filename: sdi_io_port_api.c
 */


/******************************************************************************
 * Implements SDI IO Read/Write APIs
 * Note: Only byte read/byte write for IO is implemented.
 * TODO: When the need arises, Add support for word/4byte and support for other.
 *****************************************************************************/

#include "sdi_bus_api.h"
#include "sdi_io_port_api.h"
#include "std_assert.h"

/*
 * Read a byte of data from a specified address.
 * param[in] bus_hdl - Bus handle on which port data to be read is attached.
 * param[in] addr    - Port address
 * param[out] buffer - Data read from port is populated in this buffer.
 * return STD_ERR_OK for success and the respective error code in case of failure.
 */
t_std_error sdi_io_read_byte(sdi_bus_hdl_t bus_hdl, uint16_t addr, uint8_t *buffer)
{
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(bus_hdl->bus_type == SDI_IO_BUS);
    STD_ASSERT(buffer != NULL);

    /**
     * @todo: needs to set the permission as part of device init
     */
    sdi_io_port_read_byte(addr, buffer);

    return STD_ERR_OK;
}

/*
 * Write a byte of data to a specified address.
 * param[in] bus_hdl - Bus handle on which port data to be written is attached.
 * param[in] addr    - Port address
 * param[in] buffer  - Data to be written to port offset address
 * return STD_ERR_OK for success and the respective error code in case of failure.
 */
t_std_error sdi_io_write_byte(sdi_bus_hdl_t bus_hdl, uint16_t addr, uint8_t buffer)
{
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(bus_hdl->bus_type == SDI_IO_BUS);

    /**
     * @todo: needs to set the permission as part of device init
     */
    sdi_io_port_write_byte(addr, buffer);

    return STD_ERR_OK;
}
