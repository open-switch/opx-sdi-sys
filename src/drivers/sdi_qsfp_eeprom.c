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
 * filename: sdi_qsfp_eeprom.c
 */


/******************************************************************************
 * sdi_qsfp_eeprom.c
 * Implements the QSFP eeprom related APIs
 *****************************************************************************/
#include "sdi_resource_internal.h"
#include "sdi_device_common.h"
#include "sdi_pin_group_bus_framework.h"
#include "sdi_pin_group_bus_api.h"
#include "sdi_i2c_bus_api.h"
#include "sdi_media.h"
#include "sdi_sfp.h"
#include "sdi_qsfp.h"
#include "sdi_qsfp_reg.h"
#include "sdi_media_internal.h"
#include "sdi_media_phy_mgmt.h"
#include "std_error_codes.h"
#include "std_assert.h"
#include "std_time_tools.h"
#include "std_bit_ops.h"
#include "sdi_platform_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "math.h"

#define QSFP_TX_LOS_FLAG(x) ( QSFP_TX_LOS_BIT_OFFSET << (x) )
#define QSFP_RX_LOS_FLAG(x) ( QSFP_RX_LOS_BIT_OFFSET << (x) )

#define QSFP_DD_TX_LOS_FLAG(x) ( QSFP_DD_TX_LOS_BIT_OFFSET << (x) )
#define QSFP_DD_RX_LOS_FLAG(x) ( QSFP_DD_RX_LOS_BIT_OFFSET << (x) )

#define QSFP_TX_ENABLE_DELAY (400 * 1000)
#define QSFP_TX_DISABLE_DELAY (100 * 1000)

#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define SDI_QSFP_PADDING_CHAR 0
#define SDI_QSFP_GARBAGE_CHAR_INDICATOR '?'

/* To be removed once full qsfp28 dd rev 2 support is added */
#define QSFP28_DD_EEPROM_VERSION_OFFSET   1
#define QSFP28_DD_EEPROM_VERSION_2        0x20
#define QSFP28_DD_EEPROM_VERSION_3        0x30
#define QSFP28_DD_DATAPATH_POWERUP_OFFSET 92
#define QSFP28_DD_DATAPATH_POWERUP_OFFSET_REV3 128 // on page 16
#define QSFP28_DD_DATAPATH_POWERUP_VALUE  0xFF

/* QSFP channel numbers */
enum {
    SDI_QSFP_CHANNEL_ONE = 0,
    SDI_QSFP_CHANNEL_TWO,
    SDI_QSFP_CHANNEL_THREE,
    SDI_QSFP_CHANNEL_FOUR,
    SDI_QSFP_CHANNEL_FIVE,
    SDI_QSFP_CHANNEL_SIX,
    SDI_QSFP_CHANNEL_SEVEN,
    SDI_QSFP_CHANNEL_EIGHT,
};

 /*QSFP parameter sizes */
enum {
    SDI_QSFP_BYTE_SIZE = 1,
    SDI_QSFP_WORD_SIZE = 2,
    SDI_QSFP_DOUBLE_WORD_SIZE = 4,
    SDI_QSFP_QUAD_WORD_SIZE = 8,
};

enum {
    SDI_QSFP_RX_PWR_OMA = 0,
    SDI_QSFP_RX_PWR_AVG
};

/* qsfp register information structure */
typedef struct sdi_qsfp_reg_info {
    uint_t offset; /* register offset */
    uint_t size; /* register size */
    bool   printable; /* field contains printable data */
} sdi_qsfp_reg_info_t;

/* parameter register information strucutre. Parameters should be defined in the
 * same order of sdi_media_param_type_t */
