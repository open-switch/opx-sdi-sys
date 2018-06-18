/*
 * Copyright (c) 2018 Dell EMC.
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
 * filename: sdi_fpga_pci.h
 */


/******************************************************************************
 * @file sdi_fpga_pci.h
 * @brief Defines SDI FPGA PCI Bus Data Structures and Bus Operator functions
 *****************************************************************************/

#ifndef __SDI_FPGA_PCI_H___
#define __SDI_FPGA_PCI_H___

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_bus.h"
#include "sdi_bus_framework.h"


/**
 * @defgroup sdi_internal_fpga_pci_bus_data_types SDI Internal Fpga PCI Bus Data Type
 * @brief  SDI Fpga PCI Bus Data Structure and Bus Operator functions
 */


/**
 * @typedef sdi_fpga_pci_bus_t
 * FPGA PCI BUS type
 */
typedef struct sdi_fpga_pci_bus sdi_fpga_pci_bus_t;

/**
 * @typedef sdi_fpga_pci_bus_hdl_t
 * FPGA PCI BUS Handle
 */
typedef sdi_fpga_pci_bus_t *sdi_fpga_pci_bus_hdl_t;

/**
 * @struct sdi_fpga_pci_bus_ops
 * SDI FPGA PCI Bus Operations
 */
typedef struct sdi_fpga_pci_bus_ops {
    /**
     * @brief sdi_fpga_pci_bus_read_byte
     * Read a byte from Fpga PCI BUS Address
     */
    t_std_error (*sdi_fpga_pci_bus_read_byte) (sdi_fpga_pci_bus_hdl_t bus_handle,
                                            uint_t addr, uint8_t *value);
    /**
     * @brief sdi_fpga_pci_bus_write_byte
     * Write given byte to Fpga PCI BUS Address
     */
    t_std_error (*sdi_fpga_pci_bus_write_byte) (sdi_fpga_pci_bus_hdl_t bus_handle,
                                             uint_t addr, uint8_t value);
} sdi_fpga_pci_bus_ops_t;

/**
 * @struct sdi_fpga_pci_bus
 * SDI SmartFusion Bus Structure
 */
typedef struct sdi_fpga_pci_bus {
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
    sdi_fpga_pci_bus_ops_t *ops;
    /**
     * @brief lock
     * To serialize access to bus
     */
    pthread_mutex_t lock;
} sdi_fpga_pci_bus_t;

/**
 * @}
 */
#endif /* __SDI_FPGA_PCI_H___ */
