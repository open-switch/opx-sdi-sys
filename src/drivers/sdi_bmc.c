/*
 * Copyright (c) 2018 Dell EMC.
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
 * filename: sdi_bmc.c 
 */

#include "sdi_bmc.h"
#include "sdi_bmc_db.h"
#include "sdi_bmc_internal.h"
#include "std_bit_ops.h"
#include "std_bit_masks.h"
#include "std_condition_variable.h"
#include "std_thread_tools.h"
#include "sdi_bmc_bus_api.h"
#include "sdi_entity_internal.h"
#include "sdi_device_common.h"
#include "std_time_tools.h"
#include "std_utils.h"
#include "std_assert.h"

#include <OpenIPMI/ipmiif.h>
#include <OpenIPMI/ipmi_smi.h>
#include <OpenIPMI/ipmi_err.h>
#include <OpenIPMI/ipmi_posix.h>
#include <OpenIPMI/ipmi_fru.h>
#include <OpenIPMI/ipmi_mc.h>
#include <OpenIPMI/internal/ipmi_domain.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static ipmi_domain_id_t domain_id;

static int32_t sdi_bmc_entity_presence_handler (ipmi_entity_t *entity, int32_t present,
                                                void *cb_data, ipmi_event_t  *event);
static sdi_bmc_sensor_t *sdi_bmc_sensor_create (ipmi_entity_t *entity, ipmi_sensor_t *sensor);
sdi_bmc_sensor_t *sdi_bmc_add_entity_fru_info (ipmi_entity_t *entity);

std_dll_head *oem_poller_head;

/**
 * sdi_bmc_sensor_threshold_event_handler is a callback function which is used to 
 * register for threshold events with openipmi library. It will update current sensor
 * data in local sensor database.
 */

static
int sdi_bmc_sensor_threshold_event_handler(ipmi_sensor_t *sensor,
                                    enum ipmi_event_dir_e dir, enum ipmi_thresh_e threshold,
                                    enum ipmi_event_value_dir_e high_low,
                                    enum ipmi_value_present_e value_present,
                                    uint32_t raw_value, double value, void *cb_data,
                                    ipmi_event_t *event)
{
    ipmi_entity_t      *ent = ipmi_sensor_get_entity(sensor);
    uint32_t           id, instance;
    char               name[SDI_MAX_NAME_LEN];
    sdi_bmc_sensor_t   *sen = NULL;

    id = ipmi_entity_get_entity_id(ent);
    instance = ipmi_entity_get_entity_instance(ent);
    ipmi_sensor_get_id(sensor, name, sizeof(name) );

    sen = sdi_bmc_db_sensor_get(id, instance, name);
    if (sen != NULL) {
        switch (value_present)
        {
            case IPMI_NO_VALUES_PRESENT:
                SDI_DEVICE_TRACEMSG_LOG("Threshold event handling No value present.");
                break;
            case IPMI_BOTH_VALUES_PRESENT:
                sen->res.reading.data = value;
                sen->res.reading.raw_data = raw_value;
                break;
            case IPMI_RAW_VALUE_PRESENT:
                sen->res.reading.raw_data = raw_value;
                break;
            default:
                SDI_DEVICE_TRACEMSG_LOG("Invalid value_present data.");
                break;
        }
    }

    return IPMI_EVENT_HANDLED;
}

/**
 * sdi_bmc_sensor_discrete_event_handler is a callback function which is used to
 * register for  discrete state changes with openipmi library. It will update current
 * sensor data in local sensor database.
 */

static int
sdi_bmc_sensor_discrete_event_handler (ipmi_sensor_t *sensor, enum ipmi_event_dir_e dir,
                                       int offset, int severity, int prev_severity,
                                       void *cb_data, ipmi_event_t *event)
{
    ipmi_entity_t   *ent = ipmi_sensor_get_entity(sensor);
    int             id, instance;
    char            name[SDI_MAX_NAME_LEN] = "";

    id = ipmi_entity_get_entity_id(ent);
    instance = ipmi_entity_get_entity_instance(ent);
    ipmi_sensor_get_id(sensor, name, sizeof(name));

    /*
     * todo -- add discrete sensor event handling logic based on sdi resource type.
     */
    SDI_DEVICE_TRACEMSG_LOG("Event for Discrete sensor (%d.%d.%s)", id, instance, name);
    return IPMI_EVENT_HANDLED;
}

/**
 * sdi_bmc_sensor_discrete_states is a callback function to read and update 
 * discrete states of sensor.
 */

