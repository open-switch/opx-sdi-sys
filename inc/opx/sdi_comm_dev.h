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
 * filename: sdi_comm_dev.h
 */

/**
 * @file sdi_comm_dev.h
 * @brief This file contains the structure definitions of the
 *        comm_dev handling and function declarations.
 * Comm_Dev is a device located in the Host System and stores
 * static and dynamic information that is exchanged between the
 * the Host System and the Controller
 */

#ifndef __SDI_COMM_DEV_H_
#define __SDI_COMM_DEV_H_

#include "std_error_codes.h"
#include "ds_common_types.h"
#include "sdi_entity.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** Size of string Service Tag */
#define SDI_COMM_DEV_SERVICE_TAG_SIZE                               35
/** Size of string Firmware Version */
#define SDI_COMM_DEV_FW_VERSION_SIZE                                65
/** Maximum size for Message Buffer */
#define SDI_COMM_DEV_BUFFER_MAX_SIZE                              4100

/** SDI Platform Info Structure: Contains platform static
 *  information */
typedef struct sdi_platform_info {
    char         service_tag[SDI_COMM_DEV_SERVICE_TAG_SIZE];     /**< chassis service tag */
    char         comm_dev_fw_ver[SDI_COMM_DEV_FW_VERSION_SIZE];  /**< comm dev firmware version */
    unsigned int slot_occupation;                                /**< slot of Host System */
} sdi_platform_info_t;

/** SDI COMM DEV - Read data from Comm_Dev SB message buffer
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] size_data; size of read buffer
 * @param[out] read_data; buffer for data to be read
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_msg_read(sdi_resource_hdl_t comm_dev_resource_hdl, uint16_t size_data, uint8_t *read_data);

/** SDI COMM DEV - Write data to Comm_Dev NB message buffer
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] size_data; size of write buffer
 * @param[in] write_data; buffer for data to be written
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_msg_write(sdi_resource_hdl_t comm_dev_resource_hdl, uint16_t size_data, uint8_t *write_data);

/** SDI COMM DEV - Read static data from Comm_Dev
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[out] platform_info; buffer for data to be read
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_platform_info_get(sdi_resource_hdl_t comm_dev_resource_hdl, sdi_platform_info_t *platform_info);

/** SDI COMM DEV - Read Host System Firmware Version from
 *  Comm_Dev
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[out] fw_rev; buffer for data to be read
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_host_sw_version_get(sdi_resource_hdl_t comm_dev_resource_hdl, uint8_t *fw_rev);

/** SDI COMM DEV - Write Host System Firmware Version into
 *  Comm_Dev
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] fw_rev; buffer for data to be written
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_host_sw_version_set(sdi_resource_hdl_t comm_dev_resource_hdl, uint8_t *fw_rev);

/** SDI COMM DEV - Write temperature reading from Host System
 *  into Comm_Dev
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] temp; temperature to be written
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_host_temperature_set(sdi_resource_hdl_t comm_dev_resource_hdl, int temp);

/** SDI COMM DEV - Write NPU temperature reading from Host System
 *  into Comm_Dev
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] temp; temperature to be written
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_host_npu_temperature_set(sdi_resource_hdl_t comm_dev_resource_hdl, int temp);

/** SDI COMM DEV - Write Ambient temperature reading from Host System
 *  into Comm_Dev
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] temp; temperature to be written
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_host_ambient_temperature_set(sdi_resource_hdl_t comm_dev_resource_hdl, int temp);

/** SDI COMM DEV - Flush write message buffer
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_flush_msg_buffer(sdi_resource_hdl_t comm_dev_resource_hdl);

/** SDI COMM DEV - Read write comm Message Buffer readiness
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[out] ready; true/false is message buffer ready
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_is_write_buffer_ready(sdi_resource_hdl_t comm_dev_resource_hdl, bool *ready);

/** SDI COMM DEV - Read comm Message presence status
 * @param[in] host_system_resource_hdl; handle of the resource
 * @param[out] presence; true/false is message present
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_is_msg_present(sdi_resource_hdl_t host_system_resource_hdl, bool *presence);



/** SDI COMM DEV - Comm Dev messaging enable
 * @param[in] comm_dev_resource_hdl; handle of the resource
 * @param[in] booted; true/false booted flag value
 * @return Success: STD_ERR_OK,
 *         Failure: SDI_ERRCODE(EPERM)
 */
t_std_error sdi_comm_dev_messaging_enable(sdi_resource_hdl_t comm_dev_resource_hdl, bool messaging_enable);


#ifdef __cplusplus
}
#endif
#endif   /* __SDI_COMM_DEV_H_ */
