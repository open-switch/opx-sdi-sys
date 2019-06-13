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
 * filename: sdi_bmc_internal.h 
 */

#ifndef __SDI_BMC_INTERNAL_H__
#define __SDI_BMC_INTERNAL_H__

#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_entity_info.h"
#include "sdi_driver_internal.h"

#include <OpenIPMI/ipmiif.h>

#define MAX_THRESHOLDS         (6)   /* Max thresholds for a sensor */
#define BMC_DEFAULT_POLLING    (5)   /* BMC default polling interval 5 seconds */

/*
 * BMC configuration attributes
 */
#define SDI_BMC_DEV_ATR_POLLING_INT     "polling_interval"
#define SDI_BMC_DEV_ATTR_FAN            "bmc_fan"
#define SDI_BMC_DEV_ATTR_TEMP           "bmc_temp"
#define SDI_BMC_DEV_ATTR_EEPROM         "bmc_eeprom"
#define SDI_BMC_DEV_ATTR_DATA           "data_sdr_id"
#define SDI_BMC_DEV_ATTR_STATUS         "status_sdr_id"
#define SDI_BMC_DEV_ATTR_STATUS_BIT     "status_bit"
#define SDI_BMC_OEM_ATTR_DATA           "oem_status"
#define SDI_BMC_DEV_ATTR_OEM_POLLER     "oem_poller"
#define SDI_BMC_DEV_ATTR_SENSR_REQ      "sensor_req"
#define SDI_BMC_DEV_ATTR_SENSR_REQ_BIT  "sensor_req_bit"
#define SDI_BMC_DEV_ATTR_OEM_STATUS_SDR "oem_status_sdr"
#define SDI_BMC_DEV_ATTR_OEM_NETFN      "oem_netfn"
#define SDI_BMC_DEV_ATTR_OEM_CMD        "oem_cmd"
#define SDI_BMC_DEV_ATTR_OEM_BUSID      "oem_busid"
#define SDI_BMC_DEV_ATTR_OEM_SLAVE_ADDR "oem_slave_addr"
#define SDI_BMC_DEV_ATTR_OEM_DATA_SIZE  "oem_data_size"
#define SDI_BMC_DEV_ATTR_OEM_OFFSET     "oem_offset"
#define SDI_BMC_DEV_ATTR_OEM_DATA       "oem_data"


#define SDI_BMC_INT_USE_ELM_OFFSET      "int_use_elm_offset"
#define SDI_BMC_AIRFLOW_OFFSET          "airflow_offset"
#define SDI_BMC_PSU_TYPE_OFFSET         "psu_type_offset"
#define SDI_BMC_HDR_SZ_OFFSET           "hdr_sz_offset"
#define SDI_BMC_ENTITY_INFO_DUMMY       "entity_info_dummy"
#define SDI_BMC_OEM_PSU_TYPE            "oem_psu_type"
#define SDI_BMC_OEM_AIRFLOW             "oem_airflow"

#define SDI_BMC_MAX_INT_USE_AREA        (256)
#define SDI_BMC_INVALID_OFFSET          (0xFFFFFFFF)
#define SDI_BMC_HDR_SZ_OFFSET_DEF       (0)
#define SDI_BMC_OEM_DATA_LEN            33

#define SDI_BMC_INVALID_BIT             (0xff)

/*
 * BMC entity info offsets.
 */

#define SDI_DELL_PN_SIZE        (6)
#define SDI_DELL_CN_SIZE        (2)
#define SDI_MGID_OFFSET         (SDI_DELL_PN_SIZE + SDI_DELL_CN_SIZE)
/*
 * BMC entity presence states.
 */
typedef enum {
    SDI_BMC_ENTITY_ABSENT  = 0,
    SDI_BMC_ENTITY_PRESENT = 1,
    SDI_BMC_ENTITY_DISABLE = 2,
} sdi_bmc_ent_present_status;

/*
 * BMC PSU presence and operating status bits.
 */
