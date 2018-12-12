/*
 * Copyright (c) 2018 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * filename: sdi_media.h
 */



/**
 * @file sdi_media.h
 * @brief Public APIs for manipulating the media related information.
 * For example : sfp, qsfp
 *
 */

#ifndef __SDI_MEDIA_H_
#define __SDI_MEDIA_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sdi_media_api SDI media API.
 * media related API. All media related API take an argument
 * of type sdi_resource_hdl_t.  Application should first identify the
 * right sdi_resource_hdl_t by using @ref sdi_entity_resource_lookup
 * @ingroup sdi_sys
 * @{
 */

/**
 * @defgroup sdi_media_vendor_info_size SDI media vendor information size
 * @ingroup sdi_media_api
 * @{
 */
/**
 * @def SDI_MEDIA_MAX_VENDOR_NAME_LEN
 * Maximum length of the vendor name for a media
 */
#define SDI_MEDIA_MAX_VENDOR_NAME_LEN           17

/**
 * @def SDI_MEDIA_MAX_VENDOR_OUI_LEN
 * Maximum length of the vendor OUI for a media
 */
#define SDI_MEDIA_MAX_VENDOR_OUI_LEN            4

/**
 * @def SDI_MEDIA_MAX_VENDOR_REVISION_LEN
 * Maximum length of the vendor's product revision
 */
#define SDI_MEDIA_MAX_VENDOR_REVISION_LEN       3

/**
 * @def SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN
 * Maximum length of the vendor part number
 */
#define SDI_MEDIA_MAX_VENDOR_PART_NUMBER_LEN    17

/**
 * @def SDI_MEDIA_MAX_VENDOR_SERIAL_NUMBER_LEN
 * Maximum length of the vendor serial number
 */
#define SDI_MEDIA_MAX_VENDOR_SERIAL_NUMBER_LEN  17

/**
 * @def SDI_MEDIA_MAX_VENDOR_DATE_LEN
 * Maximum length for vendor's date code
 */
#define SDI_MEDIA_MAX_VENDOR_DATE_LEN           9

/**
 * @}
 */


/**
 * @defgroup sdi media power info
 * @ingroup sdi_media_api
 * @{
 */

/**
 * @def SDI_MEDIA_NO_MAX_POWER_DEFINED
 * Max power for media is undef
 */
#define SDI_MEDIA_NO_MAX_POWER_DEFINED -1
/**
 * @def SDI_SFP_ZERO_WATT_POWER_IN_DBM
 * Value of rx power for modules which do not support power reading
 */
#define SDI_SFP_ZERO_WATT_POWER_IN_DBM NAN
/**
 * @}
 */


/**
 * @defgroup sdi_media_address_page_select. SDI media device address and page select values
 * @ingroup sdi_media_api
 * @{
 */

/**
 * @def SDI_MEDIA_PAGE_SIZE
 * Size of each upper page
 */
#define SDI_MEDIA_PAGE_SIZE               128

/**
 * @def SDI_MEDIA_MINIMUM_PAGE
 * Minimum value of page
 */
#define SDI_MEDIA_MINIMUM_PAGE            0

/**
 * @}
 */


/**
 * @defgroup sdi_media_status SDI media status.
 * Flags for the status(Alarms, warnings) of media module
 * Ref: Qsfp - Page A0,Module monitor flags/values
 *      SFP - Page A2,Real Time Diagnostic and Control Registers
 * @ingroup sdi_media_api
 * @{
 */

/**
 * @def SDI_MEDIA_STATUS_TEMP_HIGH_ALARM
 * Flag to indicate the temperature high Alarm of a media
 */
#define SDI_MEDIA_STATUS_TEMP_HIGH_ALARM     (1)
/**
 * @def SDI_MEDIA_STATUS_TEMP_LOW_ALARM
 * Flag to indicate the temperature low Alarm of a media
 */
#define SDI_MEDIA_STATUS_TEMP_LOW_ALARM     (1 << 1)
/**
 * @def SDI_MEDIA_STATUS_TEMP_HIGH_WARNING
 * Flag to indicate the temperature high warning of a media
 */
#define SDI_MEDIA_STATUS_TEMP_HIGH_WARNING (1 << 2)
/**
 * @def SDI_MEDIA_STATUS_TEMP_LOW_WARNING
 * Flag to indicate the temperature low warning of a media
 */
#define SDI_MEDIA_STATUS_TEMP_LOW_WARNING     (1 << 3)
/**
 * @def SDI_MEDIA_STATUS_VOLT_HIGH_ALARM
 * Flag to indicate the high voltage Alarm of a media
 */
#define SDI_MEDIA_STATUS_VOLT_HIGH_ALARM     (1 << 4)
/**
 * @def SDI_MEDIA_STATUS_VOLT_LOW_ALARM
 * Flag to indicate the low voltage Alarm of a media
 */
#define SDI_MEDIA_STATUS_VOLT_LOW_ALARM     (1 << 5)
/**
 * @def SDI_MEDIA_STATUS_VOLT_HIGH_WARNING
 * Flag to indicate the high voltage warning of a media
 */
#define SDI_MEDIA_STATUS_VOLT_HIGH_WARNING     (1 << 6)
/**
 * @def SDI_MEDIA_STATUS_VOLT_LOW_WARNING
 * Flag to indicate the low voltage warning of a media
 */
#define SDI_MEDIA_STATUS_VOLT_LOW_WARNING     (1 << 7)
/**
 * @}
 */

/**
 * @defgroup sdi_media_channel_status SDI media channel status.
 * Flag for the channel status of the media
 * Ref: Qsfp - Page A0,Module monitor flags/values
 *      SFP - Page A2,Real Time Diagnostic and Control Registers
 * @ingroup sdi_media_api
 * @{
 */
/**
 * @def SDI_MEDIA_STATUS_TXDISABLE
 * Flag to indicate the Tx disable status for a specific channel of a media
 */
#define SDI_MEDIA_STATUS_TXDISABLE    (1)
/**
 * @def SDI_MEDIA_STATUS_TXFAULT
 * Flag to indicate the Tx fault status for a specific channel of a media
 */
#define SDI_MEDIA_STATUS_TXFAULT         (1 << 1)
/**
 * @def SDI_MEDIA_STATUS_TXLOSS
 * Flag to indicate the Tx loss status for a specific channel of a media
 */
#define SDI_MEDIA_STATUS_TXLOSS         (1 << 2)
/**
 * @def SDI_MEDIA_STATUS_RXLOSS
 * Flag to indicate the Rx loss status for a specific channel of a media
 */
#define SDI_MEDIA_STATUS_RXLOSS         (1 << 3)
/**
 * @}
 */

/**
 * @defgroup sdi_media_channel_monitoring_status SDI media channel monitoring status.
 * Flag for the channel alarm status of the media
 * @ingroup sdi_media_api
 * @{
 */
/**
 * @def SDI_MEDIA_RX_PWR_HIGH_ALARM
 * Flag to indicate the rx_power high alarm status for a specific channel of a media
 */
#define SDI_MEDIA_RX_PWR_HIGH_ALARM     (1)
/**
 * @def SDI_MEDIA_RX_PWR_LOW_ALARM
 * Flag to indicate the rx_power low alarm status for a specific channel of a media
 */
#define SDI_MEDIA_RX_PWR_LOW_ALARM      (1 << 1)
/**
 * @def SDI_MEDIA_RX_PWR_HIGH_WARNING
 * Flag to indicate the rx_power high warning status for a specific channel of a media
 */
#define SDI_MEDIA_RX_PWR_HIGH_WARNING   (1 << 2)
/**
 * @def SDI_MEDIA_RX_PWR_LOW_WARNING
 * Flag to indicate the rx_power low warning status for a specific channel of a media
 */
#define SDI_MEDIA_RX_PWR_LOW_WARNING    (1 << 3)
/**
 * @def SDI_MEDIA_TX_BIAS_HIGH_ALARM
 * Flag to indicate the tx_bias high alarm status for a specific channel of a media
 */
#define SDI_MEDIA_TX_BIAS_HIGH_ALARM    (1 << 4)
/**
 * @def SDI_MEDIA_TX_BIAS_LOW_ALARM
 * Flag to indicate the tx_bias low alarm status for a specific channel of a media
 */
#define SDI_MEDIA_TX_BIAS_LOW_ALARM     (1 << 5)
/**
 * @def SDI_MEDIA_TX_BIAS_HIGH_WARNING
 * Flag to indicate the tx_bias high warning status for a specific channel of a media
 */
#define SDI_MEDIA_TX_BIAS_HIGH_WARNING  (1 << 6)
/**
 * @def SDI_MEDIA_TX_BIAS_LOW_WARNING
 * Flag to indicate the tx_bias low warning status for a specific channel of a media
 */
#define SDI_MEDIA_TX_BIAS_LOW_WARNING   (1 << 7)
/**
 * @def SDI_MEDIA_TX_PWR_HIGH_ALARM
 * Flag to indicate the tx_power high alarm status for a specific channel of a media
 */
#define SDI_MEDIA_TX_PWR_HIGH_ALARM     (1 << 8)
/**
 * @def SDI_MEDIA_TX_PWR_LOW_ALARM
 * Flag to indicate the tx_power low alarm status for a specific channel of a media
 */
#define SDI_MEDIA_TX_PWR_LOW_ALARM      (1 << 9)
/**
 * @def SDI_MEDIA_TX_PWR_HIGH_WARNING
 * Flag to indicate the tx_power high warning status for a specific channel of a media
 */
#define SDI_MEDIA_TX_PWR_HIGH_WARNING   (1 << 10)
/**
 * @def SDI_MEDIA_TX_PWR_LOW_WARNING
 * Flag to indicate the tx_power low warning status for a specific channel of a media
 */
#define SDI_MEDIA_TX_PWR_LOW_WARNING    (1 << 11)

/**
 * @}
 */


/**
 * @enum sdi_port_type_t
 * Port types
 */
typedef enum {
    SDI_MEDIA_PORT_TYPE_BACKPLANE,
    SDI_MEDIA_PORT_TYPE_RJ45,
    SDI_MEDIA_PORT_TYPE_SFP,
    SDI_MEDIA_PORT_TYPE_SFP_PLUS,
    SDI_MEDIA_PORT_TYPE_SFP28,
    SDI_MEDIA_PORT_TYPE_SFP56,
    SDI_MEDIA_PORT_TYPE_QSFP,
    SDI_MEDIA_PORT_TYPE_QSFP_PLUS,
    SDI_MEDIA_PORT_TYPE_QSFP28,
    SDI_MEDIA_PORT_TYPE_QSFP28_DD_1,  /* upper half of double density port. Master */
    SDI_MEDIA_PORT_TYPE_QSFP28_DD_2,  /* Lower half. Slave */
    SDI_MEDIA_PORT_TYPE_QSFP56,
    SDI_MEDIA_PORT_TYPE_QSFP56_DD,

} sdi_port_type_t;

/**
 * @enum sdi_qsa_adapter_type_t
 * QSA adapter type
 */
typedef enum {

    SDI_QSA_ADAPTER_NONE,
    SDI_QSA_ADAPTER_UNKNOWN,
    SDI_QSA_ADAPTER_QSA,
    SDI_QSA_ADAPTER_QSA28
} sdi_qsa_adapter_type_t;

/**
 * @enum sdi_media_page_t
 * Media eeprom page select values
 * There is one fixed lower page byte [0 ~ 127], while there are four upper pages 0 to 3, each with offset [128 ~ 255]
 * When accessing addresses less than 128, only the lower page is accessed.
 * On the other hand, to read bytes 128 to 255, the upper page has to be selected (or a default of page 0 is used).
 * This means the content of bytes 128 to 255 depend on the page selected, while 0 to 127 in page-independent.
 */

typedef enum {

    /* Do not perform any page select action */
    SDI_MEDIA_PAGE_SELECT_IGNORE           = -1,

    /* When page select feature is not supported */
    SDI_MEDIA_PAGE_SELECT_NOT_SUPPORTED    =  SDI_MEDIA_PAGE_SELECT_IGNORE,

    /* Media EEPROM page 0 */
    SDI_MEDIA_PAGE_00                      =  0,

    /* Media EEPROM page 1 */
    SDI_MEDIA_PAGE_01                      =  1,

    /* Media EEPROM page 2 */
    SDI_MEDIA_PAGE_02                      =  2,

    /* Media EEPROM page 3 */
    SDI_MEDIA_PAGE_03                      =  3,

    /* Media EEPROM page 16 */
    SDI_MEDIA_PAGE_16                      = 16,
    /* Media EEPROM default page when no page is selected */
    SDI_MEDIA_PAGE_DEFAULT                 = SDI_MEDIA_PAGE_00,
} sdi_media_page_t;

/**
 * @enum sdi_media_device_addr_t
 * Device address of media for eeprom access
 * Some media are represented as separate logical i2c devices
 */

typedef enum {

    /* Inherit address from media object/config */
    SDI_MEDIA_DEVICE_ADDR_AUTO             = -1,

    /* Device address A0. This is the first */
    SDI_MEDIA_DEVICE_ADDR_A0               =  0x50,

    /* Device address A1 is not defined */

    /* Device address A2 */
    SDI_MEDIA_DEVICE_ADDR_A2               =  0x51,
} sdi_media_device_addr_t;

/**
 * @enum sdi_media_eeprom_addr_t
 * Group of params for eeprom access
 * To be used as arguments to media eeprom access functions
 */

typedef struct _sdi_media_eeprom_addr_t {

    sdi_media_device_addr_t     device_addr;
    sdi_media_page_t            page;
    uint_t                      offset;

} sdi_media_eeprom_addr_t;

/**
 * @enum sdi_media_fw_rev_t
 * Media firmware revision.
 */

typedef enum {
    SDI_MEDIA_FW_REV0,
    SDI_MEDIA_FW_REV1
}sdi_media_fw_rev_t;

/**
 * @enum sdi_media_mode_t
 * mode of control for the media
 */


typedef enum {
    /**
     * SDI MEDIA mode is MII
     */
    SDI_MEDIA_MODE_MII,
    /**
     * SDI MEDIA mode is GMII
     */
    SDI_MEDIA_MODE_GMII,
    /**
     * SDI MEDIA mode is SGMII
     */
    SDI_MEDIA_MODE_SGMII,
}sdi_media_mode_t;

/**
 * @}
 */
/**
 * @enum sdi_media_speed_t
 * Speed of the media
 */
typedef enum {
    /**
     * SDI MEDIA Speed is 10Mbps
     */
    SDI_MEDIA_SPEED_10M,
    /**
     * SDI MEDIA Speed is 100Mbps
     */
    SDI_MEDIA_SPEED_100M,
    /**
     * SDI MEDIA Speed is 1G
     */
    SDI_MEDIA_SPEED_1G,
    /**
     * SDI MEDIA Speed is 10G
     */
    SDI_MEDIA_SPEED_10G,
    /**
     * SDI MEDIA Speed is 25G
     */
    SDI_MEDIA_SPEED_25G,
    /**
     * SDI MEDIA Speed is 40G
     */
    SDI_MEDIA_SPEED_40G,
    /**
     * SDI MEDIA Speed is 100G
     */
    SDI_MEDIA_SPEED_100G,
    /**
     * SDI MEDIA Speed is 20G
     */
    SDI_MEDIA_SPEED_20G,
    /**
     * SDI MEDIA Speed is 50G
     */
    SDI_MEDIA_SPEED_50G,
    /**
     * SDI MEDIA Speed is 200G
     */
    SDI_MEDIA_SPEED_200G,
    /**
     * SDI MEDIA Speed is 400G
     */
    SDI_MEDIA_SPEED_400G,
    /**
     * SDI MEDIA Speed is 4GFC
     */
    SDI_MEDIA_SPEED_4GFC,
    /**
     * SDI MEDIA Speed is 8GFC
     */
    SDI_MEDIA_SPEED_8GFC,
    /**
     * SDI MEDIA Speed is 16GFC
     */
    SDI_MEDIA_SPEED_16GFC,
    /**
     * SDI MEDIA Speed is 32GFC
     */
    SDI_MEDIA_SPEED_32GFC,
}sdi_media_speed_t;

/**
 * @enum sdi_media_module_monitor_t
 * Monitoring parameters for media module
 * Ref: Qsfp - Page A0,Module monitor flags/values
 *      SFP - Page A2,Real Time Diagnostic and Control Registers
 */
typedef enum {
    /**
     * Temperature of a media module
     */
    SDI_MEDIA_TEMP,
    /**
     * Supply voltage of a media module
     */
    SDI_MEDIA_VOLT
}sdi_media_module_monitor_t;

/**
 * @enum sdi_media_channel_monitor_t
 * media channel monitoring parameters
 * Ref: Qsfp - Page A0,Channel monitor flags/values
 *      SFP - Page A2,Real Time Diagnostic and Control Registers
 */
typedef enum {
    /**
     * SDI MEDIA Internal RX Power
     */
    SDI_MEDIA_INTERNAL_RX_POWER_MONITOR,
    /**
     * SDI MEDIA Internal TX Bias Current
     */
    SDI_MEDIA_INTERNAL_TX_BIAS_CURRENT,
    /**
     * Deprecated alias to refer to the above
     */
    SDI_MEDIA_INTERNAL_TX_POWER_BIAS = SDI_MEDIA_INTERNAL_TX_BIAS_CURRENT,
    /**
     * SDI MEDIA Internal TX Output Power
     */
    SDI_MEDIA_INTERNAL_TX_OUTPUT_POWER
}sdi_media_channel_monitor_t;

/**
  * @enum sdi_media_param_type_t
  * media parameter type
  */
typedef enum {
    /** SDI MEDIA Nominal laser wavelength (units : nm) */
    SDI_MEDIA_WAVELENGTH,
    /** SDI MEDIA Guaranteed range of laser wavelength(+/- value) from nominal
     * wavelength (units : nm). This field is not applicable for SFP */
    SDI_MEDIA_WAVELENGTH_TOLERANCE,
    /** SDI MEDIA Maximum case temperature in degrees C. This field is not applicable for SFP */
    SDI_MEDIA_MAX_CASE_TEMP,
    /** SDI MEDIA Check code for base ID fields*/
    SDI_MEDIA_CC_BASE,
    /** SDI MEDIA check code for the Extended ID Fields */
    SDI_MEDIA_CC_EXT,
    /** SDI MEDIA Code for connector type */
    SDI_MEDIA_CONNECTOR,
    /** SDI MEDIA Code for serial encoding algorithm */
    SDI_MEDIA_ENCODING_TYPE,
    /** SDI MEDIA Nominal bit rate, units of 100 MBits/s*/
    SDI_MEDIA_NM_BITRATE,
    /** SDI MEDIA Identifier Type of serial Module */
    SDI_MEDIA_IDENTIFIER,
    /** SDI MEDIA Extended Identifier of Serial Module */
    SDI_MEDIA_EXT_IDENTIFIER,
    /** SDI MEDIA Link length supported for SMF fiber, units of km */
    SDI_MEDIA_LENGTH_SMF_KM,
    /** SDI MEDIA Link length supported for OM1 fiber, units of 1m */
    SDI_MEDIA_LENGTH_OM1,
    /** SDI MEDIA Link length supported for OM2 fiber, units of 1m */
    SDI_MEDIA_LENGTH_OM2,
    /** SDI MEDIA Link length supported for OM3 fiber, units of 2m */
    SDI_MEDIA_LENGTH_OM3,
    /** SDI MEDIA Link length supported for copper or direct attach cable, units
     * of m*/
    SDI_MEDIA_LENGTH_CABLE_ASSEMBLY,
    /** SDI MEDIA Link length of copper or active cable, units of 1m. This field
     * is not applicable for QSFP */
    SDI_MEDIA_LENGTH_SMF,
    /** SDI MEDIA Rate Select, TX Disable, TX Fault, LOS, Warning indicators for:
     * Temperature,  VCC, RX power, TX Bias */
    SDI_MEDIA_OPTIONS,
    /** SDI MEDIA Indicates which optional enhanced features are implemented in
      * the Module */
    SDI_MEDIA_ENHANCED_OPTIONS,
    /** SDI MEDIA Indicates which types of diagnostic monitoring are implemented
     * (if any) in the Module */
    SDI_MEDIA_DIAG_MON_TYPE,
    /** Used to describe the transmitter technology and device technology. Not
     * applicable for SFP */
    SDI_MEDIA_DEVICE_TECH,
    /** Upper bit rate margin, units of %(BR, max). This field is not applicable
     * for QSFP */
    SDI_MEDIA_MAX_BITRATE,
    /** Lower bit rate margin, units of %(BR, min). This field is not applicable
     * for QSFP */
    SDI_MEDIA_MIN_BITRATE,
    /** Extended transceiver Code for electronic or optical compatibility
     * for SFP */
    SDI_MEDIA_EXT_COMPLIANCE_CODE,
    /** Free Side device properties this field is not applicable for SFP
     */
    SDI_FREE_SIDE_DEV_PROP,
    /** This field is valid for tunable media. SDI_MEDIA_WAVELENGTH is zero for tunable media
     * Instead SDI_TUNE_WAVELENGTH_PICO_METERS is used, which is in picometers, not nanometers
     * pm is used so as to avoid using floats
     */
    SDI_TUNE_WAVELENGTH_PICO_METERS,

} sdi_media_param_type_t;

/**
  * @enum sdi_media_vendor_info_type_t
  * media vendor information types
  */
typedef enum {
    /** SDI MEDIA vendor name(ASCII) */
    SDI_MEDIA_VENDOR_NAME,
    /** SDI MEDIA vendor IEEE company ID */
    SDI_MEDIA_VENDOR_OUI,
    /** SDI MEDIA Serial number provided by vendor (ASCII)*/
    SDI_MEDIA_VENDOR_SN,
    /** SDI MEDIA Vendor’s manufacturing date code (ASCII)*/
    SDI_MEDIA_VENDOR_DATE,
    /** SDI MEDIA Part number provided by vendor(ASCII)*/
    SDI_MEDIA_VENDOR_PN,
    /** SDI MEDIA Revision level for part number provided by vendor(ASCII)*/
    SDI_MEDIA_VENDOR_REVISION
} sdi_media_vendor_info_type_t;

/**
  *@enum sdi_media_threshold_type_t
  *media threshold type
  */
typedef enum {
    /** Temperature High Alarm Threshold */
    SDI_MEDIA_TEMP_HIGH_ALARM_THRESHOLD,
    /** Temperature Low Alarm Threshold */
    SDI_MEDIA_TEMP_LOW_ALARM_THRESHOLD,
    /** Temperature High Warning Threshold */
    SDI_MEDIA_TEMP_HIGH_WARNING_THRESHOLD,
    /** Temperature Low Warning Threshold */
    SDI_MEDIA_TEMP_LOW_WARNING_THRESHOLD,
    /** Voltage High Alarm Threshold */
    SDI_MEDIA_VOLT_HIGH_ALARM_THRESHOLD,
    /** Voltage Low Alarm Threshold */
    SDI_MEDIA_VOLT_LOW_ALARM_THRESHOLD,
    /** Voltage High Warning Threshold */
    SDI_MEDIA_VOLT_HIGH_WARNING_THRESHOLD,
    /** Voltage Low Warning Threshold */
    SDI_MEDIA_VOLT_LOW_WARNING_THRESHOLD,
    /** Rx Power High Alarm Threshold */
    SDI_MEDIA_RX_PWR_HIGH_ALARM_THRESHOLD,
    /** Rx Power Low Alarm Threshold */
    SDI_MEDIA_RX_PWR_LOW_ALARM_THRESHOLD,
    /** Rx Power High Warning Threshold */
    SDI_MEDIA_RX_PWR_HIGH_WARNING_THRESHOLD,
    /** Rx Power Low Warning Threshold */
    SDI_MEDIA_RX_PWR_LOW_WARNING_THRESHOLD,
    /** Tx Bias High Alarm Threshold */
    SDI_MEDIA_TX_BIAS_HIGH_ALARM_THRESHOLD,
    /** Tx Bias Low Alarm Threshold */
    SDI_MEDIA_TX_BIAS_LOW_ALARM_THRESHOLD,
    /** Tx Bias High Warning Threshold */
    SDI_MEDIA_TX_BIAS_HIGH_WARNING_THRESHOLD,
    /** Tx Bias Low Warning Threshold */
    SDI_MEDIA_TX_BIAS_LOW_WARNING_THRESHOLD,
    /** Tx Power High Alarm Threshold */
    SDI_MEDIA_TX_PWR_HIGH_ALARM_THRESHOLD,
    /** Tx Power Low Alarm Threshold */
    SDI_MEDIA_TX_PWR_LOW_ALARM_THRESHOLD,
    /** Tx Power High Warning Threshold */
    SDI_MEDIA_TX_PWR_HIGH_WARNING_THRESHOLD,
    /** Tx Power Low Warning Threshold */
    SDI_MEDIA_TX_PWR_LOW_WARNING_THRESHOLD,
} sdi_media_threshold_type_t;

/**
  * @enum sdi_media_module_ctrl_type_t
  * media module control types
  */
typedef enum {
    /** SDI MEDIA low power mode */
    SDI_MEDIA_LP_MODE,
    /** SDI MEDIA reset */
    SDI_MEDIA_RESET,
} sdi_media_module_ctrl_type_t;

/**
  * @enum sdi_media_rx_pwr_type_t
  * media module rx power type
  */
typedef enum {
    /** Received power measurements type - OMA */
    SDI_MEDIA_RX_PWR_OMA,
    /** Received power measurements type - Average Power */
    SDI_MEDIA_RX_PWR_AVG,
} sdi_media_rx_pwr_type_t;



/*
 * @enum sdi_media_connector_t
 * Common connector EEPROM values
 */
typedef enum {

    SDI_MEDIA_CONNECTOR_SC                   = 0x01,
    SDI_MEDIA_CONNECTOR_FC_STYLE1_COPPER     = 0x02,
    SDI_MEDIA_CONNECTOR_FC_STYLE2_COPPER     = 0x03,
    SDI_MEDIA_CONNECTOR_FC_BNC_OR_TNC        = 0x04,
    SDI_MEDIA_CONNECTOR_FC_COAX              = 0x05,
    SDI_MEDIA_CONNECTOR_FIBERJACK            = 0x06,
    SDI_MEDIA_CONNECTOR_LC                   = 0x07,  /* Commonly used for FC */
    SDI_MEDIA_CONNECTOR_MT_RJ                = 0x08,
    SDI_MEDIA_CONNECTOR_MU                   = 0x09,
    SDI_MEDIA_CONNECTOR_SG                   = 0x0A,
    SDI_MEDIA_CONNECTOR_OPTICAL_PIGTAIL      = 0x0B,
    SDI_MEDIA_CONNECTOR_MPO_1X12             = 0x0C,
    SDI_MEDIA_CONNECTOR_MPO_2X16             = 0x0D,

    /* 0x0E to 0x1F are reserved */

    SDI_MEDIA_CONNECTOR_HSSDII               = 0x20,
    SDI_MEDIA_CONNECTOR_COPPER_PIGTAIL       = 0x21,  /* Used for DAC in some cases */
    SDI_MEDIA_CONNECTOR_RJ45                 = 0x22,
    SDI_MEDIA_CONNECTOR_NON_SEPARABLE        = 0x23,  /* Can be used for DAC and AOC */
    SDI_MEDIA_CONNECTOR_MXC_2X16             = 0x24

    /* 0x25 to 0x7F are reserved */
    /* 0x80 to 0xFF are vendor specific*/
} sdi_media_connector_t;


/*
 * @enum sdi_media_form_factor_id_t
 * IDs used to represent different media form factors 
 */

typedef enum {
    SDI_MEDIA_DOUBLE_DENSITY_MODULE_ID       = 0x18
} sdi_media_form_factor_id_t;

/**
 * @enum sdi_qsfp_eth_1040g_code_bitmask_t
 * 10/40G Ethernet compliance codes
 */
typedef enum {
    /** 40G Active Cable (XLPPI) */
    QSFP_40G_ACTIVE_CABLE = 0x01,
    /** 40GBASE-LR4 */
    QSFP_40GBASE_LR4 = 0x02,
    /** 40GBASE-SR4 */
    QSFP_40GBASE_SR4 = 0x04,
    /** 40GBASE-CR4 */
    QSFP_40GBASE_CR4 = 0x08,
    /** 40GBASE-ER4 */
    QSFP_40GBASE_ER4 = 0x10,
    /** 10GBASE-SR */
    QSFP_10GBASE_SR  = 0x10,
    /** 10GBASE-LR */
    QSFP_10GBASE_LR  = 0x20,
    /** 10GBASE-LRM */
    QSFP_10GBASE_LRM = 0x40,
} sdi_qsfp_eth_1040g_code_bitmask_t;

/**
 * @enum sdi_qsfp28_eth_code_bitmask_t
 * 100G Ethernet type codes
 */
typedef enum {
    /** 100GBASE-AOC */
    QSFP_100GBASE_AOC = 0x1,
    /** 100GBASE-SR4 */
    QSFP_100GBASE_SR4 = 0x2,
    /** 100GBASE-LR4 */
    QSFP_100GBASE_LR4 = 0x3,
    /** 100GBASE-CWDM4 */
    QSFP_100GBASE_CWDM4 = 0x6,
    /** 100GBASE-PSM4_IR */
    QSFP_100GBASE_PSM4_IR = 0x7,
    /** 100GBASE-CR4 */
    QSFP_100GBASE_CR4 = 0xb,
    /** 2x50Gbase/4x25Gbase CR_CAS */
    QSFP28_BRKOUT_CR_CAS = 0xc,
    /** 2x50Gbase/4x25Gbase CR_CAN */
    QSFP28_BRKOUT_CR_CAN = 0xd,
} sdi_qsfp28_eth_code_bitmask_t;

/**
 * @enum sdi_qsfp_sonet_code_bitmask_t
 * SONET Compliance codes
 */
typedef enum {
    /** OC 48, short reach */
    QSFP_OC_48_SR = 0x01,
    /** OC 48, intermediate reach */
    QSFP_OC_48_IR = 0x02,
    /** OC 48, long reach */
    QSFP_OC_48_LR = 0x04,
    /** 40G OTN (OTU3B/OTU3C) */
    QSFP_40G_OTN  = 0x08,
} sdi_qsfp_sonet_code_bitmask_t;

/**
 * @enum sdi_qsfp_sas_code_bitmask_t
 * SAS/SATA compliance codes
 */
typedef enum {
    /** SAS 3.0G */
    QSFP_SAS_3G = 0x10,
    /** SAS 6.0G */
    QSFP_SAS_6G = 0x20,
} sdi_qsfp_sas_code_bitmask_t;

/**
 * @enum sdi_qsfp_eth_1g_code_bitmask_t
 * Gigabit Ethernet Compliant codes
 */
typedef enum {
    /** 1000BASE-SX */
    QSFP_1GBASE_SX = 0x01,
    /** 1000BASE-LX */
    QSFP_1GBASE_LX = 0x02,
    /** 1000BASE-CX */
    QSFP_1GBASE_CX = 0x04,
    /** 1000BASE-T */
    QSFP_1GBASE_T  = 0x08,
} sdi_qsfp_eth_1g_code_bitmask_t;

/**
 * @enum sdi_qsfp_fc_distance_code_bitmask_t
 * Fibre Channel link length
 */
typedef enum {
    /** medium distance  */
    QSFP_FP_DISTANCE_MEDIUM     = 0x02,
    /** long distance */
    QSFP_FP_DISTANCE_LONG       = 0x04,
    /** intermediate distance */
    QSFP_FP_DISTANCE_INTERMEDIATE   = 0x08,
    /** short distance */
    QSFP_FP_DISTANCE_SHORT      = 0x10,
    /** very long distance */
    QSFP_FP_DISTANCE_VLONG      = 0x20,
} sdi_qsfp_fc_distance_code_bitmask_t;

/**
 * @enum sdi_qsfp_fc_tech_code_bitmask_t
 * Fibre Channel/Cable technology
 */
typedef enum {
    /** Longwave Laser (LL)  */
    QSFP_FC_TECH_LL     = 0x10,
    /** Shortwave laser with OFC (SL) */
    QSFP_FC_TECH_SL     = 0x20,
    /** Shortwave laser w/o OFC (SN) */
    QSFP_FC_TECH_SN     = 0x40,
    /** Electrical intra-enclosure */
    QSFP_FC_TECH_INTRA_EL   = 0x80,
    /** Electrical inter-enclosure (EL) */
    QSFP_FC_TECH_INTER_EL   = 0x100,
    /** Longwave laser (LC) */
    QSFP_FC_TECH_LC         = 0x200,
} sdi_qsfp_fc_tech_code_bitmask_t;

/**
 * @enum sdi_qsfp_fc_media_code_bitmask_t
 * Fibre Channel Transmission media
 */
typedef enum {
    /** Single Mode (SM) */
    QSFP_FC_MEDIA_SM    = 0x01,
    /** Multi-mode 50um (OM3) */
    QSFP_FC_MEDIA_OM3   = 0x02,
    /** Multi-mode 50m (M5) */
    QSFP_FC_MEDIA_M5    = 0x04,
    /** Multi-mode 62.5m (M6) */
    QSFP_FC_MEDIA_M6    = 0x08,
    /** Video Coax (TV) */
    QSFP_FC_MEDIA_TV    = 0x10,
    /** Miniature Coax (MI) */
    QSFP_FC_MEDIA_MI    = 0x20,
    /** Shielded Twisted Pair (TP) */
    QSFP_FC_MEDIA_TP    = 0x40,
    /** Twin Axial Pair (TW) */
    QSFP_FC_MEDIA_TW    = 0x80,
} sdi_qsfp_fc_media_code_bitmask_t;

/**
 * @enum sdi_qsfp_fc_speed_code_bitmask_t
 * Fibre Channel Speed
 */
typedef enum {
    /** 100 Mbytes/Sec */
    QSFP_FC_SPEED_100M  = 0x01,
    /** 200 Mbytes/Sec */
    QSFP_FC_SPEED_200M  = 0x04,
    /** 400 Mbytes/Sec */
    QSFP_FC_SPEED_400M  = 0x10,
    /** 1600 Mbytes/Sec */
    QSFP_FC_SPEED_1600M = 0x20,
    /** 800 Mbytes/Sec */
    QSFP_FC_SPEED_800M  = 0x40,
    /** 1200 Mbytes/Sec */
    QSFP_FC_SPEED_1200M = 0x80,
} sdi_qsfp_fc_speed_code_bitmask_t;

/*
 * @struct sdi_qsfp_transceiver_descr_t
 * maps to the QSFP transceiver specification compiance
 */

typedef struct __attribute__((packed)) {
    uint8_t sdi_qsfp_eth_1040g_code : 8; /**< 10/40G Ethernet Compliance Code */
    uint8_t sdi_qsfp_sonet_code : 8; /**< SONET Compliance codes */
    uint8_t sdi_qsfp_sas_code : 8; /**< SAS/SATA compliance codes */
    uint8_t sdi_qsfp_eth_1g_code : 8; /**< Gigabit Ethernet Compliant codes */
    uint16_t sdi_qsfp_fc_distance : 6; /**< Fibre Channel link length */
    uint16_t sdi_qsfp_fc_technology : 10; /**< Fibre Channel/Cable technology */
    uint8_t sdi_qsfp_fc_media : 8; /**< Fibre Channel Transmission media */
    uint8_t sdi_qsfp_fc_speed : 8; /**< Fibre Channel Speed */
} sdi_qsfp_transceiver_descr_t;

/**
 * @enum sdi_sfp_eth_10g_code_bitmask_t
 * 10G Ethernet Compliance Codes
 */
typedef enum {
    /** 10G base SR */
    SFP_10GBASE_SR  = 0x1,
    /** 10G base LR */
    SFP_10GBASE_LR  = 0x2,
    /** 10G base LRM */
    SFP_10GBASE_LRM = 0x4,
    /** 10G base ER */
    SFP_10GBASE_ER  = 0x8,
} sdi_sfp_eth_10g_code_bitmask_t;

/**
 * @enum sdi_sfp_infiniband_code_bitmask_t
 * Infiniband Compliance Codes
 */
typedef enum {
    /** SFP 1X Copper Passive */
    SFP_INFINIBAND_COPPER_PASSIVE   = 0x1,
    /** SFP 1X Copper Active */
    SFP_INFINIBAND_COPPER_ACTIVE    = 0x2,
    /** SFP 1X Copper LX */
    SFP_INFINIBAND_LX               = 0x4,
    /** SFP 1X Copper SX */
    SFP_INFINIBAND_SX               = 0x8,
} sdi_sfp_infiniband_code_bitmask_t;

/**
 * @enum sdi_sfp_escon_code_bitmask_t
 * ESCON Compliance Codes
 */
typedef enum {
    /** ESCON SMF, 1310nm Laser */
    SFP_ESCON_SMF   = 0x1,
    /** ESCON MMF, 1310nm LED */
    SFP_ESCON_MMF   = 0x2,
} sdi_sfp_escon_code_bitmask_t;

/**
 * @enum sdi_qsfp_sonet_code_bitmask_t
 * SONET Compliance codes for SFP
 */
typedef enum {
    /** OC-3, short reach */
    SFP_OC3_SR  = 0x0001,
    /** OC-3, single mode, intermediate reach */
    SFP_OC3_IR  = 0x0002,
    /** OC-3, single mode, long reach */
    SFP_OC3_LR  = 0x0004,
    /** OC12, short reach */
    SFP_OC12_SR = 0x0010,
    /** OC12, single mode, intermediate reach */
    SFP_OC12_IR = 0x0020,
    /** OC12, single mode, long reach */
    SFP_OC12_LR = 0x0040,
    /** OC48, short reach */
    SFP_OC48_SR = 0x0100,
    /** OC48, intermediate reach */
    SFP_OC48_IR = 0x0200,
    /** OC48, long reach */
    SFP_OC48_LR = 0x0400,
    /** SONET reach specifier bit 2 */
    SFP_RS_BIT_2 = 0x0800,
    /** SONET reach specifier bit 1 */
    SFP_RS_BIT_1 = 0x1000,
    /** OC-192, short reach */
    SFP_OC192_SR = 0x2000,
} sdi_sfp_sonet_code_bitmask_t;

/**
 * @énum sdi_sfp_eth_1g_code_bitmask_t
 * Ethernet Compliance Codes
 */
typedef enum {
    /** 1000BASE-SX */
    SFP_1G_BASE_SX  = 0x01,
    /** 1000BASE-LX */
    SFP_1G_BASE_LX  = 0x02,
    /** 1000BASE-CX */
    SFP_1G_BASE_CX  = 0x04,
    /** 1000BASE-T */
    SFP_1G_BASE_T   = 0x08,
    /** 100BASE-LX/LX10 */
    SFP_100M_BASE_LX = 0x10,
    /** 100BASE-FX */
    SFP_100M_BASE_FX = 0x20,
    /** BASE-BX10 */
    SFP_BASE_BX10   = 0x40,
    /** BASE-PX */
    SFP_BASE_PX     = 0x80,
} sdi_sfp_eth_1g_code_bitmask_t;

/**
 * @enum sdi_sfp_fc_distance_bitmask_t
 * Fibre Channel Link Length
 */
typedef enum {
    /** medium distance  */
    SFP_FC_DISTANCE_MEDIUM  = 0x01,
    /** long distance */
    SFP_FC_DISTANCE_LONG    = 0x02,
    /** intermediate distance */
    SFP_FC_DISTANCE_INTERMEDIATE = 0x04,
    /** short distance */
    SFP_FC_DISTANCE_SHORT   = 0x08,
    /** very long distance */
    SFP_FC_DISTANCE_VLONG   = 0x10,
} sdi_sfp_fc_distance_bitmask_t;

/**
 * @enum sdi_sfp_fc_tech_code_bitmask_t
 * Fibre Channel Technology for SFP
 */
typedef enum {
    /** Longwave Laser (LL)  */
    SFP_FC_TECH_LL  = 0x01,
    /** Shortwave laser with OFC4 (SL) */
    SFP_FC_TECH_SL  = 0x02,
    /** Shortwave laser w/o OFC (SN) */
    SFP_FC_TECH_SN  = 0x04,
    /** Electrical intra-enclosure (EL) */
    SFP_FC_TECH_INTRA_EL = 0x08,
    /** Electrical inter-enclosure (EL) */
    SFP_FC_TECH_INTER_EL = 0x10,
    /** Longwave laser (LC) */
    SFP_FC_TECH_LC       = 0x20,
    /** Shortwave laser, linear Rx (SA) */
    SFP_FC_TECH_SA       = 0x40,
} sdi_sfp_fc_tech_code_bitmask_t;

/**
 * @enum sdi_sfp_plus_cable_tech_bitmask_t
 * SFP+ Cable Technology
 */
typedef enum {
    /** Passive Cable */
    SFP_PLUS_PASSIVE_CABLE  = 0x1,
    /** Active cable */
    SFP_PLUS_ACTIVE_CABLE   = 0x2,
} sdi_sfp_plus_cable_tech_bitmask_t;

/**
 * @enum sdi_sfp_fc_media_bitmask_t
 * Fibre Channel Transmission Media
 */
typedef enum {
    /** Single Mode (SM) */
    SFP_FC_MEDIA_SM = 0x01,
    /** Multimode, 50um (M5, M5E) */
    SFP_FC_MEDIA_M5 = 0x04,
    /** Multimode, 62.5um (M6) */
    SFP_FC_MEDIA_M6 = 0x08,
    /** Video Coax (TV) */
    SFP_FC_MEDIA_TV = 0x10,
    /** Miniature Coax (MI) */
    SFP_FC_MEDIA_MI = 0x20,
    /** Twisted Pair (TP) */
    SFP_FC_MEDIA_TP = 0x40,
    /** Twin Axial Pair (TW) */
    SFP_FC_MEDIA_TW = 0x80,
} sdi_sfp_fc_media_bitmask_t;

/**
 * @enum sdi_sfp_fc_speed_bitmask_t
 * Fibre Channel Speed
 */
typedef enum {
    /** 100 Mbytes/Sec */
    SFP_FC_SPEED_100M   = 0x01,
    /** 200 Mbytes/Sec */
    SFP_FC_SPEED_200M   = 0x04,
    /** 400 Mbytes/Sec */
    SFP_FC_SPEED_400M   = 0x10,
    /** 1600 Mbytes/Sec */
    SFP_FC_SPEED_1600M  = 0x20,
    /** 800 Mbytes/Sec */
    SFP_FC_SPEED_800M   = 0x40,
    /** 1200 Mbytes/Sec */
    SFP_FC_SPEED_1200M  = 0x80,
} sdi_sfp_fc_speed_bitmask_t;

typedef enum {
    /** QSFP 4X1_1000BASE_T type */
    SDI_MEDIA_DEFAULT     = 0,
    QSFP_4X1_1000BASE_T   = 1,
    QSFP_QSA_ADAPTER      = 2,
} sdi_media_type_t;
/*
 * @struct sdi_sfp_transceiver_descr_t
 * maps to the SFP transceiver specification compiance
 */

typedef struct __attribute__((packed)) {
    uint8_t sdi_sfp_eth_10g_code:4; /**< 10G Ethernet Compliance Codes */
    uint8_t sdi_sfp_infiniband_code:4; /**< Infiniband Compliance Codes */
    uint16_t sdi_sfp_escon_code:2; /**< ESCON Compliance Codes */
    uint16_t sdi_sfp_sonet_code:14; /**< SONET Compliance Codes */
    uint8_t sdi_sfp_eth_1g_code:8; /**< Ethernet Compliance Codes */
    uint16_t sdi_sfp_fc_distance:5; /**< Fibre Channel Link Length */
    uint16_t sdi_sfp_fc_technology:7; /**< Fibre Channel Technology */
    uint16_t sdi_sfp_plus_cable_technology:2; /**< SFP+ Cable Technology */
    uint16_t reserved:2;    /**< Unallocated */
    uint8_t sdi_sfp_fc_media:8; /**< Fibre Channel Transmission Media */
    uint8_t sdi_sfp_fc_speed:8; /**< Fibre Channel speed */
} sdi_sfp_transceiver_descr_t;

/*
 * @union sdi_media_transceiver_descr_t
 * maps to the transceiver specification compiance
 */
typedef union {
    sdi_qsfp_transceiver_descr_t  qsfp_descr; /**< QSFP specification compiance */
    sdi_sfp_transceiver_descr_t   sfp_descr;  /**< SFP specification compiance */
} sdi_media_transceiver_descr_t;

/**
 * @struct sdi_qsfp_supported_feature_t
 * QSFP optional feature support status
 */
typedef struct {
    bool rate_select_status;
    bool tx_control_support_status;
    bool paging_support_status;
    bool software_controlled_power_mode_status;
} sdi_qsfp_supported_feature_t;

/**
 * @struct sdi_sfp_supported_feature_t
 * SFP optional feature support status
 */
typedef struct {
    bool rate_select_status;
    bool alarm_support_status;
    bool diag_mntr_support_status;
    bool wavelength_tune_support_status;
} sdi_sfp_supported_feature_t;

/**
 * @union sdi_media_supported_feature_t
 *  Media optional feature support status
 */
typedef union {
    sdi_qsfp_supported_feature_t qsfp_features;
    sdi_sfp_supported_feature_t sfp_features;
} sdi_media_supported_feature_t;

typedef struct {
    uint_t max_port_speed_mbps;
    uint_t port_density;
    int    max_port_power_mw;
    sdi_port_type_t port_type;
    uint_t sub_port_rank;
    uint_t sub_port_channel_offset;
} sdi_media_port_info_t;

typedef struct {
    uint_t module_density;
    int    eeprom_map_version;
    bool   software_controlled_power_mode;
    int    max_module_power_mw;
} sdi_media_module_info_t;

/**
 * @brief Get the presence status of a specific media
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[out] presence  - fill with "true" if module is present else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_presence_get (sdi_resource_hdl_t resource_hdl,
                                    bool *presence);

/**
 * @brief Get the required module monitoring (temperature and voltage alarm)
 * status of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] flags - flags for status that are of interest.Refer @ref sdi_media_status for valid flags.
 * @param[out] status - returns the set of status flags which are
 * asserted.Refer @ref sdi_media_status for valid flags.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_module_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                 uint_t flags, uint_t *status);

/**
 * @brief Get the required channel monitoring(rx_power and tx_bias alarm) status
 * of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] flags - flags for channel status,Refer @ref sdi_media_channel_monitoring_status
 * for the valid flags.
 * @param[out] status - returns the set of status flags which are
 * asserted.Refer @ref sdi_media_channel_monitoring_status for the valid flags.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_channel_monitor_status_get (sdi_resource_hdl_t resource_hdl,
                                                  uint_t channel, uint_t flags, uint_t *status);

/**
 * @brief Get the required channel status of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] flags - flags for channel status,Refer @ref sdi_media_channel_status for the valid flags.
 * @param[out] status - returns the set of status flags which are
 * asserted.Refer @ref sdi_media_channel_status for the valid flags.
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_channel_status_get (sdi_resource_hdl_t resource_hdl,
                                          uint_t channel, uint_t flags, uint_t *status);

/**
 * @brief Disable/Enable the transmitter of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] enable - "false" to disable and "true" to enable
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_tx_control (sdi_resource_hdl_t resource_hdl, uint_t channel, bool enable);

/**
 * @brief Get the transmitter status on a particular channel of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[out] status - "true" if transmitter enabled else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_tx_control_status_get (sdi_resource_hdl_t resource_hdl,
                                             uint_t channel, bool *status);

/**
 * @brief Disable/Enable the cdr of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] enable - "false" to disable and "true" to enable
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_cdr_status_set (sdi_resource_hdl_t resource_hdl, uint_t channel, bool enable);

/**
 * @brief Get the cdr status on a particular channel of the specific media.
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[out] status - "true" if cdr enabled else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_cdr_status_get (sdi_resource_hdl_t resource_hdl,
                                             uint_t channel, bool *status);

/**
 * @brief Get the maximum speed that can be supported by the media
 * @param[in] resource_hdl - handle of the media resource that is of interest.
 * @param[out] speed - max speed that can be supported by media
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_speed_get (sdi_resource_hdl_t resource_hdl, sdi_media_speed_t *speed);

/**
* @brief Reads the parameter value from eeprom
* @param[in] resource_hdl - handle of the media resource
* @param[in] param_type - parameter type. Refer @ref sdi_media_param_type_t for
*                         valid parameters
* @param[out] value - value of the parameter read from eeprom.
* @return - standard @ref t_std_error
*/
t_std_error sdi_media_parameter_get (sdi_resource_hdl_t resource_hdl,
                                     sdi_media_param_type_t param_type, uint_t *value);

/**
 * @brief Reads the requested vendor information from eeprom
 * @param[in] resource_hdl - handle of the media resource
 * @param[in] vendor_info_type - vendor information that is of interest. Refer
                                 @ref sdi_media_vendor_info_type_t for valid types
 * @param[out] vendor_info - vendor information which is read from eeprom.
 * @param[in] buf_size - size of the input buffer which is allocated by user(vendor_info)
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_vendor_info_get (sdi_resource_hdl_t resource_hdl,
                                       sdi_media_vendor_info_type_t vendor_info_type,
                                       char *vendor_info, size_t buf_size);

/**
 * @brief Get the transceiver code information from optic eeprom.  Transceiver
 * Compliance Code indicators define the electronic or optical interfaces that
 * are supported by the transceiver. For example, For Fibre Channel
 * transceivers, the Fibre Channel speed, transmission media, transmitter
 * technology, and distance capability shall all be indicated.
 * @param[in] resource_hdl      - handle of the media resource
 * @param[out] transceiver_info - buffer to store transceiver compliance code
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_transceiver_code_get (sdi_resource_hdl_t resource_hdl,
                                            sdi_media_transceiver_descr_t *transceiver_info);

/**
 * @brief Get the inforamtion of whether optional features supported or not on a
 * particular optic. For QSFP we can get the status of rate_select, tx_disable
 * and paging support status
 * @param resource_hdl[in] - Handle of the resource
 * @param feature_support[out] - feature support flags. Flag will be set to "true"
 * if feature is supported else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_feature_support_status_get (sdi_resource_hdl_t resource_hdl,
                                                  sdi_media_supported_feature_t *feature_support);

/**
 * @brief Get the threshold values for module monitors like temperature and
 * voltage
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] threshold_type - Should be one of the value @ref sdi_media_threshold_type_t
 * @param[out] value - threshold value
 * @return - standard @ref t_std_error
 * TODO: This is a Deprecated API and should be removed once upper layer adopted
 * to new API(sdi_media_threshold_get)
 */
t_std_error sdi_media_module_monitor_threshold_get (sdi_resource_hdl_t resource_hdl,
                                                    uint_t threshold_type, uint_t *value);

/**
 * @brief Get the threshold values for channel monitors like rx power and tx
 * bias
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] threshold_type - Should be one of the value @ref sdi_media_threshold_type_t
 * @param[out] value - threshold value
 * @return - standard @ref t_std_error
 * TODO: This is a Deprecated API and should be removed once upper layer adopted
 * to new API(sdi_media_threshold_get)
 */
t_std_error sdi_media_channel_monitor_threshold_get (sdi_resource_hdl_t resource_hdl,
                                                     uint_t threshold_type, uint_t *value);

/**
 * @brief Get alarm and warning threshold values for the given optic
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] threshold_type - Should be one of the value @ref sdi_media_threshold_type_t
 * @param[out] value - threshold value
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_threshold_get (sdi_resource_hdl_t resource_hdl,
                                     sdi_media_threshold_type_t threshold_type,
                                     float *value);

/**
 * @brief Enable/Disable the LP mode/Reset control on a specified media
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] ctrl_type - Should be one of the type @ref sdi_media_module_ctrl_type_t
 * @param[in] enable - "true" for enable and "false" for disable
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_module_control (sdi_resource_hdl_t resource_hdl,
                                      sdi_media_module_ctrl_type_t ctrl_type, bool enable);

/**
 * @brief Get the status of LP mode or Reset status of the specific media
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] ctrl_type - Control type that is of interest. Refer @ref sdi_media_module_ctrl_type_t
 *                        for valid control types
 * @param[out] status - "true" if requested control is enabled else "false"
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_module_control_status_get (sdi_resource_hdl_t resource_hdl,
                                                 sdi_media_module_ctrl_type_t ctrl_type,
                                                 bool *status);

/**
 * @brief Set the port LED based on the speed settings of the port.
 * @note this api should be used only on platforms where port LED is not
 * controlled by BCM(e.g. In S3000, sfpp port LEDs are controlled by CPLD)
 * @param resource_hdl[in] - Handle of the resource
 * @param channel[in] - Channel number. Should be 0, if only one channel is
 * present
 * @param speed[in] - LED mode setting is derived from speed
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_led_set (sdi_resource_hdl_t resource_hdl, uint_t channel, sdi_media_speed_t speed);

/**
 * @defgroup sdi_media_debug_api SDI media debug API.
 * media related debug APIs.
 * @ingroup sdi_sys
 * @{
 */

/**
 * @brief Debug api to retrieve module monitors assoicated with the specified media.
 * @param[in] resource_hdl - handle to media
 * @param[in] monitor - The monitor which needs to be retrieved.
 * @param[out] value - Value of the monitor
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_module_monitor_get (sdi_resource_hdl_t resource_hdl,
                                          sdi_media_module_monitor_t monitor, float *value);

/**
 * @brief Debug api to retrieve channel monitors assoicated with the specified media.
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] monitor - The monitor which needs to be retrieved.
 * @param[out] value - Value of the monitor
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_channel_monitor_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_channel_monitor_t monitor, float *value);


/* debug API for raw read from a specific offset */
/**
 * @brief Debug api to read data from media
 * @param[in] resource_hdl - handle to the media
 * @param[in] offset - offset from which to read
 * @param[out] data - pointer to the memory in which data_len number of bytes would be read
 * @param[in] data_len - length of the data to be read
 * @return - standard @ref t_std_error
 */

t_std_error sdi_media_read (sdi_resource_hdl_t resource_hdl, uint_t offset,
                            uint8_t *data, size_t data_len);
/**
 * @brief Debug api to write data in to media
 * @param[in] resource_hdl - handle to the media
 * @param[in] offset - offset from which to read
 * @param[in] data - input buffer which contains the data to be written
 * @param[in] data_len - length of the data to be written
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_write (sdi_resource_hdl_t resource_hdl, uint_t offset,
                             uint8_t *data, size_t data_len);

/*
 * @brief Raw read api for media eeprom
 * @param resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * @param data[in] - Data to read
 * @param data_len[in] - length of the data to read
 * @return return - standard @ref t_std_error
*/
t_std_error sdi_media_read_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len);