static sdi_qsfp_reg_info_t param_reg_info[] = {
    { QSFP_WAVELENGTH_OFFSET, SDI_QSFP_WORD_SIZE }, /* for SDI_MEDIA_WAVELENGTH */
    { QSFP_WAVELENGTH_TOLERANCE_OFFSET, SDI_QSFP_WORD_SIZE }, /* for SDI_MEDIA_WAVELENGTH_TOLERANCE */
    { QSFP_MAX_CASE_TEMP_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_MAX_CASE_TEMP */
    { QSFP_CC_BASE_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_CC_BASE */
    { QSFP_CC_EXT_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_CC_EXT */
    { QSFP_CONNECTOR_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_CONNECTOR */
    { QSFP_ENCODING_TYPE_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_ENCODING_TYPE */
    { QSFP_NM_BITRATE_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_NM_BITRATE */
    { QSFP_IDENTIFIER_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_IDENTIFIER */
    { QSFP_EXT_IDENTIFIER_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_EXT_IDENTIFIER */
    { QSFP_LENGTH_SMF_KM_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_SMF_KM */
    { QSFP_LENGTH_OM1_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_OM1 */
    { QSFP_LENGTH_OM2_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_OM2 */
    { QSFP_LENGTH_OM3_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_LENGTH_OM3 */
    { QSFP_LENGTH_CABLE_ASSEMBLY_OFFSET, SDI_QSFP_BYTE_SIZE  }, /* for SDI_MEDIA_LENGTH_CABLE_ASSEMBLY */
    { 0, 0}, /* for SDI_MEDIA_LENGTH_SMF, not supported on QSFP */
    { QSFP_OPTIONS1_OFFSET, SDI_QSFP_DOUBLE_WORD_SIZE }, /* for SDI_MEDIA_OPTIONS */
    { QSFP_ENHANCED_OPTIONS_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_ENHANCED_OPTIONS */
    { QSFP_DIAG_MON_TYPE_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_DIAG_MON_TYPE */
    { QSFP_DEVICE_TECH_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_DEVICE_TECH */
    { 0, 0}, /* for SDI_MEDIA_MAX_BITRATE, not supported on QSFP */
    { 0, 0}, /* for SDI_MEDIA_MIN_BITRATE, not supported on QSFP */
    { 0, 0}, /* for SDI_MEDIA_EXT_COMPLIANCE_CODE, not supported on QSFP */
    { QSFP_FREE_SIDE_DEV_PROP_OFFSET, SDI_QSFP_BYTE_SIZE}, /* For  SDI_FREE_SIDE_DEV_PROP */
    { 0, 0}, /* for SDI_TUNE_WAVELENGTH_PICO_METERS, not applicable for QSFP yet */
};

/* For QSFP28-DD rev 3: parameter register information strucutre. Parameters should be defined in the
 * same order of sdi_media_param_type_t */
   static sdi_qsfp_reg_info_t param_reg_info_qsfp28_dd_r3[] = {
    { 0, 0 }, /* not yet implemented */
    { 0, 0 }, /* for 0 */
    { 0, 0 }, /* for SDI_MEDIA_MAX_CASE_TEMP */
    { 0, 0 }, /* for SDI_MEDIA_CC_BASE */
    { 0, 0 }, /* for SDI_MEDIA_CC_EXT */
    { QSFP28_DD_R3_CONNECTOR_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_CONNECTOR */
    { 0, 0 }, /* for SDI_MEDIA_ENCODING_TYPE */
    { 0, 0 }, /* for SDI_MEDIA_NM_BITRATE */
    { QSFP28_DD_R3_IDENTIFIER_OFFSET, SDI_QSFP_BYTE_SIZE }, /* for SDI_MEDIA_IDENTIFIER */
    { 0, 0 }, /* for SDI_MEDIA_EXT_IDENTIFIER */
    { 0, 0 }, /* for SDI_MEDIA_LENGTH_SMF_KM */
    { 0, 0 }, /* for SDI_MEDIA_LENGTH_OM1 */
    { 0, 0 }, /* for SDI_MEDIA_LENGTH_OM2 */
    { 0, 0 }, /* for SDI_MEDIA_LENGTH_OM3 */
    { QSFP28_DD_R3_LENGTH_CABLE_ASSEMBLY_OFFSET, SDI_QSFP_BYTE_SIZE  }, /* for SDI_MEDIA_LENGTH_CABLE_ASSEMBLY */
    { 0, 0}, /* for SDI_MEDIA_LENGTH_SMF, not supported on QSFP28_DD_R3 */
    { 0, 0 }, /* for SDI_MEDIA_OPTIONS */
    { 0, 0 }, /* for SDI_MEDIA_ENHANCED_OPTIONS */
    { 0, 0 }, /* for SDI_MEDIA_DIAG_MON_TYPE */
    { 0, 0 }, /* for SDI_MEDIA_DEVICE_TECH */
    { 0, 0 }, /* for SDI_MEDIA_MAX_BITRATE, not supported on QSFP28_DD_R3 */
    { 0, 0 }, /* for SDI_MEDIA_MIN_BITRATE, not supported on QSFP28_DD_R3 */
    { 0, 0 }, /* for SDI_MEDIA_EXT_COMPLIANCE_CODE, not supported on QSFP28_DD_R3 */
    { 0, 0 }, /* For  SDI_FREE_SIDE_DEV_PROP */
};

/* vendor register information structure. Parameters in this structure should be
 * defined in the same order of sdi_media_vendor_info_type_t */
static sdi_qsfp_reg_info_t vendor_reg_info[] = {
    { QSFP_VENDOR_NAME_OFFSET, SDI_MEDIA_MAX_VENDOR_NAME_LEN, true }, /* for SDI_MEDIA_VENDOR_NAME */
    { QSFP_VENDOR_OUI_OFFSET, SDI_MEDIA_MAX_VENDOR_OUI_LEN, false }, /* for SDI_MEDIA_VENDOR_OUI */
    { QSFP_VENDOR_SN_OFFSET, SDI_MEDIA_MAX_VENDOR_SERIAL_NUMBER_LEN, true }, /* for SDI_MEDIA_VENDOR_SN */
    { QSFP_VENDOR_DATE_OFFSET, SDI_MEDIA_MAX_VENDOR_DATE_LEN, true }, /* for SDI_MEDIA_VENDOR_DATE */
    { QSFP_VENDOR_PN_OFFSET, SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN, true }, /* for SDI_MEDIA_VENDOR_PN */
    { QSFP_VENDOR_REVISION_OFFSET, SDI_MEDIA_MAX_VENDOR_REVISION_LEN, true }, /* for SDI_MEDIA_VENDOR_REVISION */
    { QSFP_VENDOR_PN_OFFSET, SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN, true } /* Repeated because field not applicable in QSFP*/
};

/* vendor register information structure. Parameters in this structure should be
 * defined in the same order of sdi_media_vendor_info_type_t */
static sdi_qsfp_reg_info_t vendor_reg_info_qsfp28_dd_r3[] = {
    { QSFP28_DD_R3_VENDOR_NAME_OFFSET, SDI_MEDIA_MAX_VENDOR_NAME_LEN, true }, /* for SDI_MEDIA_VENDOR_NAME */
    { QSFP28_DD_R3_VENDOR_OUI_OFFSET, SDI_MEDIA_MAX_VENDOR_OUI_LEN, false }, /* for SDI_MEDIA_VENDOR_OUI */
    { QSFP28_DD_R3_VENDOR_SN_OFFSET, SDI_MEDIA_MAX_VENDOR_SERIAL_NUMBER_LEN, true }, /* for SDI_MEDIA_VENDOR_SN */
    { QSFP28_DD_R3_VENDOR_DATE_OFFSET, SDI_MEDIA_MAX_VENDOR_DATE_LEN, true }, /* for SDI_MEDIA_VENDOR_DATE */
    { QSFP28_DD_R3_VENDOR_PN_OFFSET, SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN, true }, /* for SDI_MEDIA_VENDOR_PN */
    { QSFP28_DD_R3_VENDOR_REVISION_OFFSET, SDI_MEDIA_MAX_VENDOR_REVISION_LEN, true }, /* for SDI_MEDIA_VENDOR_REVISION */
    { QSFP28_DD_R3_VENDOR_PN_OFFSET, SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN, true } /* Repeated because field not applicable in QSFP*/
};


/* threshold value register information structure. Parameters in this structure
 * should be defined in the same order of sdi_media_threshold_type_t */
static sdi_qsfp_reg_info_t threshold_reg_info[] = {
    /* for SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD */
    { QSFP_TEMP_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD */
    { QSFP_TEMP_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD */
    { QSFP_TEMP_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD */
    { QSFP_TEMP_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD */
    { QSFP_VOLT_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD */
    { QSFP_VOLT_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD */
    { QSFP_VOLT_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD */
    { QSFP_VOLT_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD */
    { QSFP_RX_PWR_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD */
    { QSFP_RX_PWR_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD */
    { QSFP_RX_PWR_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD */
    { QSFP_RX_PWR_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD */
    { QSFP_TX_BIAS_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD */
    { QSFP_TX_BIAS_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD */
    { QSFP_TX_BIAS_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD */
    { QSFP_TX_BIAS_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_HIGH_ALARM_THRESHOLD */
    { 0, 0 },
    /* for SDI_MEDIA_TX_PWR_LOW_ALARM_THRESHOLD */
    { 0, 0 },
    /* for SDI_MEDIA_TX_PWR_HIGH_WARNING_THRESHOLD */
    { 0, 0 },
    /* for SDI_MEDIA_TX_PWR_LOW_WARNING_THRESHOLD */
    { 0, 0 },
};

/* threshold value register information structure. Parameters in this structure
 * should be defined in the same order of sdi_media_threshold_type_t */
 /* These are used for QSFP28-DD rev 3 and up */
static sdi_qsfp_reg_info_t threshold_reg_info_qsfp28_dd_r3[] = {
    /* for SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD */
    { QSFP28_DD_R3_TEMP_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD */
    { QSFP28_DD_R3_TEMP_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD */
    { QSFP28_DD_R3_TEMP_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD */
    { QSFP28_DD_R3_TEMP_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD */
    { QSFP28_DD_R3_VOLT_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD */
    { QSFP28_DD_R3_VOLT_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD */
    { QSFP28_DD_R3_VOLT_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD */
    { QSFP28_DD_R3_VOLT_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD */
    { QSFP28_DD_R3_RX_PWR_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD */
    { QSFP28_DD_R3_RX_PWR_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD */
    { QSFP28_DD_R3_RX_PWR_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD */
    { QSFP28_DD_R3_RX_PWR_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD */
    { QSFP28_DD_R3_TX_BIAS_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },

    /* for SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD */
    { QSFP28_DD_R3_TX_BIAS_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD */
    { QSFP28_DD_R3_TX_BIAS_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD */
    { QSFP28_DD_R3_TX_BIAS_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_HIGH_ALARM_THRESHOLD */
    { QSFP28_DD_R3_TX_PWR_HIGH_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_LOW_ALARM_THRESHOLD */
    { QSFP28_DD_R3_TX_PWR_LOW_ALARM_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_HIGH_WARNING_THRESHOLD */
    { QSFP28_DD_R3_TX_PWR_HIGH_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
    /* for SDI_MEDIA_TX_PWR_LOW_WARNING_THRESHOLD */
    { QSFP28_DD_R3_TX_PWR_LOW_WARNING_THRESHOLD_OFFSET, SDI_QSFP_WORD_SIZE },
};


/* For some QSFP28-DD mediia using revision 0x30 and up, length code is used. One has to extract the actual length using algo below */
/* Length code is given as such: take the lower 5 bits and multiply by the 10 ^ (upper 2 bits), all divided by 10  */
/*This evalates to: [lower 5 bits] * pow(10, [upper 2 bits] - 1) */
static double sdi_convert_length_code_to_length (uint8_t len_code)
{
    return ((LEN_CODE_MANTISSA_BITMASK & len_code) >> LEN_CODE_MANTISSA_SHIFT)
                * pow(10, ((len_code & LEN_CODE_EXPONENT_BITMASK) >> LEN_CODE_EXPONENT_SHIFT) - 1);
}

static inline t_std_error sdi_qsfp_module_select (sdi_device_hdl_t qsfp_device)
{
    t_std_error rc = STD_ERR_OK;
    qsfp_device_t *qsfp_priv_data = NULL;

    STD_ASSERT(qsfp_device != NULL);
    qsfp_priv_data = (qsfp_device_t *) qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    /* Check whether mux selection required or not on this particular qsfp. If
     * mux selectin is not required just move on to module selection */

    if(qsfp_priv_data->mux_sel_hdl != NULL) {
        rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mux_sel_hdl);
        if (rc != STD_ERR_OK){
            return rc;
        }

        rc = sdi_pin_group_write_level(qsfp_priv_data->mux_sel_hdl,
                                       qsfp_priv_data->mux_sel_value);

        if (rc != STD_ERR_OK){
            /* mux selection failed, hence release the lock.*/
            sdi_pin_group_release_bus(qsfp_priv_data->mux_sel_hdl);
            SDI_DEVICE_ERRMSG_LOG("mux selection is failed for %s rc : %d",
                    qsfp_device->alias, rc);
            return rc;
        }

    }

    if (qsfp_priv_data->mod_sel_hdl != NULL) {
        rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_sel_hdl);
        if (rc != STD_ERR_OK){

            if(qsfp_priv_data->mux_sel_hdl != NULL) {
                sdi_pin_group_release_bus(qsfp_priv_data->mux_sel_hdl);
            }

            return rc;
        }

        rc = sdi_pin_group_write_level(qsfp_priv_data->mod_sel_hdl,
                                       qsfp_priv_data->mod_sel_value);

        if (rc != STD_ERR_OK){
            /* module selection failed, hence release the lock.*/

            if(qsfp_priv_data->mux_sel_hdl != NULL) {
                sdi_pin_group_release_bus(qsfp_priv_data->mux_sel_hdl);
            }
            sdi_pin_group_release_bus(qsfp_priv_data->mod_sel_hdl);
            SDI_DEVICE_ERRMSG_LOG("module selection is failed for %s rc : %d",
                    qsfp_device->alias, rc);
            return rc;
        }
    }

    /* If module selection success, releasing the lock taken care by
     * sdi_qsfp_module_deselect api */

    return rc;
}

static inline void sdi_qsfp_module_deselect(qsfp_device_t *qsfp_priv_data)
{
    STD_ASSERT(qsfp_priv_data != NULL);

    if(qsfp_priv_data->mux_sel_hdl != NULL) {
        sdi_pin_group_release_bus(qsfp_priv_data->mux_sel_hdl);
    }

    if(qsfp_priv_data->mod_sel_hdl != NULL) {
        sdi_pin_group_release_bus(qsfp_priv_data->mod_sel_hdl);
    }
}

/* This function validates the channel number */
static inline bool sdi_qsfp_validate_channel (uint_t channel, qsfp_category_t category)
{
    if (category == SDI_CATEGORY_QSFPDD) {
        return (channel <= SDI_QSFP_CHANNEL_EIGHT);
    } else {
        return( channel <= SDI_QSFP_CHANNEL_FOUR );
    }
}

/* This function checks whether paging is supported or not on a QSFP. If paging
 * is supported then selects requested page. */
static inline t_std_error sdi_qsfp_page_select (sdi_device_hdl_t qsfp_device,
                                                uint_t page_num)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(qsfp_device != NULL);

    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_STATUS_INDICATOR_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d ", qsfp_device->addr);
        return rc;
    }

    if( (STD_BIT_TEST(buf, QSFP_FLAT_MEM_BIT_OFFSET)) != 0 ) {
        return SDI_DEVICE_ERRCODE(ENOTSUP);
    }

    rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                              QSFP_PAGE_SELECT_BYTE_OFFSET, page_num, SDI_I2C_FLAG_NONE);
    if(rc != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d ", qsfp_device->addr);
    }
    return rc;
}

/* This function checks whether tx_disable implemented for this module */
static inline t_std_error sdi_is_tx_control_supported(sdi_device_hdl_t qsfp_device,
                                                      bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(qsfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status = false;

    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_OPTIONS4_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    if( (STD_BIT_TEST(buf, QSFP_TX_DISABLE_BIT_OFFSET) != 0) ) {
        *support_status = true;
    } else {
        *support_status = false;
    }
    return rc;
}

/* This function checks whether tx/rx cdr control implemented for this module */
static inline t_std_error sdi_is_cdr_control_supported(sdi_device_hdl_t qsfp_device,
                                                       uint_t *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(qsfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status = 0;

    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_OPTIONS3_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    if (STD_BIT_TEST(buf, QSFP_TX_CDR_CONTROL_BIT_OFFSET) != 0) {
        *support_status |= (1 << QSFP_TX_CDR_CONTROL_BIT_OFFSET);
    }

    if (STD_BIT_TEST(buf, QSFP_RX_CDR_CONTROL_BIT_OFFSET) != 0) {
        *support_status |= (1 << QSFP_RX_CDR_CONTROL_BIT_OFFSET);
    }

    return rc;
}

/* This function checks whether paging is supported or not on a given module */
static inline t_std_error sdi_is_paging_supported(sdi_device_hdl_t qsfp_device,
                                                  bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(qsfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status = false;

    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_STATUS_INDICATOR_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d ", qsfp_device->addr);
        return rc;
    }

    if( (STD_BIT_TEST(buf, QSFP_FLAT_MEM_BIT_OFFSET)) != 0 ) {
        *support_status = false;
    } else {
        *support_status = true;
    }
    return rc;
}

/* This function checks whether rate select supported or not for a given module */
static inline t_std_error sdi_is_rate_select_supported(sdi_device_hdl_t qsfp_device,
                                                       bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(qsfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status = false;

    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_OPTIONS4_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                              qsfp_device->addr, rc);
        return rc;
    }

    if ( (STD_BIT_TEST(buf, QSFP_RATE_SELECT_BIT_OFFSET) != 0) ) {
        *support_status = true;
    } else {
        *support_status = false;
    }

    return rc;
}

static inline t_std_error sdi_is_software_controlled_power_mode_supported (
                            sdi_device_hdl_t qsfp_device, bool *support_status)
{
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;
    bool paging_support = true;

    STD_ASSERT(qsfp_device != NULL);
    STD_ASSERT(support_status != NULL);

    *support_status = false;

    if (sdi_is_paging_supported(qsfp_device, &paging_support) != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("Unable to check for paging support on %s",
                    qsfp_device->alias);
    } else if (paging_support) {
        rc = sdi_qsfp_page_select(qsfp_device, SDI_MEDIA_PAGE_DEFAULT);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("page 0 selection is failed for %s",
                        qsfp_device->alias);
        }
    }

    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_EXT_IDENTIFIER_OFFSET, &buf, SDI_I2C_FLAG_NONE);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Soft power mode check: qsfp smbus read failed at addr : %d rc : %d",
                              qsfp_device->addr, rc);
        return rc;
    }

    if ( buf & 0x03 ) {
        *support_status = true;
    } else {
        *support_status = false;
    }

    return rc;
}


t_std_error sdi_qsfp_media_max_power_get (sdi_resource_hdl_t resource_hdl, int *pwr_milliwatts, bool* soft_ctrl)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    uint8_t buf = 0;
    t_std_error rc = STD_ERR_OK;


    STD_ASSERT(resource_hdl != NULL);
    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);
    *soft_ctrl = false;
    *pwr_milliwatts = SDI_MEDIA_NO_MAX_POWER_DEFINED;

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return SDI_ERRCODE(ENOTSUP);
    }

    if ((rc = sdi_qsfp_module_select(qsfp_device)) != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("QSFP module selection failed when attempting max power get for %s",
             qsfp_device->alias);
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                                QSFP_EXT_IDENTIFIER_OFFSET, &buf, SDI_I2C_FLAG_NONE);

        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Unable to get max power info for media on %s",
                 qsfp_device->alias);
            return rc;
        }

        /* Check the upper 2 bits first: for legacy QSFP(28) */
        switch ((buf & 0xC0) >> 6) {
            case 0:
                *pwr_milliwatts = 1500;
                break;
            case 1:
                *pwr_milliwatts = 2000;
                break;
            case 2:
                *pwr_milliwatts = 2500;
                break;
            case 3:
                *pwr_milliwatts = 3500;
                break;
        }

        /* If lower 2 bits are set, they override the upper 2 bit power value */
        switch ((buf & 0x03)) {
            case 0:
                /* use upper two bits defined previously */
                break;
            case 1:
                *soft_ctrl = true;
                *pwr_milliwatts = 4000;
                break;
            case 2:
                *soft_ctrl = true;
                *pwr_milliwatts = 4500;
                break;
            case 3:
                *soft_ctrl = true;
                *pwr_milliwatts = 5000;
                break;
        }

    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}


/* This function overrides the LP_MODE hardware pin. Use carefully */
/* If this function is used to set power HIGH, one must also use it to set power LOW */
t_std_error sdi_qsfp_media_force_power_mode_set(sdi_resource_hdl_t resource_hdl, bool state)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    uint8_t buf = 0;
    bool feature_sup, power_set_bit, hp_class_enable_bit, lp_mode_pin_override_bit;
    t_std_error rc = STD_ERR_OK;


    power_set_bit = !state; /* Power set bit is active low. This sets the value when power override bit is used */
    hp_class_enable_bit = state;
    lp_mode_pin_override_bit = true;  /* Always override the hw pin*/

    STD_ASSERT(resource_hdl != NULL);
    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return SDI_ERRCODE(ENOTSUP);
    }

    if ((rc = sdi_qsfp_module_select(qsfp_device)) != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("QSFP module selection failed when attempting mode class set for %s",
             qsfp_device->alias);
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_is_software_controlled_power_mode_supported(qsfp_device, &feature_sup);

        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Unable to get soft control power info for media on %s; rc %u",
                 qsfp_device->alias, rc);
            break;
        } else if (!feature_sup){
            rc = SDI_ERRCODE(ENOTSUP);
            break;
        }

        rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_POWER_CLASS_CONTROL_OFFSET, &buf, SDI_I2C_FLAG_NONE);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Unable to read EEPROM to get power class state for media on %s;, rc %u",
                 qsfp_device->alias, rc);
            break;
        }

        hp_class_enable_bit ? STD_BIT_SET(buf, 2) : STD_BIT_CLEAR(buf, 2);
        power_set_bit ? STD_BIT_SET(buf, 1) : STD_BIT_CLEAR(buf, 1);
        lp_mode_pin_override_bit ? STD_BIT_SET(buf, 0) : STD_BIT_CLEAR(buf, 0);

        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));
        rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                             QSFP_POWER_CLASS_CONTROL_OFFSET, buf, SDI_I2C_FLAG_NONE);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Unable to write EEPROM to set power class state for media on %s;, rc %u",
                 qsfp_device->alias, rc);
            break;
        }
    } while (0);

    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}




/* Internally measured Module temperature are represented as a
 * 16-bit signed twos complement value in increments of 1/256
 * degrees Celsius */
static inline float convert_qsfp_temp(uint8_t *buf)
{
    int16_t temp = 0;
    float calib_temp = 0;
    bool is_negative = false;

    temp = ( (buf[0] << 8) | (buf[1]) );

    if( temp < 0 ){
        /* Negative value */
        temp = ((~temp) + 1);
        is_negative = true;
    }

    calib_temp = ((float)(temp & 0xFF) / 256.0) + ((temp & 0xFF00) >> 8);

    if(is_negative == true) {
        calib_temp = -calib_temp;
    }

    return calib_temp;
}

/* Internally measured Module supply voltage are represented as a
 * 16-bit unsigned integer with the voltage defined as the full 16
 * bit value with LSB equal to 100 uVolt, yielding a total
 * measurement range of 0 to +6.55 Volts. */
static inline float convert_qsfp_volt(uint8_t *buf)
{
   return ((float)((buf[0] << 8) | (buf[1]))/10000.0);
}

/* Represented as a 16 bit unsigned integer with the power defined
 * as the full 16 bit value (0 – 65535) with LSB equal to 0.1 uW, yielding a
 * total measurement range of 0 to 6.5535 mW (~-40 to +8.2 dBm). */
static inline float convert_qsfp_rx_power(uint8_t *buf)
{
        return sdi_convert_mw_to_dbm((float)((buf[0] << 8) | (buf[1]))/10000.0);
}

/* Measured TX bias current is in mA and are represented as a 16-bit unsigned
 * integer with the current defined as the full 16 bit value (0 – 65535) with
 * LSB equal to 2 uA, yielding  a total measurement range of 0 to 131 mA.*/
static inline float convert_qsfp_tx_bias(uint8_t *buf)
{
    return ((float)(((buf[0] << 8) | (buf[1])) * 2)/1000.0);
}

/**
 * Get the required module status of the specific qsfp
 * resource_hdl[in] - Handle of the resource
 * flags[in]        - flags for status that are of interest
 * status[out]    - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_qsfp_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                uint_t flags, uint_t *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t temp_status_buf = 0;
    uint8_t volt_status_buf = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_module_monitor_status_get(qsfp_priv_data->sfp_device,
                                                 flags, status);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }
    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        if( (flags) & ( (SDI_MEDIA_STATUS_TEMP_HIGH_ALARM)  |
                    (SDI_MEDIA_STATUS_TEMP_LOW_ALARM)   |
                    (SDI_MEDIA_STATUS_TEMP_HIGH_WARNING)|
                    (SDI_MEDIA_STATUS_TEMP_LOW_WARNING) ) ) {

            uint_t temp_intr = (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD)
                                     ? QSFP_DD_TEMP_INTERRUPT_OFFSET
                                     : QSFP_TEMP_INTERRUPT_OFFSET;

            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    temp_intr, &temp_status_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                        "rc : %d", qsfp_device->addr, rc);
                break;
            }
        }

        if( (flags) & ( (SDI_MEDIA_STATUS_VOLT_HIGH_ALARM)  |
                    (SDI_MEDIA_STATUS_VOLT_LOW_ALARM)   |
                    (SDI_MEDIA_STATUS_VOLT_HIGH_WARNING)|
                    (SDI_MEDIA_STATUS_VOLT_LOW_WARNING) ) ) {

            uint_t volt_intr = (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD)
                                     ? QSFP_DD_VOLT_INTERRUPT_OFFSET
                                     : QSFP_VOLT_INTERRUPT_OFFSET;

            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    volt_intr, &volt_status_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                        "rc : %d", qsfp_device->addr, rc);
            }
        }
    } while(0);
    sdi_qsfp_module_deselect(qsfp_priv_data);

    if(rc == STD_ERR_OK) {
        if (temp_status_buf & QSFP_TEMP_HIGH_ALARM_FLAG){
            *status |= SDI_MEDIA_STATUS_TEMP_HIGH_ALARM;
        } else if (temp_status_buf & QSFP_TEMP_LOW_ALARM_FLAG){
            *status |= SDI_MEDIA_STATUS_TEMP_LOW_ALARM;
        }

        if (temp_status_buf & QSFP_TEMP_HIGH_WARNING_FLAG){
            *status |= SDI_MEDIA_STATUS_TEMP_HIGH_WARNING;
        } else if (temp_status_buf & QSFP_TEMP_LOW_WARNING_FLAG){
            *status |= SDI_MEDIA_STATUS_TEMP_LOW_WARNING;
        }

        if (volt_status_buf & QSFP_VOLT_HIGH_ALARM_FLAG){
            *status |= SDI_MEDIA_STATUS_VOLT_HIGH_ALARM;
        } else if (volt_status_buf & QSFP_VOLT_LOW_ALARM_FLAG){
            *status |= SDI_MEDIA_STATUS_VOLT_LOW_ALARM;
        }

        if (volt_status_buf & QSFP_VOLT_HIGH_WARNING_FLAG){
            *status |= SDI_MEDIA_STATUS_VOLT_HIGH_WARNING;
        } else if (volt_status_buf & QSFP_VOLT_LOW_WARNING_FLAG){
            *status |= SDI_MEDIA_STATUS_VOLT_LOW_WARNING;
        }
    }

    return rc;
}

/**
 * Get the required channel monitoring(rx_power, tx_bias alarm) status of the specific QSFP
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest
 * flags[in]        - flags for channel status
 * status[out]      - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_qsfp_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                                 uint_t flags, uint_t *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t offset = 0;
    uint8_t rx_pwr_buf = 0;
    uint8_t tx_bias_buf = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_channel_monitor_status_get(qsfp_priv_data->sfp_device,
                                                  channel, flags, status);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        if( ((flags) & ((SDI_MEDIA_RX_PWR_HIGH_ALARM) | (SDI_MEDIA_RX_PWR_LOW_ALARM) |
                        (SDI_MEDIA_RX_PWR_HIGH_WARNING) | (SDI_MEDIA_RX_PWR_LOW_WARNING))) != 0 )
        {
            if( (channel == SDI_QSFP_CHANNEL_ONE) || (channel == SDI_QSFP_CHANNEL_TWO) ) {
                offset = QSFP_RX12_POWER_INTERRUPT_OFFSET;
            } else {
                offset = QSFP_RX34_POWER_INTERRUPT_OFFSET;
            }
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                                     offset, &rx_pwr_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                        "rc : %d", qsfp_device->addr, rc);
                break;
            }
        }

        if( ((flags) & ((SDI_MEDIA_TX_BIAS_HIGH_ALARM) | (SDI_MEDIA_TX_BIAS_LOW_ALARM) |
                        (SDI_MEDIA_TX_BIAS_HIGH_WARNING) | (SDI_MEDIA_TX_BIAS_LOW_WARNING))) != 0)
        {
            if( (channel == SDI_QSFP_CHANNEL_ONE) || (channel == SDI_QSFP_CHANNEL_TWO) ) {
                offset = QSFP_TX12_BIAS_INTERRUPT_OFFSET;
            } else {
                offset = QSFP_TX34_BIAS_INTERRUPT_OFFSET;
            }
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                                     offset, &tx_bias_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                        "rc : %d", qsfp_device->addr, rc);
                break;
            }
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if(rc == STD_ERR_OK) {
        if( ((rx_pwr_buf) & ((QSFP_RX13_POWER_HIGH_ALARM_FLAG) |
                             (QSFP_RX24_POWER_HIGH_ALARM_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_RX_PWR_HIGH_ALARM;
        } else if ( ((rx_pwr_buf) & ((QSFP_RX13_POWER_LOW_ALARM_FLAG) |
                                     (QSFP_RX24_POWER_LOW_ALARM_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_RX_PWR_LOW_ALARM;
        }

        if( ((rx_pwr_buf) & ((QSFP_RX13_POWER_HIGH_WARNING_FLAG) |
                             (QSFP_RX24_POWER_HIGH_WARNING_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_RX_PWR_HIGH_WARNING;
        } else if ( ((rx_pwr_buf) & ((QSFP_RX13_POWER_LOW_WARNING_FLAG) |
                                     (QSFP_RX24_POWER_LOW_WARNING_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_RX_PWR_LOW_WARNING;
        }

        if( ((tx_bias_buf) & ((QSFP_TX13_BIAS_HIGH_ALARM_FLAG) |
                              (QSFP_TX24_BIAS_HIGH_ALARM_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_TX_BIAS_HIGH_ALARM;
        } else if( ((tx_bias_buf) & ((QSFP_TX13_BIAS_LOW_ALARM_FLAG) |
                                     (QSFP_TX24_BIAS_LOW_ALARM_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_TX_BIAS_LOW_ALARM;
        }

        if( ((tx_bias_buf) & ((QSFP_TX13_BIAS_HIGH_WARNING_FLAG) |
                              (QSFP_TX24_BIAS_HIGH_WARNING_FLAG))) != 0) {
            *status |= SDI_MEDIA_TX_BIAS_HIGH_WARNING;
        } else if( ((tx_bias_buf) & ((QSFP_TX13_BIAS_LOW_WARNING_FLAG) |
                                     (QSFP_TX24_BIAS_LOW_WARNING_FLAG))) != 0 ) {
            *status |= SDI_MEDIA_TX_BIAS_LOW_WARNING;
        }
    }
    return rc;
}

/**
 * Get the required channel status of the specific QSFP
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number that is of interest
 * flags[in]        - flags for channel status
 * status[out]    - returns the set of status flags which are asserted
 * return           - t_std_error
 */
t_std_error sdi_qsfp_channel_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         uint_t flags, uint_t *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);


    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_channel_status_get(qsfp_priv_data->sfp_device,
                                          channel, flags, status);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        if( ((flags) & (SDI_MEDIA_STATUS_TXDISABLE)) ) {
            uint_t tx_control = (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD)
                                     ? QSFP_DD_TX_CONTROL_OFFSET
                                     : QSFP_TX_CONTROL_OFFSET;
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    tx_control, &buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                        "rc : %d", qsfp_device->addr, rc);
                break;
            }

            if ( (STD_BIT_TEST(buf, channel)) != 0 ) {
                *status |= SDI_MEDIA_STATUS_TXDISABLE;
            }
        }

        if( ((flags) & (SDI_MEDIA_STATUS_TXFAULT)) ) {

            uint_t tx_fault = (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD)
                                ? QSFP_DD_CHANNEL_TXFAULT_INDICATOR
                                : QSFP_CHANNEL_TXFAULT_INDICATOR;
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    tx_fault, &buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                        "rc : %d", qsfp_device->addr, rc);
                break;
            }

            if ( (STD_BIT_TEST(buf, channel)) != 0 ) {
                *status |= SDI_MEDIA_STATUS_TXFAULT;
            }
        }

        if( ( (flags) & ((SDI_MEDIA_STATUS_TXLOSS)|(SDI_MEDIA_STATUS_RXLOSS)) ) ) {

            if (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD) {

                if( flags & SDI_MEDIA_STATUS_TXLOSS ) {

                    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl,
                            qsfp_device->addr.i2c_addr,
                            QSFP_DD_CHANNEL_TX_LOS_INDICATOR, &buf,
                            SDI_I2C_FLAG_NONE);

                    if (rc != STD_ERR_OK){
                        SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                                "rc : %d", qsfp_device->addr, rc);
                        break;
                    }

                    if ( buf & QSFP_DD_TX_LOS_FLAG(channel) ) {

                        *status |= SDI_MEDIA_STATUS_TXLOSS;
                    }
                }

                if( flags & SDI_MEDIA_STATUS_RXLOSS ) {

                    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl,
                            qsfp_device->addr.i2c_addr,
                            QSFP_DD_CHANNEL_RX_LOS_INDICATOR, &buf,
                            SDI_I2C_FLAG_NONE);

                    if (rc != STD_ERR_OK){
                        SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                                "rc : %d", qsfp_device->addr, rc);
                        break;
                    }

                    if ( buf & QSFP_DD_RX_LOS_FLAG(channel) ) {

                        *status |= SDI_MEDIA_STATUS_RXLOSS;
                    }
                }

            } else {

                rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                        QSFP_CHANNEL_LOS_INDICATOR, &buf, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                            "rc : %d", qsfp_device->addr, rc);
                    break;
                }

                if( (buf & QSFP_TX_LOS_FLAG(channel))  ) {
                    *status |= SDI_MEDIA_STATUS_TXLOSS;
                }

                if( (buf & QSFP_RX_LOS_FLAG(channel))  ) {
                    *status |= SDI_MEDIA_STATUS_RXLOSS;
                }
            }
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}

/**
 * Disable/Enable the transmitter of the specific QSFP
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * enable[in]       - "false" to disable and "true" to enable
 * return           - t_std_error
 */
t_std_error sdi_qsfp_tx_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                 bool enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;
    bool support_status = false;
    uint_t delay = 0;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_tx_control(qsfp_priv_data->sfp_device,
                                  channel, enable);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_is_tx_control_supported(qsfp_device, &support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        if(support_status == false) {
           rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
           break;
        }

        uint_t tx_control = (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD)
                                   ? QSFP_DD_TX_CONTROL_OFFSET
                                   : QSFP_TX_CONTROL_OFFSET;
        rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                tx_control, &buf, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                    qsfp_device->addr, rc);
            break;
        }

        if (enable == true){
            STD_BIT_CLEAR(buf, channel);
            /*After enabling transmitter on a particular channel, we need to
             * wait 400ms */
            delay = QSFP_TX_ENABLE_DELAY;
        } else {
            STD_BIT_SET(buf, channel);
            /*After disabling transmitter on a particular channel, we need to
             * wait 100ms */
            delay = QSFP_TX_DISABLE_DELAY;
        }

        rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                tx_control, buf, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d rc : %d",
                    qsfp_device->addr, rc);
        }
        std_usleep(delay);
    } while(0);
    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}

/**
 * Gets the transmitter status on the specific channel of a QSFP
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * status[out]      - "true" if transmitter enabled, else "false"
 * return           - t_std_error
 */
t_std_error sdi_qsfp_tx_control_status_get(sdi_resource_hdl_t resource_hdl,
                                           uint_t channel, bool *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_tx_control_status_get(qsfp_priv_data->sfp_device,
                                  channel, status);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        uint_t tx_control = (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD)
                                   ? QSFP_DD_TX_CONTROL_OFFSET
                                   : QSFP_TX_CONTROL_OFFSET;
        rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                tx_control, &buf, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                    qsfp_device->addr, rc);
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if(rc == STD_ERR_OK) {
        /* If bit is set, transmitter is disabled on the channel and if bit is not
         * set, transmitter is enabled on the channel*/
        if ( (STD_BIT_TEST(buf, channel) == 0) ) {
            *status = true;
        } else {
            *status = false;
        }
    }

    return rc;
}

/**
 * Disable/Enable the tx/rx cdr of the specific QSFP
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * enable[in]       - "false" to disable and "true" to enable
 * return           - t_std_error
 */
t_std_error sdi_qsfp_cdr_status_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                     bool enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf = 0;
    uint_t support_status = 0;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_is_cdr_control_supported(qsfp_device, &support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        if(support_status == 0) {
           rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
           break;
        }

        if (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD) {

            //TX_CDR_CONTROL
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                QSFP_DD_TX_CDR_CONTROL_OFFSET, &buf, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
                break;
            }

            if (enable == true){
                if (support_status & (1 << QSFP_TX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_SET(buf, QSFP_DD_TX_CDR_CONTROL_BIT(channel));
                }

            } else {
                if (support_status & (1 << QSFP_TX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_CLEAR(buf, QSFP_DD_TX_CDR_CONTROL_BIT(channel));
                }
            }

            rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_DD_TX_CDR_CONTROL_OFFSET, buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
            }


            //RX_CDR_CONTROL
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                QSFP_DD_RX_CDR_CONTROL_OFFSET, &buf, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
                break;
            }

            if (enable == true){
                if (support_status & (1 << QSFP_RX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_SET(buf, QSFP_DD_RX_CDR_CONTROL_BIT(channel));
                }

            } else {
                if (support_status & (1 << QSFP_RX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_CLEAR(buf, QSFP_DD_RX_CDR_CONTROL_BIT(channel));
                }
            }

            rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_DD_RX_CDR_CONTROL_OFFSET, buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
            }

        } else {

            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                QSFP_CDR_CONTROL_OFFSET, &buf, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
                break;
            }

            if (enable == true){
                if (support_status & (1 << QSFP_TX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_SET(buf, QSFP_TX_CDR_CONTROL_BIT(channel));
                }

                if (support_status & (1 << QSFP_RX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_SET(buf, QSFP_RX_CDR_CONTROL_BIT(channel));
                }
            } else {
                if (support_status & (1 << QSFP_TX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_CLEAR(buf, QSFP_TX_CDR_CONTROL_BIT(channel));
                }

                if (support_status & (1 << QSFP_RX_CDR_CONTROL_BIT_OFFSET)) {
                    STD_BIT_CLEAR(buf, QSFP_RX_CDR_CONTROL_BIT(channel));
                }
            }

            rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_CDR_CONTROL_OFFSET, buf, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
            }
        }
    } while(0);
    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}

/**
 * Gets the transmitter status on the specific channel of a QSFP
 * resource_hdl[in] - handle of the resource
 * channel[in]      - channel number that is of interest
 * status[out]      - "true" if transmitter enabled, else "false"
 * return           - t_std_error
 */
t_std_error sdi_qsfp_cdr_status_get(sdi_resource_hdl_t resource_hdl,
                                           uint_t channel, bool *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t support_status = 0;
    uint8_t buf = 0;
    uint8_t buf1 = 0;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_is_cdr_control_supported(qsfp_device, &support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        if(support_status == 0) {
           rc = SDI_DEVICE_ERRCODE(EOPNOTSUPP);
           break;
        }

        if (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD) {
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_DD_TX_CDR_CONTROL_OFFSET, &buf, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
            }

            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_DD_RX_CDR_CONTROL_OFFSET, &buf1, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
            }
        } else {
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_CDR_CONTROL_OFFSET, &buf, SDI_I2C_FLAG_NONE);

            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        qsfp_device->addr, rc);
            }
       }

    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if(rc == STD_ERR_OK) {
        /* If bit is set,  CDR is enabled on the channel and if bit is not
         * set, CDR is disabled on the channel*/

        if (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD) {
            if (STD_BIT_TEST(buf, QSFP_DD_TX_CDR_CONTROL_BIT(channel))
                    || STD_BIT_TEST(buf1, QSFP_DD_RX_CDR_CONTROL_BIT(channel))) {
                *status = true;
            } else {
                *status = false;
            }
        } else {
            if (STD_BIT_TEST(buf, QSFP_TX_CDR_CONTROL_BIT(channel))
                    || STD_BIT_TEST(buf, QSFP_RX_CDR_CONTROL_BIT(channel))) {
                *status = true;
            } else {
                *status = false;
            }
        }
    }

    return rc;
}

/**
 * Get the maximum speed that can be supported by a specific media resource
 * resource_hdl[in] - handle of the media resource
 * speed[out]     - speed of the media will be filled in this
 * return           - standard t_std_error
 */
t_std_error  sdi_qsfp_speed_get(sdi_resource_hdl_t resource_hdl,
                                sdi_media_speed_t *speed)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;

    STD_ASSERT(qsfp_priv_data != NULL);

    STD_ASSERT(speed != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_speed_get(qsfp_priv_data->sfp_device,
                                  speed);
    }

    *speed = qsfp_priv_data->capability;

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
t_std_error sdi_qsfp_parameter_get(sdi_resource_hdl_t resource_hdl,
                                   sdi_media_param_type_t param, uint_t *value)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t byte_buf = 0;
    uint8_t word_buf[2] = { 0 };
    uint8_t buf[4] = { 0 };
    uint_t offset = 0;
    uint_t size = 0;
    uint16_t temp_buf = 0;
    bool length_code_conversion_needed = false;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_parameter_get(qsfp_priv_data->sfp_device,
                                     param, value);
    }

    if (qsfp_priv_data->eeprom_version >= QSFP28_DD_EEPROM_VERSION_3) {
        offset = param_reg_info_qsfp28_dd_r3[param].offset;
        size = param_reg_info_qsfp28_dd_r3[param].size;

        if (offset == QSFP28_DD_R3_LENGTH_CABLE_ASSEMBLY_OFFSET){
             /* Assume length code is needed. Check later */
             length_code_conversion_needed = true;
        }
    } else {
        offset = param_reg_info[param].offset;
        size =  param_reg_info[param].size;
    }

    if( (offset == 0) && (size == 0) ) {

        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }
    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        /* verify assumption that length code conversion is needed  */
        if (length_code_conversion_needed) {
            rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                   QSFP28_DD_R3_LANE_ASSIGNMENT_OFFSET, &byte_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                        ", when attempting lane assignment read for cable  ength checking."
                                ,qsfp_device->addr, rc);
                length_code_conversion_needed = false;
                break;
            }

            /* length code method is only implemented in spec versions that use non-zero lane assignment */
            length_code_conversion_needed = (bool)(byte_buf > 0);
        }

        switch (size)
        {
            case SDI_QSFP_BYTE_SIZE:
                rc = sdi_smbus_read_byte(qsfp_device->bus_hdl,
                        qsfp_device->addr.i2c_addr, offset,
                        &byte_buf, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                            "rc : %d", qsfp_device->addr, offset, rc);
                }
                break;

            case SDI_QSFP_WORD_SIZE:
                rc = sdi_smbus_read_word(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                        offset,&temp_buf, SDI_I2C_FLAG_NONE);
               sdi_platform_util_write_16bit_to_bytearray_le(word_buf,temp_buf);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                            "rc : %d", qsfp_device->addr, offset, rc);
                }
                break;

            case SDI_QSFP_DOUBLE_WORD_SIZE:
                rc = sdi_smbus_read_multi_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                        offset, buf, SDI_QSFP_DOUBLE_WORD_SIZE, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                            "rc : %d", qsfp_device->addr, offset, rc);
                }
                break;

            default:
                rc = SDI_DEVICE_ERRCODE(EINVAL);
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if(rc == STD_ERR_OK) {
        if(size == SDI_QSFP_BYTE_SIZE) {
            if(param == SDI_MEDIA_DIAG_MON_TYPE) {
                if(STD_BIT_TEST(byte_buf, 3) != 0) {
                    *value = SDI_MEDIA_RX_PWR_AVG;
                } else {
                    *value = SDI_MEDIA_RX_PWR_OMA;
                }
            } else if (length_code_conversion_needed) {
                *value = (uint_t)sdi_convert_length_code_to_length (byte_buf);
            } else {
                *value = (uint_t)byte_buf;
            }
        } else if(size == SDI_QSFP_WORD_SIZE) {
            *value = ( (word_buf[0] << 8) | (word_buf[1]) );
            if(param == SDI_MEDIA_WAVELENGTH) {
                /* wavelength=value/20 in nm */
                *value = ( (*value) / QSFP_WAVELENGTH_DIVIDER);
            } else if(param == SDI_MEDIA_WAVELENGTH_TOLERANCE) {
                /* Guaranteed range of laser wavelength(+/- value) from nominal
                 * wavelength. (wavelength Tol.=value/200 in nm) */
                *value = ( (*value) / QSFP_WAVELENGTH_TOLERANCE_DIVIDER);
            }
        } else if(size == SDI_QSFP_DOUBLE_WORD_SIZE) {
            *value = ( (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3] );
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
t_std_error sdi_qsfp_vendor_info_get(sdi_resource_hdl_t resource_hdl,
                                     sdi_media_vendor_info_type_t vendor_info_type,
                                     char *vendor_info, size_t size)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    size_t data_len = 0;
    uint_t offset = 0;
    uint8_t *buf_ptr = NULL;
    uint8_t data_buf[SDI_MAX_NAME_LEN];
    bool printable = false;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(vendor_info != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    memset(data_buf, 0, SDI_MAX_NAME_LEN);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_vendor_info_get(qsfp_priv_data->sfp_device,
                                       vendor_info_type, vendor_info,
                                       size);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        if (qsfp_priv_data->eeprom_version >= QSFP28_DD_EEPROM_VERSION_3) {
            offset = vendor_reg_info_qsfp28_dd_r3[vendor_info_type].offset;
            data_len = vendor_reg_info_qsfp28_dd_r3[vendor_info_type].size;
            printable = vendor_reg_info_qsfp28_dd_r3[vendor_info_type].printable;
        } else {
            offset = vendor_reg_info[vendor_info_type].offset;
            data_len = vendor_reg_info[vendor_info_type].size;
            printable = vendor_reg_info[vendor_info_type].printable;
        }

        /* Input buffer size should be greater than or equal to data len*/
        STD_ASSERT(size >= data_len);

        rc = sdi_smbus_read_multi_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                offset, data_buf, data_len - 1, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                    "rc : %d", qsfp_device->addr, offset, rc);
            break;
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if( rc == STD_ERR_OK) {
        /* If the field is marked printable, then ensure that it contains only
         * printable characters
         */
        if (printable) {
            for (buf_ptr = &data_buf[0];
                 buf_ptr < &data_buf[data_len - 1];
                 buf_ptr++) {

                if (!isprint(*buf_ptr) && *buf_ptr != SDI_QSFP_PADDING_CHAR) {
                    /* Replace with a garbled character indicator */
                    *buf_ptr = SDI_QSFP_GARBAGE_CHAR_INDICATOR;
                }
            }
            *buf_ptr = '\0';
        }

        /* vendor name, part number, serial number and revision fields contains
         * ASCII characters, left-aligned and padded on the right with ASCII
         * spaces (20h).*/
        for(buf_ptr = &data_buf[data_len - 1];
                (*(buf_ptr - 1) == 0x20)
                || (*(buf_ptr - 1) == '\0'); buf_ptr--);
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
t_std_error sdi_qsfp_transceiver_code_get(sdi_resource_hdl_t resource_hdl,
                                          sdi_media_transceiver_descr_t *transceiver_info)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf[SDI_QSFP_QUAD_WORD_SIZE] = { 0 };

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(transceiver_info != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_transceiver_code_get(qsfp_priv_data->sfp_device,
                                            transceiver_info);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_smbus_read_multi_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                QSFP_COMPLIANCE_CODE_OFFSET, buf, SDI_QSFP_QUAD_WORD_SIZE,
                SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                    qsfp_device->addr, rc);
            break;
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if (rc == STD_ERR_OK) {
        memcpy((char *)transceiver_info, (char *)buf, SDI_QSFP_QUAD_WORD_SIZE);
    }

    return rc;
}

/**
 * Get the alarm and warning thresholds for the given optic
 * resource_hdl[in] - Handle of the resource
 * threshold_type[in] - Type of the threshold
 * value[out] - Threshold value
 * return - standard t_std_error
 */
t_std_error sdi_qsfp_threshold_get (sdi_resource_hdl_t resource_hdl,
                                    sdi_media_threshold_type_t threshold_type,
                                    float *value)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint_t offset = 0;
    bool paging_support_flag = true;
    uint8_t threshold_buf[2] = { 0 };
    uint16_t temp_buf = 0;
    uint8_t page_to_use = SDI_MEDIA_PAGE_03;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_threshold_get(qsfp_priv_data->sfp_device,
                                     threshold_type, value);
    }
    if (qsfp_priv_data->eeprom_version >= QSFP28_DD_EEPROM_VERSION_3){
        page_to_use = SDI_MEDIA_PAGE_02;
        offset = threshold_reg_info_qsfp28_dd_r3[threshold_type].offset;
    } else {
        offset = threshold_reg_info[threshold_type].offset;
    }

    if( offset == 0 )  {
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        /* Select the appropriate eeprom page where threshold values are located */
        rc = sdi_qsfp_page_select(qsfp_device, page_to_use);
        if(rc != STD_ERR_OK){
            if( rc == SDI_DEVICE_ERRCODE(ENOTSUP) ) {
                paging_support_flag = false;
            } else {
                SDI_DEVICE_ERRMSG_LOG("page 3 selection is failed for %s",
                                       qsfp_device->alias);
            }
            break;
        }

        rc = sdi_smbus_read_word(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                                 offset, &temp_buf, SDI_I2C_FLAG_NONE);
        sdi_platform_util_write_16bit_to_bytearray_le(threshold_buf,temp_buf);
        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                                  "rc : %d", qsfp_device->addr, offset, rc);
            break;
        }
    } while(0);

    if(paging_support_flag == true) {
        /* Select the default page */
        rc = sdi_qsfp_page_select(qsfp_device, SDI_MEDIA_PAGE_DEFAULT);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("page 0 selection is failed for %s",
                                   qsfp_device->alias);
        }
    }

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if( (threshold_type == SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD) ||
        (threshold_type == SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD) ||
        (threshold_type == SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD) ||
        (threshold_type == SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD) ) {
        *value = convert_qsfp_temp(threshold_buf);
    } else if( (threshold_type == SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD) ) {
        *value = convert_qsfp_volt(threshold_buf);
    } else if( (threshold_type == SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD) ) {
        *value = convert_qsfp_rx_power(threshold_buf);
    } else if( (threshold_type == SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD) ||
               (threshold_type == SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD) ) {
        *value = convert_qsfp_tx_bias(threshold_buf);
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
t_std_error sdi_qsfp_module_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                  uint_t threshold_type, uint_t *value)
{
    /* TODO:Depricated API and should me removed upper layers adopted new API */
    return STD_ERR_OK;
}