typedef enum {
    SDI_BMC_PSU_STATUS_PRSNT = 0,
    SDI_BMC_PSU_STATUS_FAILURE = 1,
    SDI_BMC_PSU_STATUS_PRED_FAILURE = 2,
    SDI_BMC_PSU_STATUS_INPUT_LOST = 3,
    SDI_BMC_PSU_STATUS_INPUT_LOST_OR_OUTRNG = 4,
    SDI_BMC_PSU_STATUS_INPUT_OUTRNG = 5,
    SDI_BMC_PSU_STATUS_CFG_ERR = 6,
} sdi_bmc_psu_status_bit_masks;

/*
 * BMC sensor reading types.
 */
typedef enum {
    SDI_SDR_READING_THRESHOLD = 1,
    SDI_SDR_READING_DISCRETE = 2,
    SDI_SDR_READING_ENTITY_INFO = 3,
    SDI_SDR_READING_OEM_DISCRETE = 4
} sdi_sdr_rd_type_t;

/*
 * BMC sensor (threshold/discrete) data.
 */
typedef struct sdi_bmc_reading_s {
    double    data;  /** threshold sensor reading. */
    uint32_t  raw_data; /** threshold sensor raw reading */
    uint32_t  discrete_state; /** Discrete sensor states */
} sdi_bmc_reading_t;

typedef struct sdi_bmc_entity_info_s {
    bool     valid;
    char     board_manufacturer[SDI_MAX_NAME_LEN]; /* Manufature name */
    char     board_product_name[SDI_MAX_NAME_LEN]; /* Product name */
    char     board_serial_number[SDI_PPID_LEN];   /* board serial number is complete PPID of fantray and partial PPID of PSU's*/
    char     board_part_number[SDI_PART_NUM_LEN];  /* board part number is DNP 6bytes + hardware revision 3 bytes */
    uint8_t  air_flow; /* Fan Airflow direction */
    uint8_t  type; /* This field is applicable for PSU's only */
    int      num_fans; /* Number of fans in entity instance */
    int      max_speed; /* Max fan speed of the fan */
    bool     is_dummy;  /* If BMC FRU implementation not available,
                           use this flag and populate dummy eeprom info */
} sdi_bmc_entity_info_t;

/*
 * sdi_bmc_res_data_t is a union to hold sensor data or
 * entity info.
 */
typedef union sdi_bmc_res_data_u {
    sdi_bmc_reading_t     reading; /** Sensor reading */
    sdi_bmc_entity_info_t entity_info; /** Entity info */
} sdi_bmc_res_data_t;

/*
 * Entity record which will hold all the entity details
 * and presence status.
 */
typedef struct sdi_bmc_entity_s {
    uint32_t          entity_id;
    uint32_t          entity_instance;
    uint32_t          type;  /** BMC enetity type */
    sdi_entity_type_t sdi_type; /** SDI entity type */
    bool              present; /** Presence status */
} sdi_bmc_entity_t;

/*
 * BMC sensor recorda which holds all sensor details
 * and threshold values.
 */
typedef struct sdi_bmc_sensor_s {
    uint32_t            entity_id; /** Entity id */
    uint32_t            entity_instance; /** Entity instance */
    char                name[IPMI_MAX_NAME_LEN]; /** Sensor Id */
    uint32_t            type; /** BMC sensor type */
    sdi_sdr_rd_type_t   reading_type; /** Sensor reading type */
    sdi_bmc_res_data_t  res;   /** Sensor data */
    ipmi_event_state_t  *ev_state; /** Sensor event status */
    ipmi_thresholds_t   *thresholds; /** Sensor threshold states */
    int32_t             state_sup; /* Event support status */
    int32_t             thresh_sup; /* Threshold support status */
    double              threshold[MAX_THRESHOLDS]; /** BMC configured threshold values */
} sdi_bmc_sensor_t;



/*
 * BMC OEM speicific IPMI command details
 *
 */
typedef struct sdi_bmc_oem_cmd_info_s {
    uint32_t             netfn;
    uint32_t             cmd;
    uint32_t             bus_id;
    uint32_t             slave_addr;
    uint32_t             offset;
    uint32_t             data_size;
    uint8_t              data;
    uint8_t              req_data[SDI_BMC_OEM_DATA_LEN + 4];
    uint8_t              *resp_data;
} sdi_bmc_oem_cmd_info_t;

