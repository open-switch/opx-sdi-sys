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
 * filename: sdi_ina219.c
 */


/******************************************************************************
 * Implements the driver for ina219 power monitor chip.
 * The chip provides register interface for obtaining current, voltage and power
 ******************************************************************************/

#include "sdi_driver_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_ina219_reg.h" 
#include "sdi_i2c_bus_api.h"
#include "std_assert.h"
#include "std_utils.h"
#include "sdi_device_common.h"
#include "sdi_power_monitor_internal.h" 
#include "sdi_power_monitor_resource_attr.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 

/*
 * INA219 power monitor device private data
 */
typedef struct ina219_device
{
    float    psu_offset_loss;
    uint16_t config_reg_val;
    uint16_t calib_reg_val;
} ina219_device_t;

/*Register and chip init function declarations for the ina219 driver*/
t_std_error sdi_ina219_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl);
t_std_error sdi_ina219_chip_init(sdi_device_hdl_t device_hdl);

/*
 * Retrieve current in amps of the chip refered by resource.
 * This is a callback function for  power monitor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [out] current_amp - pointer to a buffer to get the current value in amps
 * Return - STD_ERR_OK for success and the respective error code from i2c api in case of failure
 */
static t_std_error sdi_ina219_current_amp_get(void *resource_hdl, float *current_amp)
{
    sdi_device_hdl_t chip = NULL;
    ina219_device_t *ina219_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint16_t current_reg_val;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(current_amp != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ina219_data = (ina219_device_t*)chip->private_data;
    STD_ASSERT(ina219_data != NULL);

    rc = sdi_smbus_read_word(chip->bus_hdl, chip->addr.i2c_addr, 
                             INA219_CURRENT_REG_OFFSET,
                             &current_reg_val, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK)
    {
        SDI_DEVICE_ERRMSG_LOG("ina219 read failure at addr: 0x%x reg offset: %d rc: %d\n",
                chip->addr.i2c_addr.i2c_addr, INA219_CURRENT_REG_OFFSET, rc);
        return rc;
    }

    /*
     * current_reg_val is in mA. 
     * Divide by psu_offset_loss to offset any psu loss
     * Divide by 1000 to convert to Amp. 
     */
    *current_amp = (current_reg_val/ina219_data->psu_offset_loss)/1000;

    return rc;
}

/*
 * Retrieve voltage in volts of the chip refered by resource.
 * This is a callback function for  power monitor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [out] voltage_volt - pointer to a buffer to get the voltage value in volts
 * Return - STD_ERR_OK for success and the respective error code from i2c api in case of failure
 */
static t_std_error sdi_ina219_voltage_volt_get(void *resource_hdl, float *voltage_volt)
{
    sdi_device_hdl_t chip = NULL;
    ina219_device_t *ina219_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint16_t voltage_reg_val;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(voltage_volt != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ina219_data = (ina219_device_t*)chip->private_data;
    STD_ASSERT(ina219_data != NULL);

    rc = sdi_smbus_read_word(chip->bus_hdl, chip->addr.i2c_addr, 
                             INA219_BUS_VOLTAGE_REG_OFFSET,
                             &voltage_reg_val, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK)
    {
        SDI_DEVICE_ERRMSG_LOG("ina219 read failure at addr: 0x%x reg offset: %d rc: %d\n",
                chip->addr.i2c_addr.i2c_addr, INA219_BUS_VOLTAGE_REG_OFFSET, rc);
        return rc;
    }

    /*
     * Calculate the voltage in volt taking into consideration:
     * LSB bits that are not part of the voltage. 
     * Multiply by the voltage value that each bit represents
     */
    *voltage_volt = (voltage_reg_val >> INA219_VOLTAGE_REG_BIT_SHIFT) * 
                    INA219_VOLTAGE_REG_BIT_VAL_IN_VOLT;

    return rc;
}

/*
 * Retrieve power in watts of the chip refered by resource.
 * This is a callback function for  power monitor resource
 * [in] resource_hdl - callback data for this function,chip instance is passed as a callback data
 * [out] power_watt - pointer to a buffer to get the power value in watts
 * Return - STD_ERR_OK for success and the respective error code from i2c api in case of failure
 */
static t_std_error sdi_ina219_power_watt_get(void *resource_hdl, float *power_watt)
{
    sdi_device_hdl_t chip = NULL;
    ina219_device_t *ina219_data = NULL;
    t_std_error rc = STD_ERR_OK;
    float current_amp;
    float voltage_volt;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(power_watt != NULL);

    chip = (sdi_device_hdl_t)resource_hdl;

    ina219_data = (ina219_device_t*)chip->private_data;
    STD_ASSERT(ina219_data != NULL);

    rc = sdi_ina219_current_amp_get(chip, &current_amp);
    if (rc != STD_ERR_OK) 
    {
        SDI_DEVICE_ERRMSG_LOG("sdi_ina219_current_amp_get failed for chip %s", chip->alias);
        return rc;
    }

    rc = sdi_ina219_voltage_volt_get(chip, &voltage_volt);
    if (rc != STD_ERR_OK) 
    {
        SDI_DEVICE_ERRMSG_LOG("sdi_ina219_voltage_volt_get failed for chip %s", chip->alias);
        return rc;
    }

    *power_watt = current_amp * voltage_volt;

    return rc;
}

static power_monitor_t ina219_power_monitor={
        NULL, /*As the init is done as part of chip init, resource init is not required*/
        sdi_ina219_current_amp_get,
        sdi_ina219_voltage_volt_get,
        sdi_ina219_power_watt_get
};

/*
 * Every driver must export function with name sdi_<driver_name>_entry_callbacks
 * so that the driver framework is able to look up and invoke it to get the callbacks
 */
const sdi_driver_t *sdi_ina219_entry_callbacks (void)
{ 
    /* Export the Driver table */ 
    static const sdi_driver_t sdi_ina219_entry={
        sdi_ina219_register,
        sdi_ina219_chip_init
    };
    return &sdi_ina219_entry;
};

/*
 * The config file format will be as below for ina219 devices
 *
 * <ina219 instance="<chip_instance>"
 * addr="<Address of the device>"
 * config_reg_val = "<value to program in config reg>"
 * calib_reg_val = "<value to pogram in calibration reg>"
 * psu_offset_loss = "<value to use to PSU offset loss>"
 * alias="<Alias name for the particular devide>">
 * </ina219>
 * Mandatory attributes    : instance and addr
 */
 /*
 * This the call back function for the device registration
 * [in] node - Config node for the device
 * [in] bus_handle - Parent bus handle of the device
 * [out] device_hdl - pointer to the device handle which will be updated by this function
 * Return - STD_ERR_OK, this function is kept as non void as the driver table fp requires it
 */
t_std_error sdi_ina219_register(std_config_node_t node,void *bus_handle,sdi_device_hdl_t* device_hdl)
{
    char *node_attr = NULL;
    sdi_device_hdl_t chip = NULL;
    ina219_device_t *ina219_data = NULL;

    STD_ASSERT(node != NULL);
    STD_ASSERT(bus_handle != NULL);
    STD_ASSERT(device_hdl != NULL);
    STD_ASSERT(((sdi_bus_t*)bus_handle)->bus_type == SDI_I2C_BUS);

    chip = calloc(sizeof(sdi_device_entry_t),1);
    STD_ASSERT(chip != NULL);

    ina219_data = calloc(sizeof(ina219_device_t),1);
    STD_ASSERT(ina219_data != NULL);

    chip->bus_hdl = bus_handle;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_INSTANCE);
    STD_ASSERT(node_attr != NULL);
    chip->instance = (uint_t) strtoul(node_attr, NULL, 0);

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ADDRESS);
    STD_ASSERT(node_attr != NULL);
    chip->addr.i2c_addr.i2c_addr = (i2c_addr_t) strtoul(node_attr, NULL, 16);

    chip->callbacks = sdi_ina219_entry_callbacks();
    chip->private_data = (void*)ina219_data;

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_ALIAS);
    if (node_attr == NULL)
    {
        snprintf(chip->alias,SDI_MAX_NAME_LEN,"ina219-%d", chip->instance );
    }
    else
    {
        safestrncpy(chip->alias,node_attr,SDI_MAX_NAME_LEN);
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_POWER_CONFIG_REG_VAL);
    if(node_attr != NULL)
    {
        ina219_data->config_reg_val = (uint16_t) strtol(node_attr, NULL, 0);
    }
    else
    {
        ina219_data->config_reg_val = INA219_DEFAULT_CONFIG_REG_VAL;
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_POWER_CALIB_REG_VAL);
    if(node_attr != NULL)
    {
        ina219_data->calib_reg_val = (uint16_t) strtol(node_attr, NULL, 0);
    }
    else
    {
        ina219_data->calib_reg_val = INA219_DEFAULT_CALIB_REG_VAL;
    }

    node_attr = std_config_attr_get(node, SDI_DEV_ATTR_POWER_PSU_OFFSET_LOSS);
    if(node_attr != NULL)
    {
        ina219_data->psu_offset_loss =  strtof(node_attr, NULL);
    }
    else
    {
        ina219_data->psu_offset_loss = INA219_DEFAULT_PSU_OFFSET_LOSS;
    }

    sdi_resource_add(SDI_RESOURCE_POWER_MONITOR, chip->alias,(void*)chip,
            &ina219_power_monitor);

    *device_hdl = chip;

    return STD_ERR_OK;
}


