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
 * filename: sdi_entity_info_internal.h
 */



/**
 * @file sdi_entity_info_internal.h
 * @brief Entity Info resource related declarations.
 * - init -  callback function for resource init.
 * - entity_info_data_get - callback function for retrieving the entity info content.
 */

#ifndef __SDI_ENTITY_INFO_INTERNAL_H_
#define __SDI_ENTITY_INFO_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity_info.h"

/**
 * Each ENTITY INFO resource provides the following callbacks
 * - init -  callback function for resource init.
 * - entity_info_data_get - callback function for retrieving the entity info content.
 */
typedef struct {

    /**
     * callback function for resource init
     */
    t_std_error (*init)(void *resource_hdl);
    /**
     * callback function for getting the entity info content
     */
    t_std_error (*entity_info_data_get)(void *resource_hdl,
                                        sdi_entity_info_t *entity_info);
} entity_info_t;

#endif //__SDI_ENTITY_INFO_INTERNAL_H_
