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
 * filename: sdi_smartfusion_io_bus_api.c
 */


/******************************************************************************
 * Defines SDI FPGA PCI Bus APIs
 *****************************************************************************/

#include "sdi_fpga_pci_bus_api.h"
#include "std_assert.h"

/**
 * sdi_fpga_pci_acquire_bus
 * Acquire FPGA PCI Bus lock
 */
t_std_error sdi_fpga_pci_acquire_bus(sdi_fpga_pci_bus_hdl_t bus_handle)
{
    STD_ASSERT(bus_handle != NULL);
    return (std_mutex_lock (&(bus_handle->lock)));
}

/**
 * sdi_fpga_pci_release_bus
 * Release acquired FPGA PCI Bus lock
 */
void sdi_fpga_pci_release_bus(sdi_fpga_pci_bus_hdl_t bus_handle)
{
    STD_ASSERT(bus_handle != NULL);
    std_mutex_unlock (&(bus_handle->lock));
}

/**
 * sdi_fpga_pci_bus_read_byte
 * Read Byte from FPGA PCI Bus
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_fpga_pci_bus_read_byte(sdi_fpga_pci_bus_hdl_t bus_handle, uint_t addr, uint8_t *value)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(value != NULL);

    error = bus_handle->ops->sdi_fpga_pci_bus_read_byte(bus_handle, addr, value);

    return error;
}

/**
 * sdi_fpga_pci_bus_write_byte
 * Write given byte data to FPGA PCI Bus
 * return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_fpga_pci_bus_write_byte(sdi_fpga_pci_bus_hdl_t bus_handle, uint_t addr, uint8_t value)
{
    t_std_error error = STD_ERR_OK;

    STD_ASSERT(bus_handle != NULL);

    error = bus_handle->ops->sdi_fpga_pci_bus_write_byte(bus_handle, addr, value);

    return error;
}
