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
 * filename: sdi_i2c_bus_api.c
 */


/******************************************************************************
 * Implements SDI I2C BUS APIs to execute I2C transactions
 *****************************************************************************/

#include <linux/i2c.h>
#include "std_assert.h"
#include "sdi_i2c_bus_api.h"

/* note on SMBUS Command Format specified in Format: comment before every
 * sdi_smbus* function:
 * All SDI I2C SMBUS APIs format explained follows these conventions:
 * - Transaction from initiator (master of I2C BUS) are denoted in
 * all small letters
 * - Transaction from responder (I2C slave) are denoted in all
 * CAPITAL letters
 * - Size of Every Transaction is marked inside parenthesis
 */

/**
 * sdi_smbus_recv_byte
 * Execute SMBUS Receive Byte from Slave.
 * Format:
 * start (1) : slave Address (7) : rd (1) : ACK (1) : DATABYTE (8) : ack(1) :
 * stop (1)
 */
t_std_error sdi_smbus_recv_byte(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint8_t *buffer,
                                uint_t flags)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    STD_ASSERT(buffer != NULL);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_READ, SDI_SMBUS_BYTE,
                                SDI_SMBUS_OFFSET_NON_CMD, buffer,
                              SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;

}

/**
 * sdi_smbus_send_byte
 * Execute SMBUS Send Byte to Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : databyte (8) : ACK(1) :
 * stop (1)
 */
t_std_error sdi_smbus_send_byte(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint8_t buffer,
                                uint_t flags)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_WRITE, SDI_SMBUS_BYTE,
                              SDI_SMBUS_OFFSET_NON_CMD, &buffer,
                              SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;
}


/**
 * sdi_smbus_read_byte
 * Execute SMBUS Read Byte From Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * start (1) : slave address (7) : rd (1) : ACK (1) : DATABYTE (8) : ack(1) :
 * stop (1)
 */
t_std_error sdi_smbus_read_byte(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint_t cmd,
                                uint8_t *buffer, uint_t flags)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    STD_ASSERT(buffer != NULL);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_READ, SDI_SMBUS_BYTE_DATA,
                              cmd, buffer, SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;
}


/**
 * sdi_smbus_write_byte
 * Execute SMBUS Write Byte to Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * databyte (8) : ACK(1) : stop (1)
 */
t_std_error sdi_smbus_write_byte(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint_t cmd,
                                uint8_t buffer, uint_t flags)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_WRITE, SDI_SMBUS_BYTE_DATA,
                              cmd, &buffer, SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;
}

/**
 * sdi_i2c_read
 * Execute I2C Read From Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * start (1) : slave address (7) : rd (1) : ACK (1) : DATABYTE (8) : ack(1) :
 * stop (1)
 */
t_std_error sdi_i2c_read(sdi_i2c_bus_hdl_t bus_handle,
                         sdi_i2c_addr_t i2c_addr, const uint8_t *cmd, uint_t cmdlen,
                         uint8_t *buffer, uint_t buflen, uint_t flags)
{
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    if (cmdlen != 0) {
        STD_ASSERT(cmd != NULL);
    }
    STD_ASSERT(buffer != NULL);

    rc = sdi_i2c_acquire_bus(bus_handle);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_i2c_bus_execute(bus_handle, i2c_addr, SDI_I2C_READ,
                             cmd, cmdlen, buffer, buflen, flags);

    sdi_i2c_release_bus(bus_handle);

    return rc;
}


/**
 * sdi_i2c_write
 * Execute I2C Write to Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * databyte (8) : ACK(1) : stop (1)
 */
t_std_error sdi_i2c_write(sdi_i2c_bus_hdl_t bus_handle,
                          sdi_i2c_addr_t i2c_addr, const uint8_t *cmd, uint_t cmdlen,
                          uint8_t buffer, uint_t buflen, uint_t flags)
{
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    if (cmdlen != 0) {
        STD_ASSERT(cmd != NULL);
    }
    rc = sdi_i2c_acquire_bus(bus_handle);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_i2c_bus_execute(bus_handle, i2c_addr, SDI_I2C_WRITE,
                             cmd, cmdlen, &buffer, buflen, flags);

    sdi_i2c_release_bus(bus_handle);

    return rc;
}

/**
 * sdi_smbus_read_multi_byte
 * Execute SMBUS Read multiple bytes From Slave one after another.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * start (1) : slave address (7) : rd (1) : ACK (1) : DATABYTE (8) : ack(1) :
 * stop (1)
 */
