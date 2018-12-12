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
 * filename: sdi_ina219_reg.h
 */


/************************************************************************************
 * @file sdi_ina219_reg.h
 * @brief  Defines the ina219 power monitor chip register macros for the use of ina219 driver.
 * The chip provides register interface for obtaining the current, voltage and power usage values. 
 * The data sheet of ina219 is available in the below link
 * http://www.ti.com/lit/ds/symlink/ina219.pdf
 ************************************************************************************/

#ifndef __SDI_INA219_REG_H_
#define __SDI_INA219_REG_H_

/**
 * INA219 Register definitions
 */
#define INA219_CONFIG_REG_OFFSET             0x00
#define INA219_SHUNT_VOLTAGE_REG_OFFSET      0x01
#define INA219_BUS_VOLTAGE_REG_OFFSET        0x02
#define INA219_POWER_REG_OFFSET              0x03
#define INA219_CURRENT_REG_OFFSET            0x04
#define INA219_CALIBRATION_REG_OFFSET        0x05

/**
 * Default Register values
 */
#define INA219_DEFAULT_CONFIG_REG_VAL    0x399F 
#define INA219_DEFAULT_CALIB_REG_VAL     0x0
#define INA219_DEFAULT_PSU_OFFSET_LOSS   0x1

/*
 * Number of LSB bits in BUS_VOLTAGE register that are not part of voltage
 */
#define INA219_VOLTAGE_REG_BIT_SHIFT 3

/*
 * Each Bit value in BUS_VOLTAGE register (in volts)
 */
#define INA219_VOLTAGE_REG_BIT_VAL_IN_VOLT 0.004

#endif //__SDI_INA219_REG_H_
