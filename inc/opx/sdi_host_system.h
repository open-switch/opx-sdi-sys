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
 * filename: sdi_host_system.h
 */

/**
 * @file sdi_host_system.h
 * @brief This file contains the structure definitions of the
 *        host system handling and function declarations.
 * The Host System is a generic blade or board inserted in the
 * Chassis. For SDI, Host System is a HW device that stores
 * static and dynamic information that is exchanged between the
 * the Host System and the Controller.
 */

#ifndef __SDI_HOST_SYSTEM_H_
#define __SDI_HOST_SYSTEM_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "ds_common_types.h"
#include "sdi_entity.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** SDI HOST SYSTEM - Read slot position
 * @param[in] host_system_resource_hdl; handle of the resource
 * @param[out] pos; slot position
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_host_system_slot_pos_get(sdi_resource_hdl_t host_system_resource_hdl, uint8_t *slot);

/** SDI HOST SYSTEM - Set boot status for host system
 * @param[in] host_system_resource_hdl; handle of the resource
 * @param[in] booted; true/false booted flag value
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_host_system_booted_set(sdi_resource_hdl_t host_system_resource_hdl, bool booted);

#ifdef __cplusplus
}
#endif
#endif   /* __SDI_HOST_SYSTEM_H_ */
