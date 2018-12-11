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
 * filename: sdi_comm_dev.c
 * Implementation of Comm_Dev resource API.
 */

#include "sdi_resource_internal.h"
#include "sdi_sys_common.h"
#include "std_assert.h"
#include "sdi_comm_dev.h"
#include "sdi_comm_dev_internal.h"
#include "sdi_host_system_internal.h"

#include <string.h>

/*
 * Read message from Comm_Dev incoming message buffer
 */
t_std_error sdi_comm_dev_msg_read(sdi_resource_hdl_t resource_hdl, uint16_t size_data, uint8_t *read_data) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(read_data != NULL);
    STD_ASSERT(size_data != 0 && size_data <= SDI_COMM_DEV_BUFFER_MAX_SIZE);

    memset(read_data, 0, size_data);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->read_mbox(resource_hdl, read_data);

    // Set Package Read and Verified bit in Comm_Dev
    if (STD_ERR_OK == rc) {
        rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->write_pckg_read_verif(resource_hdl);
    }

    SDI_TRACEMSG_LOG("mbox data read: size %d data %s", size_data, (char*) read_data);

    return rc;
}

/*
 * Write message to Comm_Dev outgoing message buffer
 */
t_std_error sdi_comm_dev_msg_write(sdi_resource_hdl_t resource_hdl, uint16_t size_data, uint8_t *write_data) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(write_data != NULL);
    STD_ASSERT(size_data != 0 && size_data <= SDI_COMM_DEV_BUFFER_MAX_SIZE);

    SDI_TRACEMSG_LOG("mbox data write: size %d data %s", size_data, (char*) write_data);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;

    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->write_mbox(resource_hdl, size_data, write_data);

    // Set Package Download Complete bit in Comm_Dev
    if (STD_ERR_OK == rc) {
        rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->write_pckg_downl_compl(resource_hdl);
    }

    return rc;
}

/*
 * Read static data from Comm_Dev
 */
t_std_error sdi_comm_dev_platform_info_get(sdi_resource_hdl_t resource_hdl, sdi_platform_info_t *platform_info) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(platform_info != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    memset(platform_info, 0, sizeof(sdi_platform_info_t));

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->read_platform_info(resource_hdl, platform_info);

    return rc;
}

/*
 * Read Host System Firmware Version from Comm_Dev
 */
t_std_error sdi_comm_dev_host_sw_version_get(sdi_resource_hdl_t resource_hdl, uint8_t *fw_rev) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(fw_rev != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->access_fw_rev(resource_hdl, false, fw_rev);

    return rc;
}

/*
 * Write Host System Firmware Version into Comm_Dev
 */
t_std_error sdi_comm_dev_host_sw_version_set(sdi_resource_hdl_t resource_hdl, uint8_t *fw_rev) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(fw_rev != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->access_fw_rev(resource_hdl, true, fw_rev);

    return rc;
}

/*
 * Write temperature reading from Host System into Comm_Dev
 */
t_std_error sdi_comm_dev_host_temperature_set(sdi_resource_hdl_t resource_hdl, int temp) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->write_temp_sensor(resource_hdl, temp);

    return rc;
}

/*
 * Write temperature reading from Host System into Comm_Dev
 */
t_std_error sdi_comm_dev_host_npu_temperature_set(sdi_resource_hdl_t resource_hdl, int temp) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->write_npu_temp_sensor(resource_hdl, temp);

    return rc;
}

/*
 * Write temperature reading from Host System into Comm_Dev
 */
t_std_error sdi_comm_dev_host_ambient_temperature_set(sdi_resource_hdl_t resource_hdl, int temp) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->write_ambient_temp_sensor(resource_hdl, temp);

    return rc;
}

/*
 * Enable comm dev messaging
 */
t_std_error sdi_comm_dev_messaging_enable(sdi_resource_hdl_t resource_hdl, bool messaging_enable)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->messaging_enable(resource_hdl, messaging_enable);

    return rc;
}

/*
 * Flush message buffer
 */
t_std_error sdi_comm_dev_flush_msg_buffer(sdi_resource_hdl_t resource_hdl)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->flush_msg_buffer(resource_hdl);

    return rc;
}

/*
 * Read whether write Message Buffer is ready
 */
t_std_error sdi_comm_dev_is_write_buffer_ready(sdi_resource_hdl_t resource_hdl, bool *ready)
{
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t comm_dev_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);

    comm_dev_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (comm_dev_hdl->type != SDI_RESOURCE_COMM_DEV) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((comm_dev_ctrl_t *)comm_dev_hdl->callback_fns)->get_buffer_ready(resource_hdl, ready);

    return rc;
}

/*
 * Read Mailbox Message presence status from host system
 */
t_std_error sdi_comm_dev_is_msg_present(sdi_resource_hdl_t resource_hdl, bool *presence) {
    t_std_error rc = STD_ERR_OK;
    sdi_resource_priv_hdl_t host_system_hdl = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(presence != NULL);

    host_system_hdl = (sdi_resource_priv_hdl_t)resource_hdl;
    if (host_system_hdl->type != SDI_RESOURCE_HOST_SYSTEM) {
        return (SDI_ERRCODE(EPERM));
    }

    rc = ((host_system_ctrl_t *)host_system_hdl->callback_fns)->get_pkg_notify(host_system_hdl->callback_hdl, presence);

    return rc;
}