/**
 * @brief Raw write api for media eeprom
 * @param resource_hdl[in] - Handle of the resource
 * addr[in]          - pointer to struct that holds address, page and offset info
 * @param data[in] - Data to write
 * @param data_len[in] - length of the data to be written
 * @return return - standard @ref t_std_error
*/
t_std_error sdi_media_write_generic (sdi_resource_hdl_t resource_hdl, sdi_media_eeprom_addr_t* addr,
                           uint8_t *data, size_t data_len);


/**
 * @brief Api to enable/disable Autoneg on media PHY .
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] type - media type which is present in front panel port
 * @param[in] enable - true-enable Autonge,false-disable Autoneg.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_phy_autoneg_set (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_type_t type, bool enable);
/**
 * @brief Api to set mode on media PHY .
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] type - media type which is present in front panel port
 * @param[in] mode - SGMII/MII/GMII, Should be of type Refer @ref sdi_media_mode_t.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_phy_mode_set (sdi_resource_hdl_t resource_hdl,
                                    uint_t channel, sdi_media_type_t type,
                                    sdi_media_mode_t mode);
/**
 * @brief Api to  set speed  on media PHY .
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] type - media type which is present in front panel port
 * @param[in] speed - phy supported speed's. Should be of type @ref sdi_media_speed_t .
   @count[in] count - count for number of phy supported speed's 10/100/1000.
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_phy_speed_set (sdi_resource_hdl_t resource_hdl,
                                     uint_t channel, sdi_media_type_t type,
                                     sdi_media_speed_t *speed, uint_t count);

/**
 * @brief Api to get phy link status .
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] type - media type which is present in front panel port
 * @param[out] status - fill with "true" if link is up else "false"
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_phy_link_status_get (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                           sdi_media_type_t type, bool *status);

/**
 * @brief Api to control media phy and MAC interfaces power down enable/disable .
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] type - media type which is present in front panel port
 * @param[in] enable - true - power down media phy, false - power up media phy
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_phy_power_down_enable (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                             sdi_media_type_t type, bool enable);

/**
 * @brief Api to enable/disable Fiber/Serdes transmitter and receiver .
 * @param[in] resource_hdl - handle to media
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] type - media type which is present in front panel port
 * @param[in] enable - true - enable serdes, false - disable serdes
 * @return - standard @ref t_std_error
 *
 */
