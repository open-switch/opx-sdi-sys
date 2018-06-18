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
 * filename: sdi_fpga_pci_bus_api.h
 */


/******************************************************************************
 * @file sdi_fpga_pci_bus_api.h
 * @brief Defines SDI Fpga pci Bus APIs
 *****************************************************************************/

#ifndef __SDI_FPGA_PCI_BUS_API_H__
#define __SDI_FPGA_PCI_BUS_API_H__

#include "sdi_bus_framework.h"
#include "sdi_pin.h"
#include "sdi_fpga_pci.h"

/**
 * @defgroup sdi_internal_fpga_pci_bus_api SDI Internal Fpga pci Bus API
 * @brief SDI Fpga pci Bus APIs
 * Implements APIs to read the status of Fpga pci Bus
 *
 * @ingroup sdi_internal_bus
 *
 * @{
 */

/**
 * @brief sdi_fpga_pci_acquire_bus
 * Acquire Fpga pci Bus lock
 * @param[in] bus sdi Fpga pci bus object
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_fpga_pci_acquire_bus(sdi_fpga_pci_bus_hdl_t bus);

/**
 * @brief sdi_fpga_pci_release_bus
 * Release acquired Fpga pci Bus lock
 * @param[in] bus sdi Fpga pci bus object
 * @return none
 */
void sdi_fpga_pci_release_bus(sdi_fpga_pci_bus_hdl_t bus);

/**
 * @brief sdi_fpga_pci_bus_read_byte
 * Read Byte from Fpga pci Bus
 * @param[in] bus sdi Fpga pci bus object
 * @param[in] addr sdi Fpga pci bus address
 * @param[out] value data pointer to store byte read from BUS
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_fpga_pci_bus_read_byte(sdi_fpga_pci_bus_hdl_t bus, uint_t addr, uint8_t *value);

/**
 * @brief sdi_fpga_pci_bus_write_byte
 * Write given byte data to Fpga pci Bus
 * @param[in] bus sdi Fpga pci bus object
 * @param[in] addr sdi Fpga pci bus address
 * @param[out] value data to be written to Fpga pci bus
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_fpga_pci_bus_write_byte(sdi_fpga_pci_bus_hdl_t bus, uint_t addr, uint8_t value);

/**
 * @}
 */

#endif /* __SDI_FPGA_PCI_BUS_API_H__ */
