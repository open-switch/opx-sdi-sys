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
 * filename: sdi_fan.c
 */


/**************************************************************************************
 * sdi_fan.c
 * API Implementation for FAN resource related functionalities.
***************************************************************************************/
#include "sdi_fan_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_sys_common.h"
#include "std_assert.h"

/*
 * API implementation to retrieve the speed of the fan refered by resource.
 * [in] hdl - resource handle of the fan
 * [out] speed - speed(in RPM) is returned in this
 */
t_std_error sdi_fan_speed_get(sdi_resource_hdl_t hdl, uint_t *speed)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t fan_hdl=(sdi_resource_priv_hdl_t)hdl;

    STD_ASSERT(fan_hdl != NULL);
    STD_ASSERT(speed != NULL);
    STD_ASSERT(is_sdi_inited());

    if(fan_hdl->type != SDI_RESOURCE_FAN)
    {
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((fan_ctrl_t *)fan_hdl->callback_fns)->speed_get(hdl, fan_hdl->callback_hdl, speed);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to get the speed for %s Fan",fan_hdl->name);
    }
    return rc;
}

/*
 * API implementation to set the speed of the fan(in RPM) refered by resource.
 * [in] hdl - resource handle of the fan
 * [in] speed - speed(in RPM) to be set
 */
t_std_error sdi_fan_speed_set(sdi_resource_hdl_t hdl, uint_t speed)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t fan_hdl=(sdi_resource_priv_hdl_t)hdl;

    STD_ASSERT(fan_hdl != NULL);
    STD_ASSERT(is_sdi_inited());

    if(fan_hdl->type != SDI_RESOURCE_FAN)
    {
        return(SDI_ERRCODE(EPERM));
    }

    if(((fan_ctrl_t *)fan_hdl->callback_fns)->speed_set == NULL) {
        return  SDI_ERRCODE(EOPNOTSUPP);
    }

    rc = ((fan_ctrl_t *)fan_hdl->callback_fns)->speed_set(hdl, fan_hdl->callback_hdl, speed);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to set the speed for %s Fan",fan_hdl->name);
    }

    return rc;
}

/*
 * API implementation to retrieve the fault status of the fan refered by resource.
 * [in] hdl - resource handle of the fan
 * [out] status - fan's fault status is returned in this
 */
t_std_error sdi_fan_status_get(sdi_resource_hdl_t hdl, bool *status)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t fan_hdl=(sdi_resource_priv_hdl_t)hdl;

    STD_ASSERT(fan_hdl != NULL);
    STD_ASSERT(status != NULL);
    STD_ASSERT(is_sdi_inited());

    if(fan_hdl->type != SDI_RESOURCE_FAN)
    {
        return(SDI_ERRCODE(EPERM));
    }

    rc = ((fan_ctrl_t *)fan_hdl->callback_fns)->status_get(fan_hdl->callback_hdl,status);
    if(rc != STD_ERR_OK)
    {
        SDI_ERRMSG_LOG("Failed to get the status for %s Fan",fan_hdl->name);
    }

    return rc;
}

/* Convert fan speed RPM to percent */

uint_t sdi_fan_speed_rpm_to_pct(sdi_resource_hdl_t hdl, uint_t rpm)
{
    sdi_resource_priv_hdl_t fan_hdl = (sdi_resource_priv_hdl_t) hdl;
    uint_t (*f)(sdi_resource_hdl_t, void *, uint_t)
        = ((fan_ctrl_t *) fan_hdl->callback_fns)->speed_rpm_to_pct;
    if (f == 0) {
        /* No callback given => fall back to % of max speed */
        sdi_entity_priv_hdl_t e = fan_hdl->parent;
        return (e->entity_info_valid ? (100 * rpm) / e->entity_info.max_speed : 100);
    }

    return ((*f)(hdl, fan_hdl->callback_hdl, rpm));
}

/* Convert fan speed percent to RPM */

uint_t sdi_fan_speed_pct_to_rpm(sdi_resource_hdl_t hdl, uint_t pct)
{
    sdi_resource_priv_hdl_t fan_hdl = (sdi_resource_priv_hdl_t) hdl;
    uint_t (*f)(sdi_resource_hdl_t, void *, uint_t)
        = ((fan_ctrl_t *) fan_hdl->callback_fns)->speed_pct_to_rpm;
    if (f == 0) {
        /* No callback given => fall back to fraction of max speed */
        sdi_entity_priv_hdl_t e = fan_hdl->parent;
        return (e->entity_info_valid ? (pct * e->entity_info.max_speed) / 100 : 0);
    }

    return ((*f)(hdl, fan_hdl->callback_hdl, pct));
}
