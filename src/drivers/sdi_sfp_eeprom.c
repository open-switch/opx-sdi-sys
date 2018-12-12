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
 * filename: sdi_sfp_eeprom.c
 */


/******************************************************************************
 * sdi_sfp_eeprom.c
 * Implements the SFP eeprom related APIs
 *****************************************************************************/
#include "sdi_sfp.h"
#include "sdi_sfp_reg.h"
#include "sdi_media.h"
#include "sdi_media_attr.h"
#include "sdi_media_internal.h"
#include "sdi_resource_internal.h"
#include "sdi_device_common.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_i2c_bus_api.h"
#include "sdi_media_phy_mgmt.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include "std_time_tools.h"
#include "std_bit_ops.h"
#include "sdi_platform_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* Delay for accesing phy device
   When phy device is enabled it needs delay before access*/
#define SFP_PHY_DELAY (100 * 1000)
/* Magic number description not given in appnote from Marvell
   an-2036 app note from Mavell give below magic value to enable
   SGMII mode for phy device */
#define PHY_SGMII_MODE 0x9084
#define MIN(x,y) ((x) < (y) ? (x) : (y))

static t_std_error sdi_is_wavelength_tune_supported (sdi_device_hdl_t sfp_device, bool* status);
static t_std_error sdi_sfp_page_select (sdi_device_hdl_t sfp_device, uint_t page);

 /*SFP parameter sizes */
enum {
    SDI_SFP_BYTE_SIZE = 1,
    SDI_SFP_WORD_SIZE = 2,
    SDI_SFP_DOUBLE_WORD_SIZE = 4,
    SDI_SFP_QUAD_WORD_SIZE = 8,
};

/* sfp register information structure */
typedef struct sdi_sfp_reg_info {
    uint_t offset; /* register offset */
    uint_t size; /* register size */
    bool   printable; /* field contains printable data */
} sdi_sfp_reg_info_t;

#define SDI_SFP_PADDING_CHAR 0
#define SDI_SFP_GARBAGE_CHAR_INDICATOR '?'

/* parameter register information strucutre. Parameters should be defined in the
 * same order of sdi_media_param_type_t */
