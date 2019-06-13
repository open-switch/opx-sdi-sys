/*
 * Copyright (c) 2019 Dell Inc.
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
 * filename: sdi_sfp_plus_aq.c
 */


/******************************************************************************
 * sdi_sfp_plus_aq.c
 * Implements the SFP+ APIs for using Aquantia 10G BASE T media, based on ext mod control 
 *****************************************************************************/

/* This uses the extended module control for implementing basic features of Aquantia's 10G BASE T module */

#include "sdi_media_ext_mod_ctrl.h"
#include "sdi_sfp_plus_aq.h"


#define ARR_SIZE(x)                   (sizeof(x))/(sizeof(x[0]))


#define AQ_TX_STATE_SET_BITMASK       1
#define AQ_SYS_STATUS_IF_RX_BITMASK       (1 << 0x0D)
#define AQ_SYS_STATUS_IF_TX_BITMASK       (1 << 0x0C)
#define AQ_SYS_STATUS_IF_SPEED_BITMASK    (0x0F << 0x08)
#define AQ_SYS_STATUS_IF_SPEED_GET(x) ((AQ_SYS_STATUS_IF_SPEED_BITMASK & (x)) >> 0x08)

#define AQ_LINK_STATE_BITMASK         0x01
#define AQ_LINE_SIDE_LINK_RATE_ADDR   0x07
#define AQ_LINE_SIDE_SPEED_BITMASK    (7<<1)
#define AQ_LINE_SIDE_SPEED_GET(x)     ((AQ_LINE_SIDE_SPEED_BITMASK & (x)) >>  0x01)

#define AQ_SPEED_SET_SERDES_ON_DELAY_US   (800 * 1000)

/* Set of operations needed to set certain speeds */
/* This 10g actually advertises all other rates */
/*static ext_dev_ctrl_oper_packet_t aq_10g_multigig_rate_set_packets[] =
        {{mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x2000, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0010, data_buf_big_endian: 0x9101, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0xC400, data_buf_big_endian: 0x9C40, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0020, data_buf_big_endian: 0x00A1, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x3000, oper: MEM_WRITE}};
*/
/* Advertise 10g only: Ideally, this should never be used because we want the far end to be aware of other speeds, however link flap issues are seen when using 10g mutirate (aq_10g_multigig_rate_set_packets) */
static ext_dev_ctrl_oper_packet_t aq_10g_rate_set_packets[] =
        {{mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x2000, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0010, data_buf_big_endian: 0x9001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0xC400, data_buf_big_endian: 0x0040, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0020, data_buf_big_endian: 0x1001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x3000, oper: MEM_WRITE}};

/* Advertise 1g only*/
static ext_dev_ctrl_oper_packet_t aq_1g_rate_set_packets[] =
        {{mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x2000, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0010, data_buf_big_endian: 0x8001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0xC400, data_buf_big_endian: 0x8040, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0020, data_buf_big_endian: 0x0001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x3000, oper: MEM_WRITE}};

/* Advertise 100m only*/
static ext_dev_ctrl_oper_packet_t aq_100m_rate_set_packets[] =
        {{mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x2000, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0010, data_buf_big_endian: 0x0101, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0xC400, data_buf_big_endian: 0x0040, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0020, data_buf_big_endian: 0x0001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x3000, oper: MEM_WRITE}};

/* Advertise 2.5g only*/
static ext_dev_ctrl_oper_packet_t aq_2500m_rate_set_packets[] =
        {{mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x2000, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0010, data_buf_big_endian: 0x9001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0xC400, data_buf_big_endian: 0x0040, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0020, data_buf_big_endian: 0x00A1, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x3000, oper: MEM_WRITE}};

/* Advertise 5g only*/
static ext_dev_ctrl_oper_packet_t aq_5g_rate_set_packets[] =
        {{mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x2000, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0010, data_buf_big_endian: 0x9001, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0xC400, data_buf_big_endian: 0x0040, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0020, data_buf_big_endian: 0x0141, oper: MEM_WRITE},
        {mem_addr: AQ_RATE_SET_MEM_ADDR, reg_offset: 0x0000, data_buf_big_endian: 0x3000, oper: MEM_WRITE}};

t_std_error sdi_media_aq_set_tx_state (sdi_resource_hdl_t resource_hdl, bool state);
t_std_error sdi_media_aq_get_line_side_status (sdi_resource_hdl_t resource_hdl, bool *link_status, sdi_media_speed_t *link_speed_rate);