/**
 * Read the threshold values for channel monitors like rx-ower and tx-bias
 * resource_hdl[in]     - Handle of the resource
 * threshold_type[in]   - type of threshold
 * value[out]           - threshold value
 * return               - standard t_std_error
 */
t_std_error sdi_qsfp_channel_monitor_threshold_get(sdi_resource_hdl_t resource_hdl,
                                                   uint_t threshold_type, uint_t *value)
{
    /* TODO:Depricated API and should me removed upper layers adopted new API */
    return STD_ERR_OK;
}

/**
 * Debug api to retrieve module monitors assoicated with the specified QSFP
 * resource_hdl[in] - Handle of the resource
 * monitor[in]      - monitor which needs to be retrieved
 * value[out]     - Value of the monitor
 * return           - t_std_error
 */
t_std_error sdi_qsfp_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                         sdi_media_module_monitor_t monitor, float *value)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    int temp_offset = QSFP_DD_TEMPERATURE_OFFSET;
    int volt_offset = QSFP_DD_VOLTAGE_OFFSET;
    uint8_t temp_buf[2] = { 0 };
    uint8_t volt_buf[2] = { 0 };
    uint16_t word_buf = 0;
    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->eeprom_version >= QSFP28_DD_EEPROM_VERSION_3 ){
        temp_offset = QSFP28_DD_R3_TEMPERATURE_OFFSET;
        volt_offset = QSFP28_DD_R3_VOLTAGE_OFFSET;
    } else if (qsfp_priv_data->eeprom_version < QSFP28_DD_EEPROM_VERSION_2) {
        temp_offset = QSFP_TEMPERATURE_OFFSET;
        volt_offset = QSFP_DD_VOLTAGE_OFFSET;
    } else {
        temp_offset = QSFP28_DD_R2_TEMPERATURE_OFFSET;
        volt_offset = QSFP28_DD_R2_VOLTAGE_OFFSET;
    }

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_module_monitor_get(qsfp_priv_data->sfp_device,
                                          monitor, value);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        switch (monitor)
        {
            case SDI_MEDIA_TEMP:
                rc = sdi_smbus_read_word(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                        temp_offset, &word_buf,
                        SDI_I2C_FLAG_NONE);
                sdi_platform_util_write_16bit_to_bytearray_le(temp_buf,word_buf);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d ",
                            qsfp_device->addr);
                }
                break;

            case SDI_MEDIA_VOLT:
                rc = sdi_smbus_read_word(qsfp_device->bus_hdl,
                        qsfp_device->addr.i2c_addr, volt_offset,
                        &word_buf, SDI_I2C_FLAG_NONE);
                sdi_platform_util_write_16bit_to_bytearray_le(volt_buf,word_buf);
                if (rc != STD_ERR_OK){
                    SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d ",
                            qsfp_device->addr);
                }
                break;

            default:
                rc = SDI_DEVICE_ERRCODE(EINVAL);
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if(rc == STD_ERR_OK) {
        if(monitor == SDI_MEDIA_TEMP) {
            *value = convert_qsfp_temp(temp_buf);
        } else if(monitor == SDI_MEDIA_VOLT) {
            *value = convert_qsfp_volt(volt_buf);
        }
    }

    return rc;
}

