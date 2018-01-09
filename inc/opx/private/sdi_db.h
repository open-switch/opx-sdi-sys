/*
 * Copyright (c) 2016 Dell Inc.
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

/**
 * @file    sdi_db.h
 * @brief   Dell Networking SDI (System Device Interface) VM
 *          (Virtual Machine) database interface
 * @date    04-2015
 */

#ifndef _SDI_DB_H_
#define _SDI_DB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "std_error_codes.h"
#include <std_error_ids.h>
#include "sdi_entity.h"
#include "sdi_entity_info.h"
#include "sdi_media.h"
#include "db_sql_ops.h"

/**
 * @defgroup sdi_db SDI VM Database API
 * Open and close the access to the database
 * 
 * @{
 */

/**
 * @brief Open a connection to a database
 *
 * @param[out] db_handle Database handle to return
 * @param[in]  masterf   Boolean flag; true <=> Be DB master (i.e. init mutex)
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_open(db_sql_handle_t * db_handle, uint_t masterf);

/**
 * @brief Close a database connection
 *
 * @param[in]   db_handle   Handle to the database
 *
 * @return None
 */
void sdi_db_close(db_sql_handle_t db_handle);

/**
 * @brief Reinitialize the database with the initial data
 *
 * @return None
 */
void sdi_db_reinit_database(void);

/** Maximum length of a SQL buffer **/
#define SDI_DB_SQL_DEFAULT_BUFFER_LENGTH    128

/**
 * @}
 */

/**
 * @defgroup sdi_table SDI VM DB table identifiers
 * 
 * These identifiers should be in sync with the SQL create script
 *
 * @ingroup sdi_db
 * @{
 */

/** Thermal sensor table */
#define TABLE_THERMAL_SENSOR    "Thermal"
/** Fan table */
#define TABLE_FAN               "Fan"
/** LED table */
#define TABLE_LED_ON_OFF        "LED"
/** Digit Display LED */
#define TABLE_LED_DIGIT         "Digit_Display_LED"
/** Physical LED */
#define TABLE_LED_PHYSICAL      "Physical_LED"
/** Programmable logical devices */
#define TABLE_PLD               "PLD"
/** Entity information */
#define TABLE_INFO              "Entity_Info"
/** Media devices */
#define TABLE_MEDIA             "Media"
/** Global resource list */
#define TABLE_RESOURCES         "Entity_Resource"
/** Media channel table */
#define TABLE_MEDIA_CHANNEL     "Media_Channel"
/** Media parameters table */
#define TABLE_MEDIA_PARAMS      "Media_Parameters"
/** Media vendor info table */
#define TABLE_MEDIA_VENDOR_INFO "Media_Vendor_Info"
/** Media module/channel monitor thresholds table */
#define TABLE_MEDIA_THRESHOLD   "Media_Monitor_Thresholds"
/** NVRAM table */
#define TABLE_NVRAM             "NVRAM"

/**
 * @}
 */

/** @defgroup sdi_table_common SDI VM DB common fields
 *
 * @ingroup sdi_table
 * @{
 */

/** Entity Handle */
#define TBL_ENTITY_HDL          "Entity_Handle"
/** Resource Handle */
#define TBL_RESOURCE_HDL        "Resource_Handle"
/** Entity Type */
#define TBL_ENTITY_TYPE         "Entity_Type"
/** Entity Instance */
#define TBL_ENTITY_INSTANCE     "Instance"
/** Resource Type */
#define TBL_RESOURCE_TYPE       "Resource_Type"
/** Resource Instance */
#define TBL_RESOURCE_INSTANCE   "Instance"
/** Resource Alias */
#define TBL_RESOURCE_ALIAS      "Alias"

/**
 * @}
 */

