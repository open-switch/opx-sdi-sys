/*
 * Copyright (c) 2016 Dell Inc.
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

/**
 * @file sdi_vm_fan.c
 * @brief Fan simulation functionality implements sdi-api headers
 */

#include "sdi_sys_vm.h"
#include "sdi_entity.h"
#include "sdi_fan.h"

/*
 * @brief Retrieve the speed of specified fan.
 */
t_std_error sdi_fan_speed_get(sdi_resource_hdl_t fan_hdl, uint_t *speed)
{
    STD_ASSERT(speed != NULL);

    /* Get the fan speed */
    return sdi_db_int_field_get(sdi_get_db_handle(), fan_hdl,
                                TABLE_FAN, FAN_SPEED, (int *)speed);
}

/*
 * Set the speed of specified fan.
 */
t_std_error sdi_fan_speed_set(sdi_resource_hdl_t fan_hdl, uint_t speed)
{
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;
    uint_t max_speed;

    /* Get the associated info handle for the fan */
    rc = sdi_db_resource_get_associated_info(sdi_get_db_handle(), fan_hdl,
                                             SDI_RESOURCE_FAN, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Use the info handle to lookup the maximum speed of the fan */
    rc = sdi_db_int_field_get(sdi_get_db_handle(), info_hdl,
                              TABLE_INFO, INFO_FAN_MAX_SPEED,
                              (int *)&max_speed);
    if (rc != STD_ERR_OK) {
        return rc;
    }
    
    /* Check if speed exceeds the allowable maximum speed */
    if (speed > max_speed) {
        return STD_ERR(BOARD, PARAM, EINVAL);
    }

    return sdi_db_int_field_target_set(sdi_get_db_handle(), fan_hdl,
                                TABLE_FAN, FAN_SPEED, (int *)&speed);
}

/*
 * Retrieve the health of given fan.
 */
t_std_error sdi_fan_status_get(sdi_resource_hdl_t fan_hdl, bool *alert_on)
{
    int db_value;
    t_std_error rc;

    STD_ASSERT(alert_on != NULL);

    rc = sdi_db_int_field_get(sdi_get_db_handle(), fan_hdl,
                              TABLE_FAN, FAN_FAULT, &db_value);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    *alert_on = (bool)db_value;
    return STD_ERR_OK;
}