/**
 * Retrieve channel monitors assoicated with the specified QSFP
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel whose monitor has to be retreived
 * monitor[in]      - monitor which needs to be retrieved
 * value[out]     - Value of the monitor
 * return           - t_std_error
 */
t_std_error sdi_qsfp_channel_monitor_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_channel_monitor_t monitor, float *value)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t buf[2] = { 0 };
    uint_t reg_offset = 0;
    uint16_t temp_buf =0;
    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(value != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_channel_monitor_get(qsfp_priv_data->sfp_device,
                                           channel, monitor, value);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category) != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    switch (monitor)
    {
        case SDI_MEDIA_INTERNAL_RX_POWER_MONITOR:
            if (channel == SDI_QSFP_CHANNEL_ONE){
                reg_offset = QSFP_RX1_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_TWO){
                reg_offset = QSFP_RX2_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_THREE){
                reg_offset = QSFP_RX3_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_FOUR){
                reg_offset = QSFP_RX4_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_FIVE){
                reg_offset = QSFP_DD_RX5_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_SIX){
                reg_offset = QSFP_DD_RX6_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_SEVEN){
                reg_offset = QSFP_DD_RX7_POWER_OFFSET;
            } else if (channel == SDI_QSFP_CHANNEL_EIGHT){
                reg_offset = QSFP_DD_RX8_POWER_OFFSET;
            } else {
                return SDI_DEVICE_ERRCODE(EINVAL);
            }
            break;

        case SDI_MEDIA_INTERNAL_TX_POWER_BIAS:
            if (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD) {
                if (channel == SDI_QSFP_CHANNEL_ONE){
                    reg_offset = QSFP_DD_TX1_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_TWO){
                    reg_offset = QSFP_DD_TX2_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_THREE){
                    reg_offset = QSFP_DD_TX3_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_FOUR){
                    reg_offset = QSFP_DD_TX4_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_FIVE){
                    reg_offset = QSFP_DD_TX5_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_SIX){
                    reg_offset = QSFP_DD_TX6_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_SEVEN){
                    reg_offset = QSFP_DD_TX7_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_EIGHT){
                    reg_offset = QSFP_DD_TX8_BIAS_OFFSET;
                } else {
                    return SDI_DEVICE_ERRCODE(EINVAL);
                }
            } else {
                if (channel == SDI_QSFP_CHANNEL_ONE){
                    reg_offset = QSFP_TX1_POWER_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_TWO){
                    reg_offset = QSFP_TX2_POWER_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_THREE){
                    reg_offset = QSFP_TX3_POWER_BIAS_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_FOUR){
                    reg_offset = QSFP_TX4_POWER_BIAS_OFFSET;
                } else {
                    return SDI_DEVICE_ERRCODE(EINVAL);
                }
            }
            break;

        case SDI_MEDIA_INTERNAL_TX_OUTPUT_POWER:
            if (qsfp_priv_data->mod_category == SDI_CATEGORY_QSFPDD) {
                if (channel == SDI_QSFP_CHANNEL_ONE){
                    reg_offset = QSFP_DD_TX1_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_TWO){
                    reg_offset = QSFP_DD_TX2_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_THREE){
                    reg_offset = QSFP_DD_TX3_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_FOUR){
                    reg_offset = QSFP_DD_TX4_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_FIVE){
                    reg_offset = QSFP_DD_TX5_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_SIX){
                    reg_offset = QSFP_DD_TX6_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_SEVEN){
                    reg_offset = QSFP_DD_TX7_POWER_OFFSET;
                } else if (channel == SDI_QSFP_CHANNEL_EIGHT){
                    reg_offset = QSFP_DD_TX8_POWER_OFFSET;
                } else {
                    return SDI_DEVICE_ERRCODE(EINVAL);
                }
            } else {
                return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
            }
            break;

        default:
            return SDI_DEVICE_ERRCODE(EINVAL);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_smbus_read_word(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                reg_offset, &temp_buf, SDI_I2C_FLAG_NONE);
        sdi_platform_util_write_16bit_to_bytearray_le(buf,temp_buf);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d"
                    "reg : %d ", qsfp_device->addr, reg_offset);
            break;
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    if( rc == STD_ERR_OK) {
        if(monitor == SDI_MEDIA_INTERNAL_RX_POWER_MONITOR) {
            *value = convert_qsfp_rx_power(buf);
        } else if(monitor == SDI_MEDIA_INTERNAL_TX_POWER_BIAS) {
            *value = convert_qsfp_tx_bias(buf);
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
t_std_error sdi_qsfp_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                 sdi_media_supported_feature_t *feature_support)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(feature_support != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_feature_support_status_get(qsfp_priv_data->sfp_device,
                                                  feature_support);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_is_tx_control_supported(qsfp_device,
                                        &feature_support->qsfp_features.tx_control_support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        rc = sdi_is_paging_supported(qsfp_device,
                                    &feature_support->qsfp_features.paging_support_status);
        if (rc != STD_ERR_OK){
            break;
        }

        rc = sdi_is_rate_select_supported(qsfp_device,
                                         &feature_support->qsfp_features.rate_select_status);
        if (rc != STD_ERR_OK){
            break;
        }

        rc = sdi_is_software_controlled_power_mode_supported(qsfp_device,
                        &feature_support->qsfp_features.software_controlled_power_mode_status);
        if (rc != STD_ERR_OK){
            break;
        }

    }while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}

/**
 * Raw read api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * data[out]      - Data will be filled after read
 * data_len[in]     - length of the data to be read
 * return           - t_std_error
 */
t_std_error sdi_qsfp_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_i2c_addr_t address;
    bool paging_support_flag = true;
    int device_addr = addr->device_addr;
    int page = addr->page;
    uint_t offset = addr->offset;


    STD_ASSERT(data != NULL);
    STD_ASSERT(data_len > 0);
    STD_ASSERT(resource_hdl != NULL);


    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    address = qsfp_device->addr.i2c_addr;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_read_generic (qsfp_priv_data->sfp_device, addr, data, data_len);
    }

    if (device_addr == SDI_MEDIA_DEVICE_ADDR_AUTO) {
        /* Currently do nothing. Leave address as is (derived from config file) */
    } else if ( device_addr < SDI_MEDIA_DEVICE_ADDR_AUTO) {
        SDI_DEVICE_ERRMSG_LOG("Invalid media device address value: %d ", device_addr);
        return SDI_DEVICE_ERRCODE(EINVAL);
    } else {
        /* Use address provided by arg*/
        address.i2c_addr = device_addr;
    }

    if ((page == SDI_MEDIA_PAGE_SELECT_NOT_SUPPORTED) || (page == SDI_MEDIA_PAGE_SELECT_IGNORE)) {
        paging_support_flag = false;
    } else if (page < SDI_MEDIA_MINIMUM_PAGE) {
        SDI_DEVICE_ERRMSG_LOG("Invalid page select value: %d", page);
        return SDI_DEVICE_ERRCODE(EINVAL);
    } else if (sdi_is_paging_supported(qsfp_device, &paging_support_flag) != STD_ERR_OK) {
        if ((page > SDI_MEDIA_MINIMUM_PAGE) && (!paging_support_flag) ) {
            SDI_DEVICE_ERRMSG_LOG("Unable to select page %d. Feature not supported on %s",
                page, qsfp_device->alias);
        }
    }


    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = (paging_support_flag) ? sdi_qsfp_page_select(qsfp_device, page) : STD_ERR_OK;
        if(rc != STD_ERR_OK){
            if( rc == SDI_DEVICE_ERRCODE(ENOTSUP) ) {
                paging_support_flag = false;
            } else {
                SDI_DEVICE_ERRMSG_LOG("page %u selection failed for %s", page,
                                       qsfp_device->alias);
            }
            break;
        }

        rc = sdi_smbus_read_multi_byte(qsfp_device->bus_hdl, address,
               offset, data, data_len, SDI_I2C_FLAG_NONE);

        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                                  "rc : %d", address, offset, rc);
            break;
        }
    } while(0);

    if(paging_support_flag) {
        /* Select the default page */
        rc = sdi_qsfp_page_select(qsfp_device, SDI_MEDIA_PAGE_DEFAULT);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("page 0 selection failed for %s",
                                   qsfp_device->alias);
        }
    }

    sdi_qsfp_module_deselect(qsfp_priv_data);

    return rc;
}