/** @defgroup sdi_table_led SDI VM DB LED table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** LED State */
#define LED_STATE               "LED_On"
/** Logical to Physical mapping */
#define LED_PHYSICAL            "Phy_LED"
/** Physical LED State when turned on */
#define LED_PHY_STATE_ON        "LED_State_On"
/** Physical LED State when turned off */
#define LED_PHY_STATE_OFF       "LED_State_Off"
/** Physical LED State */
#define LED_PHY_STATE           "LED_State"
/** LED supports being turned off */
#define LED_SUPPORTS_OFF        "Supports_Off"
/** Maximum size of the LED state field */
#define LED_STATE_SZ            64
/** Digit display LED String */
#define DIGIT_LED_DISPLAY       "Display_String"
/** Digit display LED State */
#define DIGIT_LED_STATE         "Display_On"

/**
 * @}
 */

/** @defgroup sdi_table_thermal SDI VM DB Thermal sensor table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** Temperature value */
#define THERMAL_TEMPERATURE         "Temp_C"
/** Low Threshold */
#define THERMAL_THRESHOLD_LOW       "Threshold_Low"
/** High Threshold */
#define THERMAL_THRESHOLD_HIGH      "Threshold_High"
/** Critical Threshold */
#define THERMAL_THRESHOLD_CRITICAL  "Threshold_Critical"
/** Thermal sensor fault status */
#define THERMAL_FAULT               "Alert_On"

/**
 * @}
 */

/** @defgroup sdi_table_fan SDI VM DB Fan table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** Fan speed */
#define FAN_SPEED   "Speed"
/** Fan Fault */
#define FAN_FAULT   "Alert_On"

/**
 * @}
 */

/** @defgroup sdi_table_info SDI VM DB Entity info table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** Entity Name */
#define INFO_ENTITY_NAME    "Entity_Name"
/** Entity reset status */
#define INFO_ENTITY_RESET   "Entity_Reset"
/** Entity power enabled */
#define INFO_ENTITY_POWER   "Entity_Power"
/** Entity Info Product name */
#define INFO_PRODUCT        "Product_Name"
/** Entity Info PPID */
#define INFO_PPID           "PPID"
/** Entity Info HW Revision */
#define INFO_HW_REV         "HW_Revision"
/** Entity Info Platform name */
#define INFO_PLATFORM       "Platform_Name"
/** Entity Info Vendor name */
#define INFO_VENDOR         "Vendor_Name"
/** Entity Info Service Tag */
#define INFO_SERVICE_TAG    "Service_Tag"
/** Entity Info Base MAC */
#define INFO_BASE_MAC       "Base_MAC"
/** Entity Info Num MACs */
#define INFO_NUM_MACS       "MAC_Size"
/** Entity Info num fans */
#define INFO_NUM_FANS       "Num_Fans"
/** Entity info fan max speed */
#define INFO_FAN_MAX_SPEED  "Max_Speed"
/** Entity Info Airflow type */
#define INFO_FAN_AIRFLOW    "Air_Flow_Type"
/** Entity Info Power rating */
#define INFO_POWER_RATING   "Power_Rating"
/** Entity Info Power type */
#define INFO_POWER_TYPE     "Power_Type"
/** Entity Info Presence flag */
#define INFO_PRESENCE       "Presence"
/** Entity Info Fault flag */
#define INFO_FAULT          "Fault_Status"

/**
 * @}
 */

/** @defgroup sdi_pld SDI VM PLD table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** PLD Version */
#define PLD_VERSION         "Firmware_Version"

/**
 * @}
 */

/** @defgroup sdi_nvram SDI VM NVRAM table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** NVRAM Size */
#define NVRAM_SIZE          "Data_Size"
/** NVRAM Content */
#define NVRAM_DATA_FILE     "Data_File"
/** Offset in file of NVRAM data */
#define NVRAM_OFFSET        "Offset"

/**
 * @}
 */

/** @defgroup sdi_table_media SDI VM DB Media table fields
 *
 * @ingroup sdi_table
 * @{
 */

