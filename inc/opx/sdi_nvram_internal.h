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
 * filename: sdi_nvram_internal.h
 */



/**
 * @file sdi_nvram_internal.h
 * @brief Internal NVRAM resource related declarations.
 *
 */

#ifndef __SDI_NVRAM_INTERNAL_H_
#define __SDI_NVRAM_INTERNAL_H_

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"

/**
 * Each NVRAM resource provides the following callbacks.so the following api have
 * to be provided by the driver when registering an NVRAM resource with the sdi
 * framework
 */
typedef struct {
    /**
     * callback function for resource init
     */
    t_std_error (*init)(void *resource_hdl);
    /**
     * callback function to get NVRAM size
     */
    t_std_error (*size)(void *resource_hdl, uint_t *size);
    /**
     * callback function for NVRAM read
     */
    t_std_error (*read)(void *resource_hdl, uint8_t *buf, uint_t ofs, uint_t len);
    /**
     * callback function for NVRAM write
     */
    t_std_error (*write)(void *resource_hdl, uint8_t *buf, uint_t ofs, uint_t len);
} nvram_t;

#endif /* !defined(__SDI_NVRAM_INTERNAL_H_) */
