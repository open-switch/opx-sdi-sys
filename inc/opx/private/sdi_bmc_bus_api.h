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
 * filename: sdi_bmc_bus_api.h
 */


#ifndef __SDI_BMC_BUS_API_H__
#define __SDI_BMC_BUS_API_H__

#include "sdi_entity.h"

/**
 * sdi_bmc_dc_sensor_reading_get_by_name is to get the discrete sensor 
 * reading by using sensor name.
 */
t_std_error sdi_bmc_dc_sensor_reading_get_by_name (char *sensor_id, uint32_t *data);

/*
 * sdi_bmc_th_sensor_reading_get_by_name is to get the threshold sensor
 * reading by using sensor name.
 */
t_std_error sdi_bmc_th_sensor_reading_get_by_name (char *sensor_id, double *data);

/*
 * sdi_bmc_get_entity_name is to get entity name using entity type and entity instance.
 */

char *sdi_bmc_get_entity_name (uint32_t id, uint32_t instance, char *name, uint32_t len);

/*
 * sdi_bmc_sdi_entity_type_get provides mapping betwee bmc entity type 
 * and sdi entity type.
 */

sdi_entity_type_t sdi_bmc_sdi_entity_type_get (uint32_t bmc_type);


#endif /* __SDI_BMC_BUS_API_H__ */