/** Media presence */
#define MEDIA_PRESENCE          "Present"
/** Media type */
#define MEDIA_TYPE              "Media_Type"
/** Optic speed */
#define MEDIA_OPTIC_SPEED       "Optic_Speed"
/** DELL Qualified */
#define MEDIA_DELL_QUALIFIED    "DELL_Qualified"
/** DELL Product Info */
#define MEDIA_DELL_PROD_INFO    "DELL_Product_Info"
/** Media Supported Features */
#define MEDIA_SUPPORTED_FEATURES "Media_Supported_Features"
/** Media low power mode */
#define MEDIA_LP_MODE           "Low_Power_Mode"
/** Media reset */
#define MEDIA_RESET             "Reset"
/** Media transceiver code */
#define MEDIA_TRANSCEIVER_CODE  "XCVR_Code"

/** Media OUI */
#define MEDIA_VENDOR_OUI        "Vendor_OUI"
/** Media OUI length */
#define MEDIA_VENDOR_OUI_LEN    (3)

/** Media Channel */
#define MEDIA_CHANNEL           "Channel"
/** Indicator for no media channel */
#define MEDIA_NO_CHANNEL        (0xFFFF)
/** Default channel */
#define MEDIA_DEFAULT_CHANNEL   (0)
/** Media Channel Status */
#define MEDIA_CHANNEL_STATUS    "Channel_Status"
/** Media monitor status */
#define MEDIA_MONITOR_STATUS    "Monitor_Status"
/** TX Enable */
#define MEDIA_TX_ENABLE         "TX_Enable"
/** Temperature monitor */
#define MEDIA_MONITOR_TEMP      "Temp_or_RX_Power"
/** Voltage monitor */
#define MEDIA_MONITOR_VOLT      "Volt_or_TX_Bias"

/* The module monitor and channel monitors share the same columns */
/** RX Power monitor */
#define MEDIA_MONITOR_RX_POWER  MEDIA_MONITOR_TEMP
/** TX Power Bias */
#define MEDIA_MONITOR_TX_BIAS   MEDIA_MONITOR_VOLT

/** Media parameter type */
#define MEDIA_PARAM_TYPE        "Parameter"
/** Media parameter value */
#define MEDIA_PARAM_VALUE       "Value"

/** Media vendor info type */
#define MEDIA_VENDOR_INFO_TYPE  "Vendor_Info"
/** Media vendor info value */
#define MEDIA_VENDOR_INFO_VALUE "Value"

/** Media module/channel threshold type */
#define MEDIA_THRESHOLD_TYPE    "Threshold"
/** Media module/channel threshold value */
#define MEDIA_THRESHOLD_VALUE   "Value"

/** Target field - Writes always go to this column, and may go to the
 * normal column depending on the environment variable setting
 */
#define TARGET_PREFIX           "Target_"

/**
 * @}
 */

/**
 * @brief Construct a path to a SDI DB file
 *
 * @param[out]  buffer  Buffer in which to write the full path. Buffer must be
 *                      NAME_MAX long.
 * @param[in]   file    Name of the file to get the path to
 *
 * @return None
 */
void sdi_db_construct_path(char *buffer, const char *file);

/**
 * @defgroup sdi_entity_resource SDI Entity/Resource API
 * Retrieve the handles for entity and/or resource
 *
 * @ingroup sdi_db
 * @{
 */