/**
 * Raw write api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * data[in]      - Data to write
 * data_len[in]     - length of the data to be written
 * return           - t_std_error
 */
t_std_error sdi_qsfp_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    sdi_i2c_addr_t address;
    bool paging_support_flag = true;
    int device_addr = addr->device_addr;
    int page = addr->page;
    uint_t offset = addr->offset;


    STD_ASSERT(data != NULL);
    STD_ASSERT(data_len > 0);
    STD_ASSERT(resource_hdl != NULL);


    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    address = qsfp_device->addr.i2c_addr;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_write_generic (qsfp_priv_data->sfp_device, addr, data, data_len);
    }

    if (device_addr == SDI_MEDIA_DEVICE_ADDR_AUTO) {
        /* Currently do nothing. Leave address as is (derived from config file)  */
    } else if ( device_addr < SDI_MEDIA_DEVICE_ADDR_AUTO) {
        SDI_DEVICE_ERRMSG_LOG("Invalid media device address value: %d ", device_addr);
        return SDI_DEVICE_ERRCODE(EINVAL);
    } else {
        /* Use address provided by arg*/
        address.i2c_addr = device_addr;
    }

    if ( (page == SDI_MEDIA_PAGE_SELECT_NOT_SUPPORTED)  || (page == SDI_MEDIA_PAGE_SELECT_IGNORE) ) {
        paging_support_flag = false;
    } else if (page < SDI_MEDIA_MINIMUM_PAGE) {
        SDI_DEVICE_ERRMSG_LOG("Invalid page select value: %d", page);
        return SDI_DEVICE_ERRCODE(EINVAL);
    } else if (sdi_is_paging_supported(qsfp_device, &paging_support_flag) != STD_ERR_OK) {
        if ((page > SDI_MEDIA_MINIMUM_PAGE) && (!paging_support_flag) ) {
            SDI_DEVICE_ERRMSG_LOG("Unable to select page %d. Feature not supported on %s",
                page, qsfp_device->alias);
        }
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = (paging_support_flag) ? sdi_qsfp_page_select(qsfp_device, page) : STD_ERR_OK;
        if(rc != STD_ERR_OK){
            if( rc == SDI_DEVICE_ERRCODE(ENOTSUP) ) {
                paging_support_flag = false;
            } else {
                SDI_DEVICE_ERRMSG_LOG("page %u selection failed for %s", page,
                                       qsfp_device->alias);
            }
            break;
        }

        rc = sdi_smbus_write_multi_byte(qsfp_device->bus_hdl, address,
               offset, data, data_len, SDI_I2C_FLAG_NONE);

        if (rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d reg : %d"
                                  "rc : %d", address, offset, rc);
            break;
        }
    } while(0);

    if(paging_support_flag) {
        /* Select default page */
        rc = sdi_qsfp_page_select(qsfp_device, SDI_MEDIA_PAGE_DEFAULT);
        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("page 0 selection failed for %s",
                                   qsfp_device->alias);
        }
    }

    sdi_qsfp_module_deselect(qsfp_priv_data);

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
t_std_error sdi_qsfp_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len)
{
    sdi_media_eeprom_addr_t addr = {
                                    .device_addr = SDI_MEDIA_DEVICE_ADDR_AUTO,
                                    .page        = SDI_MEDIA_PAGE_SELECT_IGNORE,
                                    .offset      = offset
                                   };
    return sdi_qsfp_read_generic(resource_hdl, &addr, data, data_len);
}