static void
sdi_bmc_sensor_discrete_states (ipmi_sensor_t *sensor, int err,
                                       ipmi_states_t *states, void *cb_data)
{
    ipmi_entity_t     *ent = ipmi_sensor_get_entity(sensor);
    uint32_t          id, instance;
    char              name[SDI_MAX_NAME_LEN] = "";
    sdi_bmc_sensor_t  *sen = NULL;

    id = ipmi_entity_get_entity_id(ent);
    instance = ipmi_entity_get_entity_instance(ent);
    ipmi_sensor_get_id(sensor, name, sizeof(name));

    sen = sdi_bmc_db_sensor_get(id, instance, name);
    if (sen == NULL) {
         SDI_DEVICE_TRACEMSG_LOG("Sensor discrete event handling failed (%d - %d : %s)",
                 id, instance, name);
        return;
    }

    uint32_t bit;
    sen->res.reading.discrete_state = 0;
    for (bit = 0; bit < sizeof(sen->res.reading.discrete_state) * BITS_PER_BYTE ; bit++) {
        int val, rv;
        rv = ipmi_sensor_discrete_event_readable(sensor, bit, &val);
        if ((rv != 0) || (val == 0))
            continue;
        if (ipmi_is_state_set(states, bit)) {
            STD_BIT_SET(sen->res.reading.discrete_state, bit);
        }
    }
    return;
}

/**
 * sdi_bmc_sensor_event_set_done is a callback function used by openipmi library
 * to notify applications about the status of event config.
 */

static void sdi_bmc_sensor_event_set_done (ipmi_sensor_t *sensor, int err, void *cb_data)
{
    sdi_bmc_sensor_t *srp = (sdi_bmc_sensor_t *) cb_data;

    if (err != 0) {
        SDI_DEVICE_ERRMSG_LOG("Error 0x%x setting events for sensor : %s", err, srp->name);
    }
    return;
}

/**
 * sdi_bmc_sensor_enable_events is callback function to enable events and scanning.
 */

static void sdi_bmc_sensor_enable_events(ipmi_sensor_t *sensor, int err, ipmi_event_state_t *states,
                       void *cb_data)
{
    int32_t rv = 0;
    sdi_bmc_sensor_t *srp = (sdi_bmc_sensor_t *) cb_data;

    if (srp->ev_state != NULL) {
        ipmi_event_state_set_events_enabled(srp->ev_state, 1);
        ipmi_event_state_set_scanning_enabled(srp->ev_state, 1);
        rv = ipmi_sensor_set_event_enables(sensor, srp->ev_state,
                sdi_bmc_sensor_event_set_done, srp);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("Error 0x%x enabling events for sensor : %s", err, srp->name);
        }
    }
    return;
}

/**
 * Sensor update handler callback function.
 */
static void sdi_bmc_sensor_update_handler (enum ipmi_update_e op, ipmi_entity_t *entity,
                                           ipmi_sensor_t *sensor, void *cb_data)
{
    uint32_t          id, instance;
    char              name[SDI_MAX_NAME_LEN] = "";
    sdi_bmc_sensor_t  *srp = NULL;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);
    ipmi_sensor_get_id(sensor, name, sizeof(name));

    if (op == IPMI_ADDED) {
        if ((srp = sdi_bmc_sensor_create(entity, sensor)) == NULL) {
            SDI_DEVICE_ERRMSG_LOG("Creating and adding sensor failed: %d - %d - %s",
                    id, instance, name);
        }
    }

}

static void oem_resp_handler (ipmi_mc_t  *src, ipmi_msg_t *rsp, void *rsp_data)
{
    if (src == NULL) {
        SDI_DEVICE_ERRMSG_LOG(" oem_call_back src is NULL");
        return;
    }
    sdi_bmc_oem_cmd_info_t *oem_cmd = (sdi_bmc_oem_cmd_info_t *)rsp_data;
    if (rsp->data[0] != 0) {
        SDI_DEVICE_ERRMSG_LOG("BMC OEM Failed (%d) for netfn %d:%d - %d:%d:%d", rsp->data[0],
            oem_cmd->netfn, oem_cmd->cmd, oem_cmd->bus_id, oem_cmd->slave_addr, oem_cmd->offset);
        return;
    }
    for (uint32_t i = 0; i < oem_cmd->data_size; i++) {
        oem_cmd->resp_data[i]  = rsp->data[i+1];
    }
    SDI_DEVICE_TRACEMSG_LOG("OEM Resp (%d %d) netfn %d:%d - %d:%d:%d", rsp->data[0], rsp->data[1],
            oem_cmd->netfn, oem_cmd->cmd, oem_cmd->bus_id, oem_cmd->slave_addr, oem_cmd->offset);
}

/*
 * Execute OEM specific IPMI command 
 */
t_std_error sdi_bmc_oem_cmd_execute(sdi_bmc_oem_cmd_info_t *oem_cmd, uint8_t *data)
{
    ipmi_msg_t msg;

    if (oem_cmd->data_size > SDI_BMC_OEM_DATA_LEN) {
        return SDI_ERRCODE(EMSGSIZE);
    }
    msg.netfn = oem_cmd->netfn;
    msg.cmd = oem_cmd->cmd;
    msg.data_len = (oem_cmd->data)? 1 : 4 + (data != NULL)? oem_cmd->data_size : 0;
    msg.data = oem_cmd->req_data;
    if (oem_cmd->data == 0) {
        msg.data[0] = oem_cmd->bus_id;
        msg.data[1] = oem_cmd->slave_addr;
        msg.data[2] = oem_cmd->data_size;
        msg.data[3] = oem_cmd->offset;
    } else {
        msg.data[0] = oem_cmd->data;
    }
    if (data != NULL) {
        for(size_t i = 0; i < oem_cmd->data_size; i++) {
            msg.data[4+i] = data[i];
        }
    }

    int        rv;
    ipmi_system_interface_addr_t si;
    ipmi_mc_t                    *si_mc;

    si.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
    si.channel = 0xf;
    si.lun = 0;
    si_mc = _ipmi_find_mc_by_addr(domain_id.domain, (ipmi_addr_t *) &si, sizeof(si));

    if (!si_mc) {
        SDI_DEVICE_ERRMSG_LOG("_ipmi_find_mc_by_addr returned null.");
        return SDI_ERRCODE(EINVAL);
    }

    rv = ipmi_mc_send_command(si_mc, 0, &msg, (!data)? oem_resp_handler : NULL, (void *)oem_cmd);

    SDI_DEVICE_TRACEMSG_LOG("ipmi_mc_send_command Return val: %d", rv);
    return  (rv != 0)? SDI_ERRCODE(EIO) : STD_ERR_OK;
}