/**
 * @brief Retrieve Entity Handle
 *
 * This is a unique value representing an entity in the system
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   entity_type     Type of the entity
 * @param[in]   entity_instance Instance of the entity (1-based)
 * @param[out]  entity_handle   Pointer to a location to save the handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_entity_handle(db_sql_handle_t db_handle,
                                     sdi_entity_type_t entity_type,
                                     uint_t entity_instance,
                                     sdi_entity_hdl_t *entity_handle);

/**
 * @brief Retrieve entity count
 *
 * Number of entities of the specified type
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   entity_type     Type of the entity
 * @param[out]  count           Pointer to a location to save the count
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_entity_count(db_sql_handle_t db_handle,
                                    sdi_entity_type_t entity_type,
                                    uint_t *count);

/**
 * @brief Get first entity
 *
 * Get first entity in the list of all entities
 *
 * @param[in]   db_handle       Handle to the database
 * @param[out]  entity_handle   Pointer to a location to save the entity handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_first_entity(db_sql_handle_t db_handle,
                                    sdi_entity_hdl_t *entity_handle);

/**
 * @brief Get next entity
 *
 * Get the next entity in the list of all entities
 *
 * @param[in]       db_handle       Handle to the database
 * @param[in,out]   entity_handle   Pointer to a location to save the entity handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_next_entity(db_sql_handle_t db_handle,
                                   sdi_entity_hdl_t *entity_handle);

/**
 * @brief Get first resource
 *
 * Get first resource of the given type for the parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Parent entity handle
 * @param[in]   resource_type       Resource type to lookup
 * @param[out]  resource_handle     Pointer to a location to save the resource handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_first_resource(db_sql_handle_t db_handle,
                                      sdi_entity_hdl_t entity_handle,
                                      sdi_resource_type_t resource_type,
                                      sdi_resource_hdl_t *resource_handle);

/**
 * @brief Get next resource
 *
 * Get the next resource of the given type from the given resource
 *
 * @param[in]       db_handle           Handle to the database
 * @param[in]       entity_handle       Parent entity_handle
 * @param[in]       resource_type       Resource type to lookup
 * @param[in,out]   resource_handle     Pointer to a location to save the resource handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_next_resource(db_sql_handle_t db_handle,
                                     sdi_entity_hdl_t entity_handle,
                                     sdi_resource_type_t resource_type,
                                     sdi_resource_hdl_t *resource_handle);

/**
 * @brief Retrieve Resource count
 *
 * Number of resources of the given type within a parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the parent entity
 * @param[in]   resource_type       Type of the resource
 * @param[out]  count               Pointer to a location to save the count
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_resource_count(db_sql_handle_t db_handle,
                                      sdi_entity_hdl_t entity_handle,
                                      sdi_resource_type_t resource_type,
                                      uint_t *count);

/**
 * @brief Retrieve Resource Handle by instance
 *
 * This is a unique value representing a resource of the given type within a
 * parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the parent entity
 * @param[in]   resource_type       Type of the resource
 * @param[in]   resource_instance   Instance of the resource (1-based)
 * @param[out]  resource_handle     Pointer to a location to save the handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_resource_handle(db_sql_handle_t db_handle,
                                       sdi_entity_hdl_t entity_handle,
                                       sdi_resource_type_t resource_type,
                                       uint_t resource_instance,
                                       sdi_resource_hdl_t *resource_handle);


/**
 * @brief Retrieve Resource Handle by alias
 *
 * This is a unique value representing a resource of the given type within a
 * parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the parent entity
 * @param[in]   resource_type       Type of the resource
 * @param[in]   alias               Pointer to a string for the alias
 * @param[out]  resource_handle     Pointer to a location to save the handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_resource_handle_by_alias(db_sql_handle_t db_handle,
                                                sdi_entity_hdl_t entity_handle,
                                                sdi_resource_type_t resource_type,
                                                const char *alias,
                                                sdi_resource_hdl_t *resource_handle);

/**
 * @brief Retrieve the entity presence
 *
 * Flag indicating whether the entity is present or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[out]  present             Pointer to a location to save the presence flag
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_presence_get(db_sql_handle_t db_handle,
                                       sdi_entity_hdl_t entity_handle,
                                       bool *present);

/**
 * @brief Set the entity presence
 *
 * Flag indicating whether the entity is present or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[in]   present             Presence flag value to set
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_presence_set(db_sql_handle_t db_handle,
                                       sdi_entity_hdl_t entity_handle,
                                       bool present);

/**
 * @brief Retrieve the entity fault status
 *
 * Flag indicating whether the entity is faulty or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[out]  fault_status        Pointer to a location to save the fault status flag
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_fault_status_get(db_sql_handle_t db_handle,
                                           sdi_entity_hdl_t entity_handle,
                                           bool *fault_status);

/**
 * @brief Set the entity fault status
 *
 * Flag indicating whether the entity is faulty or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[in]   fault_status        Fault status flag value to set
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_fault_status_set(db_sql_handle_t db_handle,
                                           sdi_entity_hdl_t entity_handle,
                                           bool fault_status);

/**
 * @brief Get the parent entity handle for a resource
 *
 * For a given resource and table, retrieve the handle of the parent entity.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   resource_handle     Handle to the resource
 * @param[in]   resource_type       Type of the resource
 * @param[out]  entity_handle       Handle to parent entity
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_resource_get_parent_entity(db_sql_handle_t db_handle,
                                              sdi_resource_hdl_t resource_handle,
                                              sdi_resource_type_t resource_type,
                                              sdi_entity_hdl_t *entity_handle);

/**
 * @brief Get associated info handle for a resource
 * 
 * For any given resource, retrieve the resource handle of its associated info
 * structure.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   resource_handle     Handle to the resource
 * @param[in]   resource_type       Type of the resource
 * @param[out]  info_handle         Handle to associated info
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_resource_get_associated_info(db_sql_handle_t db_handle,
                                                sdi_resource_hdl_t resource_handle,
                                                sdi_resource_type_t resource_type,
                                                sdi_resource_hdl_t *info_handle);

/**
 * @}
 */

