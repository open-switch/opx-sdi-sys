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
 * filename: sdi_entity_info_vdriver.h
 */

/**
 * @file sdi_entity_info_vdriver.h
 *
 * @{
 */

#ifndef __SDI_ENTITY_INFO_VDRIVER_H__
#define __SDI_ENTITY_INFO_VDRIVER_H__

/**
 * FAN/PSU entity_info data get
 *
 * param[in] resource_hdl    - resource handler
 * param[out] entity_info  - entity_info structure to fill
 *
 * return STD_ERR_OK for success and the respective error code in case of failure.
 */
t_std_error sdi_entity_info_data_get(void *resource_hdl,
                                     sdi_entity_info_t *entity_info);

/**
 * @struct entity_info_t
 * ENTITY INFO (virtual device) private data
 */
typedef struct {
    /** Store the current entity info data */
    /**< No.of fan in the entity */   
    uint_t                    no_of_fans;            
    /**< Max Speed of the fan in the entity */
    uint_t                    max_fan_speed;         
    /**< SDI pin group bus hdl for obtaining fan airflow dir */
    sdi_pin_group_bus_hdl_t   airflow_dir_hdl;       
    /**< SDI pin bus hdl for obtaining psu power type: ac or dc */      
    sdi_pin_bus_hdl_t         psu_type_hdl;          
    /**< H/w specified value for normal airflow dir on this platform */
    uint_t                    normal_airflow_val;    
    /**< H/w specified value for reverse airflow dir on this platform */
    uint_t                    reverse_airflow_val;   
    /**< H/w specified value for ac power type on this platform */
    uint_t                    ac_power_val;          
    /**< H/w specified value for dc power type on this platform */
    uint_t                    dc_power_val;          
    /**< Device Alias */
    char                      alias[SDI_MAX_NAME_LEN];
}entity_info_data_t ;

/**
 * @}
 */

#endif   /* __SDI_ENTITY_INFO_VDRIVER_H__ */