static sdi_sfp_reg_info_t param_reg_info[] = {
    { SFP_WAVELENGTH_OFFSET, SDI_SFP_WORD_SIZE }, /* for SDI_MEDIA_WAVELENGTH */
    { 0, 0 }, /* for SDI_MEDIA_WAVELENGTH_TOLERANCE */
    { 0, 0 }, /* for SDI_MEDIA_MAX_CASE_TEMP */
    { SFP_CC_BASE_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_CC_BASE */
    { SFP_CC_EXT_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_CC_EXT */
    { SFP_CONNECTOR_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_CONNECTOR */
    { SFP_ENCODING_TYPE_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_ENCODING_TYPE */
    { SFP_NM_BITRATE_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_NM_BITRATE */
    { SFP_IDENTIFIER_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_IDENTIFIER */
    { SFP_EXT_IDENTIFIER_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_EXT_IDENTIFIER */
    { SFP_LENGTH_SMF_KM_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_SMF_KM */
    { SFP_LENGTH_OM1_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_OM1 */
    { SFP_LENGTH_OM2_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_OM2 */
    { SFP_LENGTH_OM3_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_OM3 */
    { SFP_LENGTH_CABLE_ASSEMBLY_OFFSET, SDI_SFP_BYTE_SIZE  }, /* for SDI_MEDIA_LENGTH_CABLE_ASSEMBLY */
    { SFP_LENGTH_SMF_OFFSET, SDI_SFP_BYTE_SIZE}, /* for SDI_MEDIA_LENGTH_SMF, not supported on SFP */
    { SFP_OPTIONS_OFFSET, SDI_SFP_WORD_SIZE }, /* for SDI_MEDIA_OPTIONS */
    { SFP_ENHANCED_OPTIONS_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_ENHANCED_OPTIONS */
    { SFP_DIAG_MON_TYPE_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_DIAG_MON_TYPE */
    { 0,0 }, /* for SDI_MEDIA_DEVICE_TECH */
    { SFP_MAX_BITRATE_OFFSET, SDI_SFP_BYTE_SIZE } , /* for SDI_MEDIA_MAX_BITRATE */
    { SFP_MIN_BITRATE_OFFSET, SDI_SFP_BYTE_SIZE }, /* for SDI_MEDIA_MIN_BITRATE */
    { SFP_EXT_COMPLIANCE_CODE_OFFSET, SDI_SFP_BYTE_SIZE}, /* for SDI_MEDIA_EXT_COMPLIANCE_CODE */
    { 0, 0 }, /* For SDI_FREE_SIDE_DEV_PROP , not applicable for SFP */

    /* Note this offset is in device A2, page 02 */
    { SFP_WAVELENGTH_SET_OFFSET, SDI_SFP_WORD_SIZE}, /* For SDI_TUNE_WAVELENGTH_PICO_METERS*/
};

/* vendor register information structure. Parameters in this structure should be
 * defined in the same order of sdi_media_vendor_info_type_t */
static sdi_sfp_reg_info_t vendor_reg_info[] = {
    { SFP_VENDOR_NAME_OFFSET, SDI_MEDIA_MAX_VENDOR_NAME_LEN, true }, /* for SDI_MEDIA_VENDOR_NAME */
    { SFP_VENDOR_OUI_OFFSET, SDI_MEDIA_MAX_VENDOR_OUI_LEN, false }, /* for SDI_MEDIA_VENDOR_OUI */
    { SFP_VENDOR_SN_OFFSET, SDI_MEDIA_MAX_VENDOR_SERIAL_NUMBER_LEN, true }, /* for SDI_MEDIA_VENDOR_SN */
    { SFP_VENDOR_DATE_OFFSET, SDI_MEDIA_MAX_VENDOR_DATE_LEN, true }, /* for SDI_MEDIA_VENDOR_DATE */
    { SFP_VENDOR_PN_OFFSET, SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN, true }, /* for SDI_MEDIA_VENDOR_PN */
    { SFP_VENDOR_REVISION_OFFSET, SDI_MEDIA_MAX_VENDOR_REVISION_LEN, true } /* for SDI_MEDIA_VENDOR_REVISION */
};


/* threshold value register information structure. Parameters in this structure
 * should be defined in the same order of sdi_media_threshold_type_t */
static sdi_sfp_reg_info_t threshold_reg_info[] = {
    /* for SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD */
    { SFP_TEMP_HIGH_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD */
    { SFP_TEMP_LOW_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD */
    { SFP_TEMP_HIGH_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD */
    { SFP_TEMP_LOW_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD */
    { SFP_VOLT_HIGH_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD */
    { SFP_VOLT_LOW_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD */
    { SFP_VOLT_HIGH_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD */
    { SFP_VOLT_LOW_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD */
    { SFP_RX_PWR_HIGH_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD */
    { SFP_RX_PWR_LOW_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD */
    { SFP_RX_PWR_HIGH_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD */
    { SFP_RX_PWR_LOW_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD */
    { SFP_BIAS_HIGH_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD */
    { SFP_BIAS_LOW_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD */
    { SFP_BIAS_HIGH_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD */
    { SFP_BIAS_LOW_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_HIGH_ALARM_THRESHOLD */
    { SFP_TX_PWR_HIGH_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_LOW_ALARM_THRESHOLD */
    { SFP_TX_PWR_LOW_ALARM_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_HIGH_WARNING_THRESHOLD */
    { SFP_TX_PWR_HIGH_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_LOW_WARNING_THRESHOLD */
    { SFP_TX_PWR_LOW_WARNING_THRESHOLD_OFFSET, SDI_SFP_WORD_SIZE },
};

static sdi_i2c_addr_t sfp_i2c_addr = { .i2c_addr = SFP_DIAG_MNTR_I2C_ADDR, .addr_mode_16bit = 0};

float sdi_convert_mw_to_dbm(float power_mw)
{
    return (power_mw == 0.00000)
          ? (SDI_SFP_ZERO_WATT_POWER_IN_DBM)
          : (10.00000 * log10f(power_mw));
}

/* This function enables the particular device on a bus */
static inline t_std_error sdi_sfp_module_select(sdi_device_hdl_t sfp_device)
{
    t_std_error rc = STD_ERR_OK;
    sfp_device_t *sfp_priv_data = NULL;

    STD_ASSERT(sfp_device != NULL);
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    /* Check whether module selection required or not on this particular sfp. If
     * module selectin is not required just return STD_ERR_OK */
    if(sfp_priv_data->mux_sel_hdl != NULL) {
        rc = sdi_pin_group_acquire_bus(sfp_priv_data->mux_sel_hdl);
        if (rc != STD_ERR_OK){
            return rc;
        }

        rc = sdi_pin_group_write_level(sfp_priv_data->mux_sel_hdl,
                                       sfp_priv_data->mux_sel_value);
        if (rc != STD_ERR_OK){
            /* module selection failed, hence release the lock.*/
            sdi_pin_group_release_bus(sfp_priv_data->mux_sel_hdl);
            SDI_DEVICE_ERRMSG_LOG("Mux selection is failed for %s rc : %d",
                    sfp_device->alias, rc);
            return rc;
        }
    }

    if(sfp_priv_data->mod_sel_hdl != NULL) {
        rc = sdi_pin_group_acquire_bus(sfp_priv_data->mod_sel_hdl);
        if (rc != STD_ERR_OK){
            if(sfp_priv_data->mux_sel_hdl != NULL) {
                sdi_pin_group_release_bus(sfp_priv_data->mux_sel_hdl);
            }

            return rc;
        }

        rc = sdi_pin_group_write_level(sfp_priv_data->mod_sel_hdl,
                                       sfp_priv_data->mod_sel_value);
        if (rc != STD_ERR_OK){
            /* module selection failed, hence release the lock.*/
            if(sfp_priv_data->mux_sel_hdl != NULL) {
                sdi_pin_group_release_bus(sfp_priv_data->mux_sel_hdl);
            }
            sdi_pin_group_release_bus(sfp_priv_data->mod_sel_hdl);
            SDI_DEVICE_ERRMSG_LOG("module selection is failed for %s rc : %d",
                    sfp_device->alias, rc);

            return rc;
        }
    }
    /* If module selection success, releasing the lock taken care by
     * sdi_sfp_module_deselect api */
    return rc;
}

/* This function disables the particular device on a bus */
static inline void sdi_sfp_module_deselect(sfp_device_t *sfp_priv_data)
{
    STD_ASSERT(sfp_priv_data != NULL);

    if(sfp_priv_data->mux_sel_hdl != NULL) {
        sdi_pin_group_release_bus(sfp_priv_data->mux_sel_hdl);
    }

    if(sfp_priv_data->mod_sel_hdl != NULL) {
        sdi_pin_group_release_bus(sfp_priv_data->mod_sel_hdl);
    }
}

/* This function checks whether Alarm/warning flags implemented for this module.
 * Make sure that module is already selected before calling this function */
static inline t_std_error sdi_is_alarm_flags_supported(sdi_device_hdl_t sfp_device,
                                                       bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(sfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status =  false;

    rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr,
                             SFP_ENHANCED_OPTIONS_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d reg : %d for %s rc : %d",
                              sfp_device->addr, SFP_ENHANCED_OPTIONS_OFFSET, sfp_device->alias, rc);
        return rc;
    }

    if( (STD_BIT_TEST(buf, SFP_ALARM_SUPPORT_BIT_OFFSET) != 0 ) ) {
       *support_status = true;
    } else {
       *support_status = false;
    }

   return rc;
}

/* This function checks whether diag monitoring is supported or not on this divice
 * Make sure that module is already selected before calling this function */
static inline t_std_error sdi_is_diag_monitoring_supported(sdi_device_hdl_t sfp_device,
                                                           bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(sfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status =  false;

    rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr,
                             SFP_DIAG_MON_TYPE_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Getting of diag monitoring value is failed for %s rc : %d",
                              sfp_device->alias, rc);
        return rc;
    }

    if( (STD_BIT_TEST(buf, SFP_DDM_SUPPORT_BIT_OFFSET) == 0) ) {
        *support_status = false;
    } else {
        *support_status = true;
    }

    return rc;
}

/* This function checks whether rate select is supported or not on this divice
 * Make sure that module is already selected before calling this function */
static inline t_std_error sdi_is_rate_select_supported(sdi_device_hdl_t sfp_device,
                                                       bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(sfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status =  false;

    rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr,
                             SFP_ENHANCED_OPTIONS_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Getting of diag monitoring value is failed for %s rc : %d",
                              sfp_device->alias, rc);
        return rc;
    }

    if( (STD_BIT_TEST(buf, SFP_RATE_SELECT_BIT_OFFSET) == 0) ) {
        *support_status = false;
    } else {
        *support_status = true;
    }

    return rc;
}

/* This function converts temperature raw data to human readable format based on
 * calibration types. SFP standard supports two types of calibration. One is
 * internal calibration and other is external calibration.
 *
 * 1. Internal calibration : Internally measured transceiver temperature
 * represented as a 16 bit signed twos complement value in increments of 1/256
 * degrees Celsius, yielding a total range of -128C to +128C. The temperature in
 * degrees Celsius is given by the signed twos complement value with LSB equal
 * to 1/256 C. For more details "Internal caibration" section in sff-8472 spec.
 *
 * 2. External calibration : Internally measured transceiver temperature, T is
 * given by the following quation
 * T(C) = T(slope)* Reg_value(16 bit signed twos complement value) + T(offset).
 * The result is in units of 1/256C, yielding a total range of –128C to +128C.
 * */
static inline float convert_sfp_temp(uint8_t *buf, sfp_calib_info_t *calib_info)
{
    int16_t temp = 0;
    float calib_temp = 0;
    float slope = 0;
    bool is_negative = false;

    temp = ( (buf[0] << BITS_PER_BYTE ) | (buf[1]) );

    if( temp < 0 ){
        /* Negative value */
        temp = ((~temp) + 1);
        is_negative = true;
    }

    if(calib_info->type == SFP_CALIB_TYPE_INTERNAL) {
        calib_temp = ((float)(temp & 0xFF) / 256.0) + ((temp & 0xFF00) >> 8);
    } else if(calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        slope = ( (float)(calib_info->slope[1] / 256 ) + calib_info->slope[0] );
        calib_temp = ( ((slope * temp) + calib_info->offset) / 256 );
    }

    if(is_negative == true) {
        calib_temp = -calib_temp;
    }
    return calib_temp;
}

/* This function converts voltage raw data to human readable format based on
 * calibration types. SFP standard supports two types of calibration. One is
 * internal calibration and other is external calibration.
 *
 * 1. Internal calibration : Internally measured transceiver supply voltage,
 * represented as a 16 bit unsigned integer with the voltage defined as the full
 * 16 bit value (0 – 65535) with LSB equal to 100 uVolt, yielding a total range
 * of 0 to +6.55 Volts.
 *
 * 2. External calibration : Internally measured supply voltage. Module internal
 * supply voltage, V, is given in microvolts by the following equation:
 * V(uV) = VSLOPE* VAD (16 bit unsigned integer)+ VOFFSET.
 * The result is in units of 100uV, yielding a total range of 0 – 6.55V.
 */
static inline float convert_sfp_volt(uint8_t *buf, sfp_calib_info_t *calib_info)
{
    uint16_t volt = 0;
    float calib_volt = 0;
    float slope = 0;

    volt = ( (buf[0] << BITS_PER_BYTE ) | (buf[1]) );

    if(calib_info->type == SFP_CALIB_TYPE_INTERNAL) {
        /* LSB is equal to 100uV.converting to V */
        calib_volt = ((float)(volt) / (10000.0));
    } else if(calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        slope = ( (float)(calib_info->slope[1] / 256 ) + calib_info->slope[0] );
        calib_volt = ( ((slope * volt) + calib_info->offset) / (10000.0) );
    }
    return calib_volt;
}

/* This function converts temperature raw data to human readable format based on
 * calibration types. SFP standard supports two types of calibration. One is
 * internal calibration and other is external calibration.
 *
 * 1. Internal calibration : Internally measured RX received optical power in
 * mW, represented as a 16 bit unsigned integer with the power defined as the
 * full 16 bit value (0 – 65535) with LSB equal to 0.1 uW, yielding a total
 * range of 0 to 6.5535 mW (~ -40 to +8.2 dBm).
 *
 * 2. External calibration : Internally measured received optical power(RX_PWR),
 * is given in uW by the following equation:
 *      Rx_PWR (uW) = Rx_PWR(4) * Rx_PWRAD4 (16 bit unsigned integer) +
 *                    Rx_PWR(3) * Rx_PWRAD3 (16 bit unsigned integer) +
 *                    Rx_PWR(2) * Rx_PWRAD2 (16 bit unsigned integer) +
 *                    Rx_PWR(1) * Rx_PWRAD (16 bit unsigned integer) +
 *                    Rx_PWR(0)
 * The result is in units of 0.1uW yielding a total range of 0 – 6.5mW.
 * The final result then gets converted to dbm. A value of 0mW yields SDI_SFP_ZERO_WATT_POWER_IN_DBM
 * */
static inline float convert_sfp_rx_power(uint8_t *buf, sfp_rx_power_calib_info_t *calib_info)
{
    uint16_t rx_power = 0;
    float calib_rx_power = 0;

    rx_power = ( (buf[0] << BITS_PER_BYTE ) | (buf[1]) );

    if(calib_info->type == SFP_CALIB_TYPE_INTERNAL) {
        calib_rx_power = ( ((float)rx_power) / 10000.0 );
    } else if(calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        calib_rx_power = ( (((float)(*(calib_info->rx_power_const_4))) * (powf((float)rx_power, 4.0))) +
                           (((float)(*(calib_info->rx_power_const_3))) * (powf((float)rx_power, 3.0))) +
                           (((float)(*(calib_info->rx_power_const_2))) * (powf((float)rx_power, 2.0))) +
                           (((float)(*(calib_info->rx_power_const_1))) * ((float)rx_power)) +
                           ((float)(*calib_info->rx_power_const_0)) );
    }
    return sdi_convert_mw_to_dbm(calib_rx_power);
}

/* This function converts temperature raw data to human readable format based on
 * calibration types. SFP standard supports two types of calibration. One is
 * internal calibration and other is external calibration.
 *
 * 1. Internal calibration : Internally measured TX output power in mW.
 * Represented as a 16 bit unsigned integer with the power defined as the full
 * 16 bit value (0 – 65535) with LSB equal to 0.1 uW, yielding a total range of
 * 0 to 6.5535 mW (~ -40 to +8.2 dBm).
 *
 * 2. External calibration : Internally measured module transmitter coupled
 * output power, TX_PWR, is given in uW by the following equation:
 *      TX_PWR (uW) = TX_PWRSLOPE * TX_PWRAD  +  TX_PWROFFSET.
 * This result is in units of 0.1uW yielding a total range of 0 – 6.5mW.
 * The final result then gets converted to dbm. A value of 0mW yields SDI_SFP_ZERO_WATT_POWER_IN_DBM
 * */
static inline float convert_sfp_tx_power(uint8_t *buf, sfp_calib_info_t *calib_info)
{
    uint16_t tx_pwr = 0;
    float calib_tx_pwr = 0;
    float slope = 0;

    tx_pwr = ( (buf[0] << BITS_PER_BYTE ) | (buf[1]) );

    if(calib_info->type == SFP_CALIB_TYPE_INTERNAL) {
        /* LSB is 0.1uW. converting to mW */
        calib_tx_pwr = ((float)(tx_pwr) / (10000.0));
    } else if(calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        slope = ( (float)(calib_info->slope[1] / 256 ) + calib_info->slope[0] );
        /* Result is in units of 0.1uW. converting to mW */
        calib_tx_pwr = ( ((slope * tx_pwr) + calib_info->offset) / (10000.0) );
    }
    return  sdi_convert_mw_to_dbm(calib_tx_pwr);
}

/* This function converts temperature raw data to human readable format based on
 * calibration types. SFP standard supports two types of calibration. One is
 * internal calibration and other is external calibration.
 *
 * 1. Internal calibration : Internally measured TX bias current in uA is
 * represented as a 16 bit unsigned integer with the current defined as the full
 * 16 bit value (0 – 65535) with LSB equal to 2 uA, yielding a total range of 0
 * to 131 mA.
 *
 * 2. External calibration : Internally measured module laser bias current, I,
 * is given in microamps by the following equation:
 *      I (uA) = ISLOPE * IAD (16 bit unsigned integer) + IOFFSET.
 * This result is in units of 2 uA, yielding a total range of 0 to 131 mA.
 * */
static inline float convert_sfp_tx_bias_current(uint8_t *buf, sfp_calib_info_t *calib_info)
{
    uint16_t tx_bias = 0;
    float calib_tx_bias = 0;
    float slope = 0;

    tx_bias = ( (buf[0] << BITS_PER_BYTE ) | (buf[1]) );

    if(calib_info->type == SFP_CALIB_TYPE_INTERNAL) {
        /*LSB is equal to 2uA. Converting to mA */
        calib_tx_bias = ( ((float)(tx_bias * 2)) / (1000.0) );
    } else if(calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        slope = ( (float)(calib_info->slope[1] / 256 ) + calib_info->slope[0] );
        calib_tx_bias = ( (slope * tx_bias) + calib_info->offset );
        /* Converting to mA as the calc gives units of 2uA */
        calib_tx_bias = ( ((float)(calib_tx_bias * 2)) / (1000.0) );
    }
    return calib_tx_bias;
}

/**
 * Get the required module status of the specific sfp
 * resource_hdl[in] - Handle of the resource
 * flags[in]        - flags for status that are of interest
 * status[out]    - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_sfp_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                               uint_t flags, uint_t *status)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    bool alarm_support_status = false;
    uint8_t alarm_status_buf = 0;
    uint8_t warning_status_buf = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = 0;

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {
        /* Check whether Alarm/warning flags implemented for this module */
        rc = sdi_is_alarm_flags_supported(sfp_device, &alarm_support_status);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("SFP alarm support status get is failed for %s, rc : %d",
                                  sfp_device->alias, rc);
            break;
        }

        if(!alarm_support_status) {
            rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            break;
        }

        if( ( (flags) & ( (SDI_MEDIA_STATUS_TEMP_HIGH_ALARM)  |
                          (SDI_MEDIA_STATUS_TEMP_LOW_ALARM)   |
                          (SDI_MEDIA_STATUS_VOLT_HIGH_ALARM)  |
                          (SDI_MEDIA_STATUS_VOLT_LOW_ALARM) ) ) != 0) {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                    SFP_ALARM_STATUS_1_OFFSET, &alarm_status_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s, rc : %d",
                        sfp_device->addr, sfp_device->alias, rc);
                break;
            }
        }

        if( ( (flags) & ( (SDI_MEDIA_STATUS_TEMP_HIGH_WARNING)  |
                          (SDI_MEDIA_STATUS_TEMP_LOW_WARNING)   |
                          (SDI_MEDIA_STATUS_VOLT_HIGH_WARNING)  |
                          (SDI_MEDIA_STATUS_VOLT_LOW_WARNING) ) ) != 0) {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                    SFP_WARNING_STATUS_1_OFFSET,
                    &warning_status_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                        sfp_device->addr, sfp_device->alias, rc);
                break;
            }
        }
    } while(0);

    sdi_sfp_module_deselect(sfp_priv_data);

    if( rc == STD_ERR_OK) {
        if((alarm_status_buf & SFP_TEMP_HIGH_ALARM_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_TEMP_HIGH_ALARM;
        } else if ((alarm_status_buf & SFP_TEMP_LOW_ALARM_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_TEMP_LOW_ALARM;
        }

        if((alarm_status_buf & SFP_VOLT_HIGH_ALARM_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_VOLT_HIGH_ALARM;
        } else if ((alarm_status_buf & SFP_VOLT_LOW_ALARM_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_VOLT_LOW_ALARM;
        }
        if((warning_status_buf & SFP_TEMP_HIGH_WARNING_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_TEMP_HIGH_WARNING;
        } else if ((warning_status_buf & SFP_TEMP_LOW_WARNING_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_TEMP_LOW_WARNING;
        }

        if((warning_status_buf & SFP_VOLT_HIGH_WARNING_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_VOLT_HIGH_WARNING;
        } else if ((warning_status_buf & SFP_VOLT_LOW_WARNING_FLAG) != 0) {
            *status |= SDI_MEDIA_STATUS_VOLT_LOW_WARNING;
        }
    }
    return rc;
}

/**
 * Get the required channel monitoring(rx_power, tx_bias alarm) status of the specific SFP
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest
 * flags[in]        - flags for channel status
 * status[out]      - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_sfp_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                                uint_t flags, uint_t *status)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    bool alarm_support_status = false;
    uint8_t tx_alarm_status = 0;
    uint8_t rx_alarm_status = 0;
    uint8_t tx_warn_status = 0;
    uint8_t rx_warn_status = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = 0;

    if(channel != SDI_SFP_CHANNEL_NUM) {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {
        /* Check whether Alarm/warning flags implemented for this module */
        rc = sdi_is_alarm_flags_supported(sfp_device, &alarm_support_status);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("SFP alarm support status get is failed for %s, rc : %d",
                                  sfp_device->alias, rc);
            break;
        }

        if(!alarm_support_status) {
            rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            break;
        }

        if( ( (flags) & ((SDI_MEDIA_TX_BIAS_HIGH_ALARM) | (SDI_MEDIA_TX_BIAS_LOW_ALARM) |
                         (SDI_MEDIA_TX_PWR_HIGH_ALARM) | (SDI_MEDIA_TX_PWR_LOW_ALARM)) ) != 0 ) {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                                     SFP_ALARM_STATUS_1_OFFSET, &tx_alarm_status, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                                      sfp_device->addr, sfp_device->alias, rc);
                break;
            }
        }

        if( ( (flags) & ((SDI_MEDIA_RX_PWR_HIGH_ALARM) | (SDI_MEDIA_RX_PWR_LOW_ALARM)) ) != 0 ) {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                                     SFP_ALARM_STATUS_2_OFFSET, &rx_alarm_status, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                                      sfp_device->addr, sfp_device->alias, rc);
                break;
            }
        }

        if( ( (flags) & ((SDI_MEDIA_TX_PWR_HIGH_WARNING) | (SDI_MEDIA_TX_PWR_LOW_WARNING) |
                        (SDI_MEDIA_TX_BIAS_HIGH_WARNING) | (SDI_MEDIA_TX_BIAS_LOW_WARNING))) != 0 ) {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                                     SFP_WARNING_STATUS_1_OFFSET, &tx_warn_status, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                                      sfp_device->addr, sfp_device->alias, rc);
                break;
            }
        }

        if( ((flags) & ((SDI_MEDIA_RX_PWR_HIGH_WARNING) | (SDI_MEDIA_RX_PWR_LOW_WARNING)) ) != 0 ) {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                                     SFP_WARNING_STATUS_2_OFFSET, &rx_warn_status, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                                      sfp_device->addr, sfp_device->alias, rc);
                break;
            }
        }
    }while(0);
    sdi_sfp_module_deselect(sfp_priv_data);

    if( rc == STD_ERR_OK) {
        if( ( (tx_alarm_status & SFP_TX_BIAS_HIGH_ALARM_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_BIAS_HIGH_ALARM;
        } else if ( ( (tx_alarm_status & SFP_TX_BIAS_LOW_ALARM_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_BIAS_LOW_ALARM;
        }

        if( ( (tx_alarm_status & SFP_TX_PWR_HIGH_ALARM_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_PWR_HIGH_ALARM;
        } else if ( ( (tx_alarm_status & SFP_TX_PWR_LOW_ALARM_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_PWR_LOW_ALARM;
        }

        if( ( (rx_alarm_status & SFP_RX_PWR_HIGH_ALARM_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_RX_PWR_HIGH_ALARM;
        } else if ( ( (tx_alarm_status & SFP_RX_PWR_LOW_ALARM_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_RX_PWR_LOW_ALARM;
        }

        if( ( (tx_warn_status & SFP_TX_BIAS_HIGH_WARNING_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_BIAS_HIGH_WARNING;
        } else if ( ( (tx_warn_status & SFP_TX_BIAS_LOW_WARNING_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_BIAS_LOW_WARNING;
        }

        if( ( (tx_warn_status & SFP_TX_PWR_HIGH_WARNING_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_PWR_HIGH_WARNING;
        } else if ( ( (tx_warn_status & SFP_TX_PWR_LOW_WARNING_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_TX_PWR_LOW_WARNING;
        }

        if( ( (rx_warn_status & SFP_RX_PWR_HIGH_WARNING_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_RX_PWR_HIGH_WARNING;
        } else if ( ( (tx_warn_status & SFP_RX_PWR_LOW_WARNING_FLAG) != 0 ) ) {
            *status |= SDI_MEDIA_RX_PWR_LOW_WARNING;
        }
    }
    return rc;

}

/**
 * Get the required channel status of the specific SFP
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest
 * flags[in]        - flags for channel status
 * status[out]    - returns the set of status flags which are asserted
 * return           - t_std_error
 * TODO : As of now this API supports to get the status of required flags from
 * system CPLD. Need to add support to get the same from optic eeprom
 */
t_std_error sdi_sfp_channel_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                        uint_t flags, uint_t *status)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t value = 0;
    uint8_t buf_sup, buf_data = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);
    *status = 0;

    if(channel != SDI_SFP_CHANNEL_NUM) {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if ((sfp_priv_data->mod_tx_control_hdl == NULL)
            || (sfp_priv_data->mod_sfp_rx_los_hdl == NULL)
            || (sfp_priv_data->mod_sfp_tx_fault_hdl == NULL)){

        rc = sdi_sfp_module_select(sfp_device);
        if(rc != STD_ERR_OK) {
            return rc;
        }

        do {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                    sfp_device->addr.i2c_addr, SFP_ENHANCED_OPTIONS_OFFSET,
                    &buf_sup, SDI_I2C_FLAG_NONE);
            if ((rc != STD_ERR_OK) || (buf_sup == 0x0)) {
                break;
            }

            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                    sfp_i2c_addr, SFP_OPTIONAL_STATUS_CONTROL_OFFSET,
                    &buf_data, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK) {
                break;
            }
        } while (0);

        sdi_sfp_module_deselect(sfp_priv_data);
        if (rc != STD_ERR_OK) {
            return rc;
        }
    }

    if( ((flags) & (SDI_MEDIA_STATUS_TXDISABLE)) != 0 ) {
        if (sfp_priv_data->mod_tx_control_hdl == NULL) {

            if (STD_BIT_TEST(buf_sup, SFP_SOFT_TX_DISABLE_CTRL_BIT_OFFSET) != 0) {
                if (STD_BIT_TEST(buf_data, SFP_OPTIONAL_TX_DISABLE_STATE_BIT_OFFSET) != 0) {
                    *status |= SDI_MEDIA_STATUS_TXDISABLE;
                }
            } else {
                rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            }

        } else {
            rc = sdi_pin_group_acquire_bus(sfp_priv_data->mod_tx_control_hdl);
            if(rc != STD_ERR_OK) {
                return rc;
            }

            rc = sdi_pin_group_read_level(sfp_priv_data->mod_tx_control_hdl, &value);
            sdi_pin_group_release_bus(sfp_priv_data->mod_tx_control_hdl);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("tx disable status get failed for %s rc : %d",
                        sfp_device->alias, rc);
                return rc;
            }

            if( (STD_BIT_TEST(value, sfp_priv_data->mod_tx_control_bitmask) != 0) ) {
                *status |= SDI_MEDIA_STATUS_TXDISABLE;
            }
        }
    }

    if( ((flags) & (SDI_MEDIA_STATUS_TXFAULT)) != 0 ) {
        if (sfp_priv_data->mod_sfp_tx_fault_hdl == NULL) {

            if (STD_BIT_TEST(buf_sup, SFP_SOFT_TX_FAULT_MNTR_BIT_OFFSET) != 0) {
                if (STD_BIT_TEST(buf_data, SFP_OPTIONAL_TX_FAULT_STATE_BIT_OFFSET) != 0) {
                    *status |= SDI_MEDIA_STATUS_TXFAULT;
                }
            } else {
                rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            }
        } else {

            value = 0;

            rc = sdi_pin_group_acquire_bus(sfp_priv_data->mod_sfp_tx_fault_hdl);
            if(rc != STD_ERR_OK) {
                return rc;
            }

            rc = sdi_pin_group_read_level(sfp_priv_data->mod_sfp_tx_fault_hdl, &value);
            sdi_pin_group_release_bus(sfp_priv_data->mod_sfp_tx_fault_hdl);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("tx disable status get failed for %s rc : %d",
                        sfp_device->alias, rc);
                return rc;
            }

            if ( (STD_BIT_TEST(value, sfp_priv_data->mod_sfp_tx_fault_bitmask) != 0) )  {
                *status |= SDI_MEDIA_STATUS_TXFAULT;
            }
        }
    }

    if( ((flags) & (SDI_MEDIA_STATUS_RXLOSS)) != 0 ) {
        if (sfp_priv_data->mod_sfp_rx_los_hdl == NULL) {

            if (STD_BIT_TEST(buf_sup, SFP_SOFT_RX_LOS_MNTR_BIT_OFFSET) != 0) {
                if (STD_BIT_TEST(buf_data, SFP_OPTIONAL_RX_LOSS_STATE_BIT_OFFSET) != 0) {
                    *status |= SDI_MEDIA_STATUS_RXLOSS;
                }
            } else {
                rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            }
        } else {
            value = 0;

            rc = sdi_pin_group_acquire_bus(sfp_priv_data->mod_sfp_rx_los_hdl);
            if(rc != STD_ERR_OK) {
                return rc;
            }

            rc = sdi_pin_group_read_level(sfp_priv_data->mod_sfp_rx_los_hdl, &value);
            sdi_pin_group_release_bus(sfp_priv_data->mod_sfp_rx_los_hdl);

            if (rc != STD_ERR_OK){

                SDI_DEVICE_ERRMSG_LOG("rx los status get failed for %s rc : %d",
                        sfp_device->alias, rc);
                return rc;
            }

            if ( (STD_BIT_TEST(value, sfp_priv_data->mod_sfp_rx_los_bitmask) != 0) ) {
                *status |= SDI_MEDIA_STATUS_RXLOSS;
            }
        }
    }

    if( ( (flags) & (SDI_MEDIA_STATUS_TXLOSS) ) != 0 ) {
        /* tx_los is not supported on SFP */
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    return rc;
}

/**
 * Disable/Enable the transmitter of the specific SFP
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest. This variable should
 * be 0 for SFP.
 * enable[in]       - "false" to disable and "true" to enable
 * return           - t_std_error
 * TODO : As of now this API supports to control the tx disable from system
 * CPLD. Need to add support to set the same in optice eeprom.
 */
t_std_error sdi_sfp_tx_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                bool enable)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t value = 0;
    uint8_t buf_sup, buf_data = 0;

    STD_ASSERT(resource_hdl != NULL);

    if(channel != SDI_SFP_CHANNEL_NUM) {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (sfp_priv_data->mod_tx_control_hdl == NULL) {

        rc = sdi_sfp_module_select(sfp_device);
        if(rc != STD_ERR_OK) {
            return rc;
        }

        do {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                    sfp_device->addr.i2c_addr, SFP_ENHANCED_OPTIONS_OFFSET,
                    &buf_sup, SDI_I2C_FLAG_NONE);
            if ((rc != STD_ERR_OK) || (buf_sup == 0x0)) {
                break;
            }

            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                    sfp_i2c_addr, SFP_OPTIONAL_STATUS_CONTROL_OFFSET,
                    &buf_data, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK) {
                break;
            }

            if (STD_BIT_TEST(buf_sup, SFP_SOFT_TX_DISABLE_CTRL_BIT_OFFSET) != 0) {
                if(enable) {
                    STD_BIT_CLEAR(buf_data, SFP_OPTIONAL_SOFT_TX_DISABLE_STATE_BIT_OFFSET);
                } else {
                    STD_BIT_SET(buf_data, SFP_OPTIONAL_SOFT_TX_DISABLE_STATE_BIT_OFFSET);
                }
                rc = sdi_smbus_write_byte(sfp_device->bus_hdl,
                        sfp_i2c_addr, SFP_OPTIONAL_STATUS_CONTROL_OFFSET,
                        buf_data, SDI_I2C_FLAG_NONE);
            } else {
                rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            }

        } while (0);

        sdi_sfp_module_deselect(sfp_priv_data);

    } else {

        rc = sdi_pin_group_acquire_bus(sfp_priv_data->mod_tx_control_hdl);
        if(rc != STD_ERR_OK) {
            return rc;
        }

        do {
            rc = sdi_pin_group_read_level(sfp_priv_data->mod_tx_control_hdl, &value);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("tx control status get failed for %s rc : %d",
                        sfp_device->alias, rc);
                break;
            }

            if(enable) {
                STD_BIT_CLEAR(value, sfp_priv_data->mod_tx_control_bitmask);
            } else {
                STD_BIT_SET(value, sfp_priv_data->mod_tx_control_bitmask);
            }

            rc = sdi_pin_group_write_level(sfp_priv_data->mod_tx_control_hdl, value);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("tx control status set failed for %s rc : %d",
                        sfp_device->alias, rc);
            }
        } while(0);

        sdi_pin_group_release_bus(sfp_priv_data->mod_tx_control_hdl);
    }
    return rc;
}

/**
 * Gets the transmitter status on the specific channel of a SFP
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * status[out]      - "true" if transmitter enabled, else "false"
 * return           - t_std_error
 */
t_std_error sdi_sfp_tx_control_status_get(sdi_resource_hdl_t resource_hdl,
                                          uint_t channel, bool *status)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t value = 0;
    uint8_t buf_sup, buf_data = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    if(channel != SDI_SFP_CHANNEL_NUM) {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (sfp_priv_data->mod_tx_control_hdl == NULL) {

        rc = sdi_sfp_module_select(sfp_device);
        if(rc != STD_ERR_OK) {
            return rc;
        }

        do {
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                    sfp_device->addr.i2c_addr, SFP_ENHANCED_OPTIONS_OFFSET,
                    &buf_sup, SDI_I2C_FLAG_NONE);
            if ((rc != STD_ERR_OK) || (buf_sup == 0x0)) {
                break;
            }

            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                    sfp_i2c_addr, SFP_OPTIONAL_STATUS_CONTROL_OFFSET,
                    &buf_data, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK) {
                break;
            }

            if (STD_BIT_TEST(buf_sup, SFP_SOFT_TX_DISABLE_CTRL_BIT_OFFSET) != 0) {
                if (STD_BIT_TEST(buf_data, SFP_OPTIONAL_SOFT_TX_DISABLE_STATE_BIT_OFFSET) != 0) {
                    *status = false;
                } else {
                    *status = true;
                }
            } else {
                rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            }

        } while (0);

        sdi_sfp_module_deselect(sfp_priv_data);

    } else {

        rc = sdi_pin_group_acquire_bus(sfp_priv_data->mod_tx_control_hdl);
        if (rc != STD_ERR_OK) {
            return rc;
        }

        rc = sdi_pin_group_read_level(sfp_priv_data->mod_tx_control_hdl, &value);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("tx control status get failed for %s rc : %d",
                                  sfp_device->alias, rc);
        }
        sdi_pin_group_release_bus(sfp_priv_data->mod_tx_control_hdl);

        if(rc == STD_ERR_OK) {
            if( (STD_BIT_TEST(value, sfp_priv_data->mod_tx_control_bitmask) != 0) ) {
                *status = false;
            } else {
                *status = true;
            }
        }
    }
    return rc;
}

/**
 * Returns the maximum speed that can be supported by SFP module
 * resource_hdl[in] - handle of the media resource
 * speed[out]     - speed of the media will be filled in this
 * return           - standard t_std_error
 */
t_std_error  sdi_sfp_speed_get(sdi_resource_hdl_t resource_hdl,
                               sdi_media_speed_t *speed)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(speed != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    *speed = sfp_priv_data->capability;
    return STD_ERR_OK;
}

/**
 * Reads the requested parameter value from eeprom
 * resource_hdl[in] - handle of the media resource
 * param[in]        - parametr type that is of interest(e.g wavelength, maximum
 *                    case temperature etc)
 * value[out]     - parameter value which is read from eeprom
 * return           - standard t_std_error
 */
t_std_error sdi_sfp_parameter_get(sdi_resource_hdl_t resource_hdl,
                                  sdi_media_param_type_t param, uint_t *value)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t byte_buf = 0;
    uint8_t word_buf[2] = { 0 } ;
    uint_t offset = 0;
    uint_t size = 0;
    uint16_t temp_buf = 0;
    bool wavelength_tune_sup_status = false;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    offset = param_reg_info[param].offset;
    size =  param_reg_info[param].size;

    if( (offset == 0) && (size == 0) ) {
        SDI_DEVICE_ERRMSG_LOG("parameter type : %u is unsupported on %s", param,
                              sfp_device->alias);
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {

        /* For wavelength request, check if media is tunable */
        if (param == SDI_MEDIA_WAVELENGTH) {
           rc = sdi_is_wavelength_tune_supported(sfp_device, &wavelength_tune_sup_status);
           if (rc != STD_ERR_OK){
               SDI_DEVICE_ERRMSG_LOG("Wavelength tune support status get failed for module %s, rc %u",
                   sfp_device->alias, rc);
               break;
            }
        }
        /* This special case requires page select and i2c device A2h */
        if ((param == SDI_TUNE_WAVELENGTH_PICO_METERS) || (wavelength_tune_sup_status)) {

            rc = sdi_sfp_page_select(sfp_device, SDI_SFP_TUNABLE_PAGE);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("Page select failed for %s, rc %u ",
                        sfp_device->alias, rc);
                break;
            }

            /* Wavelength is 2 byte big endian */
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                sfp_i2c_addr, SFP_WAVELENGTH_SET_OFFSET, &byte_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("Tunable wavelength read failed for module %s", sfp_device->alias);
                return rc;
                break;
            }
            *value = (uint_t)(byte_buf << 8);
            rc = sdi_smbus_read_byte(sfp_device->bus_hdl,
                sfp_i2c_addr, SFP_WAVELENGTH_SET_OFFSET + 1, &byte_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("Tunable wavelength read failed for module %s", sfp_device->alias);
                return rc;
                break;
            }
            *value |= byte_buf;

            /* convert to nm, if pico meters requested, further multiply by 1000  */
            *value *= (param == SDI_TUNE_WAVELENGTH_PICO_METERS) ? (SDI_FREQ_WAVELENGTH_DIVISION_FACTOR * 1000)
                                                                 : SDI_FREQ_WAVELENGTH_DIVISION_FACTOR;

            /* Return page select to default */
            rc = sdi_sfp_page_select(sfp_device, SDI_SFP_DEFAULT_PAGE);
            if (rc != STD_ERR_OK) {
                SDI_DEVICE_ERRMSG_LOG("Page select (default page) failed for %s, rc %u ",
                        sfp_device->alias, rc);
                break;
            }
            break;
        }

        switch (size)
        {
            case SDI_SFP_BYTE_SIZE:
                rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr, offset,
                                         &byte_buf, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d reg : %d for %s rc : %d",
                            sfp_device->addr, offset, sfp_device->alias, rc);
                }
                break;

            case SDI_SFP_WORD_SIZE:

                rc = sdi_smbus_read_word(sfp_device->bus_hdl, sfp_device->addr.i2c_addr, offset,
                                       &temp_buf, SDI_I2C_FLAG_NONE);

                sdi_platform_util_write_16bit_to_bytearray_le(word_buf,temp_buf);

                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d reg : %d for %s rc : %d",
                            sfp_device->addr, offset, sfp_device->alias, rc);
                }
                break;

            default:
                rc = SDI_DEVICE_ERRCODE(EINVAL);
        }
    } while(0);

    sdi_sfp_module_deselect(sfp_priv_data);

	if((rc == STD_ERR_OK) && (param != SDI_TUNE_WAVELENGTH_PICO_METERS) && (!wavelength_tune_sup_status)) {
        if(size == SDI_SFP_BYTE_SIZE) {
            *value = (uint_t)byte_buf;
        } else if(size == SDI_SFP_WORD_SIZE) {
            *value = ( (word_buf[0] << 8) | (word_buf[1]) );
        }
    }
    return rc;
}

/**
 * Read the requested vendor information of a specific media resource
 * resource_hdl[in]     - handle of the media resource
 * vendor_info_type[in] - vendor information that is of interest.
 * vendor_info[out]   - vendor information which is read from eeprom
 * buf_size[in]         - size of the input buffer(vendor_info)
 * return               - standard t_std_error
 */
t_std_error sdi_sfp_vendor_info_get(sdi_resource_hdl_t resource_hdl,
                                    sdi_media_vendor_info_type_t vendor_info_type,
                                    char *vendor_info, size_t size)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    size_t data_len = 0;
    uint_t offset = 0;
    uint8_t *buf_ptr = NULL;
    uint8_t data_buf[SDI_MAX_NAME_LEN];
    bool printable = false;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(vendor_info != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    memset(data_buf, 0, SDI_MAX_NAME_LEN);

    offset = vendor_reg_info[vendor_info_type].offset;
    data_len = vendor_reg_info[vendor_info_type].size;
    printable = vendor_reg_info[vendor_info_type].printable;

    /* Input buffer size should be greater than or equal to data len*/
    STD_ASSERT(size >= data_len);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_smbus_read_multi_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr,
                                   offset, data_buf, data_len - 1, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d reg : %d for %s rc : %d",
                              sfp_device->addr, offset, sfp_device->alias, rc);
    }

    sdi_sfp_module_deselect(sfp_priv_data);

    if(rc == STD_ERR_OK) {
        /* If the field is marked printable, then ensure that it contains only
         * printable characters
         */
        if (printable) {
            for (buf_ptr = &data_buf[0];
                 buf_ptr < &data_buf[data_len - 1];
                 buf_ptr++) {

                if (!isprint(*buf_ptr) && *buf_ptr != SDI_SFP_PADDING_CHAR) {
                    /* Replace with a garbled character indicator */
                    *buf_ptr = SDI_SFP_GARBAGE_CHAR_INDICATOR;
                }
            }
            *buf_ptr = '\0';
        }

        /* vendor name, part number, serial number and revision fields contains
         * ASCII characters, left-aligned and padded on the right with ASCII
         * spaces (20h).*/
         for(buf_ptr = &data_buf[data_len - 1];
                (*(buf_ptr - 1) == SDI_SFP_PADDING_CHAR)
                || (*(buf_ptr - 1) == 0x20); buf_ptr--);
         *buf_ptr = '\0';
         memcpy(vendor_info, data_buf, MIN((buf_ptr-data_buf)+1, size));
    }
    return rc;
}

/**
 * Read the transceiver compliance code information for a specific media resource
 * resource_hdl[in]         - handle of the media resource
 * transceiver_info[out]    - transceiver information which is read from eeprom
 * return                   - standard t_std_error
 */
t_std_error sdi_sfp_transceiver_code_get(sdi_resource_hdl_t resource_hdl,
                                         sdi_media_transceiver_descr_t *transceiver_info)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    uint8_t xvr_buff[SDI_SFP_QUAD_WORD_SIZE];
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(transceiver_info != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    memset(xvr_buff, 0, sizeof(xvr_buff));
    rc = sdi_smbus_read_multi_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr,
                                   SFP_COMPLIANCE_CODE_OFFSET, xvr_buff,
                                   SDI_SFP_QUAD_WORD_SIZE, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                              sfp_device->addr, sfp_device->alias, rc);
    }

    sdi_sfp_module_deselect(sfp_priv_data);

    transceiver_info->sfp_descr.sdi_sfp_eth_10g_code
        = (xvr_buff[0] >> SFP_ETH_10G_CODE_BIT_SHIFT) & SFP_ETH_10G_CODE_MASK;
    transceiver_info->sfp_descr.sdi_sfp_infiniband_code
        = xvr_buff[0] & SFP_INFINIBAND_CODE_MASK;
    transceiver_info->sfp_descr.sdi_sfp_escon_code
        = (xvr_buff[1] >> SFP_ESCON_CODE_BIT_SHIFT) & SFP_ESCON_CODE_MASK;
    transceiver_info->sfp_descr.sdi_sfp_sonet_code
        = xvr_buff[2] | ((xvr_buff[1] & SFP_SONET_CODE_MASK) << SFP_SONET_CODE_BIT_SHIFT);
    transceiver_info->sfp_descr.sdi_sfp_eth_1g_code = xvr_buff[3];
    transceiver_info->sfp_descr.sdi_sfp_fc_distance
        = (xvr_buff[4] >> SFP_FC_DISTANCE_BIT_SHIFT) & SFP_FC_DISTANCE_MASK;
    transceiver_info->sfp_descr.sdi_sfp_fc_technology
        = ((xvr_buff[5] >> SFP_FC_TECH_BIT_SHIFT) & SFP_FC_TECH_MASK2)
             | ((xvr_buff[4] & SFP_FC_TECH_MASK1) << SFP_FC_TECH_BIT_SHIFT);
    transceiver_info->sfp_descr.sdi_sfp_plus_cable_technology
        = (xvr_buff[5] >> SFP_PLUS_CABLE_TECH_BIT_SHIFT) & SFP_PLUS_CABLE_TECH_MASK;
    transceiver_info->sfp_descr.sdi_sfp_fc_media = xvr_buff[6];
    transceiver_info->sfp_descr.sdi_sfp_fc_speed = xvr_buff[7];

    return rc;
}

/**
 * Get the alarm and warning theresholds for the given optic
 * resource_hdl[in]     - Handle of the resource
 * threshold_type[in]   - type of threshold
 * value[out]           - threshold value
 * return               - standard t_std_error
 */
t_std_error sdi_sfp_threshold_get(sdi_resource_hdl_t resource_hdl,
                                  sdi_media_threshold_type_t threshold_type,
                                  float *value)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t diag_mon_value = 0;
    uint_t offset = 0;
    uint8_t threshold_buf[2] = { 0 };
    uint16_t temp_buf = 0;
    sfp_calib_info_t calib_info = { 0 };
    uint_t slope_offset = 0;
    uint_t const_offset = 0;
    sfp_rx_power_calib_info_t rx_pwr_calib_info = { 0 };
    uint8_t rx_power_data[EXT_CAL_RX_POWER_LEN] = { 0 };
    uint8_t *rx_power_data_ptr = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    /* Check whether diag monitoring is supported on this device or not */
    rc = sdi_sfp_parameter_get(resource_hdl, SDI_MEDIA_DIAG_MON_TYPE, &diag_mon_value);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Getting of diag monitoring value is failed for %s rc : %d",
                              sfp_device->alias, rc);
        return rc;
    }

    if( (STD_BIT_TEST(diag_mon_value, SFP_DDM_SUPPORT_BIT_OFFSET) == 0) ) {
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    /* Get the calibration type */
    if( (STD_BIT_TEST(diag_mon_value, SFP_CALIB_TYPE_EXTERNAL_BIT_OFFSET) != 0) ) {
        calib_info.type = SFP_CALIB_TYPE_EXTERNAL;
    } else  if( (STD_BIT_TEST(diag_mon_value, SFP_CALIB_TYPE_INTERNAL_BIT_OFFSET) != 0) ) {
        calib_info.type = SFP_CALIB_TYPE_INTERNAL;
    } else {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    offset = threshold_reg_info[threshold_type].offset;

    switch(threshold_type)
    {
        case SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD:
        case SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD:
        case SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD:
        case SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD:
            slope_offset = SFP_CALIB_TEMP_SLOPE_OFFSET;
            const_offset = SFP_CALIB_TEMP_CONST_OFFSET;
            break;

        case SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD:
        case SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD:
        case SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD:
        case SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD:
            slope_offset = SFP_CALIB_VOLT_SLOPE_OFFSET;
            const_offset = SFP_CALIB_VOLT_CONST_OFFSET;
            break;

        case SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD:
        case SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD:
        case SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD:
        case SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD:
            slope_offset = SFP_CALIB_TX_BIAS_SLOPE_OFFSET;
            const_offset = SFP_CALIB_TX_BIAS_CONST_OFFSET;
            break;

        case SDI_MEDIA_TX_PWR_HIGH_ALARM_THRESHOLD:
        case SDI_MEDIA_TX_PWR_LOW_ALARM_THRESHOLD:
        case SDI_MEDIA_TX_PWR_HIGH_WARNING_THRESHOLD:
        case SDI_MEDIA_TX_PWR_LOW_WARNING_THRESHOLD:
            slope_offset = SFP_CALIB_TX_POWER_SLOPE_OFFSET;
            const_offset = SFP_CALIB_TX_POWER_CONST_OFFSET;
            break;

        case SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD:
        case SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD:
        case SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD:
        case SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD:
            const_offset = SFP_CALIB_RX_POWER_CONST_START_OFFSET;
            break;

        default:
            return SDI_DEVICE_ERRCODE(EINVAL);
    }

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {
        rc = sdi_smbus_read_word(sfp_device->bus_hdl, sfp_i2c_addr,
                offset,&temp_buf, SDI_I2C_FLAG_NONE);
        sdi_platform_util_write_16bit_to_bytearray_le(threshold_buf,temp_buf);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("smbus read failed for threshold values with rc : %d", rc);
            break;
        }

        if(calib_info.type == SFP_CALIB_TYPE_EXTERNAL) {
            if(const_offset == SFP_CALIB_RX_POWER_CONST_START_OFFSET) {
                rc = sdi_smbus_read_multi_byte(sfp_device->bus_hdl, sfp_i2c_addr,
                        const_offset, rx_power_data, EXT_CAL_RX_POWER_LEN, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("smbus read failed for rx power calibration constant with rc : %d",
                                          rc);
                    break;
                }
            } else {
                rc = sdi_smbus_read_word(sfp_device->bus_hdl, sfp_i2c_addr, slope_offset,
                      &temp_buf, SDI_I2C_FLAG_NONE);
                sdi_platform_util_write_16bit_to_bytearray_le((calib_info.slope),temp_buf);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("smbus read failed for temp slope constant rc : %d",rc);
                    break;

                }

                rc = sdi_smbus_read_word(sfp_device->bus_hdl, sfp_i2c_addr, const_offset,
                        &calib_info.offset, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("smbus read failed for temp offset constant rc : %d", rc);
                    break;
                }
            }
        }
    } while(0);

    sdi_sfp_module_deselect(sfp_priv_data);

    if(rc == STD_ERR_OK) {
        if( (threshold_type == SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD) ||
            (threshold_type == SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD) ||
            (threshold_type == SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD) ||
            (threshold_type == SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD) ) {
            *value = convert_sfp_temp(threshold_buf, &calib_info);
        } else if( (threshold_type == SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD) ) {
            *value = convert_sfp_volt(threshold_buf, &calib_info);
        } else if( (threshold_type == SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD) ) {
            rx_pwr_calib_info.type = calib_info.type;
            if(calib_info.type == SFP_CALIB_TYPE_EXTERNAL) {
                rx_power_data_ptr = rx_power_data;
                memcpy(rx_pwr_calib_info.rx_power_const_4, rx_power_data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
                rx_power_data_ptr = rx_power_data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
                memcpy(rx_pwr_calib_info.rx_power_const_3, rx_power_data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
                rx_power_data_ptr = rx_power_data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
                memcpy(rx_pwr_calib_info.rx_power_const_2, rx_power_data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
                rx_power_data_ptr = rx_power_data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
                memcpy(rx_pwr_calib_info.rx_power_const_1, rx_power_data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
                rx_power_data_ptr = rx_power_data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
                memcpy(rx_pwr_calib_info.rx_power_const_0, rx_power_data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
            }
            *value = convert_sfp_rx_power(threshold_buf, &rx_pwr_calib_info);
        } else if( (threshold_type == SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD) ) {
            *value = convert_sfp_tx_bias_current(threshold_buf, &calib_info);
        } else if( (threshold_type == SDI_MEDIA_TX_PWR_HIGH_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_TX_PWR_LOW_ALARM_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_TX_PWR_HIGH_WARNING_THRESHOLD) ||
                   (threshold_type == SDI_MEDIA_TX_PWR_LOW_WARNING_THRESHOLD) ) {
            *value = convert_sfp_tx_power(threshold_buf, &calib_info);
        }
    }
    return rc;
}

/**
 * Read the threshold values for module monitors like temperature and voltage
 * resource_hdl[in]     - Handle of the resource
 * threshold_type[in]   - type of threshold
 * value[out]           - threshold value
 * return               - standard t_std_error
 */
t_std_error sdi_sfp_module_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                 uint_t threshold_type, uint_t *value)
{
    return STD_ERR_UNIMPLEMENTED;
}

/**
 * Read the threshold values for channel monitors like rx-ower and tx-bias
 * resource_hdl[in]     - Handle of the resource
 * threshold_type[in]   - type of threshold
 * value[out]           - threshold value
 * return               - standard t_std_error
 */
t_std_error sdi_sfp_channel_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                  uint_t threshold_type, uint_t *value)
{
    return STD_ERR_UNIMPLEMENTED;
}

/**
 * Read the received output power and calibration constant for output rx_power
 *
 * bus_hdl[in] - Handle of the i2c bus
 * val_offset[in] - Register offset for rx output power
 * buf[out] - buffer for storing rx output power
 * rx_pwr_calib_info[out] - structure contains calibration related information
 * for rx power
 * vs_offset[in] - rx power calibration constant start offset
 *
 * return - standard t_std_error
 */
t_std_error sdi_sfp_rx_power_value_read(sdi_i2c_bus_hdl_t bus_hdl, uint_t val_offset,
                                        uint16_t *buf, sfp_rx_power_calib_info_t *rx_pwr_calib_info,
                                        uint_t vs_offset)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t data[EXT_CAL_RX_POWER_LEN] = { 0 };
    uint8_t *data_ptr = NULL;

    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(rx_pwr_calib_info != NULL);
    STD_ASSERT(buf != NULL);

    rc = sdi_smbus_read_word(bus_hdl, sfp_i2c_addr, val_offset, buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("smbus read failed for temperature with rc : %d", rc);
        return rc;
    }

    if(rx_pwr_calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        rc = sdi_smbus_read_multi_byte(bus_hdl, sfp_i2c_addr, vs_offset, data,
                                       EXT_CAL_RX_POWER_LEN, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("smbus read failed for rx power calibration constant with rc : %d", rc);
            return rc;
        }
        data_ptr = data;
        memcpy(rx_pwr_calib_info->rx_power_const_4, data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
        data_ptr = data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
        memcpy(rx_pwr_calib_info->rx_power_const_3, data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
        data_ptr = data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
        memcpy(rx_pwr_calib_info->rx_power_const_2, data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
        data_ptr = data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
        memcpy(rx_pwr_calib_info->rx_power_const_1, data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
        data_ptr = data_ptr + EXT_CAL_RX_POWER_DATA_LEN;
        memcpy(rx_pwr_calib_info->rx_power_const_0, data_ptr, EXT_CAL_RX_POWER_DATA_LEN);
    }
    return rc;
}

/**
 * Read the requested module monitor values(temperature/votage) from offset specified by
 * val_offset and calibration constants for external calibration types
 *
 * bus_hdl[in] - Handle of the i2c bus
 * val_offset[in] - Register offset for module monitors(temperature/voltage)
 * buf[out] - buffer for storing module monitor values (temperature/voltage)
 * calib_info[out] - structure contains calibration related information
 * vs_offset[in] - Values Slope Offset for external calibration type
 * vc_offset[in] - Values Constant offset for external calibration type
 *
 * return - standard t_std_error
 */
t_std_error sdi_sfp_module_monitor_value_read(sdi_i2c_bus_hdl_t bus_hdl, uint_t val_offset,
                                              uint16_t *buf, sfp_calib_info_t *calib_info,
                                              uint_t vs_offset, uint_t vc_offset)
{
    t_std_error rc = STD_ERR_OK;
    uint16_t temp_buf = 0;
    STD_ASSERT(bus_hdl != NULL);
    STD_ASSERT(calib_info != NULL);
    STD_ASSERT(buf != NULL);

    rc = sdi_smbus_read_word(bus_hdl, sfp_i2c_addr, val_offset, buf,
            SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("smbus read failed for temperature with rc : %d", rc);
        return rc;
    }

    if(calib_info->type == SFP_CALIB_TYPE_EXTERNAL) {
        rc = sdi_smbus_read_word(bus_hdl, sfp_i2c_addr, vs_offset,
               &temp_buf, SDI_I2C_FLAG_NONE);
        sdi_platform_util_write_16bit_to_bytearray_le(calib_info->slope,temp_buf);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("smbus read failed for temp slope constant rc : %d",rc);
            return rc;
        }

        rc = sdi_smbus_read_word(bus_hdl, sfp_i2c_addr, vc_offset,
                &calib_info->offset, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("smbus read failed for temp offset constant rc : %d", rc);
            return rc;
        }
    }
    return rc;
}

/**
 * Debug api to retrieve module monitors assoicated with the specified SFP
 * resource_hdl[in] - Handle of the resource
 * monitor[in]      - monitor which needs to be retrieved
 * value[out]     - Value of the monitor
 * return           - t_std_error
 */
t_std_error sdi_sfp_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                        sdi_media_module_monitor_t monitor, float *value)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf[2] = { 0 };
    uint_t diag_mon_value = 0;
    sfp_calib_info_t calib_info = { 0 };

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);


    /* Check whether diag monitoring is supported on this device or not */
    rc = sdi_sfp_parameter_get(resource_hdl, SDI_MEDIA_DIAG_MON_TYPE, &diag_mon_value);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Getting of diag monitoring value is failed for %s rc : %d",
                              sfp_device->alias, rc);
        return rc;
    }

    if( (STD_BIT_TEST(diag_mon_value, SFP_DDM_SUPPORT_BIT_OFFSET) == 0) ) {
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    /* Get the calibration type */
    if( (STD_BIT_TEST(diag_mon_value, SFP_CALIB_TYPE_EXTERNAL_BIT_OFFSET) != 0) ) {
        calib_info.type = SFP_CALIB_TYPE_EXTERNAL;
    } else  if( (STD_BIT_TEST(diag_mon_value, SFP_CALIB_TYPE_INTERNAL_BIT_OFFSET) != 0) ) {
        calib_info.type = SFP_CALIB_TYPE_INTERNAL;
    } else {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {
        switch (monitor)
        {
            case SDI_MEDIA_TEMP:
                rc = sdi_sfp_module_monitor_value_read(sfp_device->bus_hdl, SFP_TEMPERATURE_OFFSET,
                                                       (uint16_t *)buf, &calib_info, SFP_CALIB_TEMP_SLOPE_OFFSET,
                                                       SFP_CALIB_TEMP_CONST_OFFSET);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("module monitor value read failed for temperature with rc : %d on %s",
                                          rc, sfp_device->alias);
                }
                break;

            case SDI_MEDIA_VOLT:
                rc = sdi_sfp_module_monitor_value_read(sfp_device->bus_hdl, SFP_VOLTAGE_OFFSET,
                                                       (uint16_t *)buf, &calib_info, SFP_CALIB_VOLT_SLOPE_OFFSET,
                                                       SFP_CALIB_VOLT_CONST_OFFSET);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("module monitor value read failed for voltage with rc : %d on %s",
                            rc, sfp_device->alias);
                }
                break;

            default:
                rc = SDI_DEVICE_ERRCODE(EINVAL);
        }
    } while(0);

    sdi_sfp_module_deselect(sfp_priv_data);

    if(rc == STD_ERR_OK) {
        if(monitor == SDI_MEDIA_TEMP) {
            *value = convert_sfp_temp(buf, &calib_info);
        } else if(monitor == SDI_MEDIA_VOLT) {
            *value = convert_sfp_volt(buf, &calib_info);
        }
    }
    return rc;
}

/**
 * Retrieve channel monitors assoicated with the specified SFP
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel whose monitor has to be retreived
 * monitor[in]      - monitor which needs to be retrieved
 * value[out]     - Value of the monitor
 * return           - t_std_error
 */
t_std_error sdi_sfp_channel_monitor_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_channel_monitor_t monitor, float *value)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf[2] = { 0 };
    uint_t diag_mon_value = 0;
    sfp_calib_info_t calib_info = { 0 };
    sfp_rx_power_calib_info_t rx_power_calib_info = { 0 };

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if(channel != SDI_SFP_CHANNEL_NUM) {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    /* Check whether diag monitoring is supported on this device or not */
    rc = sdi_sfp_parameter_get(resource_hdl, SDI_MEDIA_DIAG_MON_TYPE, &diag_mon_value);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Getting of diag monitoring value is failed for %s rc : %d",
                              sfp_device->alias, rc);
        return rc;
    }

    if( (STD_BIT_TEST(diag_mon_value, SFP_DDM_SUPPORT_BIT_OFFSET) == 0) ) {
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    /* Get the calibration type */
    if( (STD_BIT_TEST(diag_mon_value, SFP_CALIB_TYPE_EXTERNAL_BIT_OFFSET) != 0) ) {
        calib_info.type = SFP_CALIB_TYPE_EXTERNAL;
    } else  if( (STD_BIT_TEST(diag_mon_value, SFP_CALIB_TYPE_INTERNAL_BIT_OFFSET) != 0) ) {
        calib_info.type = SFP_CALIB_TYPE_INTERNAL;
    } else {
        return (SDI_DEVICE_ERRCODE(EINVAL));
    }

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {
        switch (monitor)
        {
            case SDI_MEDIA_INTERNAL_RX_POWER_MONITOR:
                rx_power_calib_info.type = calib_info.type;
                rc = sdi_sfp_rx_power_value_read(sfp_device->bus_hdl, SFP_RX_INPUT_POWER_OFFSET,
                                                 (uint16_t *)buf, &rx_power_calib_info,
                                                 SFP_CALIB_RX_POWER_CONST_START_OFFSET);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("smbus read failed for rx power with rc : %d", rc);
                }
                break;

            case SDI_MEDIA_INTERNAL_TX_BIAS_CURRENT:
                rc = sdi_sfp_module_monitor_value_read(sfp_device->bus_hdl, SFP_TX_BIAS_CURRENT_OFFSET,
                                                       (uint16_t *)buf, &calib_info, SFP_CALIB_TX_BIAS_SLOPE_OFFSET,
                                                       SFP_CALIB_TX_BIAS_CONST_OFFSET);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("channel monitor value read failed for tx bias current with rc : %d on %s",
                                           rc, sfp_device->alias);
                }
                break;

            case SDI_MEDIA_INTERNAL_TX_OUTPUT_POWER:
                rc = sdi_sfp_module_monitor_value_read(sfp_device->bus_hdl, SFP_TX_OUTPUT_POWER_OFFSET,
                                                       (uint16_t *)buf, &calib_info, SFP_CALIB_TX_POWER_SLOPE_OFFSET,
                                                       SFP_CALIB_TX_POWER_CONST_OFFSET);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("channel monitor value read failed for tx output power with rc : %d on %s",
                                          rc, sfp_device->alias);
                }
                break;

            default:
                rc = SDI_DEVICE_ERRCODE(EINVAL);
        }
    } while(0);
    sdi_sfp_module_deselect(sfp_priv_data);

    if( rc == STD_ERR_OK) {
        if(monitor == SDI_MEDIA_INTERNAL_RX_POWER_MONITOR) {
            *value = convert_sfp_rx_power(buf, &rx_power_calib_info);
        } else if(monitor == SDI_MEDIA_INTERNAL_TX_BIAS_CURRENT) {
            *value = convert_sfp_tx_bias_current(buf, &calib_info);
        }else if(monitor == SDI_MEDIA_INTERNAL_TX_OUTPUT_POWER) {
            *value = convert_sfp_tx_power(buf, &calib_info);
        }
    }
    return rc;
}

/**
 * Get the inforamtion of whether optional features supported or not on a given
 * module
 * resource_hdl[in] - Handle of the resource
 * feature_support[out] - feature support flags. Flag will be set to "true" if
 * feature is supported else "false"
 * return standard t_std_error
 */
t_std_error sdi_sfp_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                sdi_media_supported_feature_t *feature_support)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(feature_support != NULL);

    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {
        rc = sdi_is_alarm_flags_supported(sfp_device,
                                          &feature_support->sfp_features.alarm_support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        rc = sdi_is_diag_monitoring_supported(sfp_device,
                                              &feature_support->sfp_features.diag_mntr_support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        rc = sdi_is_rate_select_supported(sfp_device,
                                          &feature_support->sfp_features.rate_select_status);
        if (rc != STD_ERR_OK){
            break;
        }

        rc = sdi_is_wavelength_tune_supported(sfp_device,
                                          &feature_support->sfp_features.wavelength_tune_support_status);
        if (rc != STD_ERR_OK){
            break;
        }

    }while(0);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/**
 * Raw read api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address and offset info
 * data[out]      - Data will be filled after read
 * data_len[in]     - length of the data to be read
 * return           - t_std_error
 */
t_std_error sdi_sfp_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_i2c_addr_t address;

    int device_addr = addr->device_addr;
    uint_t offset = addr->offset;

    STD_ASSERT(data != NULL);
    STD_ASSERT(data_len > 0);
    STD_ASSERT(resource_hdl != NULL);


    sfp_device = (sdi_device_hdl_t)resource_hdl;
    address = sfp_device->addr.i2c_addr;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (device_addr == SDI_MEDIA_DEVICE_ADDR_AUTO) {
        /* Currently do nothing. Leave address as is (derived from config file)  */
    } else if ( device_addr < SDI_MEDIA_DEVICE_ADDR_AUTO) {
        SDI_DEVICE_ERRMSG_LOG("Invalid media device address value: %d ", device_addr);
        return SDI_DEVICE_ERRCODE(EINVAL);
    } else {
        /* Use address provided by arg*/
        address.i2c_addr = device_addr;
    }

    rc = sdi_sfp_module_select(sfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    rc = sdi_smbus_read_multi_byte(sfp_device->bus_hdl, address,
           offset, data, data_len, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d reg : %d"
                              "rc : %d", address, offset, rc);
    }

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/**
 * Raw write api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address and offset info
 * data[in]      - Data to write
 * data_len[in]     - length of the data to be written
 * return           - t_std_error
 */
t_std_error sdi_sfp_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_i2c_addr_t address;

    int device_addr = addr->device_addr;
    uint_t offset = addr->offset;

    STD_ASSERT(data != NULL);
    STD_ASSERT(data_len > 0);
    STD_ASSERT(resource_hdl != NULL);


    sfp_device = (sdi_device_hdl_t)resource_hdl;
    address = sfp_device->addr.i2c_addr;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (device_addr == SDI_MEDIA_DEVICE_ADDR_AUTO) {
        /* Currently do nothing. Leave address as is (derived from config file)  */
    } else if ( device_addr < 0) {
        SDI_DEVICE_ERRMSG_LOG("Invalid media device address value: %d ", device_addr);
        return SDI_DEVICE_ERRCODE(EINVAL);
    } else {
        /* Use address provided by arg*/
        address.i2c_addr = device_addr;
    }

    rc = sdi_sfp_module_select(sfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    rc = sdi_smbus_write_multi_byte(sfp_device->bus_hdl, address,
           offset, data, data_len, SDI_I2C_FLAG_NONE);

    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("sfp smbus write failed at addr : %d reg : %d"
                              "rc : %d", address, offset, rc);
    }

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}


/**
 * Raw read api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * offset[in]       - offset from which to read
 * data[out]      - Data will be filled after read
 * data_len[in]     - length of the data to be read
 * return           - t_std_error
 */
t_std_error sdi_sfp_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                          uint8_t *data, size_t data_len)
{
    sdi_media_eeprom_addr_t addr = {
                                    .device_addr = SDI_MEDIA_DEVICE_ADDR_AUTO,
                                    .page        = SDI_MEDIA_PAGE_SELECT_NOT_SUPPORTED,
                                    .offset      = offset
                                   };
    return sdi_sfp_read_generic(resource_hdl, &addr, data, data_len);
}

/**
 * Raw write api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * offset[in]       - offset from which to write
 * data[in]         - input buffer which contains the data to be written
 * data_len[in]     - length of the data to be written
 * return           - t_std_error
 */
t_std_error sdi_sfp_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len)
{
    sdi_media_eeprom_addr_t addr = {
                                    .device_addr = SDI_MEDIA_DEVICE_ADDR_AUTO,
                                    .page        = SDI_MEDIA_PAGE_SELECT_NOT_SUPPORTED,
                                    .offset      = offset
                                   };
    return sdi_sfp_write_generic(resource_hdl, &addr, data, data_len);
}


/**
 * Disable/Enable Auto neg for media PHY in SFP
 * resource_hdl[in] - handle of the resource
 * enable[in]       - "false" to disable and "true" to enable
 * return           - t_std_error
 */
t_std_error sdi_sfp_phy_autoneg_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                     sdi_media_type_t type, bool enable)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_cusfp_phy_autoneg_set(sfp_device, enable);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}


/**
 * SFP Phy mode set (SGMII/GMII/MII ..)
 * resource_hdl[in] - handle of the resource
 * mode[in]         - mode of the interface
 * return           - t_std_error
 */

t_std_error sdi_sfp_phy_mode_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                  sdi_media_type_t type, sdi_media_mode_t mode)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }
    /* 100 usec delay before perform a read */

    std_usleep(SFP_PHY_DELAY);

    rc = sdi_cusfp_phy_mode_set(sfp_device, mode);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/**
 * SFP Phy speed set (1G/100M/10M)
 * resource_hdl[in] - handle of the resource
 * speed[in]         - speed of the interface
 * return           - t_std_error
 */

t_std_error sdi_sfp_phy_speed_set(sdi_resource_hdl_t resource_hdl, uint_t channel,
                                  sdi_media_type_t type, sdi_media_speed_t speed)
{

    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;


    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_cusfp_phy_speed_set(sfp_device, speed);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/**
 * Get media PHY link status.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * status           - true - link up, false - link down
 * return           - t_std_error
 */
t_std_error sdi_sfp_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_type_t type, bool *status)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;


    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_cusfp_phy_link_status_get(sfp_device, status);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/**
 * Set power down state (enable/disable) on media PHY.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * enable           - true - power down, false - power up
 * return           - t_std_error
 */

t_std_error sdi_sfp_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool enable)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;


    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_cusfp_phy_power_down_enable(sfp_device, enable);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/**
 * Control (enable/disable) Fiber/Serdes tx and RX on media PHY.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * enable           - true - Enable Serdes, false - Disable Serdes
 * return           - t_std_error
 */

t_std_error sdi_sfp_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                        sdi_media_type_t type, bool enable)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_cusfp_phy_serdes_control(sfp_device, enable);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}

