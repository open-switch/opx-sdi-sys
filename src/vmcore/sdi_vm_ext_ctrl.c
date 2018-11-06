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
 * sdi_vm_ext_ctrl.c
 * Ext_Ctrl Resource VM functionality implements sdi-api headers
 */

#include "sdi_entity.h"
#include "sdi_ext_ctrl.h"
#include "sdi_sys_common.h"
#include "sdi_sys_vm.h"
#include "sdi_db.h"
#include <stdio.h>

/*
 * Retrieve the ext_ctrl using the specified resource.
 */
t_std_error sdi_ext_ctrl_get(sdi_resource_hdl_t resource_hdl, int *ext_ctrl, int *size)
{
    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);

    return STD_ERR_OK;
}

/*
 * Set the threshold for the specified ext_ctrl resource.
 */
t_std_error sdi_ext_ctrl_set(sdi_resource_hdl_t resource_hdl, int *ext_ctrl, int size)
{
    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(ext_ctrl != NULL);
    STD_ASSERT(size == 1);

    return STD_ERR_OK;
}
