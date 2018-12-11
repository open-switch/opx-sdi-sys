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

#include "sdi_resource_internal.h"
#include "sdi_sys_common.h"
#include "sdi_nvram_internal.h"

t_std_error sdi_nvram_size(
    sdi_resource_hdl_t resource_hdl,
    uint_t             *size
                           )
{
    if (sdi_resource_type_get(resource_hdl) != SDI_RESOURCE_NVRAM) {
        return (SDI_ERRCODE(EPERM));
    }

    sdi_resource_priv_hdl_t nvram_hdl = (sdi_resource_priv_hdl_t) resource_hdl;
    t_std_error rc = (*((nvram_t *) nvram_hdl->callback_fns)->size)(nvram_hdl->callback_hdl, size);
    if (rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("NVRAM get-size failed for %s", nvram_hdl->name);
    }

    return rc;
}

t_std_error sdi_nvram_read(
    sdi_resource_hdl_t resource_hdl,
    uint8_t            *buf,
    uint_t             ofs,
    uint_t             len
                           )
{
    if (sdi_resource_type_get(resource_hdl) != SDI_RESOURCE_NVRAM) {
        return (SDI_ERRCODE(EPERM));
    }

    sdi_resource_priv_hdl_t nvram_hdl = (sdi_resource_priv_hdl_t) resource_hdl;
    t_std_error rc = (*((nvram_t *) nvram_hdl->callback_fns)->read)(nvram_hdl->callback_hdl, buf, ofs, len);
    if (rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("NVRAM read failed for %s", nvram_hdl->name);
    }

    return rc;
}

t_std_error sdi_nvram_write(
    sdi_resource_hdl_t resource_hdl,
    uint8_t            *buf,
    uint_t             ofs,
    uint_t             len
                            )
{
    if (sdi_resource_type_get(resource_hdl) != SDI_RESOURCE_NVRAM) {
        return (SDI_ERRCODE(EPERM));
    }

    sdi_resource_priv_hdl_t nvram_hdl = (sdi_resource_priv_hdl_t) resource_hdl;
    t_std_error rc = (*((nvram_t *) nvram_hdl->callback_fns)->write)(nvram_hdl->callback_hdl, buf, ofs, len);
    if (rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("NVRAM write failed for %s", nvram_hdl->name);
    }

    return rc;
}