static int sdi_bmc_ipmi_traverse_fru_node_tree (ipmi_fru_node_t *node,
                                                sdi_bmc_sensor_t *srp, void *cb_data)
{
    const char                *name;
    unsigned int              i;
    enum ipmi_fru_data_type_e dtype;
    int                       intval, rv;
    time_t                    time;
    double                    floatval;
    char                      *data;
    unsigned int              data_len;
    ipmi_fru_node_t           *sub_node;

    srp->res.entity_info.valid = true;
    for (i = 0; ; i++) {
        data = NULL;

        rv = ipmi_fru_node_get_field(node, i, &name, &dtype, &intval, &time,
                                     &floatval, &data, &data_len, &sub_node);
        if (rv == EINVAL) {
            break;
        } if (rv != 0) {
            continue;
        }

        sdi_bmc_dev_resource_info_t *res = sdi_bmc_dev_get_by_data_sdr(cb_data, srp->name);
        if (res->psu_type_oem_cmd != NULL) {
            res->psu_type_oem_cmd->resp_data = &srp->res.entity_info.type;
            if (sdi_bmc_oem_cmd_execute(res->psu_type_oem_cmd, NULL) != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("BMC OEM Command Execution failed");
            }
        }
        if (res->air_flow_oem_cmd != NULL) {
            res->air_flow_oem_cmd->resp_data = &srp->res.entity_info.air_flow;
            if (sdi_bmc_oem_cmd_execute(res->air_flow_oem_cmd, NULL) != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("BMC PSU AirFlow Retrival Failed");
            }
        }
        if (strcmp("internal_use", name) == 0) {
            uint32_t elm_data = 0;
            if (res != NULL) {
                if ((SDI_BMC_INVALID_OFFSET != res->int_use_elm_offset) && (res->int_use_elm_offset < data_len)) {
                    elm_data = data[res->int_use_elm_offset];
                    if (elm_data < res->hdr_sz_offset) {
                        SDI_DEVICE_ERRMSG_LOG("BMC [%d][%d]: elm_data 0x%x invalid for hsize 0x%x\n",
                                              i, srp->entity_instance, elm_data, res->hdr_sz_offset);
                    } else {
                        elm_data -= res->hdr_sz_offset; /* adjust the elm_data based on BMC header removal */
                    }
                    if ((SDI_BMC_INVALID_OFFSET != res->airflow_offset) && ((elm_data + res->airflow_offset) <= data_len)) {
                        srp->res.entity_info.air_flow = data[(elm_data + res->airflow_offset)];
                    }
                    if ((SDI_BMC_INVALID_OFFSET != res->psu_type_offset) && ((elm_data + res->psu_type_offset) <= data_len)) {
                        srp->res.entity_info.type = data[(elm_data + res->psu_type_offset)];
                        if ((srp->res.entity_info.type != BMC_AC_TYPE) && (srp->res.entity_info.type != BMC_DC_TYPE)) {
                            SDI_DEVICE_ERRMSG_LOG("BMC [%d][%d]: [int_use 0x%x][psu 0x%x] : elm 0x%x h_sz 0x%x ofs 0x%x type 0x%x len %d\n",
                                                  i, srp->entity_instance, res->int_use_elm_offset, res->psu_type_offset,
                                                  elm_data, res->hdr_sz_offset, (elm_data + res->psu_type_offset),
                                                  srp->res.entity_info.type, data_len);
                        }
                    }
                }
            }
        } else if (strncmp("board_info_board_manufacturer", name,
                    strlen("board_info_board_manufacturer")) == 0) {
            if (dtype == IPMI_FRU_DATA_ASCII) {
                safestrncpy(srp->res.entity_info.board_manufacturer, data,
                        sizeof(srp->res.entity_info.board_manufacturer));
            } else {
                SDI_DEVICE_TRACEMSG_LOG("BMC FRU vendor_name: invalid format.");
            }
        } else if (strncmp("board_info_board_product_name", name,
                    strlen("board_info_board_product_name")) == 0) {
            if (dtype == IPMI_FRU_DATA_ASCII) {
                safestrncpy(srp->res.entity_info.board_product_name, data,
                        sizeof(srp->res.entity_info.board_product_name));
            } else {
                SDI_DEVICE_TRACEMSG_LOG("BMC FRU prod_name: invalid format.");
            }
        } else if (strncmp("board_info_board_serial_number", name,
                    strlen("board_info_board_serial_number")) == 0) {
            if (dtype == IPMI_FRU_DATA_ASCII) {
                safestrncpy(srp->res.entity_info.board_serial_number, data,
                        sizeof(srp->res.entity_info.board_serial_number));
            } else {
                SDI_DEVICE_TRACEMSG_LOG("BMC FRU service_tag: invalid format.");
            }
        } else if (strncmp("board_info_board_part_number", name,
                    strlen("board_info_board_part_number")) == 0) {
            if (dtype == IPMI_FRU_DATA_ASCII) {
                safestrncpy(srp->res.entity_info.board_part_number, data,
                        sizeof(srp->res.entity_info.board_part_number));
            } else {
                SDI_DEVICE_TRACEMSG_LOG("BMC FRU part_number: invalid format.");
            }
        }
        if (data)
            ipmi_fru_data_free(data);
    }
    
    ipmi_fru_put_node(node);

    return 0;
}