/*
 * BMC OEM specific IPMI Poller
 */
typedef struct sdi_bmc_oem_poller_s {
    std_dll                     node;
    int                         oem_cmd_count;
    char                        sensor[SDI_MAX_NAME_LEN];
    uint32_t                    sensor_bit;
    uint32_t                    *sensor_loc;
    sdi_bmc_oem_cmd_info_t      *oem_cmd;
} sdi_bmc_oem_poller_t;

#define BMC_AC_TYPE     0
#define BMC_DC_TYPE     1
#define BMC_AC_DC_TYPE  2

/*
 * SDI resource info its used to create map between sdi resource
 * to BMC sensors. 
 */
typedef struct sdi_bmc_dev_resource_info_s {
    sdi_resource_type_t resource_type; /** sdi resouce type */
    uint32_t            instance;  /** Instance number */
    char                alias[SDI_MAX_NAME_LEN]; /** Given name in cfg file */
    char                data_sdr_id[SDI_MAX_NAME_LEN]; /** Data sensor id */
    sdi_bmc_sensor_t    *data_sdr; /** Data sensor record */
    char                status_sdr_id[SDI_MAX_NAME_LEN]; /** Status sensor Id */
    uint_t              status_bit; /** Status bit which needs to be checked */
    sdi_bmc_sensor_t    *status_sdr; /** Status sensor record */
    uint32_t            int_use_elm_offset; /* Internal use area element offset */
    uint32_t            airflow_offset; /* Airflow data offset in element data */
    uint32_t            psu_type_offset; /* PSU type data offset in element data */
    sdi_bmc_oem_cmd_info_t *psu_type_oem_cmd; /* PSU Type retrived through OEM command */
    sdi_bmc_oem_cmd_info_t *air_flow_oem_cmd; /* Air Flow Direction retrived through OEM command */
    uint32_t            fan_count;   /** Number fans present in this entity */
    uint32_t            max_fan_speed /** Max fan speed */;
    sdi_device_hdl_t    dev_hdl;  /** Device handle */
    uint32_t            hdr_sz_offset; /* OpenIPMI API will strip header, making eeprom offset adjustment necessary */
    bool                is_dummy; /** Is BMC FRU support available? if not dummy should be true */
} sdi_bmc_dev_resource_info_t;

/**
 * Device list which are managed by BMC
 */
typedef struct sdi_bmc_dev_list_s {
    uint32_t                     count; /** Device/SDI resource count */
    sdi_bmc_dev_resource_info_t  data[0]; /** Resource info list */
} sdi_bmc_dev_list_t;


/*
 * BMC device config 
 */
typedef struct sdi_bmc_dev_s {
    uint32_t            instance;  /** Instance number */
    uint32_t            polling_interval; /** Polling interval */
    char                alias[SDI_MAX_NAME_LEN]; /** BMC device alias */
    sdi_bmc_dev_list_t  *dev_list; /** Device list */
    std_dll_head        *oem_poller_head; /** OEM command list */
} sdi_bmc_dev_t;


/*
 * Populate OEM Specific IPMI command info from config node
 */
void  sdi_bmc_populate_oem_cmd_info (std_config_node_t node, sdi_bmc_oem_cmd_info_t *oem_info);
/*
 * Get BMC device for given sdr Id. 
 */

sdi_bmc_dev_resource_info_t * sdi_bmc_dev_get_by_data_sdr (sdi_device_hdl_t dev_hdl, char *sdr_id);

/**
 * BMC FAN device registration function.
 */
t_std_error sdi_bmc_fan_res_register (sdi_device_hdl_t bmc_dev_hdl,
        sdi_bmc_dev_resource_info_t *bmc_res);

/**
 * BMC temperature device registration function
 */
t_std_error sdi_bmc_tmp_res_register (sdi_device_hdl_t bmc_dev_hdl,
        sdi_bmc_dev_resource_info_t *bmc_res);

/**
 * BMC FRU info registration function
 */
t_std_error sdi_bmc_entity_info_res_register (sdi_device_hdl_t bmc_dev_hdl,
        sdi_bmc_dev_resource_info_t *bmc_res);

#endif /* __SDI_BMC_INTERNAL_H__ */
