/**
 * sdi_vm_host_system.c
 *
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

#include "sdi_host_system.h"
#include "std_error_codes.h"

/* SDI HOST SYSTEM - Read slot position */
t_std_error sdi_host_system_slot_pos_get(sdi_resource_hdl_t resource_hdl, uint8_t *slot)
{
    return STD_ERR_OK;
}

/* SDI HOST SYSTEM - set boot status for host system */
t_std_error sdi_host_system_booted_set(sdi_resource_hdl_t resource_hdl, bool booted)
{
    return STD_ERR_OK;
}

