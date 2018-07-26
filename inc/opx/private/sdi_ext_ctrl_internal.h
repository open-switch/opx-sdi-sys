/*
 * Copyright (c) 2018 Dell EMC..
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
 * filename: sdi_ext_ctrl_internal.h
 */



/**
 * @file sdi_ext_ctrl_internal.h
 * @brief Internal resource related declarations.for Ext Ctrl
 *
 */

#ifndef __SDI_EXT_CTRL_INTERNAL_H_
#define __SDI_EXT_CTRL_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_ext_ctrl.h"

/**
 * Each External resource provides the following callbacks
 * - init -  callback function for resource init.
 * - ext_ctrl_get - callback function for getting the value from the external control
 * - ext_ctrl_set - callback function for setting the threshold value
 *
 */
typedef struct {
    t_std_error (*init)(void *resource_hdl);
    /* Temperature Specific extensions start from here. */
    t_std_error (*ext_ctrl_get)(void *resource_hdl, int *val, int *size);
    t_std_error (*ext_ctrl_set)(void *resource_hdl, int *val, int size);
} ext_ctrl_t;

#endif
