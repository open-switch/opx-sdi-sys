/*
 * Copyright (c) 2018 Dell Inc.
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
 * filename: sdi_fpga_pci_bus.h
 */

#ifndef __SDI_FPGA_PCI_BUS_H__
#define __SDI_FPGA_PCI_BUS_H__

#include "sdi_fpga_pci.h"

typedef struct fpga_pci_bus {
    sdi_fpga_pci_bus_t bus;
    char fpga_sysfs_name[SDI_MAX_NAME_LEN];
} fpga_pci_bus_t;

#endif /* __SDI_FPGA_PCI_BUS_H__ */