/*
 * @brief initialize pluged in module
 * @param[in] resource_hdl - handle to the sfp
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */


t_std_error sdi_sfp_module_init (sdi_resource_hdl_t resource_hdl, bool pres)
{
    return STD_ERR_OK;
}


/* See SFF 8690 for all wavelength set related spec */

/* Checks if wavelength tuning is supported */
/* Resides in device A0 */
static t_std_error sdi_is_wavelength_tune_supported (sdi_device_hdl_t sfp_device, bool* status)
{
    uint8_t buf;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(sfp_device != NULL);
    STD_ASSERT(status != NULL);
    rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_device->addr.i2c_addr,
        SFP_OPTIONS2_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
            sfp_device->addr, sfp_device->alias, rc);
        return rc;
    }
    *status = (bool)(buf & SDI_SFP_TUNABLE_SUPPORT_BITMASK);
    return rc;
}

/* This implements page select for SFP */
static t_std_error sdi_sfp_page_select (sdi_device_hdl_t sfp_device, uint_t page)
{
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(sfp_device != NULL);
    rc = sdi_smbus_write_byte(sfp_device->bus_hdl,
        sfp_i2c_addr, SFP_PAGE_SELECT_BYTE_OFFSET, page, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("Page %u select failed for module %s", page, sfp_device->alias);
        return rc;
    }

    return rc;
}

