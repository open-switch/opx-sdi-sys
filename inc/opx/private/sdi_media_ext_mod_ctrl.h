/*
 * Copyright (c) 2019 Dell Inc.
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

/*
 * filename: sdi_media_ext_mod_ctrl.h
 */


#ifndef _SDI_MEDIA_EXT_MOD_CTRL_H_
#define _SDI_MEDIA_EXT_MOD_CTRL_H_

#include "sdi_media.h"
#include "sdi_sfp.h"
#include "sdi_resource_internal.h"
#include "sdi_device_common.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_i2c_bus_api.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include "std_time_tools.h"
#include <stdlib.h>
#include <string.h>



#define EXT_MEM_SIZE                  6
#define OPER_MASK                     3
#define DATA_BUF_SIZE                 2
#define STATUS_POLL_IDLE_STATE        0
#define STATUS_POLL_COMPLETE_STATE    1
#define STATUS_POLL_ERROR_STATE       2
#define STATUS_POLL_MAX_TIMEOUT_MS    100
#define STATUS_POLL_INTERVAL_MS    (STATUS_POLL_MAX_TIMEOUT_MS/10)

#define STATUS_OPER_COMPLETE_BITMASK    ((1 << 2) | (1 << 3))

typedef enum{
    AQ_TX_STATE_SET_ADDR = 0x1,
    AQ_CURR_LINK_STATE_ADDR = 0x1,
    AQ_SYS_STATUS_GET_ADDR = 0x4,
    AQ_LINE_SIDE_LINK_RATE_ADDR = 0x7,
    AQ_RATE_SET_MEM_ADDR = 0x7,
    AQUANTIA_10G_T = 7,
} ext_mem_addr_t;

typedef enum{

    MEM_READ    = 1,
    MEM_WRITE   = 2
} ext_mem_oper_t;

typedef struct {
    ext_mem_addr_t mem_addr;
    uint16_t reg_offset;
    uint16_t data_buf_big_endian;
    ext_mem_oper_t oper;
} ext_dev_ctrl_oper_packet_t;

t_std_error sdi_media_ext_mod_ctrl_chain_oper (sdi_resource_hdl_t resource_hdl, ext_dev_ctrl_oper_packet_t* packets, size_t num_packets);

#endif
