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
 * filename: sdi_bus_api.c
 */


/******************************************************************************
 * Implements SDI BUS Read/Write APIs
 * Note: Only byte read/byte write for I2C/IO Bus are implemented.
 * TODO: When the need arises, Add support for word/4byte and support for other
 * buses.
 *****************************************************************************/

#include "sdi_bus_api.h"
#include "sdi_i2c_bus_api.h"
#include "sdi_io_port_api.h"
#include "sdi_smartfusion_io_bus_api.h"

/*
 * Read a byte of data from a specified offset of a device using bus api.
 * param[in] bus_hdl - Bus handle on which device data to be read is attached.
 * param[in] addr - Device address
 * param[in] offset - Offset within device
 * param[out] buffer - Data read from device is populated in this buffer.
 * return STD_ERR_OK on success, SDI_ERRCODE(ENOTSUP) if unsupported or
 * STD failure code on error.
 */
t_std_error sdi_bus_read_byte(sdi_bus_hdl_t bus_hdl, sdi_device_addr_t addr,
                                            uint_t offset, uint8_t *buffer)
{
    t_std_error rc = STD_ERR_OK;

    switch(bus_hdl->bus_type) {
        case SDI_I2C_BUS:
             rc = sdi_smbus_read_byte((sdi_i2c_bus_hdl_t)bus_hdl,
                                      addr.i2c_addr, offset, buffer,
                                      SDI_I2C_FLAG_NONE);
             if (rc != STD_ERR_OK) {
                 SDI_ERRMSG_LOG("%s:%d Read byte of %s Addr 0x%x Offset 0x%x "
                                "failed with error %d", __FUNCTION__, __LINE__,
                                 bus_hdl->bus_name, addr.i2c_addr.i2c_addr, offset, rc);
             }
             break;
        case SDI_IO_BUS:
             rc = sdi_io_read_byte((sdi_bus_hdl_t)bus_hdl, (uint16_t)offset, buffer);
             if (rc != STD_ERR_OK) {
                 SDI_ERRMSG_LOG("%s:%d Read byte of %s Type %d Port Addr 0x%x "
                                "failed with error %d", __FUNCTION__, __LINE__,
                                 bus_hdl->bus_name, bus_hdl->bus_type, offset, rc);
             }
             break;
        case SDI_SF_IO_BUS:
             rc = sdi_sf_io_bus_read_byte((sdi_sf_io_bus_hdl_t)bus_hdl, offset, buffer);
             if (rc != STD_ERR_OK) {
                 SDI_ERRMSG_LOG("%s:%d Read byte of %s Type %d Port Addr 0x%x "
                                "failed with error %d", __FUNCTION__, __LINE__,
                                 bus_hdl->bus_name, bus_hdl->bus_type, offset, rc);
             }
             break;
        default:
             SDI_ERRMSG_LOG("%s:%d unsupported operation %s(type %d) ", __FUNCTION__,
                             __LINE__, bus_hdl->bus_name, bus_hdl->bus_type);
             return SDI_ERRCODE(ENOTSUP);
    }
    return rc;
}

/*
 * Write a byte of data to a specified offset of a device using bus api.
 * param[in] bus_hdl - Bus handle on which device data to be written is attached.
 * param[in] addr - Device address
 * param[in] offset - Offset within device
 * param[in] buffer - Data to be written to device offset address
 * return STD_ERR_OK on success, SDI_ERRCODE(ENOTSUP) if unsupported or
 * STD failure code on error.
 */
t_std_error sdi_bus_write_byte(sdi_bus_hdl_t bus_hdl, sdi_device_addr_t addr,
                                             uint_t offset, uint8_t buffer)
{
    t_std_error rc = STD_ERR_OK;

    switch(bus_hdl->bus_type) {
        case SDI_I2C_BUS:
             rc = sdi_smbus_write_byte((sdi_i2c_bus_hdl_t)bus_hdl,
                                       addr.i2c_addr, offset, buffer,
                                       SDI_I2C_FLAG_NONE);
             if (rc != STD_ERR_OK) {
                 SDI_ERRMSG_LOG("%s:%d Write byte of %s Addr 0x%x Offset 0x%x val"
                                " %d failed with error %d", __FUNCTION__, __LINE__,
                                bus_hdl->bus_name, addr.i2c_addr.i2c_addr, offset, buffer, rc);
             }
             break;
        case SDI_IO_BUS:
             rc = sdi_io_write_byte((sdi_bus_hdl_t)bus_hdl, (uint16_t)offset, buffer);
             if (rc != STD_ERR_OK) {
                 SDI_ERRMSG_LOG("%s:%d Write byte of %s Type %d Port Addr 0x%x val"
                                " %d failed with error %d", __FUNCTION__, __LINE__,
                                 bus_hdl->bus_name, bus_hdl->bus_type, offset, rc);
             }
             break;
        case SDI_SF_IO_BUS:
             rc = sdi_sf_io_bus_write_byte((sdi_sf_io_bus_hdl_t)bus_hdl, offset, buffer);
             if (rc != STD_ERR_OK) {
                 SDI_ERRMSG_LOG("%s:%d Write byte of %s Type %d Port Addr 0x%x val"
                                " %d failed with error %d", __FUNCTION__, __LINE__,
                                 bus_hdl->bus_name, bus_hdl->bus_type, offset, rc);
             }
             break;
        default:
             SDI_ERRMSG_LOG("%s:%d unsupported operation %s(type %d) ", __FUNCTION__,
                             __LINE__, bus_hdl->bus_name, bus_hdl->bus_type);
             return SDI_ERRCODE(ENOTSUP);
    }
    return rc;
}
