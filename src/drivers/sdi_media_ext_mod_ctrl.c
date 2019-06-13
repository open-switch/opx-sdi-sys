/*
 * Copyright (c) 2019 Dell Inc.
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
 * filename: sdi_media_ext_mod_ctrl.c
 */


/******************************************************************************
 * sdi_media_ext_mod_ctrl.c
 * Implements the R/W APIs for using media ext mod control, which is based on mailbox registers 
 *****************************************************************************/

#include "sdi_media_ext_mod_ctrl.h"
#include "sdi_media.h"


#define    SDI_EXT_MEM_MODULE_ACTIVE_DEV_MEM_ADDR_OFFSET   250
#define    SDI_EXT_MEM_MODULE_REG_OFFSET_ADDR_MSB_OFFSET   251
#define    SDI_EXT_MEM_MODULE_REG_OFFSET_ADDR_LSB_OFFSET   252
#define    SDI_EXT_MEM_MODULE_DATA_MSB_OFFSET              253
#define    SDI_EXT_MEM_MODULE_DATA_LSB_OFFSET              254
#define    SDI_EXT_MEM_MODULE_CTRL_STATUS_OFFSET           255

static t_std_error _sdi_sfp_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_i2c_addr_t address;

    int device_addr = addr->device_addr;
    uint_t offset = addr->offset;

    STD_ASSERT(data != NULL);
    STD_ASSERT(data_len > 0);
    STD_ASSERT(resource_hdl != NULL);


    sfp_device = (sdi_device_hdl_t)resource_hdl;
    address = sfp_device->addr.i2c_addr;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (device_addr > SDI_MEDIA_DEVICE_ADDR_AUTO) {
        address.i2c_addr = device_addr;
    } else if ( device_addr < SDI_MEDIA_DEVICE_ADDR_AUTO) {
        SDI_DEVICE_ERRMSG_LOG("Invalid media device address value: %d ", device_addr);
        return SDI_DEVICE_ERRCODE(EINVAL);
    }

    rc = sdi_smbus_read_multi_byte(sfp_device->bus_hdl, address,
           offset, data, data_len, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d reg : %d"
                              "rc : %d", address, offset, rc);
    }

    return rc;
}

static t_std_error _sdi_sfp_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_i2c_addr_t address;

    int device_addr = addr->device_addr;
    uint_t offset = addr->offset;

    STD_ASSERT(data != NULL);
    STD_ASSERT(data_len > 0);
    STD_ASSERT(resource_hdl != NULL);


    sfp_device = (sdi_device_hdl_t)resource_hdl;
    address = sfp_device->addr.i2c_addr;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (device_addr > SDI_MEDIA_DEVICE_ADDR_AUTO) {
        address.i2c_addr = device_addr;
    } else if ( device_addr < SDI_MEDIA_DEVICE_ADDR_AUTO) {
        SDI_DEVICE_ERRMSG_LOG("Invalid media device address value: %d ", device_addr);
        return SDI_DEVICE_ERRCODE(EINVAL);
    }

    rc = sdi_smbus_write_multi_byte(sfp_device->bus_hdl, address,
           offset, data, data_len, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus write failed at addr : %d reg : %d"
                              "rc : %d", address, offset, rc);
    }

    return rc;
}

/* This performs a read or write using a set of mailbox registers; based on the following memory structure */
/*              250 |   MMD PHY Address  |  */
/*              251 |   Reg Offset MSB   |  */
/*              252 |   Reg Offset LSB   |  */
/*              253 |   16 Bit Data MSB  |  */
/*              254 |   16 Bit Data LSB  |  */
/*              255 |   Ctrl/Status Bits |  {Bit[1:0] Oper: 
                                                Read: 1, Write: 2},
                                            {Bit[3:2] Status:
                                                Idle: 0, Done: 1, Error: 2} */
/*  A write is done by setting the desired registers, adding the data to be written, and then setting the write flag.
    Afterwards, the status is polled every STATUS_POLL_INTERVAL_MS, either until the 'Done' flag is set or STATUS_POLL_MAX_TIMEOUT_MS is elapsed
    A read is similar, except that the data field is initially unset, until after the 'Done' flag indicates that data is ready to be read */

