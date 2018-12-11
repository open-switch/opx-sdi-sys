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
 * filename: sdi_s6k_psu.c
 */


/*********************************************************************************************
 * sdi_s6k_psu.c
 * Implements the driver for a delta PSU devices which is used in s4000 and s6000.
 * This chip contains one temperature sensor and one Fan in it. It exports each of them
 * as resource to the resource framework.The device is complaint to PMBUS standard and
 * hence the pmbus operations are used to manipulate the device.
 *
 *********************************************************************************************/

#include "sdi_device_common.h"
#include "sdi_pmbus_dev.h"
#include "sdi_fan_internal.h"
#include "std_assert.h"
#include <stdlib.h>
#include <string.h>

sdi_pmbus_sensor_t sdi_pmbus_sensor_s6k[] = {
    {SDI_PMBUS_TEMPERATURE_1, SDI_PMBUS_LINEAR, "Temperature-1"},
    {SDI_PMBUS_FAN_1, SDI_PMBUS_LINEAR, "Fan-1"},
};

#define SDI_MAX_PMBUS_SENSORS_S6K sizeof(sdi_pmbus_sensor_s6k)/sizeof(sdi_pmbus_sensor_t)

/* Register function for s6k psu driver*/
t_std_error sdi_s6k_psu_register(std_config_node_t node, void *bus_handle,
                                 sdi_device_hdl_t* device_hdl);
/* Initialize the device*/
t_std_error sdi_s6k_psu_chip_init(sdi_device_hdl_t device_hdl);

/* Export the Driver table */
sdi_driver_t s6k_psu_entry = {
        sdi_s6k_psu_register,
        sdi_s6k_psu_chip_init
};

/*
 * The config file format will be as below for s6k psu devices
 *
 * <s6k_psu instance="<instance>" addr="<Address of the device>" fan_speed="<default fan speed>"
 * alias="<Alias name for the particular devide>"
 * </s6k_psu>
 */
/*
 * This the call back function for the device registration
 * [in] node - Config node for the device
 * [in] bus_handle - Parent bus handle of the device
 * [out] device_hdl - pointer to the device handle which will be updated by this function
 * Return - STD_ERR_OK, this function is kept as non void as the driver table
 * function pointer
 */

t_std_error sdi_s6k_psu_register(std_config_node_t node, void *bus_handle,
                                 sdi_device_hdl_t* device_hdl)
{
    sdi_pmbus_dev_t *sdi_pmbus_device = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);

    sdi_pmbus_device = calloc(sizeof(sdi_pmbus_dev_t),1);
    STD_ASSERT(sdi_pmbus_device != NULL);

    sdi_pmbus_device->max_sensors = SDI_MAX_PMBUS_SENSORS_S6K;
    sdi_pmbus_device->sdi_pmbus_sensors = sdi_pmbus_sensor_s6k;

    /* Read and store fan speed RPM-to-percent map, if any */
    std_config_node_t fan_node;
    for (fan_node = std_config_get_child(node); fan_node != 0; fan_node = std_config_next_node(fan_node)) {
        const char *s = std_config_name_get(fan_node);
        if (strcmp(s, "fan") != 0) {
            SDI_DEVICE_ERRMSG_LOG("Invalid tag, expected \"fan\", got \"%s\" -- skipping\n", s);
            continue;
        }
        
        if ((s = std_config_attr_get(fan_node, "ppid")) == 0) {
            SDI_DEVICE_ERRMSG_LOG("No ppid attribute for fan tag -- skipping\n");
            continue;
        }

        regex_t re[1];
        if (regcomp(re, s, REG_EXTENDED | REG_NOSUB) != 0) {
            SDI_DEVICE_ERRMSG_LOG("Invalid regexp for ppid -- skipping\n");
            continue;
        }

        struct sdi_fan_speed_ppid_map *p = (struct sdi_fan_speed_ppid_map *) calloc(1, sizeof(*p));
        if (p == 0) {
            SDI_DEVICE_ERRMSG_LOG("Failed to allocate pmbus speed map -- skipping\n");
            continue;
        }
        *p->ppid_pat = *re;
        p->next = sdi_pmbus_device->fan_speed_map;
        sdi_pmbus_device->fan_speed_map = p;

        std_config_node_t speed_node;
        for (speed_node = std_config_get_child(fan_node);
             speed_node != 0;
             speed_node = std_config_next_node(speed_node)
             ) {
            s = std_config_name_get(speed_node);
            if (strcmp(s, "speed") != 0) {
                SDI_DEVICE_ERRMSG_LOG("Invalid tag, expected \"speed\", got \"%s\" -- skipping\n", s);
                continue;
            }

            if ((s = std_config_attr_get(speed_node, "rpm")) == 0) {
                SDI_DEVICE_ERRMSG_LOG("No rpm attribute for speed tag -- skipping\n");
                continue;
            }
            uint_t rpm = strtoul(s, NULL, 0);

            if ((s = std_config_attr_get(speed_node, "percent")) == 0) {
                SDI_DEVICE_ERRMSG_LOG("percent rpm attribute for speed tag -- skipping\n");
                continue;
            }
            uint_t pct = strtoul(s, NULL, 0);

            struct sdi_fan_speed_map_entry *m = (struct sdi_fan_speed_map_entry *) calloc(1, sizeof(*m));
            if (m == 0) {
                SDI_DEVICE_ERRMSG_LOG("Failed to allocate pmbus speed map entry -- skipping\n");
                continue;
            }
            m->rpm = rpm;
            m->pct = pct;
            m->next = p->speeds;
            p->speeds = m;
        }
    }

    sdi_pmbus_dev_register(node, bus_handle, sdi_pmbus_device, &s6k_psu_entry, device_hdl);

    return STD_ERR_OK;
}

/*
 * This the call back function for the device initialization
 * [in] device_hdl - Device handle for which the init to be done
 * return - std_error_t
 */
t_std_error sdi_s6k_psu_chip_init(sdi_device_hdl_t device_hdl)
{
    return STD_ERR_OK;
}