static t_std_error sdi_sfp_get_tunable_capabilities (sdi_device_hdl_t sfp_device, sdi_sfp_tunable_capabilities_t* tune_cap)
{
    uint8_t support_buf = 0;
    uint8_t capabilities_buf[SDI_SFP_TUNABLE_FREQ_CAPABILITIES_LEN] = {0};
    float min_freq_int_part = 0.0, min_freq_frac_part = 0.0;
    float max_freq_int_part = 0.0, max_freq_frac_part = 0.0;
    float grid_spacing = 0.0;
    t_std_error rc = STD_ERR_OK;


    STD_ASSERT(sfp_device != NULL);
    STD_ASSERT(tune_cap != NULL);

    rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
        SFP_TUNE_TYPE_SUPPORT_OFFSET, &support_buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
            sfp_device->addr, sfp_device->alias, rc);
        return rc;
    }

    tune_cap->channel_set_support = (bool)STD_BIT_TEST(support_buf, SDI_SFP_TX_FREQ_CHANNEL_TUNE_SUPPORT_BITMASK);
    tune_cap->wavelength_set_support = (bool)STD_BIT_TEST(support_buf, SDI_SFP_TX_WAVELENGTH_TUNE_SUPPORT_BITMASK);

    /* if none of the tune methods is supported, return  */
    if (!((tune_cap->channel_set_support) | (tune_cap->wavelength_set_support))) {
        rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
        SDI_DEVICE_ERRMSG_LOG("Attempt to get tunable support info on unsupported module %s",
            sfp_device->alias);
        return rc;
    }
    rc = sdi_smbus_read_multi_byte(sfp_device->bus_hdl, sfp_i2c_addr,
        SFP_TUNABLE_FREQ_CAPABILITIES_OFFSET, capabilities_buf,
            sizeof(capabilities_buf)/sizeof(capabilities_buf[0]), SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr %d for %s rc : %d",
            sfp_device->addr, sfp_device->alias, rc);
        return rc;
    }

    /* data is in big endian */
    min_freq_int_part = SDI_TWO_BYTE_TO_UINT16(
        capabilities_buf[SDI_SFP_CAPABILITIES_MIN_FREQ_INT_PART_MSB_BYTE_POS],
        capabilities_buf[SDI_SFP_CAPABILITIES_MIN_FREQ_INT_PART_LSB_BYTE_POS]);
    min_freq_frac_part = SDI_TWO_BYTE_TO_UINT16(
        capabilities_buf[SDI_SFP_CAPABILITIES_MIN_FREQ_FRAC_PART_MSB_BYTE_POS],
        capabilities_buf[SDI_SFP_CAPABILITIES_MIN_FREQ_FRAC_PART_LSB_BYTE_POS]);
    max_freq_int_part = SDI_TWO_BYTE_TO_UINT16(
        capabilities_buf[SDI_SFP_CAPABILITIES_MAX_FREQ_INT_PART_MSB_BYTE_POS],
        capabilities_buf[SDI_SFP_CAPABILITIES_MAX_FREQ_INT_PART_LSB_BYTE_POS]);
    max_freq_frac_part = SDI_TWO_BYTE_TO_UINT16(
        capabilities_buf[SDI_SFP_CAPABILITIES_MAX_FREQ_FRAC_PART_MSB_BYTE_POS],
        capabilities_buf[SDI_SFP_CAPABILITIES_MAX_FREQ_FRAC_PART_LSB_BYTE_POS]);
    grid_spacing = SDI_TWO_BYTE_TO_UINT16(
        capabilities_buf[SDI_SFP_CAPABILITIES_GRID_SPACING_MSB_BYTE_POS],
        capabilities_buf[SDI_SFP_CAPABILITIES_GRID_SPACING_LSB_BYTE_POS]);

    tune_cap->min_freq = SDI_FREQ_JOIN(min_freq_int_part, min_freq_frac_part);
    tune_cap->max_freq = SDI_FREQ_JOIN(max_freq_int_part, max_freq_frac_part);

    if (tune_cap->max_freq < tune_cap->min_freq) {
        /* Something really went wrong */
        rc = SDI_DEVICE_ERRCODE(EBADMSG);
        SDI_DEVICE_ERRMSG_LOG("Read bad freqency data on module %s",
            sfp_device->alias);
        return rc;
    }
    /* convert grid spacing from 0.1GHz units to THz */
    tune_cap->grid_spacing = (float)grid_spacing / SDI_FREQ_FRACTIONAL_PART_DIVISION_FACTOR;
    tune_cap->channel_count = SDI_GET_CHANNEL_NO_FROM_FREQ(tune_cap->max_freq, tune_cap->min_freq, tune_cap->grid_spacing);
    return rc;
}