/**
 * @defgroup sdi_db_fields Access fields in the database
 *
 * @ingroup sdi_db
 * @{
 */

/**
 * @brief Retrieve an integer variable from the database, given the resource
 * handle, table and field. Booleans may also be retrieved using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int_field_get(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 int *value);

/**
 * @brief Retrieve a 64-bit integer variable from the database, given the resource
 * handle, table and field. Booleans may also be retrieved using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int64_field_get(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t res_handle,
                                   const char *table,
                                   const char *field,
                                   int64_t *value);

/**
 * @brief Set an integer variable into the database, given the resource
 * handle, table and field. Booleans may also be set using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int_field_set(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 int *value);

/**
 * @brief Set a 64-bit integer variable into the database, given the resource
 * handle, table and field. Booleans may also be set using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int64_field_set(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t res_handle,
                                   const char *table,
                                   const char *field,
                                   int64_t *value);

/**
 * @brief Set an integer variable into the database, given the resource
 * handle, table and field. Booleans may also be set using this method.
 * This function will set the target field, and may set the regular field
 * as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int_field_target_set(db_sql_handle_t db_handle,
                                        sdi_resource_hdl_t res_handle,
                                        const char *table,
                                        const char *field,
                                        int *value);

/**
 * @brief Retrieve a string variable from the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_str_field_get(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 char *value);
/**
 * @brief Set a string variable into the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_str_field_set(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 const char *value);

/**
 * @brief Set a string variable into the database, given the resource
 * handle, table and field.  This function will set the target field, and may
 * set the regular field as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_str_field_target_set(db_sql_handle_t db_handle,
                                        sdi_resource_hdl_t res_handle,
                                        const char *table,
                                        const char *field,
                                        const char *value);

/**
 * @brief Retrieve a binary variable from the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 * @param[in,out]   len         Maximum length of the buffer and actual length
 *                              saved in this location
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_bin_field_get(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 uint8_t *value,
                                 uint_t *len);

/**
 * @brief Set a binary variable into the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 * @param[in]   len             Length of the buffer
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_bin_field_set(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 const uint8_t *value,
                                 uint_t len);

/**
 * @brief Set a binary variable into the database, given the resource
 * handle, table and field.  This function will set the target field, and may
 * set the regular field as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 * @param[in]   len             Length of the buffer
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_bin_field_target_set(db_sql_handle_t db_handle,
                                        sdi_resource_hdl_t res_handle,
                                        const char *table,
                                        const char *field,
                                        const uint8_t *value,
                                        uint_t len);

/**
 * @}
 */

/**
 * @defgroup sdi_media_channel  Access media channel attributes
 *
 * @ingroup sdi_db
 * @{
 */

