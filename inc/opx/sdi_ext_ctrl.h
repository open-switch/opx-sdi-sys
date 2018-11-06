/*
 * Copyright (c) 2018 Dell EMC..
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * filename: sdi_ext_ctrl.h
 * @brief SDI External API.
 *
 */

#ifndef __SDI_EXT_CTRL_H_
#define __SDI_EXT_CTRL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"

/**
 * @defgroup sdi_ext_ctrl_api SDI Ext Ctrl API.
 * All external control related API take an argument
 * of type sdi_resource_hdl_t.  Application should first identify the
 * right sdi_resource_hdl_t by using @ref sdi_entity_resource_lookup
 *
 * @ingroup sdi_sys
 * @{
 */

/**
 * @brief Retrieve the ext ctrl value.
 * @param[in] sensor_hdl - handle of the external control  that is of interest.
 * @param[in] type - The type of ctrl that needs to be set.
 * @param[out] *val - the value of will be returned in memory pointed by this.
 * @param[out] *size - the size of value returned in memory in unit of sizeof(int)
 * @return - standard @ref t_std_error
 */
t_std_error sdi_ext_ctrl_get (sdi_resource_hdl_t resource_hdl, int *val, int *size);

/**
 * @brief Set the ext ctrl using the specified value.
 * @param[in] sensor_hdl - handle to the temperature sensor.
 * @param[in] type - The type of ctrl that needs to be set.
 * @param[in] size - the size of value returned in memory in unit of sizeof(int)
 * @param[in] val - the start address of the value to be set.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_ext_ctrl_set (sdi_resource_hdl_t resource_hdl, int *val, int size);

/**
 * @}
 */


#endif   /* __SDI_EXT_CTRL_H_ */