/**
 * Raw write api for media eeprom
 * resource_hdl[in] - Handle of the resource
 * offset[in]       - offset from which to write
 * data[in]         - input buffer which contains the data to be written
 * data_len[in]     - length of the data to be written
 * return           - t_std_error
 */
t_std_error sdi_qsfp_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                           uint8_t *data, size_t data_len)
{
    sdi_media_eeprom_addr_t addr = {
                                    .device_addr = SDI_MEDIA_DEVICE_ADDR_AUTO,
                                    .page        = SDI_MEDIA_PAGE_SELECT_IGNORE,
                                    .offset      = offset
                                   };
    return sdi_qsfp_write_generic(resource_hdl, &addr, data, data_len);
}


/**
 * Set the autoneg config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * enable[in] - autoneg enable (True/Flase)
 * return standard t_std_error
 */
t_std_error sdi_qsfp_phy_autoneg_set (sdi_resource_hdl_t resource_hdl,
                                      uint_t channel, sdi_media_type_t type,
                                      bool enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_phy_autoneg_set(qsfp_priv_data->sfp_device,
                                       channel, type, enable);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    switch (type) {
        case QSFP_4X1_1000BASE_T:
            rc = sdi_qsfp_4X1_1000baseT_autoneg_set(qsfp_device, channel, enable);
            break;
        default:
            rc = SDI_DEVICE_ERR_PARAM;
            break;
    }

    sdi_qsfp_module_deselect(qsfp_priv_data);

    return rc;

}

