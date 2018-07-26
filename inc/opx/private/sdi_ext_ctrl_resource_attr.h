/*
 * Copyright (c) 2018 Dell EMC..
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
 * filename: sdi_ext_ctrl_resource_attr.h
 */

/**
 * @file sdi_ext_ctrl_resource_attr.h
 * @brief defines list of attributes commonly used for SDI_EXT_CTRL_RESOURCE configuration
 */

#ifndef __SDI_EXT_CTRL_RESOURCE_ATTR_H__
#define __SDI_EXT_CTRL_RESOURCE_ATTR_H__

/**
 * @defgroup sdi_config_ext_ctrl_resource_attributes SDI Configuration Ext_Ctrl Resource Attributes
 * @ingroup sdi_config_attributes
 * @brief Configuration Attributes commonly used by Device nodes that export
 * @ref SDI_EXT_CTRL_RESOURCE
 *
 * @{
 */

#include "sdi_common_attr.h"

/**
 * @def Attribute used for representing the default dev addr of a
 * ext_ctrl resource
 */
#define SDI_DEV_ATTR_EXT_CTRL_VAL_OFFSET   "ctrl_value_offset"

/**
 * @def Attribute used for representing the default num of dev regs of a
 * ext_ctrl resource
 */
#define SDI_DEV_ATTR_EXT_CTRL_WIDTH        "width"

/**
 * @def Attribute used for representing the default step to configure all the dev regs of a
 * ext_ctrl resource
 */
#define SDI_DEV_ATTR_EXT_CTRL_STEP         "step"

/**
 * @def Attribute used to determine if hardware refresh is need during ext_ctrl update
 */
#define SDI_DEV_ATTR_EXT_CTRL_REFRESH      "refresh"

/**
 * @def Node name used to represent resources of type SDI_EXT_CTRL_RESOURCE
 */
#define SDI_DEV_NODE_EXT_CTRL              "ext_ctrl"

/**
 * @}
 */

#endif   /* __SDI_EXT_CTRL_RESOURCE_ATTR_H__ */
