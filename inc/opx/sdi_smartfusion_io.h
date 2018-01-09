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
 * filename: sdi_smartfusion_io.h
 */


/******************************************************************************
 * @file sdi_smartfusion_io.h
 * @brief Defines SDI Smartfusion IO Bus Data Structures and Bus Operator functions
 *****************************************************************************/

#ifndef __SDI_SMARTFUSION_IO_H___
#define __SDI_SMARTFUSION_IO_H___

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_bus.h"
#include "sdi_bus_framework.h"


/**
 * @defgroup sdi_internal_sf_io_bus_data_types SDI Internal SmartFusion IO Bus Data Type
 * @brief  SDI SmartFusion IO Bus Data Structure and Bus Operator functions
 * Smart Fusion FPGA is used as a Platform Controller in Dell's Z9100, S6100,
 * S4200 products. Smart Fusion is connected to Platform Devices(FANs, PSUs) and
 * Sensors(Temperature Sensors). Smart Fusion contains an ARM core that's
 * programmed to control these Platform Devices and gather sensor information,
 * thus offloading (partially) the Main CPU from directly interfacing with the
 * platform devices and sensors. Yet, main CPU needs to access SmatFusion FPGA
 * to control platform devices and gather sensor data.
 * In few cases where Smart Fusion can't read a particular temperature sensor
 * (because it resides inside a different chip, ex: NPU, TCAM), CPU needs to gather
 * the temperature sensor value and update the Smart Fusion so the SmartFusion
 * can do Advanced Fan control.
 * Smart Fusion is accessible from main CPU via LPC bus. Smart Fusion provides
 * mailbox mechanism for accessing platform components and its data. This same
 * design is expected to be re-used in future platforms as the same code could
 * be re-used with zero/minimal configuration changes.
 * Smart Fusion's Mail Box exposes 4 Registers via LPC bus.
 *        Mailbox RAM Address High Byte Register (RAM_ADDR_H),
 *        Mailbox RAM Address Low Byte Register (RAM_ADDR_L),
 *        Mailbox RAM Read Data Register (RAM_R_DATA),
 *        Mailbox RAM Write Data Register (RAM_W_DATA).
 * Platform devices/sensors that are controlled and monitored by SmartFusion are
 * mapped at offsets as specified in the Platform specification.
 * In order to read sensor data from CPU, below sequence needs to be followed:
 *        Write offset (high byte) to RAM_ADDR_H,
 *        Write offset (low byte) to RAM_ADDR_L,
 *        Read sensor data by reading value at RAM_R_DATA
 * In order to control platform sensor from CPU, below sequence needs to be followed:
 *        Write offset(high byte) to RAM_ADDR_H,
 *        Write offset(low byte) to RAM_ADDR_L,
 *        Write data to RAM_W_DATA.
 * Smart Fusion Design Doc provides details on offsets at which platform devices
 * are mapped.
 * Smart Fusion MailBox is represented as SDI BUS (sf_io_bus in below xml)
 * and the platform devices that are managed/monitored by Smart Fusion are
 * modelled as SDI DEVICES (ex: sf_temp_sensor in below xml)
 * Smart Fusion io bus driver will register as SDI_BUS and expose read byte and
 * write byte apis @ref sdi_sf_io_bus_framework_apis. Read/Write api's acquire
 * bus lock before Read/Write sequence and release bus lock after Read/Write
 * sequence. Devices attached under SmartFusion will use the Smart Fusion Bus
 * exposed read/write APIs to read/write sensors.
 *
 * <sf_io_bus ram_addr_high=0x210 ram_addr_low=0x211 ram_read_data=0x212 ram_write_data=0x213>
 *     <sf_temp_sensor instance=0 temp_offset=0x15 temp_low_threshold_offset=0x43
 *                                temp_high_threshold_offset=0x41
 *                                  temp_sw_shutdown_limit_offset=0x3f
 *                                  temp_hw_shutdown_limit_offset=0x3d
 *                                  temp_status_offset=0xdc/>
 * </sf_io_bus>
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @typedef sdi_sf_io_bus_t
 * SmartFusion IO BUS type
 */
typedef struct sdi_sf_io_bus sdi_sf_io_bus_t;

/**
 * @typedef sdi_sf_io_bus_hdl_t
 * SmartFusion IO BUS Handle
 */
typedef sdi_sf_io_bus_t *sdi_sf_io_bus_hdl_t;

/**
 * @struct sdi_sf_io_bus_ops
 * SDI SmartFusion Bus Operations
 */
typedef struct sdi_sf_io_bus_ops {
    /**
     * @brief sdi_sf_io_bus_read_byte
     * Read a byte from SmartFusion IO BUS Address
     */
    t_std_error (*sdi_sf_io_bus_read_byte) (sdi_sf_io_bus_hdl_t bus_handle,
                                            uint_t addr, uint8_t *value);
    /**
     * @brief sdi_sf_io_bus_write_byte
     * Write given byte to SmartFusion IO BUS Address
     */
    t_std_error (*sdi_sf_io_bus_write_byte) (sdi_sf_io_bus_hdl_t bus_handle,
                                             uint_t addr, uint8_t value);
} sdi_sf_io_bus_ops_t;

/**
 * @struct sdi_sf_io_bus
 * SDI SmartFusion Bus Structure
 */
typedef struct sdi_sf_io_bus {
    /**
     * @brief bus
     * SDI BUS object, to store bus_type, bus_id and bus_name
     */
    sdi_bus_t bus;
    /**
     * @brief ops
     * SDI PIN Bus Operations to read/write status, change/get direction,
     * polarity.
     */
    sdi_sf_io_bus_ops_t *ops;
    /**
     * @brief lock
     * To serialize access to bus
     */
    pthread_mutex_t lock;
} sdi_sf_io_bus_t;

/**
 * @}
 */
#endif /* __SDI_SMARTFUSION_IO_H___ */