/**
 * Set the interface type config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * mode[in] - Interface type (GMII/SMII/XFI)
 * return standard t_std_error
 */
t_std_error sdi_qsfp_phy_mode_set (sdi_resource_hdl_t resource_hdl,
                                   uint_t channel, sdi_media_type_t type,
                                   sdi_media_mode_t mode)
{

    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_phy_mode_set(qsfp_priv_data->sfp_device,
                                    channel, type, mode);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    switch (type) {
        case QSFP_4X1_1000BASE_T:
            rc = sdi_qsfp_4X1_1000baseT_mode_set(qsfp_device, channel, mode);
            break;
        default:
            rc = SDI_DEVICE_ERR_PARAM;
            break;
    }

    sdi_qsfp_module_deselect(qsfp_priv_data);

    return rc;
}

/**
 * Set the speed config parameter based on the media type.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number
 * type[in] - Media type
 * speed[in] - Speed (10MBPS/100MBPS/1G/10G/40G)
 * return standard t_std_error
 */
t_std_error sdi_qsfp_phy_speed_set (sdi_resource_hdl_t resource_hdl,
                                   uint_t channel, sdi_media_type_t type,
                                   sdi_media_speed_t speed)
{

    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_phy_speed_set(qsfp_priv_data->sfp_device,
                                     channel, type, speed);
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    switch (type) {
        case QSFP_4X1_1000BASE_T:
            rc = sdi_qsfp_4X1_1000baseT_speed_set(qsfp_device, channel, speed);
            break;
        default:
            rc = SDI_DEVICE_ERR_PARAM;
            break;
    }

    sdi_qsfp_module_deselect(qsfp_priv_data);

    return rc;
}

/*
 * @brief get media category based on identifier
 * @pres[in]      - identifier
 * @return        - media category
 */

static qsfp_category_t sdi_qsfp_get_category(uint_t identifier)
{
    qsfp_category_t category = SDI_CATEGORY_QSFP;

    switch (identifier) {
        case 0x0c:
            category = SDI_CATEGORY_QSFP;
            break;
        case 0x0d:
            category = SDI_CATEGORY_QSFPPLUS;
            break;
        case 0x11:
            category = SDI_CATEGORY_QSFP28;
            break;
        case 0x18:
            category = SDI_CATEGORY_QSFPDD;
            break;
        default:
            break;
    }
    return category;
}

/*
 * @brief initialize the module
 * @param[in] resource_hdl - handle to the qsfp
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */

t_std_error sdi_qsfp_module_init (sdi_resource_hdl_t resource_hdl, bool pres)
{

    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t  identifier = 0;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (pres == false) {
        if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {

            qsfp_priv_data->mod_type = SDI_MEDIA_DEFAULT;

            if (qsfp_priv_data->sfp_device != NULL) {
                if (qsfp_priv_data->sfp_device->private_data != NULL) {
                    free(qsfp_priv_data->sfp_device->private_data);
                }

                free(qsfp_priv_data->sfp_device);

                qsfp_priv_data->sfp_device = NULL;
            }
        }

        return rc;
    }

    rc = sdi_qsfp_module_select(qsfp_device);
    if (rc != STD_ERR_OK){
        return rc;
    }

    do {

        if ((rc = sdi_smbus_read_byte(qsfp_device->bus_hdl,
                        qsfp_device->addr.i2c_addr, IDENTIFIER_OFFSET,
                        &identifier, SDI_I2C_FLAG_NONE)) != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d reg : %d"
                    "rc : %d", qsfp_device->addr, IDENTIFIER_OFFSET, rc);
            break;
        }

        if ((identifier == 0x03) || (identifier == 0x0b)) {
            qsfp_priv_data->mod_type = QSFP_QSA_ADAPTER;
        }

        qsfp_priv_data->mod_category = sdi_qsfp_get_category(identifier);

        if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {

            sdi_device_hdl_t sfp_device = NULL;
            sfp_device_t *sfp_priv_data = NULL;

            sfp_device = calloc(1, sizeof(sdi_device_entry_t));
            STD_ASSERT(sfp_device != NULL);

            sfp_priv_data = calloc(1, sizeof(sfp_device_t));
            STD_ASSERT(sfp_priv_data != NULL);

            memcpy(sfp_device, qsfp_device, sizeof(sdi_device_entry_t));
            sfp_device->private_data = sfp_priv_data;
            sfp_priv_data->mux_sel_hdl = qsfp_priv_data->mux_sel_hdl;
            sfp_priv_data->mux_sel_value = qsfp_priv_data->mux_sel_value;
            sfp_priv_data->mod_sel_hdl = qsfp_priv_data->mod_sel_hdl;
            sfp_priv_data->mod_sel_value = qsfp_priv_data->mod_sel_value;
            sfp_priv_data->mod_pres_hdl = qsfp_priv_data->mod_pres_hdl;
            sfp_priv_data->mod_pres_bitmask = qsfp_priv_data->mod_pres_bitmask;

            qsfp_priv_data->sfp_device = sfp_device;

        } else {
            bool paging_support = false;

            if (sdi_is_paging_supported(qsfp_device, &paging_support) != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("Unable to check for paging support on %s",
                        qsfp_device->alias);
            } else if (paging_support) {
                rc = sdi_qsfp_page_select(qsfp_device, SDI_MEDIA_PAGE_DEFAULT);
                if(rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("page 0 selection is failed for %s",
                            qsfp_device->alias);
                }
             }
            /* If QSFP28-DD, need to check revision and if revision is 0.2, powerup datapath */
            if (identifier == 0x18) {

                uint8_t eeprom_version = 0;
                uint8_t datapath_state = 0;
                uint8_t offset = QSFP28_DD_DATAPATH_POWERUP_OFFSET;
                uint8_t page   = SDI_MEDIA_PAGE_DEFAULT;

                rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP28_DD_EEPROM_VERSION_OFFSET, &eeprom_version, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("Unable to read QSFP28-DD %s eeprom version. rc: %d ",
                    qsfp_device->alias, rc);
                }

                qsfp_priv_data->eeprom_version = eeprom_version;
                if (eeprom_version == QSFP28_DD_EEPROM_VERSION_3){
                    offset = QSFP28_DD_DATAPATH_POWERUP_OFFSET_REV3;
                    page = SDI_MEDIA_PAGE_16;
                }

                rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP28_DD_EEPROM_VERSION_OFFSET, &eeprom_version, SDI_I2C_FLAG_NONE);
                if (rc != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("Unable to read QSFP28-DD %s eeprom version. rc: %d ",
                    qsfp_device->alias, rc);
                }

                if (eeprom_version >= QSFP28_DD_EEPROM_VERSION_2) {
                    SDI_DEVICE_TRACEMSG_LOG("Found media with rev %x QSFP28-DD eeprom on %s",
                        eeprom_version, qsfp_device->alias);

                    std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

                    SDI_DEVICE_TRACEMSG_LOG("Attempting to powerup datapath on %s",
                        qsfp_device->alias);

                    if (STD_ERR_OK != sdi_qsfp_page_select(qsfp_device, page)){
                        SDI_DEVICE_TRACEMSG_LOG("Page select failed for module %s",
                                       " when attempting datapath powerup", qsfp_device->alias);
                    }

                    std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

                    rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                              offset, QSFP28_DD_DATAPATH_POWERUP_VALUE, SDI_I2C_FLAG_NONE);
                    if (rc != STD_ERR_OK) {
                        SDI_DEVICE_ERRMSG_LOG("Unable to write datapath powerup byte to QSFP28-DD %s eeprom version. rc: %d ",
                            qsfp_device->alias, rc);
                    }
                    std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

                    rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                        offset, &datapath_state, SDI_I2C_FLAG_NONE);
                    if (rc != STD_ERR_OK) {
                        SDI_DEVICE_ERRMSG_LOG("Unable to read QSFP28-DD %s datapath powerup byte. rc: %d ",
                            qsfp_device->alias, rc);
                    }
                    if ((datapath_state != QSFP28_DD_DATAPATH_POWERUP_VALUE) | (rc != STD_ERR_OK)) {
                        SDI_DEVICE_ERRMSG_LOG("Datapath powerup failed. Read back %u,  error %u",
                            qsfp_device->alias, datapath_state, rc);
                    }
                    std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));
                    if (STD_ERR_OK != sdi_qsfp_page_select(qsfp_device, SDI_MEDIA_PAGE_DEFAULT)){
                        SDI_DEVICE_TRACEMSG_LOG("Page select failed for module %s when resetting page to default"
                                             , qsfp_device->alias);
                    }
                }
            } else { /* QSFP+/QSFP28. Will handle QSFP28-DD in future */
                sdi_qsfp_module_deselect(qsfp_priv_data);
                if (sdi_qsfp_media_max_power_get(resource_hdl, &(qsfp_priv_data->module_info.max_module_power_mw),
                        &(qsfp_priv_data->module_info.software_controlled_power_mode)) != STD_ERR_OK) {
                    SDI_DEVICE_ERRMSG_LOG("Unable to get media max power for module %s", qsfp_device->alias);
                    qsfp_priv_data->module_info.max_module_power_mw = SDI_MEDIA_NO_MAX_POWER_DEFINED;
                    qsfp_priv_data->module_info.software_controlled_power_mode =  false;
                } else if (qsfp_priv_data->module_info.software_controlled_power_mode) {
                    sdi_qsfp_media_force_power_mode_set(resource_hdl, false);
                    /* Put all in low power mode first */
                }
                return rc;
            }
        }

    } while (0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

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
t_std_error sdi_qsfp_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                         sdi_media_type_t type, bool *status)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;

    STD_ASSERT(resource_hdl != NULL);
    STD_ASSERT(status != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_phy_link_status_get(qsfp_priv_data->sfp_device, channel,
                                           type, status);
    }

    return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
}