/**
 * @brief Retrieve an integer value from the media channel table, given the
 * resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_int_field_get(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               const char *field,
                                               int *value);

/**
 * @brief Set an integer value in the media channel table, given the
 * resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Value to save
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_int_field_set(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               const char *field,
                                               int value);

/**
 * @brief Set an integer value in the media channel table, given the
 * resource handle, channel and field.  This function will set the target
 * field, and may set the regular field as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Value to save
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_int_field_target_set(db_sql_handle_t db_handle,
                                                      sdi_resource_hdl_t media_hdl,
                                                      int channel,
                                                      const char *field,
                                                      int value);

/**
 * @brief Retrieve a floating point value from the media channel table, given
 * the resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_float_field_get(db_sql_handle_t db_handle,
                                                 sdi_resource_hdl_t media_hdl,
                                                 int channel,
                                                 const char *field,
                                                 float *value);

/**
 * @brief Set a floating point value in the media channel table, given the
 * resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Value to save
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_float_field_set(db_sql_handle_t db_handle,
                                                 sdi_resource_hdl_t media_hdl,
                                                 int channel,
                                                 const char *field,
                                                 float value);

/**
 * @}
 */

/**
 * @defgroup sdi_media_param    Media parameters
 *
 * @ingroup sdi_db
 * @{
 */

/**
 * @brief Retrieve the media parameter value, given the resource handle and
 * parameter type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   param_type      Type of parameter
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_param_get(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t media_hdl,
                                   sdi_media_param_type_t param_type,
                                   uint_t *value);

/**
 * @brief Set the media parameter value, given the resource handle and
 * parameter type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   param_type      Type of parameter
 * @param[in]   value           Value to save in the DB
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_param_set(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t media_hdl,
                                   sdi_media_param_type_t param_type,
                                   uint_t value);

/**
 * @brief Retrieve the media vendor info value, given the resource handle and
 * vendor info type. Vendor OUI cannot be retrieved using this function and
 * must be retrieved using \ref sdi_db_bin_field_get
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   info_type       Type of vendor info
 * @param[in]   length          Length of buffer
 * @param[out]  value           Pointer to a buffer to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_vendor_info_get(db_sql_handle_t db_handle,
                                         sdi_resource_hdl_t media_hdl,
                                         sdi_media_vendor_info_type_t info_type,
                                         uint_t length,
                                         char *value);

/**
 * @brief Set the media vendor info value, given the resource handle and
 * vendor info type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   info_type       Type of vendor info
 * @param[in]   len             Length of buffer
 * @param[in]   value           Value to save in the DB
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_vendor_info_set(db_sql_handle_t db_handle,
                                         sdi_resource_hdl_t media_hdl,
                                         sdi_media_vendor_info_type_t param_type,
                                         uint_t len,
                                         char *value);

/**
 * @brief Retrieve the media monitor threshold value, given the resource handle,
 * channel and monitor type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel
 * @param[in]   type            Type of threshold
 * @param[out]  value           Pointer to a buffer to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_monitor_threshold_get(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               uint_t type,
                                               uint_t *value);

/**
 * @brief Set the media monitor threshold value, given the resource handle,
 * channel and monitor type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel
 * @param[in]   type            Type of threshold
 * @param[in]   value           Threshold value to save
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_monitor_threshold_set(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               uint_t type,
                                               uint_t value);

/**
 * @brief Retrieve the media threshold value, given the resource handle,
 * and threshold type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   type            Type of threshold
 * @param[out]  value           Pointer to a buffer to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_threshold_get(db_sql_handle_t db_handle,
                                       sdi_resource_hdl_t media_hdl,
                                       sdi_media_threshold_type_t type,
                                       float *value);

/**
 * @brief Set the media monitor threshold value, given the resource handle,
 * and threshold type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   type            Type of threshold
 * @param[in]   value           Threshold value to save
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_threshold_set(db_sql_handle_t db_handle,
                                       sdi_resource_hdl_t media_hdl,
                                       sdi_media_threshold_type_t type,
                                       float value);

/**
 * @brief Get the entity_type given the resource handle.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_hdl         Handle to the resource
 * @param[out]  entity_type     Type of the parent entity
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_entity_type(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t res_hdl,
                                   sdi_entity_type_t *entity_type);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* _SDI_DB_H_ */
