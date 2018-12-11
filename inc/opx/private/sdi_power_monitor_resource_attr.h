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
 * filename: sdi_power_monitor_resource_attr.h
 */


/**
 * @file sdi_power_monitor_resource_attr.h
 * @brief defines list of attributes commonly used for SDI_POWER_MONITOR_RESOURCE
 * configuration
 */

#ifndef __SDI_POWER_MONITOR_RESOURCE_ATTR_H__
#define __SDI_POWER_MONITOR_RESOURCE_ATTR_H__

/**
 * @defgroup sdi_config_power_monitor_resource_attributes SDI Configuration POWER MONITOR Resource Attributes
 * @ingroup sdi_config_attributes
 * @brief Configuration Attributes commonly used by Device nodes that export
 *    @ref SDI_POWER_MONITOR_RESOURCE
 *
 * @{
 */

#include "sdi_common_attr.h"

/**
 * @def Attribute used for representing the configuration register value
 * of a power monitor resource
 */
#define SDI_DEV_ATTR_POWER_CONFIG_REG_VAL        "config_reg_val"

/**
 * @def Attribute used for representing the calibration register value
 * of a power monitor resource
 */
#define SDI_DEV_ATTR_POWER_CALIB_REG_VAL         "calib_reg_val"

/**
 * @def Attribute used for representing the configuration register value
 * of a power monitor resource
 */
#define SDI_DEV_ATTR_POWER_PSU_OFFSET_LOSS       "psu_offset_loss"

/**
 * @}
 */

#endif   /* __SDI_POWER_MONITOR_RESOURCE_ATTR_H__ */