static t_std_error perform_oper (sdi_resource_hdl_t resource_hdl, ext_mem_addr_t mem_addr,
                uint16_t reg_offset, uint16_t* data_buf_be, ext_mem_oper_t oper){

    bool oper_set_success            = false;
    uint8_t set_buf[EXT_MEM_SIZE]    = {0};
    uint8_t status                   = 0;
    uint8_t buf_read[DATA_BUF_SIZE]  = {0};
    t_std_error rc                   = STD_ERR_OK;
    sdi_device_hdl_t device          = NULL;
    uint_t status_poll_timeout       = 0;

    sdi_media_eeprom_addr_t addr = {
                                    .device_addr = SDI_MEDIA_DEVICE_ADDR_A2,
                                    .page        = SDI_MEDIA_PAGE_SELECT_NOT_SUPPORTED,
                                    .offset      = SDI_EXT_MEM_MODULE_ACTIVE_DEV_MEM_ADDR_OFFSET
                                   };

    t_std_error (*read_generic)(sdi_resource_hdl_t, sdi_media_eeprom_addr_t *, uint8_t *, size_t) = NULL;
    t_std_error (*write_generic)(sdi_resource_hdl_t, sdi_media_eeprom_addr_t *, uint8_t *, size_t) = NULL;


    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(data_buf_be != NULL);

    device = (sdi_device_hdl_t)resource_hdl;

    /* Will be extended when adding QSFPx support */
    read_generic = &_sdi_sfp_read_generic;
    write_generic = &_sdi_sfp_write_generic;

    /* Setup the buffer to be sent */
    set_buf[0] = mem_addr;
    set_buf[1] = (uint8_t)(reg_offset >> 8);
    set_buf[2] = (uint8_t)(reg_offset);

    /* Only set the data portion if this is a write oper */
    if (oper == MEM_WRITE){
        set_buf[3] = (uint8_t)((*data_buf_be) >> 8);
        set_buf[4] = (uint8_t)(*data_buf_be);
    }

    /* Setup the operation (READ or WRITE) byte */
    set_buf[5] = (uint8_t)(OPER_MASK & oper);

    /* Set bytes that indicate the operation to be performed */
    rc = write_generic(resource_hdl, &addr, set_buf, sizeof(set_buf)/sizeof(set_buf[0]));
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Write error on module %s when using ext mod ctrl", device->alias);
        return rc;
    }

    addr.offset = SDI_EXT_MEM_MODULE_CTRL_STATUS_OFFSET;
    /* Now poll the status bits to see if write oper completed  */
    while (status_poll_timeout < STATUS_POLL_MAX_TIMEOUT_MS){
        /* Read the status */
        rc = read_generic(resource_hdl, &addr, &status, 1);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("Read error %u on module %s when using ext mod ctrl", rc, device->alias);
            return rc;
        }

        switch ((status & STATUS_OPER_COMPLETE_BITMASK) >> 2){
            /* Idle */
            case STATUS_POLL_IDLE_STATE:
                oper_set_success = false;
                break;
            /* Done */
            case STATUS_POLL_COMPLETE_STATE:
                oper_set_success = true;
                break;
            /* Error */
            case STATUS_POLL_ERROR_STATE:
            default:
                oper_set_success = false;
                SDI_DEVICE_ERRMSG_LOG("Error state on module %s when using ext mod ctrl", device->alias);
                break;
        }
        if (oper_set_success){
            break;
        }
        std_usleep(STATUS_POLL_INTERVAL_MS);
        status_poll_timeout += STATUS_POLL_INTERVAL_MS;
    }
    if (!oper_set_success){
        SDI_DEVICE_ERRMSG_LOG("Write did not complete on module %s when using ext mod ctrl", device->alias);
        return rc;
    }

    /* All done */
    if (oper == MEM_WRITE){
        return rc;
    }

    /* Reg setup complete. Now poll data bytes */
    addr.offset = SDI_EXT_MEM_MODULE_DATA_MSB_OFFSET;
    rc = read_generic(resource_hdl, &addr, buf_read, sizeof(buf_read)/sizeof(buf_read[0]));
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Read error on module %s when using ext mem io", device->alias);
        return rc;
    }
    /* Return as uint16_t,  two bytes as big endian */
    *data_buf_be = (buf_read[0] << 8) | (buf_read[1]);
    return rc;
}


/* Typically, multiple operations are needed to perform a certain task. This API allows one perform a chain of R/W operations */
t_std_error sdi_media_ext_mod_ctrl_chain_oper (sdi_resource_hdl_t resource_hdl, ext_dev_ctrl_oper_packet_t* packets, size_t num_packets){

    sdi_device_hdl_t device = NULL;
    t_std_error rc          = STD_ERR_OK;
    size_t count            = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(packets != NULL);

    device = (sdi_device_hdl_t)resource_hdl;

    for (count = 0; count < num_packets; count++){

        rc = perform_oper (resource_hdl, packets[count].mem_addr, packets[count].reg_offset, &(packets[count].data_buf_big_endian),
                            packets[count].oper);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("Ext mod ctrl oper failure (rc: %u) at packet %d on module %s", rc, count, device->alias);
            break;
        }
    }

    return rc;
}

