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
 * filename: sdi_bmc_io_bus.h
 */

#ifndef __SDI_BMC_IO_BUS_H__
#define __SDI_BMC_IO_BUS_H__


#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_bus.h"
#include "sdi_bus_framework.h"


/**
 * @typedef sdi_bmc_io_bus_t
 * BMC IO BUS type
 */
typedef struct sdi_bmc_io_bus sdi_bmc_io_bus_t;

/**
 * @typedef sdi_bmc_io_bus_hdl_t
 * BMC IO BUS Handle
 */
typedef sdi_bmc_io_bus_t *sdi_bmc_io_bus_hdl_t;


/**
 * @struct sdi_BMC_io_bus
 * SDI BMC Bus Structure
 */
typedef struct sdi_bmc_io_bus {
    /**
     * @brief bus
     * SDI BUS object, to store bus_type, bus_id and bus_name
     */
    sdi_bus_t bus;
    /**
     * @brief lock
     * To serialize access to bus
     */
    pthread_mutex_t lock;
} sdi_bmc_io_bus_t;

#endif /* __SDI_BMC_IO_BUS_H__ */
