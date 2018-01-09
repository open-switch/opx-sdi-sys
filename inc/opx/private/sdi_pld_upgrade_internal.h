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
 * filename: sdi_pld_upgrade_internal.h
 */



/**
 * @file sdi_pld_upgrade_internal.h
 * @brief Internal PLD resource related declarations.
 * This file will be used by the PLD upgrade drivers
 *
 */

#ifndef __SDI_PLD_UPGRADE_INTERNAL_H_
#define __SDI_PLD_UPGRADE_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include "sdi_pld_upgrade.h"

/**
 * Each PLD upgrade resource provides the following callbacks.so the following api have
 * to be provided by the driver when registering a PLD upgrade resource with the sdi
 * framework
 */
typedef struct {
    /**
     * callback function for resource init
     */
    t_std_error (*init)(void *resource_hdl);
    /**
     * callback function for getting the revision
     */
    t_std_error (*revision_get)(void *resource_hdl, char *version,size_t buf_len);
    /**
     * callback function to do the upgrade
     */
    t_std_error (*upgrade)(void *resource_hdl, size_t len, void* fw_data);
} upgrade_pld_t;

#endif //__SDI_PLD_UPGRADE_INTERNAL_H_
