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
 * filename: sdi_comm_dev_internal.h
 * Internal comm_dev resource related declarations.
 */

#ifndef __SDI_COMM_DEV_INTERNAL_H_
#define __SDI_COMM_DEV_INTERNAL_H_

#include "std_error_codes.h"
#include "sdi_driver_internal.h"

/******************************************************************************
 * @file sdi_comm_dev_internal.h
 * @brief Defines SDI FReD IO APIs
 * @{
 *****************************************************************************/

/*
 * Each comm_dev resource provides the following callbacks.
 */

typedef struct {
    /* For reading mailbox with I2C bus */
    t_std_error (*read_mbox)(sdi_resource_hdl_t resource_hdl, uint8_t *regData);

    /* For writing mailbox with I2C bus */
    t_std_error  (*write_mbox)(sdi_resource_hdl_t resource_hdl, uint16_t len, uint8_t *regData);

    /* For writing SB mbox package read & verified with I2C bus */
    t_std_error  (*write_pckg_read_verif)(sdi_resource_hdl_t resource_hdl);

    /* For writing NB mbox package download complete with I2C bus */
    t_std_error  (*write_pckg_downl_compl)(sdi_resource_hdl_t resource_hdl);

    /* For reading/writing Host System FW revision with I2C bus */
    t_std_error  (*access_fw_rev)(sdi_resource_hdl_t resource_hdl, bool writeFlag, uint8_t *regData);

    /* For writing temperature to sensor 06 telemetry with I2C bus */
    t_std_error  (*write_temp_sensor)(sdi_resource_hdl_t resource_hdl, int regData);

    /* For writing temperature to sensor 06 telemetry with I2C bus */
    t_std_error  (*write_npu_temp_sensor)(sdi_resource_hdl_t resource_hdl, int regData);

    /* For writing temperature to sensor 07 telemetry with I2C bus */
    t_std_error  (*write_ambient_temp_sensor)(sdi_resource_hdl_t resource_hdl, int regData);

    /* For reading static platform info with I2C bus */
    t_std_error  (*read_platform_info)(sdi_resource_hdl_t resource_hdl, sdi_platform_info_t *platform_info);

    /* For flushing write message buffer with I2C bus */
    t_std_error  (*flush_msg_buffer)(sdi_resource_hdl_t resource_hdl);

    /* For reading write buffer readiness info with I2C bus */
    t_std_error  (*get_buffer_ready)(sdi_resource_hdl_t resource_hdl, bool *ready);

    /* For Enabling messaging over comm dev */
    t_std_error  (*messaging_enable)(sdi_resource_hdl_t resource_hdl, bool messaging_enable);

} comm_dev_ctrl_t;

/*
 * FReD Temperature sensors device private data
 */
typedef struct sdi_comm_dev_ext_ctrl_device
{
    uint16_t ctrl_value_offset;
    uint16_t width;
    uint16_t step;
    bool     refresh;

    /* Default sensor limits */
    uint16_t default_ctrl_value_offset;
    uint16_t default_width;
    uint16_t default_step;
    bool     default_refresh;

    bool ctrl_value_offset_valid;
    bool width_valid;
    bool step_valid;
    bool refresh_valid;

    sdi_resource_hdl_t comm_dev;
} sdi_comm_dev_ext_ctrl_device_t;

#define SDI_COMM_DEV_MAX_CTRL_DATA_LEN (2)


/**
 * @brief sdi_comm_dev_read
 * Write given byte data to FReD IO
 * @param[in] resource_hdl
 * @param[in] i2c_addr sdi i2c address
 * @param[in] offset
 * @param[in] len to read
 * @param[out] value data to be written to 
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_comm_dev_read(sdi_resource_hdl_t resource_hdl, uint8_t i2c_addr, uint16_t offset, uint16_t len, uint8_t *data);

/**
 * @brief sdi_comm_dev_write
 * Write given byte data to FReD IO
 * @param[in] resource_hdl
 * @param[in] i2c_addr sdi i2c address
 * @param[in] offset
 * @param[in] len to read
 * @param[out] value data to be written
 * @return STD_ERR_OK on SUCCESS, SDI_ERRNO on FAILURE
 */
t_std_error sdi_comm_dev_write(sdi_resource_hdl_t resource_hdl, uint8_t i2c_addr, uint16_t offset, uint16_t len, uint8_t *data);

/**
 * @brief sdi_comm_dev_reset_refresh
 * refresh the resource_hdl
 * @param[in] resource_hdl
 */
t_std_error sdi_comm_dev_reset_refresh(sdi_resource_hdl_t resource_hdl);

/**
 * @}
 */

#endif