void sdi_bmc_entity_info_dummy_populate (ipmi_entity_t *entity, sdi_bmc_sensor_t *srp)
{
    uint32_t         id, instance;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);

    if (srp != NULL) {
        srp->res.entity_info.valid = true;
        safestrncpy(srp->res.entity_info.board_manufacturer, "DELL",
                sizeof(srp->res.entity_info.board_manufacturer));
        safestrncpy(srp->res.entity_info.board_product_name, "DELL-PRODUCT",
                sizeof(srp->res.entity_info.board_product_name));
        snprintf(srp->res.entity_info.board_serial_number,
                sizeof(srp->res.entity_info.board_serial_number),
                "SN-EN-%d-IN-%d", id, instance);
        snprintf(srp->res.entity_info.board_part_number,
                sizeof(srp->res.entity_info.board_part_number),
                "PN-EN-%d-IN-%d", id, instance);
        srp->res.entity_info.air_flow = 0x0;
        srp->res.entity_info.type = BMC_AC_TYPE;
    }
}

static void sdi_bmc_ipmi_fru_update_handler (enum ipmi_update_e op,
                                             ipmi_entity_t     *entity,
                                             void              *cb_data)
{
    int             rv;
    ipmi_fru_t      *fru = ipmi_entity_get_fru(entity);
    const char      *type;
    ipmi_fru_node_t *node;
    sdi_bmc_sensor_t *srp = NULL;

    if (op == IPMI_ADDED) {
        if (fru == NULL) {
            SDI_DEVICE_TRACEMSG_LOG("No fru data, NULL");
            return;
        }
        if ((srp = sdi_bmc_add_entity_fru_info(entity)) == NULL) {
            SDI_DEVICE_TRACEMSG_LOG("Adding FRU data failed.");
            return;
        }
        rv = ipmi_fru_get_root_node(fru, &type, &node);
        if (rv != 0) {
            SDI_DEVICE_TRACEMSG_LOG("ipmi fru get root node failed.");
            return;
        }
        sdi_bmc_ipmi_traverse_fru_node_tree(node, srp, cb_data);
    }
    return;

}
static void  sdi_bmc_populate_sensor_for_poller()
{
    sdi_bmc_oem_poller_t *node = NULL;
    for(node = (sdi_bmc_oem_poller_t *)std_dll_getfirst(oem_poller_head); node != NULL;
        node = (sdi_bmc_oem_poller_t *)std_dll_getnext(oem_poller_head, (std_dll *)node)) {
        sdi_bmc_sensor_t *sensor = sdi_bmc_db_sensor_get_by_name(node->sensor);
        if (sensor == NULL) return;
        node->sensor_loc = &sensor->res.reading.discrete_state;
    }
    return;
}

/**
 * Entity update handler callback function.
 */

static void sdi_bmc_entity_update_handler (enum ipmi_update_e op, ipmi_domain_t *domain,
                                           ipmi_entity_t *entity, void *cb_data)
{
    int32_t          rv;
    uint32_t         id, instance;
    sdi_bmc_entity_t *ent;
    sdi_bmc_sensor_t *srp = NULL;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);

    if (op == IPMI_ADDED) {
        if ((ent = sdi_bmc_db_entity_add(id, instance)) == NULL) {
            SDI_DEVICE_ERRMSG_LOG("sdi bmc db entity add failed (%d,%d)", id, instance);
            return;
        }
        rv = ipmi_entity_add_sensor_update_handler(entity, sdi_bmc_sensor_update_handler, NULL);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("ipmi entity add sensor update handler failed :0x%x", rv);
        }
        rv = ipmi_entity_add_presence_handler(entity, sdi_bmc_entity_presence_handler, ent);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("ipmi_entity_add_presence_handler: 0x%x", rv);
        }
        ent->type = ipmi_entity_get_entity_id(entity);
        ent->sdi_type = sdi_bmc_sdi_entity_type_get(ent->type);
        if ((ent->type == IPMI_ENTITY_ID_POWER_SUPPLY)
                || (ent->type == IPMI_ENTITY_ID_FAN_COOLING)) {
            srp = sdi_bmc_add_entity_fru_info(entity);
            if ((srp != NULL) && (srp->res.entity_info.is_dummy == true)) {
                sdi_bmc_entity_info_dummy_populate(entity, srp);
            } else {

                rv = ipmi_entity_add_fru_update_handler(entity,
                        sdi_bmc_ipmi_fru_update_handler, cb_data);
                if (rv != 0) {
                    SDI_DEVICE_ERRMSG_LOG("ipmi_entity_add_fru_update_handler: 0x%x", rv);
                }
            }
        }
    }
    sdi_bmc_populate_sensor_for_poller();

}

