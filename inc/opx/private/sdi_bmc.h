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
 * filename: sdi_bmc.h
 */

#ifndef __SDI_BMC_H__
#define __SDI_BMC_H__


#include "std_error_codes.h"
#include "sdi_driver_internal.h"
#include "sdi_bmc_internal.h"

/**
 * sdi_bmc_device_driver_init is to initialize BMC driver, it establishes
 * connection with BMC and populates entities and sensors managed by it 
 * and aslo start's bmc poller thread to updated current sensor readings.
 */
t_std_error sdi_bmc_device_driver_init (sdi_device_hdl_t dev_hdl);

/**
 * sdi_bmc_oem_cmd_execute is to execute OEM specific IPMI command.
 * 
 */
t_std_error sdi_bmc_oem_cmd_execute(sdi_bmc_oem_cmd_info_t *oem_cmd, uint8_t *data);

#endif /* __SDI_BMC_H__ */