/**
 * Set power down state (enable/disable) on media PHY.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * enable           - true - power down, false - power up
 * return           - t_std_error
 */

t_std_error sdi_qsfp_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_phy_power_down_enable(qsfp_priv_data->sfp_device, channel,
                                              type, enable);
    }

    return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
}

/**
 * Control (enable/disable) Fiber/Serdes tx and RX on media PHY.
 * resource_hdl[in] - Handle of the resource
 * channel[in]      - channel number
 * type             - media type
 * enable           - true - Enable Serdes, false - Disable Serdes
 * return           - t_std_error
 */

t_std_error sdi_qsfp_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                        sdi_media_type_t type, bool enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_phy_serdes_control(qsfp_priv_data->sfp_device, channel,
                                              type, enable);
    }

    return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
}

/**
 * @brief Api to set the rate select on optics.
 * resource_hdl[in] - Handle of the resource
 * channel[in] - channel number that is of interest.
 * rev[in]     -  Firmware revision
 * cdr_enable[in]  -  true to enable CDR, false to disable CDR via rate select bits.
 * return           - t_std_error
 */

t_std_error sdi_qsfp_ext_rate_select (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                      sdi_media_fw_rev_t rev, bool cdr_enable)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;
    t_std_error rc = STD_ERR_OK;
    uint8_t rx_buf, tx_buf = 0x0;

    STD_ASSERT(resource_hdl != NULL);
    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        /* Rate select not supported on QSA yet */
        return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
    }

    if (sdi_qsfp_validate_channel(channel, qsfp_priv_data->mod_category)
            != true){
        return SDI_DEVICE_ERR_PARAM;
    }

    if ((rc = sdi_qsfp_module_select(qsfp_device)) != STD_ERR_OK) {
        return rc;
    }

    do {
        uint8_t rx_tmp, tx_tmp = 0;
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));

        rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                QSFP_RX_EXT_RATE_SELECT_OFFSET, &rx_buf, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                    ", when attempting rate select.",qsfp_device->addr, rc);
            break;
        }

        rc = sdi_smbus_read_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                QSFP_TX_EXT_RATE_SELECT_OFFSET, &tx_buf, SDI_I2C_FLAG_NONE);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("qsfp smbus read failed at addr : %d rc : %d",
                    ", when attempting rate select.",qsfp_device->addr, rc);
            break;
        }

        rx_tmp = rx_buf;
        tx_tmp = tx_buf;
        if (rev == SDI_MEDIA_FW_REV0) {
            if (cdr_enable == true) {
                rx_buf = tx_buf = 0xAA;
            } else {
                rx_buf = tx_buf = 0x00;
            }
        } else if (rev == SDI_MEDIA_FW_REV1) {
            if (cdr_enable == true) {
                rx_buf = tx_buf = 0xFF;
            } else {
                rx_buf = tx_buf = 0x55;
            }
        } else {
            break;
        }

        if (rx_tmp != rx_buf) {
            rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_RX_EXT_RATE_SELECT_OFFSET, rx_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d rc : %d",
                        ", when attempting rx-rate select.", qsfp_device->addr, rc);
            }
        }

        if (tx_tmp != tx_buf) {
            rc = sdi_smbus_write_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                    QSFP_TX_EXT_RATE_SELECT_OFFSET, tx_buf, SDI_I2C_FLAG_NONE);
            if (rc != STD_ERR_OK){
                SDI_DEVICE_ERRMSG_LOG("qsfp smbus write failed at addr : %d rc : %d",
                        ", when attempting tx-rate select.", qsfp_device->addr, rc);
            }
        }
    } while (0);

    sdi_qsfp_module_deselect(qsfp_priv_data);
    return rc;
}

/**
 * @brief API to get QSA adapter type
 * resource_hdl[in] - Handle of the resource
 * sdi_qsa_adapter_type_t*[out] - adapter type obtained
 * return           - t_std_error
 */

t_std_error sdi_qsfp_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;

    uint8_t buf [SDI_QSFP_QSA28_OUI_LEN] = {0};
    t_std_error rc = STD_ERR_OK;

    STD_ASSERT(resource_hdl != NULL);
    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type != QSFP_QSA_ADAPTER) {
        *qsa_adapter = SDI_QSA_ADAPTER_NONE;
        return rc;
    }

    rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_reset_hdl);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Could not acquire bus when attempting module reset for QSA detection");
        *qsa_adapter = SDI_QSA_ADAPTER_UNKNOWN;
        return rc;
    }

    /* Put module in reset mode */
    do {
        uint_t value = 0;
        rc = sdi_pin_group_read_level(qsfp_priv_data->mod_reset_hdl, &value);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("module reset status get failed for %s", qsfp_device->alias);
            break;
        }
        STD_BIT_SET(value, qsfp_priv_data->mod_reset_bitmask);
        rc = sdi_pin_group_write_level(qsfp_priv_data->mod_reset_hdl, value);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("module reset set failed for %s", qsfp_device->alias);
            break;
        }
        std_usleep(1000 * qsfp_priv_data->mod_reset_delay_ms);
    } while(0);
    sdi_pin_group_release_bus(qsfp_priv_data->mod_reset_hdl);

    if ((rc = sdi_qsfp_module_select(qsfp_device)) != STD_ERR_OK) {
        SDI_DEVICE_ERRMSG_LOG("QSFP module selection failed when attempting QSA info get for %s",
             qsfp_device->alias);
        *qsa_adapter = SDI_QSA_ADAPTER_UNKNOWN;
        return rc;
    }

    do {
        std_usleep(MILLI_TO_MICRO(qsfp_priv_data->delay));
        rc = sdi_smbus_read_multi_byte(qsfp_device->bus_hdl, qsfp_device->addr.i2c_addr,
                                SDI_QSFP_QSA28_OUI_OFFSET, buf, sizeof(buf), SDI_I2C_FLAG_NONE);

        if(rc != STD_ERR_OK) {
            SDI_DEVICE_ERRMSG_LOG("Unable to read adapter info when attempting QSA info get for %s",
                 qsfp_device->alias);
            *qsa_adapter = SDI_QSA_ADAPTER_UNKNOWN;
        }
        else if ((buf[0] == SDI_QSFP_QSA28_OUI_VAL0) && (buf[1] == SDI_QSFP_QSA28_OUI_VAL1)
           && (buf[2] == SDI_QSFP_QSA28_OUI_VAL2)){
            *qsa_adapter = SDI_QSA_ADAPTER_QSA28;
        } else {
            *qsa_adapter = SDI_QSA_ADAPTER_QSA;
        }
    } while(0);

    sdi_qsfp_module_deselect(qsfp_priv_data);

    /* Bring module out of reset mode */
    rc = sdi_pin_group_acquire_bus(qsfp_priv_data->mod_reset_hdl);
    if (rc != STD_ERR_OK){
        SDI_DEVICE_ERRMSG_LOG("Could not acquire bus when attempting module reset for QSA detection");
        return rc;
    }

    do {
        uint_t value = 0;
        rc = sdi_pin_group_read_level(qsfp_priv_data->mod_reset_hdl, &value);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("module reset status get failed after QSA read for %s", qsfp_device->alias);
            break;
        }
        STD_BIT_CLEAR(value, qsfp_priv_data->mod_reset_bitmask);
        rc = sdi_pin_group_write_level(qsfp_priv_data->mod_reset_hdl, value);
        if (rc != STD_ERR_OK){
            SDI_DEVICE_ERRMSG_LOG("module reset clear failed after QSA read for %s. Device may not work as expected", qsfp_device->alias);
            break;
        }
    } while(0);
    sdi_pin_group_release_bus(qsfp_priv_data->mod_reset_hdl);
    return rc;
}


/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */

t_std_error sdi_qsfp_wavelength_set (sdi_resource_hdl_t resource_hdl, float value)
{
    sdi_device_hdl_t qsfp_device = NULL;
    qsfp_device_t *qsfp_priv_data = NULL;

    STD_ASSERT(resource_hdl != NULL);

    qsfp_device = (sdi_device_hdl_t)resource_hdl;
    qsfp_priv_data = (qsfp_device_t *)qsfp_device->private_data;
    STD_ASSERT(qsfp_priv_data != NULL);

    if (qsfp_priv_data->mod_type == QSFP_QSA_ADAPTER) {
        return sdi_sfp_wavelength_set(qsfp_priv_data->sfp_device, value);
    }

    return SDI_DEVICE_ERRCODE(EOPNOTSUPP);
}