/**
 * Threshold sensor reading callback function.
 */

static void sdi_bmc_sensor_reading_handler (ipmi_sensor_t *sensor,
                                int err, enum ipmi_value_present_e value_present,
                                unsigned int raw_value, double val, ipmi_states_t *states,
                                void *cb_data)
{
    ipmi_entity_t    *ent = ipmi_sensor_get_entity(sensor);
    sdi_bmc_sensor_t *sen = NULL;
    uint32_t         id, instance;
    char             name[SDI_MAX_NAME_LEN] = "";

    id = ipmi_entity_get_entity_id(ent);
    instance = ipmi_entity_get_entity_instance(ent);
    ipmi_sensor_get_id(sensor, name, sizeof(name));

    sen = sdi_bmc_db_sensor_get(id, instance, name);
    if (sen != NULL) {
        switch (value_present)
        {
            case IPMI_NO_VALUES_PRESENT:
                SDI_DEVICE_TRACEMSG_LOG("No value present.\n");
                break;
            case IPMI_BOTH_VALUES_PRESENT:
                sen->res.reading.data = val;
                sen->res.reading.raw_data  = raw_value;
                break;
            case IPMI_RAW_VALUE_PRESENT:
                sen->res.reading.raw_data  = raw_value;
                break;
            default:
                SDI_DEVICE_TRACEMSG_LOG("Invalid value_present data.");
                break;
        }
    }
}

/**
 * Threshold reading callback function. Its will update threshold data of a sensor in 
 * sensor database.
 */
static void sdi_bmc_got_thresholds (ipmi_sensor_t *sensor, int err, ipmi_thresholds_t *th, void *cb_data)
{
    sdi_bmc_sensor_t   *srp = NULL;
    enum ipmi_thresh_e threshold;
    int                rv, value;
    double             dvalue;

    if (err != 0) {
        SDI_DEVICE_TRACEMSG_LOG("Error in getting thresholds.");
        return;
    }

    if (cb_data != NULL) {
        srp = (sdi_bmc_sensor_t *) cb_data;
        for (threshold = IPMI_LOWER_NON_CRITICAL;
                threshold <= IPMI_UPPER_NON_RECOVERABLE;
                threshold++) {
            rv = ipmi_sensor_threshold_readable(sensor, threshold, &value);
            if ((rv != 0) || (value == 0)) {
                continue;
            }
            rv = ipmi_threshold_get(th, threshold, &dvalue);
            if (rv != 0) {
                SDI_DEVICE_TRACEMSG_LOG("Not able to feth the threshold for %s, 0x%x",
                        ipmi_get_threshold_string(threshold), rv);
            } else {
                srp->threshold[threshold] = dvalue;
            }
        }
    }
    return;
}

/**
 * sdi_bmc_sensor_create is to create and add the sensor in sensor database and
 * register callbacks for data reading / discrete states and threshold events
 * or discrete events.
 */