/* Takes wavelength in nanometers */
static t_std_error sdi_sfp_set_tunable_module_by_wavelength (sdi_device_hdl_t sfp_device,
                    sdi_sfp_tunable_capabilities_t* tune_cap, float wavelength_nm)
{
    float target_freq_thz = 0.0;
    uint16_t target_channel_number = 0;
    t_std_error rc = STD_ERR_OK;
    uint16_t wavelength_to_write = 0; /* write wavelength as 16 bit num in units of 0.05nm */


    target_freq_thz = WAVELENGTH_NM_TO_FREQ_THZ(wavelength_nm);

    /* check that freq corresponding to given wavelength is not out of bounds */
    if ( (target_freq_thz > tune_cap->max_freq) || (target_freq_thz < tune_cap->min_freq)) {
        rc = SDI_DEVICE_ERRCODE(EINVAL);
        SDI_DEVICE_ERRMSG_LOG("Out of bounds! Cannot set wavelength %fnm on module %d. "
            ,"Valid range is %fnm to %fnm"
            , FREQ_THZ_TO_WAVELENGTH_NM(target_freq_thz), sfp_device->alias
            , WAVELENGTH_NM_TO_FREQ_THZ(tune_cap->min_freq), WAVELENGTH_NM_TO_FREQ_THZ(tune_cap->max_freq));
        return rc;
    }

    wavelength_to_write = (uint16_t)(wavelength_nm / SDI_FREQ_WAVELENGTH_DIVISION_FACTOR);
    target_channel_number = (uint16_t)SDI_GET_CHANNEL_NO_FROM_FREQ(target_freq_thz, tune_cap->min_freq, tune_cap->grid_spacing);

    /* Now choose which way to set wavelength. One can use channel or direct wavelength write */
    /* Channel set gets priority since int is easier to deal with and less chance of float errors */
    /* All reads/writes are big endian */
    if (tune_cap->channel_set_support) {
        rc = sdi_smbus_write_byte(sfp_device->bus_hdl,
            sfp_i2c_addr, SFP_CHANNEL_NUMBER_SET_OFFSET, (uint8_t)(target_channel_number >> 8), SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Wavelength set failed for module %s", sfp_device->alias);
            return rc;
        }
        rc = sdi_smbus_write_byte(sfp_device->bus_hdl,
            sfp_i2c_addr, SFP_CHANNEL_NUMBER_SET_OFFSET+1, (uint8_t)(target_channel_number), SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Wavelength set failed for module %s", sfp_device->alias);
            return rc;
        }
    } else if (tune_cap->wavelength_set_support) {
        rc = sdi_smbus_write_byte(sfp_device->bus_hdl,
            sfp_i2c_addr, SFP_WAVELENGTH_SET_OFFSET, (uint8_t)(wavelength_to_write >> 8), SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Wavelength set failed for module %s", sfp_device->alias);
            return rc;
        }
        rc = sdi_smbus_write_byte(sfp_device->bus_hdl,
            sfp_i2c_addr, SFP_WAVELENGTH_SET_OFFSET+1, (uint8_t)(wavelength_to_write), SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Wavelength set failed for module %s", sfp_device->alias);
            return rc;
        }
    } else {
        rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
        SDI_DEVICE_ERRMSG_LOG("Attempt to set wavelength on unsupported module %s",
            sfp_device->alias);
        return rc;
    }

    return rc;
}

