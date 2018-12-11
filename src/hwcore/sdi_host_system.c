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
 * filename: sdi_host_system.c
 * Implementation of host_system resource API.
 */

#include "sdi_host_system.h"
#include "sdi_host_system_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_sys_common.h"
#include "std_assert.h"

/*
 * Read slot position from host system
 */
t_std_error sdi_host_system_slot_pos_get(sdi_resource_hdl_t resource_hdl, uint8_t *slot) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t host_system_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(slot != NULL);

    host_system_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (host_system_hdl->type != SDI_RESOURCE_HOST_SYSTEM) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((host_system_ctrl_t *)host_system_hdl->callback_fns)->get_slot(host_system_hdl->callback_hdl, slot);

    return rc;
}

/*
 * Set/Reset boot status for host system
 */
t_std_error sdi_host_system_booted_set(sdi_resource_hdl_t resource_hdl, bool booted) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t host_system_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    host_system_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (host_system_hdl->type != SDI_RESOURCE_HOST_SYSTEM) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((host_system_ctrl_t *)host_system_hdl->callback_fns)->set_booted(host_system_hdl->callback_hdl, booted);

    return rc;
}