static sdi_bmc_sensor_t *sdi_bmc_sensor_create (ipmi_entity_t *entity, ipmi_sensor_t *sensor)
{
    sdi_bmc_sensor_t *srp = NULL;
    uint32_t         rv, id, instance;
    char             name[SDI_MAX_NAME_LEN] = "";

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);
    ipmi_sensor_get_id(sensor, name, sizeof(name));

    if ((srp = sdi_bmc_db_sensor_get(id, instance, name)) == NULL) {
        if ((srp = sdi_bmc_db_sensor_add(id, instance, name)) == NULL) {
            SDI_DEVICE_TRACEMSG_LOG("Adding sensor info in DB failed (%u, %u, %s)",
                                     id, instance, name);
        } else {
            srp->type = ipmi_sensor_get_sensor_type(sensor);
            if (ipmi_sensor_get_event_reading_type(sensor)
                    == IPMI_EVENT_READING_TYPE_THRESHOLD) {
                srp->reading_type = SDI_SDR_READING_THRESHOLD;
                rv = ipmi_sensor_get_reading(sensor, sdi_bmc_sensor_reading_handler, NULL);
                 if (rv != 0) {
                     SDI_DEVICE_ERRMSG_LOG("Error in adding sensor reading handler : 0x%x", rv);
                 }
                 rv = ipmi_sensor_add_threshold_event_handler (sensor,
                                  sdi_bmc_sensor_threshold_event_handler, NULL);
                 if (rv != 0) {
                     SDI_DEVICE_ERRMSG_LOG("Error in adding sensor threshold handler : 0x%x\n", rv);
                 }
            } else {
                srp->reading_type = SDI_SDR_READING_DISCRETE;
                srp->res.reading.discrete_state = 0xff;
                rv = ipmi_sensor_get_states(sensor, sdi_bmc_sensor_discrete_states, NULL);
                if (rv != 0) {
                    SDI_DEVICE_ERRMSG_LOG("Error in adding discrete event handler : 0x%x\n", rv);
                }
                rv = ipmi_sensor_add_discrete_event_handler(sensor, 
                        sdi_bmc_sensor_discrete_event_handler, NULL);
                if (rv != 0) {
                    SDI_DEVICE_ERRMSG_LOG("Error in adding discrete event handler : 0x%x\n", rv);
                }
            }
            if (srp->ev_state == NULL) {
                srp->ev_state = malloc(ipmi_event_state_size());
                if (srp->ev_state == NULL) {
                    SDI_DEVICE_ERRMSG_LOG("Memory alloc failed for handling sensor events (%s)", srp->name);
                }
            }
            if (srp->ev_state != NULL) {
                    ipmi_event_state_init(srp->ev_state);
            }

            srp->state_sup = ipmi_sensor_get_event_support(sensor);
            if ((srp->state_sup != IPMI_EVENT_SUPPORT_NONE)
                    && (srp->state_sup != IPMI_EVENT_SUPPORT_GLOBAL_ENABLE)) {
                rv = ipmi_sensor_get_event_enables(sensor, sdi_bmc_sensor_enable_events, srp);
                if (rv != 0) {
                    SDI_DEVICE_ERRMSG_LOG("Error in adding sensor get event enables : 0x%x", rv);
                }
            }
            if (srp->thresholds == NULL) {
                srp->thresholds = malloc(ipmi_thresholds_size());
                if (srp->thresholds == NULL) {
                     SDI_DEVICE_ERRMSG_LOG("Memory alloc failed for handling sensor \
                             threshold events(%s)", srp->name);
                }
            }
            if (srp->thresholds != NULL) {
                ipmi_thresholds_init(srp->thresholds);
            }
            if (ipmi_sensor_get_event_reading_type(sensor)
                    == IPMI_EVENT_READING_TYPE_THRESHOLD) {
                srp->thresh_sup = ipmi_sensor_get_threshold_access(sensor);
                if ((srp->thresh_sup != IPMI_THRESHOLD_ACCESS_SUPPORT_NONE)
                        && (srp->thresh_sup != IPMI_THRESHOLD_ACCESS_SUPPORT_FIXED)) {
                    rv = ipmi_sensor_get_thresholds(sensor, sdi_bmc_got_thresholds, srp);
                    if (rv != 0) {
                        SDI_DEVICE_ERRMSG_LOG("ipmi_thresholds_get returned error 0x%x"
                                " for sensor %s\n", rv, srp->name);
                    }
                }
            }
        }
    }
    return srp;
}

/**
 * sdi_bmc_iterate_sensors_cb is callback function which is used to iterate through
 * sensors associated with the current entity.
 */

void sdi_bmc_iterate_sensors_cb (ipmi_entity_t *entity, ipmi_sensor_t *sensor,
                                 void *cb_data)
{
    uint32_t         id, instance;
    int32_t          rv = 0;
    char             name[SDI_MAX_NAME_LEN] = "";
    sdi_bmc_sensor_t *srp = NULL;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);
    ipmi_sensor_get_id(sensor, name, sizeof(name));

    if ((srp = sdi_bmc_sensor_create(entity, sensor))
            == NULL) {
        SDI_DEVICE_ERRMSG_LOG("Creating and adding sensor failed: %d - %d - %s",
                               id, instance, name);
    } else {
        if (ipmi_sensor_get_event_reading_type(sensor)
                == IPMI_EVENT_READING_TYPE_THRESHOLD) {
            rv = ipmi_sensor_get_reading(sensor, sdi_bmc_sensor_reading_handler, NULL);
            if (rv != 0) {
                SDI_DEVICE_ERRMSG_LOG("Error in adding sensor reading handler : 0x%x", rv);
            }
        } else {
            rv = ipmi_sensor_get_states(sensor, sdi_bmc_sensor_discrete_states, NULL);
            if (rv != 0) {
                SDI_DEVICE_ERRMSG_LOG("Error in adding discrete event handler : 0x%x\n", rv);
            }
        }
    }
}

/**
 * Entity present callback function to update entity presence status.
 */
static int32_t sdi_bmc_entity_presence_handler (ipmi_entity_t *entity, int32_t present,
                                                void *cb_data, ipmi_event_t  *event)
{
    uint32_t         id, instance;
    sdi_bmc_entity_t *ent = (sdi_bmc_entity_t *) cb_data;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);

    if (ent == NULL) {
        ent = sdi_bmc_db_entity_get(id, instance);
    }
    if (ent != NULL) {
        ent->present = (present != 0)? true : false;
    }
    return IPMI_EVENT_HANDLED;
}


/**
 * sdi_bmc_add_entity_fru_info is a worke around to handle FRU info for an entity.
 * Need to delete once the issue in BMC firmware is fixed.
 */
