/*
 * Copyright (c) 2016 Dell Inc.
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
 * filename: sdi_driver_internal.h
 */


/**
 * @file sdi_driver_internal.h
 * @brief  Driver  API for use within the SDI framework.
 *
 * @defgroup sdi_internal_driver_api SDI Internal Driver API
 * @brief API to manage drivers
 *
 * @ingroup sdi_internal
 * A driver is associated with each chip/board.  A driver can either be a
 * single function device, or it could also be composed of multiple
 * chips/functionality.
 * Irrespcitve of how the device is constructured, a driver in SDI framework
 * provides the following
 * - A method to initialize the device(and implicitly andy subcomponents)
 * - method to adapt how device has to be configured. Example, a pin may be
 *   configured as a GPIO or LED, and device has to be told what mode it should be in.
 * - Registers/advertises the differnet type of resources it has  to the SDI framwork.
 *
 * @{
 */

#ifndef __SDI_DRIVER_INTERNAL_H_
#define __SDI_DRIVER_INTERNAL_H_

#include "std_config_node.h"
#include "std_error_codes.h"
#include "sdi_bus.h"

typedef std_config_node_t sdi_driver_cfg;

typedef uint16_t i2c_addr_t;

/**
 * @typedef sdi_i2c_addr_t
 * SDI I2C Device Address
 */
typedef struct sdi_i2c_addr {
    i2c_addr_t i2c_addr;
    bool addr_mode_16bit;
}sdi_i2c_addr_t;

typedef union sdi_device_addr {
    sdi_i2c_addr_t i2c_addr;
    /**
     * Add other device address types when it is required
     */
}sdi_device_addr_t;

/**
 * @typedef sdi_bus_list_t
 * SDI Bus List
 */
typedef struct sdi_bus_list {
    /**
     * @brief head of the SDI Bus dynamic linked list
     */
    std_dll_head head;
    /**
     * @brief Lock for SDI Bus dynamic linked list
      */
    std_mutex_type_t lock;
} sdi_bus_list_t;

typedef struct sdi_device_entry {
    /**
     *Driver callbacks for the device
     */
    const struct sdi_driver *callbacks;
    /**
      *Instance of the device
      */
    uint_t instance;
    /**
      *bus handler for the device which resides on the bus
      */
    void *bus_hdl;
    /**
      *device address
      */
    sdi_device_addr_t addr;
    /**
      *name of the device by which its known on this platform eg : inlet temperature
      */
    char alias[SDI_MAX_NAME_LEN];
    /**
      * list of buses exported by this device (For ex: i2cmux)
      */
    sdi_bus_list_t bus_list;
    /*Device specific private data */
    void *private_data;

    /**
     * @brief callback to read from specific device offset
     * @param[in] hdl Handle to the device on from which data has to be read
     * @param[in] offset Offset within the device from which data has to be read
     * @param[out] data The data stream read from the device would be returned
     *                  in this.It should be pre-allocated by caller to hold len bytes.
     * @param[in] len Length of data to be read
     * @param[in] flags Miscellaneous flags based on device class. For current implementation this is ignored.
     *                  Examples of potential usage includes specifying the "sub-device-id" in case of PMBus devices,
     *                  CONFIG_MEM vs IO_MEM region when reading from PCI devices etc.
     * @return Returns error code encoded in standard t_std_error format.
     */
    t_std_error (*sdi_device_read_fn)(const struct sdi_device_entry *hdl, uint_t offset, uint8_t *data, uint_t len, uint flags);
    /**
     * @brief callback to write data at specified offset of the device specified
     * @param[in] hdl Handle to the device to which data has to be written
     * @param[in] offset Offset within the device at which data has to be written
     * @param[in] data The data stream to be written.
     * @param[in] len Length of data to be written
     * @param[in] flags Miscellaneous flags based on device class. For current implementation this is ignored.
     *                  Examples of potential usage includes specifying the "sub-device-id" in case of PMBus devices,
     *                  CONFIG_MEM vs IO_MEM region when writing from PCI devices etc.
     * @return Returns error code encoded in standard t_std_error format.
     */
    t_std_error (*sdi_device_write_fn)(const struct sdi_device_entry *hdl, uint_t offset, uint8_t *data, uint_t len, uint flags);

} sdi_device_entry_t ;

typedef sdi_device_entry_t *sdi_device_hdl_t;


/**
 * @typedef sdi_driver_t
 * Each driver is identified by it's type and list of resources that are exported by this
 * driver
 * In order for a driver to be recognized by SDI, it should create a variable
 * of type \typedef sdi_driver_t
 * The variable name \b must be \<driver_name_lower_case\>_driver.
 * Example, a TMP75 chip would declare itself as follows
 * @code
 * sdi_drver_t tmp75_driver = {
 *     .register_fn = tmp75_register,
 *     .init = tmp75_init
 * };
 * @endcode
 *
 */