/* This function is a simplistic solution to get the status of the wavelength set operation */
/* This function can take up to 100ms to complete. Uses blocking delays */
static t_std_error sdi_sfp_tune_set_status_get (sdi_device_hdl_t sfp_device, bool* status)
{
    uint_t elapsed_time_ms = 0;
    uint8_t unlatched_status = ~0;
    uint8_t latched_status = ~0;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(sfp_device != NULL);
    STD_ASSERT(status != NULL);
    *status =  false;

    while (elapsed_time_ms < SDI_WAVELENGTH_SET_TIMEOUT_MS){
        rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
            SFP_TUNE_TYPE_SUPPORT_OFFSET, &unlatched_status, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                sfp_device->addr, sfp_device->alias, rc);
        }
        rc = sdi_smbus_read_byte(sfp_device->bus_hdl, sfp_i2c_addr,
            SFP_TUNE_TYPE_SUPPORT_OFFSET, &latched_status, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("sfp smbus read failed at addr : %d for %s rc : %d",
                sfp_device->addr, sfp_device->alias, rc);
        }
        if (TEST_UNLATCHED_STATUS(unlatched_status) && (TEST_LATCHED_STATUS(latched_status))){
            *status = true;
            break;
        }
        std_usleep(SDI_WAVELENGTH_SET_POLL_PERIOD_MS * 1000);
        elapsed_time_ms +=SDI_WAVELENGTH_SET_POLL_PERIOD_MS;
    }
    return rc;
}