sdi_bmc_sensor_t *sdi_bmc_add_entity_fru_info (ipmi_entity_t *entity)
{
    uint32_t         id, instance;
    char             name[SDI_MAX_NAME_LEN] = "";
    sdi_bmc_sensor_t *srp = NULL;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);
    snprintf(name, sizeof(name) - 1, "Entity_info.%u.%u", id, instance);

    if ((srp = sdi_bmc_db_sensor_get(id, instance, name)) == NULL) {
        if ((srp = sdi_bmc_db_sensor_add(id, instance, name)) == NULL) {
            SDI_DEVICE_TRACEMSG_LOG("Adding FRU info in DB failed (%u, %u, %s)",
                    id, instance, name);
        } else {
            srp->reading_type = SDI_SDR_READING_ENTITY_INFO;
            srp->type = IPMI_SENSOR_TYPE_OTHER_FRU;
        }
    } else {
        srp->reading_type = SDI_SDR_READING_ENTITY_INFO;
        srp->type = IPMI_SENSOR_TYPE_OTHER_FRU;
        srp->entity_id = id;
        srp->entity_instance = instance;
    }
    return srp;
}

/**
 * sdi_bmc_iterate_entities_cb is a callback function which is used to
 * iterate entity list.
 */

static void sdi_bmc_iterate_entities_cb (ipmi_entity_t *entity, void *cb_data)
{
    uint32_t         id, instance;
    int32_t          rv;
    sdi_bmc_entity_t *ent = NULL;

    id = ipmi_entity_get_entity_id(entity);
    instance = ipmi_entity_get_entity_instance(entity);
    if ((ent = sdi_bmc_db_entity_add(id, instance)) == NULL) {
        SDI_DEVICE_ERRMSG_LOG("sdi_bmc_db_entity_add failed (%d,%d)", id, instance);
        return;
    }
    ent->type = ipmi_entity_get_entity_id(entity);
    ent->sdi_type = sdi_bmc_sdi_entity_type_get(ent->type);
    rv = ipmi_entity_add_presence_handler(entity,
                                  sdi_bmc_entity_presence_handler, ent);
    if (rv != 0) {
        SDI_DEVICE_ERRMSG_LOG("ipmi entity add presence handler failed(%d,%d): 0x%x",
                               id, instance, rv);
    }
    ipmi_entity_iterate_sensors(entity, sdi_bmc_iterate_sensors_cb, NULL);
    return;
}

/*
 * sdi_bmc_iterate_entities is used to initiate entity walk
 */
static void sdi_bmc_iterate_entities (ipmi_domain_t *domain, void *cb_data)
{
    int32_t rv;
    rv = ipmi_domain_iterate_entities(domain, sdi_bmc_iterate_entities_cb, cb_data);
    if (rv != 0) {
        SDI_DEVICE_ERRMSG_LOG("ipmi domain iterate entities failed.");
    }
}

/**
 * sdi_bmc_open_domain_handler is a domain handler callback function,
 * this will get called once the domain is created. Initialize entity update 
 * handler.
 */

static void sdi_bmc_open_domain_handler (ipmi_domain_t *domain, int32_t error,
                                              uint32_t conn_num, uint32_t port_num,
                                              int32_t con_status, void *user_data)
{
    int rv = 0;
    rv = ipmi_domain_enable_events(domain);
    if (rv != 0) {
        SDI_DEVICE_ERRMSG_LOG("ipmi domain enable events failed %d.", rv);
    }
    ipmi_domain_set_ipmb_rescan_time(domain, 10);
    rv = ipmi_domain_add_entity_update_handler(domain,
                                               sdi_bmc_entity_update_handler, user_data);
    if (rv != 0) {
        SDI_DEVICE_ERRMSG_LOG("ipmi_domain_add_entity_update_handler failed, error : %d\n", rv);
    }
    return;
}

/**
 * Callback function which gets called once the connection with BMC
 * is up completely.
 */
void sdi_bmc_conection_established (ipmi_domain_t *domain, void *cb_data)
{
    SDI_DEVICE_TRACEMSG_LOG("Connection established, fully up.");
    ipmi_domain_pointer_cb(domain_id, sdi_bmc_iterate_entities, cb_data);
}

typedef struct sdi_bmc_thread_param_s {
    os_handler_t      *os_hnd;
    sdi_device_hdl_t  dev_hdl;
} sdi_bmc_thread_param_t;

static void sdi_bmc_event_thread (void *param);
static void sdi_bmc_poller_thread (void *param);

/**
 * BMC main thread init function which handles creating a domain and
 * establishing connection with BMC.
 */