/* This is for setting lineside speed */
t_std_error sdi_media_aq_set_rate (sdi_resource_hdl_t resource_hdl, sdi_media_speed_t rate)
{
    t_std_error rc          = STD_ERR_OK;
    sdi_device_hdl_t device = NULL;
    bool link_stat = false;
    bool serdes_stat = false;
    sdi_media_speed_t curr_speed = SDI_MEDIA_INVALID_SPEED;
    ext_dev_ctrl_oper_packet_t* packets = NULL;
    size_t size = 0;

    STD_ASSERT(resource_hdl != NULL);
    device = (sdi_device_hdl_t)resource_hdl;

    rc = sdi_media_aq_get_line_side_status(resource_hdl, &link_stat, &curr_speed);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Error %u when checking current speed and link status, while attempting speed set on module %s", rc, device->alias);
        return rc;
    }

    if (curr_speed ==  rate){
        /* Nothing to do */
        return rc;
    }

    rc = sdi_media_aq_get_tx_state(resource_hdl, &serdes_stat);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Error %u when checking serdes status, while attempting speed set on module %s", rc, device->alias);
        return rc;
    }


    /* Before manipulating speed, turn off serdes */
    /* Will restore state later */
    rc = sdi_media_aq_set_tx_state(resource_hdl, false);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Error %u when turning off serdes when attempting speed set on module %s", rc, device->alias);
        return rc;
    }

    /* Sleep to allow cfg to settle*/
    std_usleep(AQ_SPEED_SET_SERDES_ON_DELAY_US);

    switch (rate){
        case SDI_MEDIA_SPEED_100M:
            packets = aq_100m_rate_set_packets;
            size = ARR_SIZE(aq_100m_rate_set_packets);
            break;
        case SDI_MEDIA_SPEED_1G:
            packets = aq_1g_rate_set_packets;
            size = ARR_SIZE(aq_1g_rate_set_packets);
            break;
        case SDI_MEDIA_SPEED_2500M:
            packets = aq_2500m_rate_set_packets;
            size = ARR_SIZE(aq_2500m_rate_set_packets);
            break;
        case SDI_MEDIA_SPEED_5G:
            packets = aq_5g_rate_set_packets;
            size = ARR_SIZE(aq_5g_rate_set_packets);
            break;
        case SDI_MEDIA_SPEED_10G:
        default:
            packets = aq_10g_rate_set_packets;
            size = ARR_SIZE(aq_10g_rate_set_packets);
            break;
    }

    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, packets, size);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Could not set rate on Aquantia media on module %s", device->alias);
        return rc;
    }
    std_usleep(AQ_SPEED_SET_SERDES_ON_DELAY_US);

    /* Wait a bit and then turn serdes on if it was previously on */
    if (serdes_stat){
        rc = sdi_media_aq_set_tx_state(resource_hdl, true);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("AQ: Error %u when turning on serdes after speed set on module %s", rc, device->alias);
        }
    }
    return rc;
}
/* This gets the system (NPU) side status. Note that there may be a disparity between lineside and system side  */
t_std_error sdi_media_aq_get_system_side_status (sdi_resource_hdl_t resource_hdl, bool *rx_if_link_status, bool *tx_if_link_status,
                    sdi_media_speed_t *if_speed_rate)
{
    sdi_device_hdl_t device = NULL;
    t_std_error rc          = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(rx_if_link_status != NULL);
    STD_ASSERT(tx_if_link_status != NULL);
    STD_ASSERT(if_speed_rate != NULL);

    device = (sdi_device_hdl_t)resource_hdl;

    *if_speed_rate = SDI_MEDIA_INVALID_SPEED;
    ext_dev_ctrl_oper_packet_t status_packet = {mem_addr: AQ_SYS_STATUS_GET_ADDR, reg_offset: 0xE812, data_buf_big_endian: 0, oper: MEM_READ};

    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, &status_packet, 1);

    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: System side status poll failed for module %s", device->alias);
        return rc;
    }
    *rx_if_link_status = (bool)(AQ_SYS_STATUS_IF_RX_BITMASK & status_packet.data_buf_big_endian);
    *tx_if_link_status = (bool)(AQ_SYS_STATUS_IF_TX_BITMASK & status_packet.data_buf_big_endian);

    switch (AQ_SYS_STATUS_IF_SPEED_GET(status_packet.data_buf_big_endian)){
        case 0x01:
            *if_speed_rate = SDI_MEDIA_SPEED_100M;
            break;
        case 0x02:
            *if_speed_rate = SDI_MEDIA_SPEED_1G;
            break;
        case 0x03:
            *if_speed_rate = SDI_MEDIA_SPEED_10G;
            break;
        case 0x04:
            *if_speed_rate = SDI_MEDIA_SPEED_2500M;
            break;
        case 0x05:
            *if_speed_rate = SDI_MEDIA_SPEED_5G;
            break;
        case 0x00:
            /* No speed */
            break;
        default:
            rc = ~STD_ERR_OK;
            SDI_DEVICE_ERRMSG_LOG("AQ: Found unknown system-side speed on module %s", device->alias);
            break;
    }
    return rc;
}