/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */

t_std_error sdi_sfp_wavelength_set (sdi_resource_hdl_t resource_hdl, float value)
{
    sdi_device_hdl_t sfp_device = NULL;
    sfp_device_t *sfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    bool status = false;
    sdi_sfp_tunable_capabilities_t tune_cap;


    STD_ASSERT(resource_hdl != NULL);
    sfp_device = (sdi_device_hdl_t)resource_hdl;
    sfp_priv_data = (sfp_device_t *)sfp_device->private_data;
    STD_ASSERT(sfp_priv_data != NULL);

    if (value <= 0.0) {
        SDI_DEVICE_ERRMSG_LOG("Invalid argument. Wavelength cannot be negative. Module %s "
            , sfp_device->alias);
        return SDI_DEVICE_ERRCODE(EINVAL);
    }

    rc = sdi_sfp_module_select(sfp_device);
    if(rc != STD_ERR_OK) {
        return rc;
    }

    do {

        /* First check for tunable support */
        rc = sdi_is_wavelength_tune_supported (sfp_device, &status);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Failed to read tunable suppport status on module %s",
                sfp_device->alias);
            break;
        }
        if (!status) {
            SDI_DEVICE_ERRMSG_LOG("Attempt to set wavelength on unsupported module %s",
                sfp_device->alias);
            break;
        }

        /* Since all tunable controls are in dev A2, page 02, page 02 needs to be selected on device A2 */
        rc = sdi_sfp_page_select (sfp_device, SDI_SFP_TUNABLE_PAGE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Failed to select page %u on module %s", SDI_SFP_TUNABLE_PAGE,
                sfp_device->alias);
            break;
        }

        /* Now get the actual tunable capabilities of module. See sdi_sfp_tunable_capabilities_t */
        rc = sdi_sfp_get_tunable_capabilities (sfp_device, &tune_cap);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Failed to get tunable module capabilities on module %s",
                sfp_device->alias);
            break;
        }

        /* Finally set the wavelength, given the module capabilities  */
        rc = sdi_sfp_set_tunable_module_by_wavelength (sfp_device, &tune_cap, value);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Failed to set wavelength %f on tunable module %s",
                value, sfp_device->alias);
            break;
        }

        /* After setting the desired wavelength, the operation may fail for multiple reasons */
        /* So one needs to get the status by polling the status registers for up to SDI_WAVELENGTH_SET_TIMEOUT_MS */
        /* Status poll is blocking */
        rc = sdi_sfp_tune_set_status_get (sfp_device, &status);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Failed to get status of wavelength set on module %s",
                sfp_device->alias);
            break;
        }
        if (!status) {
            rc = SDI_DEVICE_ERRCODE(~STD_ERR_OK);
            SDI_DEVICE_ERRMSG_LOG("Wavelength set failed on module %s",
                sfp_device->alias);
            break;
        }

        /* Return page select to default  */
        rc = sdi_sfp_page_select (sfp_device, SDI_SFP_DEFAULT_PAGE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Failed to select default page on module %s",
                sfp_device->alias);
            break;
        }

    } while (0);

    sdi_sfp_module_deselect(sfp_priv_data);

    return rc;
}


t_std_error sdi_sfp_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter) {

    *qsa_adapter = SDI_QSA_ADAPTER_NONE;
    return STD_ERR_OK;;
}