t_std_error sdi_bmc_device_driver_thread (void *param)
{
    int rv = 0;
    static os_handler_t *os_hnd;
    static ipmi_con_t  *con;
    t_std_error ret = STD_ERR_OK;
    static sdi_bmc_thread_param_t tparam;

    os_hnd = ipmi_posix_thread_setup_os_handler(SIGUSR2);
    if (!os_hnd) {
        SDI_DEVICE_ERRMSG_LOG("Unable to setup OS handlers.");
        return (SDI_ERRCODE(EPERM));
    }

    do {
        tparam.os_hnd = os_hnd;
        tparam.dev_hdl = param;
        rv = ipmi_init(os_hnd);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("IPMI initialization failed : %s.", strerror(rv));
            ret = SDI_ERRCODE(EPERM);
            break;
        }
        rv = ipmi_smi_setup_con(0, os_hnd, NULL, &con);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("IPMI SMI setup connection failed : %s.", strerror(rv));
            ret = SDI_ERRCODE(EPERM);
            break;
        }

        ipmi_open_option_t options[] = {{IPMI_OPEN_OPTION_SDRS, {1}},
                                         {IPMI_OPEN_OPTION_FRUS, {1}},
                                         {IPMI_OPEN_OPTION_SEL, {1}},
                                         {IPMI_OPEN_OPTION_SET_EVENT_RCVR, {1}}};
        rv = ipmi_open_domain("PAS-DOMAIN", &con, 1, sdi_bmc_open_domain_handler,
                param, sdi_bmc_conection_established, param, options, 4, &domain_id);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("IPMI open domain failed : %s.", strerror(rv));
            ret = SDI_ERRCODE(EPERM);
            break;
        }

        rv = os_hnd->create_thread(os_hnd, 0, sdi_bmc_event_thread, &tparam);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("Creating IPMI event thread failed: %s.", strerror(rv));
            ret = SDI_ERRCODE(EPERM);
            break;
        }

        rv = os_hnd->create_thread(os_hnd, 0, sdi_bmc_poller_thread, &tparam);
        if (rv != 0) {
            SDI_DEVICE_ERRMSG_LOG("Creating IPMI poller thread failed: %s.", strerror(rv));
            ret = SDI_ERRCODE(EPERM);
            break;
        }
        pause();
    } while (0);

    if (ret != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Free OS handlers due to error.");
        os_hnd->free_os_handler(os_hnd);
        ret = SDI_ERRCODE(EPERM);
    }
    return ret;
}

static std_thread_create_param_t bmc_thread_entry[1];

/**
 * Thread cleanup functions.
 */
void sdi_bmc_thread_cleanup (void)
{
    if ((bmc_thread_entry->name != NULL)
            || (bmc_thread_entry->thread_function != NULL)
            || (bmc_thread_entry->param != NULL)) {
        pthread_kill(*(pthread_t *)bmc_thread_entry->thread_id, SIGTERM);
        std_thread_destroy_struct(bmc_thread_entry);
    }

    sdi_bmc_db_entity_cleanup();
    sdi_bmc_db_sensor_cleanup();
}

/**
 * BMC Event thread init function and it will listening for events.
 */

static void sdi_bmc_event_thread (void *param)
{
    sdi_bmc_thread_param_t *tparam = (sdi_bmc_thread_param_t *) param;
    tparam->os_hnd->operation_loop(tparam->os_hnd);
}


static void sdi_bmc_oem_poller(void)
{
    sdi_bmc_oem_poller_t *node = NULL;
    for(node = (sdi_bmc_oem_poller_t *)std_dll_getfirst(oem_poller_head); node != NULL;
        node = (sdi_bmc_oem_poller_t *)std_dll_getnext(oem_poller_head, (std_dll *)node)) {
        if (((*(node->sensor_loc)) & node->sensor_bit)  == node->sensor_bit) {
            for (size_t i = 0; i < node->oem_cmd_count; i++) {
                sdi_bmc_oem_cmd_execute(&(node->oem_cmd[i]), NULL);
            }
        }
    }
}

/**
 * BMC poller thread init function. iterate, read and update sensor data
 * based on configured polling interval.
 */
static void sdi_bmc_poller_thread (void *param)
{
    sdi_device_hdl_t dev_hdl = ((sdi_bmc_thread_param_t *)param)->dev_hdl;
    sdi_bmc_dev_t     *bmc_dev = (sdi_bmc_dev_t *) dev_hdl->private_data;

    sleep(100);
    while (true) {
        std_usleep(MILLI_TO_MICRO((bmc_dev->polling_interval * 1000)));
        ipmi_domain_pointer_cb(domain_id, sdi_bmc_iterate_entities, dev_hdl);
        sdi_bmc_oem_poller();
    }
    return;
}

/**
 * sdi_bmc_device_driver_init is BMC driver init function. Creates BMC poller
 * and event handling threads.
 */
t_std_error sdi_bmc_device_driver_init (sdi_device_hdl_t dev_hdl)
{
    sdi_bmc_thread_cleanup();


    std_thread_init_struct(bmc_thread_entry);
    bmc_thread_entry->name = "sdi-bmc";
    bmc_thread_entry->thread_function = (std_thread_function_t) sdi_bmc_device_driver_thread;
    bmc_thread_entry->param = dev_hdl;
    if (std_thread_create(bmc_thread_entry) != STD_ERR_OK)  {
        SDI_DEVICE_ERRMSG_LOG("Error in creating BMC event handling thread.");
        return SDI_ERRCODE(EPERM);
    }
    sdi_bmc_dev_t *bmc_dev = (sdi_bmc_dev_t *)dev_hdl->private_data;
    oem_poller_head = bmc_dev->oem_poller_head; 

    return STD_ERR_OK;
}
