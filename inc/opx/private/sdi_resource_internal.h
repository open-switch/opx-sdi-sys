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
 * filename: sdi_resource_internal.h
 */


/**
 * @file sdi_resource_internal.h
 * @brief  resource  API for use within the SDI framework and drivers.
 *
 * @defgroup sdi_internal_resource_api SDI Internal Resource API
 * @brief API to declare/define and manipulate resoruces in global name space of SDI.
 *
 * @ingroup sdi_internal
 * @brief API to manipulate resources within SDI.
 * Resource is the smallest element of an SDI.  A resource is defined as part of a device
 * and it is used to control/monitor different aspects of system.
 *
 * A resource has two names
 * - name as seen in global space
 * - name as seen within the scope of given entity.
 *
 * This file only deals with resource in global space.  Once the resources are defined/declared
 * by device as part of this api, it's handle can be referenced as part of entity.
 *
 * @{
 */

#ifndef __SDI_RESOURCE_INTERNAL_H_
#define __SDI_RESOURCE_INTERNAL_H_

#include "sdi_entity.h"
#include "sdi_sys_common.h"

/**
 * Every reource is identified by
 * - Name which is unique in the global space, and is always null terminated.
 * - type - That identifies that oeprations are supported.
 * - callback functions - refer to respecitve regsource api for the list of
 *   callback functions that must be supported.
 * - callback_hdl - this is the value that will be passed to callback-function
 *   when any operation on this resource is performed.
 *
 *   @note - Users must never directly manipulate the resource. Instead an appropiate api
 *   as defined in this mdoule be used.
 *
 */
struct sdi_resource {
    /**
     * name : Name of the resource which is unique in the global space, and is always
     * null terminated
     */
    char name[SDI_MAX_NAME_LEN];
    /**
     *type : idientifies the type of operations supported by resource
     */
    sdi_resource_type_t type;
    /**
     *callback_fns - Resource specific callback APIs
     */
    void *callback_fns;
    /**
     * callback_hdl - is the value that needs to be passes to callback function when any
     * operation performed on the resource
     */
    void *callback_hdl;
    /**
     * alias : Alias name for the resource, and is always null
     * ternimated.example, "BOOT_STATUS" led
     */
    char alias[SDI_MAX_NAME_LEN];
};


/**
 * @brief Initialize the resource manager.
 * This API must be called before any other API of this module is called.
 */
void sdi_resource_mgr_init(void);

/** An opaque handle to resource.
 * @note applications must never try to interpret the fields of resource dirctly
 */
typedef struct sdi_resource *sdi_resource_priv_hdl_t;

/** @brief Allocate memory for a resource object
 *  @param[out] hdl : if successful, the handle will be returned in this.
 *  @return STD_ERR_OK if scucessful, else appropoate error value.
 */
t_std_error sdi_resource_allocate(sdi_resource_hdl_t *hdl);

/** @brief free allocated  resource object
 * @param[in] hdl handle to the resoruce that needs to be freeed.
 * @return NONE
 */
void sdi_resource_free(sdi_resource_hdl_t hdl);

/**
 * @brief retrive the name of the resource.
 * @param[in] hdl handle of the resource whose name has to be found.
 * @return a null terminated string indicating the name of the resource.
 */
const char *sdi_resource_name_get(sdi_resource_hdl_t hdl);

/**
 * @brief retrive the alias name of the resource.
 * @param[in] hdl handle of the resource whose name has to be found.
 * @return a null terminated string indicating the name of the resource.
 */
const char *sdi_resource_alias_get(sdi_resource_hdl_t hdl);

/**
 * @brief retrive the type of the resource.
 * @param[in] hdl handle of the resource whose type has to be found.
 * @return type of resource
 */
sdi_resource_type_t sdi_internal_resource_type_get(sdi_resource_hdl_t hdl);

/**
 * @brief Add a resource to SDI
 * @param[in] type type of resource
 * @param[in] name null terminated name of the resource.
 *            Name(including null charachter) should not be greater than
 *            SDI_MAX_NAME_LEN
 * @param[in] callback_hdl the value to be passed the callback functions when
 *            manipulating this resource.
 * @param[in] callback_fns Set of callbacks.  The exact structure of the callback
 *            callback_fns is defined seperated by respective resource-type api.
 */
void sdi_resource_add(sdi_resource_type_t type, const char *name,
        void *callback_hdl, void *callback_fns);

/**
 * @brief Delete/remove a resource to SDI
 * @param[in] hdl handle to the resource that must be deleted.
 * @return standard error code.
 */
t_std_error sdi_resource_delete(sdi_resource_hdl_t hdl);

/**
 * @brief retrieve the handle of resource whose name is specified.
 * @param[in] name name of the entity whose handle has to be retrieved.
 * @returns [in] hdl handle to the resource that has been found. Else NULL
 */
sdi_resource_hdl_t sdi_find_resource_by_name(const char *name);

/**
 * @brief retrieve the handle of resource whose alias name is specified.
 * @param[in] alias alias-name of the entity whose handle has to be retrieved.
 * @returns [in] hdl handle to the resource that has been fournd. Else NULL
 */
sdi_resource_hdl_t sdi_find_resource_by_alias(const char *alias);

/**
 * @brief Initialize the resource.
 * @param[in] hdl - handle to the resource whose information has to be initialised.
 * @param[in] data - data to initialise
 * @return STD_ERR_OK on success and standard error on failure
 */
t_std_error sdi_resource_init(sdi_resource_hdl_t hdl, void *data);

/**
 * @}
 */

#endif   /* __SDI_RESOURCE_H_ */
