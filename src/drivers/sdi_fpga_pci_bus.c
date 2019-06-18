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
 * filename: sdi_fpga_pci_bus.c
 */

#include "sdi_device_common.h"
#include "sdi_fpga_pci.h"
#include "sdi_fpga_pci_bus.h"
#include "sdi_fpga_pci_bus_attr.h"
#include "sdi_bus_attr.h"
#include "sdi_common_attr.h"
#include "std_assert.h"
#include "std_utils.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

static void *fpga_base_ptr = 0;

t_std_error fpga_bus_read_byte(sdi_fpga_pci_bus_hdl_t bus_hdl, uint_t addr, uint8_t *value)
{
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(bus_hdl->bus.bus_type == SDI_FPGA_PCI_BUS);

    *value = *((uint8_t *) (fpga_base_ptr + addr));

    return STD_ERR_OK;
}

t_std_error fpga_bus_write_byte(sdi_fpga_pci_bus_hdl_t bus_hdl, uint_t addr, uint8_t value)
{
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(bus_hdl->bus.bus_type == SDI_FPGA_PCI_BUS);
    *((uint8_t *) (fpga_base_ptr + addr))  = value;

    return STD_ERR_OK;
}

static sdi_fpga_pci_bus_ops_t fpga_bus_ops = {
    .sdi_fpga_pci_bus_read_byte = fpga_bus_read_byte,
    .sdi_fpga_pci_bus_write_byte = fpga_bus_write_byte,
};



static t_std_error fpga_pci_bus_driver_init(sdi_bus_hdl_t bus_hdl)
{
    t_std_error rc = STD_ERR_OK;
    fpga_pci_bus_t *fpga_pci_bus = NULL;
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(bus_hdl->bus_type == SDI_FPGA_PCI_BUS);

    fpga_pci_bus = (fpga_pci_bus_t *)bus_hdl;

    int fd = -1;
    fd = open(fpga_pci_bus->fpga_sysfs_name, O_RDWR | O_SYNC);
    if (fd >= 0) {
        fpga_base_ptr = mmap(0,24576, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        sdi_bus_init_device_list(bus_hdl);
    
        close(fd);
    } else {
        rc = SDI_DEVICE_ERRCODE(EBADFD);
    }
    return rc;
}



static t_std_error fpga_pci_bus_driver_register(std_config_node_t node,
                                             sdi_bus_hdl_t *bus_hdl)
{
    char *node_attr = NULL;
    t_std_error error = STD_ERR_OK;
    sdi_fpga_pci_bus_hdl_t fpga_bus_hdl = NULL;
    fpga_pci_bus_t *fpga_bus = (fpga_pci_bus_t *)calloc(sizeof(fpga_pci_bus_t), 1);

    STD_ASSERT(fpga_bus != NULL);

    fpga_bus_hdl = &(fpga_bus->bus);
    fpga_bus_hdl->bus.bus_init = fpga_pci_bus_driver_init;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_BUS_NAME);
    STD_ASSERT(node_attr != NULL);
    safestrncpy(fpga_bus_hdl->bus.bus_name, node_attr, SDI_MAX_NAME_LEN);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(node_attr != NULL);
    fpga_bus_hdl->bus.bus_id = (uint_t) strtoul (node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_FPGA_PCI_BUS_SYSFS_ENTRY);
    STD_ASSERT(node_attr != NULL);
    safestrncpy(fpga_bus->fpga_sysfs_name, node_attr, SDI_MAX_NAME_LEN);


    std_mutex_lock_init_non_recursive(&(fpga_bus_hdl->lock));
    fpga_bus_hdl->ops = &fpga_bus_ops;

    fpga_bus_hdl->bus.bus_type = SDI_FPGA_PCI_BUS;

    error = sdi_bus_register((sdi_bus_hdl_t )fpga_bus);
    if (error != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("%s:%d registration failed %s with error %d\n",
                              __FUNCTION__, __LINE__, fpga_bus_hdl->bus.bus_name, error);
        free(fpga_bus);
        return error;
    }

    *bus_hdl = (sdi_bus_hdl_t ) fpga_bus;

    sdi_bus_register_device_list(node, (sdi_bus_hdl_t) fpga_bus);

    return error;
}

const sdi_bus_driver_t *fpga_pci_bus_entry_callbacks(void)
{
    /* Export bus driver table */
    static const sdi_bus_driver_t fpga_pci_bus_entry = {
        .bus_register = fpga_pci_bus_driver_register,
        .bus_init = fpga_pci_bus_driver_init
    };
    return &fpga_pci_bus_entry;
}
