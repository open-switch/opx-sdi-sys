/*
 * Copyright (c) 2019 Dell Inc.
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
 * filename: sdi_entity_info_resource_attr.h
 */


/**
 * @file sdi_entity_info_resource_attr.h
 * @brief defines list of attributes commonly used for SDI_ENTITY_INFO_RESOURCE
 * configuration
 *
 * @defgroup sdi_config_entity_info_resource_attributes Sdi Config Entity Info Resource Attributes.
 * @ingroup sdi_config_attributes
 * @brief Configuration Attributes commonly used by Device nodes that export
 * @ref SDI_ENTITY_INFO_RESOURCE
 *
 * @{
 */

#ifndef __SDI_ENTITY_INFO_RESOURCE_ATTR_H__
#define __SDI_ENTITY_INFO_RESOURCE_ATTR_H__

#include "sdi_common_attr.h"

/**
 * @def Attribute used for representing the size of the entity_info resource
 */
#define SDI_DEV_ATTR_ENTITY_SIZE              "size"

/**
 * @def Attribute used for representing the parser format of the entity_info resource
 */
#define SDI_DEV_ATTR_PARSER                   "parser"

/**
 * @def Attribute used for representing the maximum fan speed of the entity
 */
#define SDI_DEV_ATTR_MAX_SPEED                "max_fan_speed"

/**
 * @def Attribute used for representing the no.of fans of the entity
 */
#define SDI_DEV_ATTR_NO_OF_FANS               "no_of_fans"

/**
 * @def Attribute used for representing the direction of the airflow: normal or reverse
 */
#define SDI_DEV_ATTR_AIRFLOW_DIR_BUS          "airflow_dir_bus"

/**
 * @def Attribute used for representing the power supply type: ac or dc
 */
#define SDI_DEV_ATTR_PSU_TYPE_BUS             "psu_type_bus"

/**
 * @def Attribute value for representing normal airflow direction
 */
#define SDI_DEV_ATTR_NORMAL_AIRFLOW_VAL       "normal_airflow_val"

/**
 * @def Attribute value for representing reverse airflow direction
 */
#define SDI_DEV_ATTR_REVERSE_AIRFLOW_VAL      "reverse_airflow_val"

/**
 * @def Attribute value for representing AC powertype
 */
#define SDI_DEV_ATTR_AC_POWER_VAL             "ac_power_val"

/**
 * @def Attribute value for representing DC powertype
 */
#define SDI_DEV_ATTR_DC_POWER_VAL             "dc_power_val"

/**
 * @def Attribute value to know PSU_TYPE and FAN_AF based on sys part number
 */
#define SDI_DEV_ATTR_PART_NUMBER_BASED_SYS     "part_no_based_sys"

/**
 * @def Attribute value to know PSU_TYPE and FAN_AF based on sys part number
 */
#define SDI_DEV_ATTR_SYS_PART_NUMBER_BASED_ENTY  "part_no_based_enty"

/**
 * @def Attribute value for representing MAX part number types
 */
#define SDI_DEV_ATTR_MAX_PART_NO_TYPES         "max_part_no_types"

/**
 * @def Attribute value representing airflow value of specific part_no type
 */
#define SDI_DEV_ATTR_AIRFLOW_VALUE             "airflow_val"

/**
 * @def Attribute value representing PSU AC type val of specific part_no type
 */
#define SDI_DEV_ATTR_PSU_AC_TYPE_VAL           "ac_type"

/**
 * @def Attribute value representing PSU DC type val of specific part_no type
 */
#define SDI_DEV_ATTR_PSU_DC_TYPE_VAL           "dc_type"

/**
 * @def Attribute value for representing part number
 */
#define SDI_DEV_ATTR_PART_NO                   "part_no"

/**
 * @def Attribute value for representing part number mapping node
 */
#define SDI_DEV_PART_NUM_MAP_NODE              "part_no_mapping"

/**
 * @}
 */

#endif   /* __SDI_ENTITY_INFO_RESOURCE_ATTR_H__ */