typedef struct sdi_driver {
    /** @brief parse and store the configuration data in internal structures
     *
     * register_fn callback should
     *  - parse \p cfg and create a data-structure
     *  - associate it with the specicified bus
     *  - if successfull, the driver returns an "device_handle" created for this device
     *
     *  @note this function \b must \b not do apply any configruation to the device
     *  but only initialize the data structures and register with SDI framework.
     */
    t_std_error (*register_fn)(std_config_node_t node, void *bus_hdl, sdi_device_hdl_t *device_hdl);

    /**
     * @brief initialize the specific device.
     * This callback function must initialize specified instance of the device.
     * It should \b do so by applying the configuration that was stored as part
     * \p cfg in register_fn
     */
    t_std_error (*init)(sdi_device_hdl_t device_hdl);
} sdi_driver_t;

/**
 * Every driver must export a function by name
 * \<driver_name\>_entry of type sdi_device_entry_callback
 * The api would return it's set of callbacks in sdi_device_hdl_t
 */
typedef const sdi_driver_t * (*sdi_device_entry_callback)(void);

/**
 * Register the specified device.
 *
 * @param[in] *node - pointer to the node configuration data.
 * @param[in] *bus_hdl - Handle to the bus service provider.
 * @param[out] *dev_hdl - Handle to registered device is filled in *dev_hdl if
 * dev_hdl is not NULL. Buses not interested in tracking device handle can pass
 * NULL.
 * @returns - Error if node or bus is not valid
 */
t_std_error sdi_register_driver(std_config_node_t node, sdi_bus_hdl_t bus_hdl,
    sdi_device_hdl_t *dev_hdl);

/**
 * @brief Get Bus Driver's Symbol Address by looking-up sdi device driver
 * library based on bus driver name
 * @param[in] bus_driver_name - Name to look-up in sdi device driver library
 * @return symbol's address on success, NULL on failure
 */
sdi_bus_driver_t *sdi_get_bus_driver(const char *bus_driver_name);

/**
 * @brief initialize the device
 *
 * @param[in] device - structure of type \typedef sdi_device_hdl_t
 *
 * @return STD_ERR_OK on success and Standard error codes on failure.
 */
t_std_error sdi_device_init(sdi_device_hdl_t device);

/**
 * @brief Initializes the internal data structures for the driver and created device-db
 * and resource-db.
 *
 * @param[in] driver_cfg_file  - device config file
 *
 */
void sdi_register_drivers(const char * driver_cfg_file);


/**
 * @brief Read from specific device offset
 * @param[in] hdl Handle to the device on from which data has to be read
 * @param[in] offset Offset within the device from which data has to be read
 * @param[out] data The data stream read from the device would be returned
 *                  in this.It should be pre-allocated by caller to hold len bytes.
 * @param[in] len Length of data to be read
 * @param[in] flags Miscellaneous flags based on device class. For current implementation this is ignored.
 *                  Examples of potential usage includes specifying the "sub-device-id" in case of PMBus devices,
 *                  CONFIG_MEM vs IO_MEM region when reading from PCI devices etc.
 * @return Returns error code encoded in standard t_std_error format.
 */
static inline t_std_error sdi_device_read(const struct sdi_device_entry *hdl, uint_t offset, uint8_t *data, uint_t len, uint flags)
{
    if ((hdl == NULL) || (data == NULL))
    {
        return SDI_ERRCODE(EINVAL);
    }

    if (hdl->sdi_device_read_fn == NULL)
    {
        return SDI_ERRCODE(ENOTSUP);
    }
    return (hdl->sdi_device_read_fn)(hdl, offset, data, len, flags);
}

/**
 * @brief Write data at specified offset of the device specified
 * @param[in] hdl Handle to the device to which data has to be written
 * @param[in] offset Offset within the device at which data has to be written
 * @param[in] data The data stream to be written.
 * @param[in] len Length of data to be written
 * @param[in] flags Miscellaneous flags based on device class. For current implementation this is ignored.
 *                  Examples of potential usage includes specifying the "sub-device-id" in case of PMBus devices,
 *                  CONFIG_MEM vs IO_MEM region when writing from PCI devices etc.
 * @return Returns error code encoded in standard t_std_error format.
 */
static inline t_std_error sdi_device_write(const struct sdi_device_entry *hdl, uint_t offset, uint8_t *data, uint_t len, uint flags)
{
    if ((hdl == NULL) || (data == NULL))
    {
        return SDI_ERRCODE(EINVAL);
    }

    if (hdl->sdi_device_write_fn == NULL)
    {
        return SDI_ERRCODE(ENOTSUP);
    }
    return (hdl->sdi_device_write_fn)(hdl, offset, data, len, flags);
}


/**
* @}
*/

#endif /*__SDI_DRIVER_INTERNAL_H_*/
