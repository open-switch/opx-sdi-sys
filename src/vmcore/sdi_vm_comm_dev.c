/**
 * sdi_vm_comm_dev.c
 *
 * Comm Dev simulation functionality
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

#include "sdi_comm_dev.h"
#include "std_error_codes.h"

#include <stdint.h>

/* SDI COMM DEV - Read data from Comm_Dev SB message buffer */
t_std_error sdi_comm_dev_msg_read(sdi_resource_hdl_t resource_hdl, uint16_t size_data, uint8_t *read_data)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Write data to Comm_Dev NB message buffer */
t_std_error sdi_comm_dev_msg_write(sdi_resource_hdl_t resource_hdl, uint16_t size_data, uint8_t *write_data)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Read static data from Comm_Dev */
t_std_error sdi_comm_dev_platform_info_get(sdi_resource_hdl_t resource_hdl, sdi_platform_info_t *platform_info)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Read Host System Firmware Version from */
t_std_error sdi_comm_dev_host_sw_version_get(sdi_resource_hdl_t resource_hdl, uint8_t *fw_rev)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Write Host System Firmware Version into */
t_std_error sdi_comm_dev_host_sw_version_set(sdi_resource_hdl_t resource_hdl, uint8_t *fw_rev)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Write temperature reading from Host System */
t_std_error sdi_comm_dev_host_temperature_set(sdi_resource_hdl_t resource_hdl, int temp)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Write temperature reading from Host System */
t_std_error sdi_comm_dev_host_npu_temperature_set(sdi_resource_hdl_t resource_hdl, int temp)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Write temperature reading from Host System */
t_std_error sdi_comm_dev_host_ambient_temperature_set(sdi_resource_hdl_t resource_hdl, int temp)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Read comm Message presence status */
t_std_error sdi_comm_dev_is_msg_present(sdi_resource_hdl_t resource_hdl, bool *presence)
{
    return STD_ERR_OK;
}

/* SDI COMM DEV - Enable comm dev messagingi */
t_std_error sdi_comm_dev_messaging_enable(sdi_resource_hdl_t resource_hdl, bool messaging_enable)
{
    return STD_ERR_OK;
}