t_std_error sdi_ina219_chip_init(sdi_device_hdl_t device_hdl)
{
    sdi_device_hdl_t chip = NULL;
    uint16_t reg_value;
    t_std_error rc = STD_ERR_OK;
    ina219_device_t *ina219_data = NULL;

    STD_ASSERT(device_hdl != NULL);

    chip = device_hdl;

    ina219_data = chip->private_data;
    STD_ASSERT(ina219_data != NULL);

    /*
     * confirm that the power-on config reg value read is INA219_DEFAULT_CONFIG_REG_VAL
     */
    rc = sdi_smbus_read_word(chip->bus_hdl, chip->addr.i2c_addr, 
                             INA219_CONFIG_REG_OFFSET, &reg_value, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK) 
    {
        SDI_DEVICE_ERRMSG_LOG("Init config reg read failed for chip %s", chip->alias);
        return rc;
    }

    reg_value = ntohs(reg_value) ;
    if (reg_value != INA219_DEFAULT_CONFIG_REG_VAL)
    {
        SDI_DEVICE_ERRMSG_LOG("Init Unexpected error: config reg power-on reset:0x%x for chip %s",
                              reg_value, chip->alias );
        return (e_std_err_code_FAIL);
    }

    /* Set configuration register to the value that is obtained from config file  
     */
    rc = sdi_smbus_write_word(chip->bus_hdl, chip->addr.i2c_addr, 
                              INA219_CONFIG_REG_OFFSET, ina219_data->config_reg_val, 
                              SDI_I2C_FLAG_NONE);
    if(rc != STD_ERR_OK)
    {
        SDI_DEVICE_ERRMSG_LOG("Init config reg write of 0x%x failed for chip %s",
                              ina219_data->config_reg_val, chip->alias);
        return rc;
    }

     /* Set calibration Register to value read from config file
     */
    rc = sdi_smbus_write_word(chip->bus_hdl, chip->addr.i2c_addr, 
                              INA219_CALIBRATION_REG_OFFSET, ina219_data->calib_reg_val, 
                              SDI_I2C_FLAG_NONE);
    if(rc != STD_ERR_OK)
    {
        SDI_DEVICE_ERRMSG_LOG("Init calib reg write of 0x%x failed for chip %s",
                              ina219_data->calib_reg_val, chip->alias);
        return rc;
    }

    return rc;
}

