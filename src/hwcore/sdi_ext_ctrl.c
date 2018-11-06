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
 * filename: sdi_ext_ctrl.c SDI API for external control
 */


/**************************************************************************************
 * sdi_ext_ctrl.c
 * API implementation for ext_ctrl related functionalities
***************************************************************************************/

#include "sdi_ext_ctrl_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_sys_common.h"
#include "std_assert.h"

/*
 * API implementation to retrieve the ext_ctrl of the chip refered by resource.
 * [in] resource_hdl - resource handle of the chip
 * [in] type - external ctrl handle type
 * [out] ext_ctrl - ext_ctrl value is returned in this
 * [out] size - size of ext_ctrl value in the unit of sizeof(int)
 */
t_std_error sdi_ext_ctrl_get(sdi_resource_hdl_t resource_hdl, int *ext_ctrl, int *size)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t ext_ctrl_hdl;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(is_sdi_inited());

    ext_ctrl_hdl = (sdi_resource_priv_hdl_t) resource_hdl;
    if(ext_ctrl_hdl->type != SDI_RESOURCE_EXT_CONTROL)
    {
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((ext_ctrl_t *)ext_ctrl_hdl->callback_fns)->
        ext_ctrl_get(ext_ctrl_hdl->callback_hdl, ext_ctrl, size);
    if(rc != STD_ERR_OK) {
        SDI_ERRMSG_LOG("Failed to get ext_ctrl %s, rc 0x%x",
		       ext_ctrl_hdl->name, rc);
    }

    return rc;
}

/*
 * API implementation to set the ext_ctrl thresholds of the chip refered by resource.
 * [in] resource_hdl - resource handle of the chip
 * [in] type - external ctrl handle type
 * [in] ext_ctrl - ext_ctrl value is returned in this
 * [in] size - size of ext_ctrl value in the unit of sizeof(int)
 */
t_std_error sdi_ext_ctrl_set(sdi_resource_hdl_t resource_hdl, int *ext_ctrl, int size)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t ext_ctrl_hdl;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(is_sdi_inited());

    ext_ctrl_hdl = (sdi_resource_priv_hdl_t) resource_hdl;
    if(ext_ctrl_hdl->type != SDI_RESOURCE_EXT_CONTROL)
    {
        SDI_ERRMSG_LOG("ext_ctrl_hdl %s wrong type %d", ext_ctrl_hdl->name, ext_ctrl_hdl->type);
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((ext_ctrl_t *)ext_ctrl_hdl->callback_fns)->
        ext_ctrl_set(ext_ctrl_hdl->callback_hdl, ext_ctrl, size);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to set ext_ctrl %s to 0x%x, rc 0x%x",
		       ext_ctrl_hdl->name, *ext_ctrl, rc);
    }

    return rc;
}