t_std_error sdi_smbus_read_multi_byte(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint_t cmd,
                                uint8_t *buffer, uint_t byte_count, uint_t flags)
{
    uint_t count = 0;
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    STD_ASSERT(buffer != NULL);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    for(count = 0; (count < byte_count); count++)
    {
        error = sdi_smbus_execute(bus_handle, i2c_addr,
                                  SDI_SMBUS_READ, SDI_SMBUS_BYTE_DATA,
                                  (cmd+count), (buffer+count),
                                  SDI_SMBUS_SIZE_NON_BLOCK, flags);

        if (error != STD_ERR_OK)
        {
            sdi_i2c_release_bus(bus_handle);
            return error;
        }
    }

    sdi_i2c_release_bus(bus_handle);

    return error;
}


/**
 * sdi_smbus_write_multi_byte
 * Execute SMBUS Write multiple bytes to Slave one after another.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * databyte (8) : ACK(1) : stop (1)
 */
t_std_error sdi_smbus_write_multi_byte(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint_t cmd,
                                uint8_t *buffer, uint_t byte_count, uint_t flags)
{
    uint_t count = 0;
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    STD_ASSERT(buffer != NULL);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    for(count = 0; (count < byte_count); count++)
    {

        error = sdi_smbus_execute(bus_handle, i2c_addr,
                                  SDI_SMBUS_WRITE, SDI_SMBUS_BYTE_DATA,
                                  (cmd+count), (buffer+count),
                                  SDI_SMBUS_SIZE_NON_BLOCK, flags);
        if (error != STD_ERR_OK)
        {
            sdi_i2c_release_bus(bus_handle);
            return error;
        }
    }

    sdi_i2c_release_bus(bus_handle);

    return error;
}

/**
 * sdi_smbus_read_word
 * Execute SMBUS Read Word From Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * start (1) : slave address (7) : rd (1) : ACK (1) : DATABYTELOW (8) : ack(1) :
 * DATABYTEHIGH (8) : ack (1) : stop (1)
 */
t_std_error sdi_smbus_read_word(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint_t cmd,
                                uint16_t *buffer, uint_t flags)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    STD_ASSERT(buffer != NULL);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_READ, SDI_SMBUS_WORD_DATA,
                              cmd, buffer, SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;
}


/**
 * sdi_smbus_write_word
 * Execute SMBUS Write Word to Slave.
 * Format:
 * start (1) : slave address (7) : wr (1) : ACK (1) : cmd (8) : ACK(1) :
 * databytelow (8) : ACK(1) : databytehigh (8) : ACK (1) : stop (1)
 */
t_std_error sdi_smbus_write_word(sdi_i2c_bus_hdl_t bus_handle,
                                sdi_i2c_addr_t i2c_addr, uint_t cmd,
                                uint16_t buffer, uint_t flags)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_WRITE, SDI_SMBUS_WORD_DATA,
                              cmd, &buffer, SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;
}

t_std_error sdi_smbus_write_i2c_block_data(sdi_i2c_bus_hdl_t bus_handle,
                                           sdi_i2c_addr_t i2c_addr, uint16_t cmd,
                                           uint8_t length, const uint8_t *values, uint_t flags)
{
    union i2c_smbus_data data;
    int i;
    t_std_error error = STD_ERR_OK;
    STD_ASSERT(bus_handle != NULL);

    STD_ASSERT(bus_handle->bus.bus_type == SDI_I2C_BUS);

    error = sdi_i2c_acquire_bus(bus_handle);
    if (error != STD_ERR_OK) {
        return error;
    }

    if (length > (I2C_SMBUS_BLOCK_MAX / 2))
        length = I2C_SMBUS_BLOCK_MAX / 2;
    for (i = 0; i < length; i++)
        data.block[i + 2] = values[i];
    data.block[0] = length + 1;      // 1 extra byte for cmd
    data.block[1] = ((cmd >> 8) & 0xff);
    error = sdi_smbus_execute(bus_handle, i2c_addr,
                              SDI_SMBUS_WRITE, SDI_SMBUS_BLOCK_DATA,
                              (cmd & 0xff), &data, SDI_SMBUS_SIZE_NON_BLOCK, flags);

    sdi_i2c_release_bus(bus_handle);

    return error;
}