/* To set the tx state (which is inverted), only one bit needs to be set. However this bit is in a byte that contains other info that must not be changed */
/* Hence a read-modify-write is used to set the tx state bit */
t_std_error sdi_media_aq_set_tx_state (sdi_resource_hdl_t resource_hdl, bool state)
{

    sdi_device_hdl_t device = NULL;
    t_std_error rc          = STD_ERR_OK;
    ext_dev_ctrl_oper_packet_t tx_state_packet = {mem_addr: AQ_TX_STATE_SET_ADDR, reg_offset: 0x0009, data_buf_big_endian: 0, oper: MEM_READ};

    STD_ASSERT(resource_hdl != NULL);

    device = (sdi_device_hdl_t)resource_hdl;

    /* Read */
    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, &tx_state_packet, 1);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Error %u while reading tx state module %s", rc, device->alias);
        return rc;
    }
    /* Modify */
    /* TX state control is inverted */
    state ? (tx_state_packet.data_buf_big_endian &= ~AQ_TX_STATE_SET_BITMASK) : (tx_state_packet.data_buf_big_endian |= AQ_TX_STATE_SET_BITMASK);

    /* Write */
    tx_state_packet.oper = MEM_WRITE;
    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, &tx_state_packet, 1);

    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Failed to set tx state to %d for module %s", state, device->alias);
    }
    return rc;
}

/* Same location used in sdi_media_aq_set_tx_state; except operation is a read */
t_std_error sdi_media_aq_get_tx_state (sdi_resource_hdl_t resource_hdl, bool *state)
{

    sdi_device_hdl_t device = NULL;
    t_std_error rc          = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(state != NULL);

    device = (sdi_device_hdl_t)resource_hdl;
    ext_dev_ctrl_oper_packet_t tx_state_packet = {mem_addr: AQ_TX_STATE_SET_ADDR, reg_offset: 0x0009, data_buf_big_endian: 0x0000, oper: MEM_READ};

    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, &tx_state_packet, 1);

    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: Failed to get tx state for module %s", device->alias);
    } else {
        /* TX state in inverted */
        *state = !((bool)(tx_state_packet.data_buf_big_endian & AQ_TX_STATE_SET_BITMASK));
    }
    return rc;
}

/* This gets the lineside status, which includes lineside speed and link status. Both are at different locations */
t_std_error sdi_media_aq_get_line_side_status (sdi_resource_hdl_t resource_hdl, bool *link_status, sdi_media_speed_t *link_speed_rate)
{
    sdi_device_hdl_t device = NULL;
    t_std_error rc          = STD_ERR_OK;
    ext_dev_ctrl_oper_packet_t link_state_packet = {mem_addr: AQ_CURR_LINK_STATE_ADDR, reg_offset: 0xE800, data_buf_big_endian: 0, oper: MEM_READ};

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(link_status != NULL);
    STD_ASSERT(link_speed_rate != NULL);

    device = (sdi_device_hdl_t)resource_hdl;
    *link_speed_rate = SDI_MEDIA_INVALID_SPEED;
    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, &link_state_packet, 1);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Error while reading lineside link status on module %s", device->alias);
        return rc;
    }
    *link_status = (bool)(link_state_packet.data_buf_big_endian & AQ_LINK_STATE_BITMASK);

    link_state_packet.mem_addr = AQ_LINE_SIDE_LINK_RATE_ADDR;
    link_state_packet.reg_offset = 0xC800;

    rc = sdi_media_ext_mod_ctrl_chain_oper(resource_hdl, &link_state_packet, 1);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Error while reading lineside link speed on module %s", device->alias);
        return rc;
    }

    switch (AQ_LINE_SIDE_SPEED_GET(link_state_packet.data_buf_big_endian)){
        case 0x01:
            *link_speed_rate = SDI_MEDIA_SPEED_100M;
            break;
        case 0x02:
            *link_speed_rate = SDI_MEDIA_SPEED_1G;
            break;
        case 0x03:
            *link_speed_rate = SDI_MEDIA_SPEED_10G;
            break;
        case 0x04:
            *link_speed_rate = SDI_MEDIA_SPEED_2500M;
            break;
        case 0x05:
            *link_speed_rate = SDI_MEDIA_SPEED_5G;
            break;
        case 0x00:
            /* No speed */
            break;
        default:
            rc = ~STD_ERR_OK;
            SDI_DEVICE_ERRMSG_LOG("AQ: Found unknown line-side speed on module %s", device->alias);
            break;
    }
    return rc;
}

/* This gets the overall link status */
t_std_error sdi_media_aq_get_link_status(sdi_resource_hdl_t resource_hdl, bool* status)
{
    sdi_device_hdl_t device = NULL;
    t_std_error rc          = STD_ERR_OK;
    bool link_status_ls = false, link_status_ss_tx = false, link_status_ss_rx = false;
    sdi_media_speed_t speed_ls = SDI_MEDIA_INVALID_SPEED, speed_ss = SDI_MEDIA_INVALID_SPEED;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    device = (sdi_device_hdl_t)resource_hdl;
    /* For link to really be up, lineside and system side links need to be up, and speeds must match */

    rc = sdi_media_aq_get_line_side_status(resource_hdl, &link_status_ls, &speed_ls);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: lineside status read failed for module %s",  device->alias);
        *status = false;
        return rc;
    }

    rc = sdi_media_aq_get_system_side_status(resource_hdl, &link_status_ss_rx, &link_status_ss_tx, &speed_ss);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("AQ: system side status read failed for module %s",  device->alias);
        *status = false;
        return rc;
    }

    *status = (speed_ls == speed_ss) && (link_status_ss_tx && link_status_ss_rx && link_status_ls);

    return rc;
}