t_std_error sdi_media_phy_serdes_control (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                          sdi_media_type_t type, bool enable);

/*
 * @brief initialize pluged in module
 * @param[in] resource_hdl - handle to the front panel port
 * @pres[in]      - presence status
 * @return - standard @ref t_std_error
 */
t_std_error sdi_media_module_init (sdi_resource_hdl_t resource_hdl, bool pres);

/**
 * @brief Api to set the rate select on optics.
 * @param[in] resource_hdl - Handle of the resource
 * @param[in] channel - channel number that is of interest. channel numbers
 * should start with 0(e.g. For qsfp valid channel number range is 0 to 3) and
 * channel number should be '0' if only one channel is present
 * @param[in] rev -  Firmware revision
 * @param[in] cdr_enable -  true to enable CDR, false to disable CDR via rate select bits.
 * return      -  t_std_error
 */
t_std_error sdi_media_ext_rate_select (sdi_resource_hdl_t resource_hdl, uint_t channel,
                                       sdi_media_fw_rev_t rev, bool cdr_enable);

/*
 * @brief Set wavelength for tunable media
 * @param[in]  - resource_hdl - handle to the front panel port
 * @param[in]  - wavelength value
 */

t_std_error sdi_media_wavelength_set (sdi_resource_hdl_t resource_hdl, float value);

/**
 * @brief API to get QSA adapter type
 * resource_hdl[in] - Handle of the resource
 * sdi_qsa_adapter_type_t*[out] - adapter type obtained
 * return           - t_std_error
 */

t_std_error sdi_media_qsa_adapter_type_get (sdi_resource_hdl_t resource_hdl,
                                   sdi_qsa_adapter_type_t* qsa_adapter);


/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif   /* __SDI_MEDIA_H_ */
