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
 * filename: sdi_nvram.h
 */

/**
 * @file sdi_nvram.h
 * @brief Public APIs for reading and writing NVRAM.
 */

#ifndef __SDI_NVRAM_H__
#define __SDI_NVRAM_H__

#include "std_error_codes.h"
#include "std_type_defs.h"
#include "sdi_entity.h"
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sdi_nvram_api SDI NVRAM API.
 * All NVRAM-related API take an argument
 * of type sdi_resource_hdl_t.  Application should first identify the
 * right sdi_resource_hdl_t by using @ref sdi_entity_resource_lookup
 *
 * @ingroup sdi_sys
 * @{
 */

/**
 * @brief Return size (bytes) of NVRAM
 * @param[in] resource_hdl - handle of the NVRAM resource that is of interest
 * @param[out] size - size of NVRAM, in bytes
 * @return - standard @ref t_std_error
 */
t_std_error sdi_nvram_size(sdi_resource_hdl_t resource_hdl, uint_t *size);

/**
 * @brief Read NVRAM
 * @param[in] resource_hdl - handle of the NVRAM resource that is of interest
 * @param[out] buf - buffer to fill with NVRAM data
 * @param[in] ofs - offset (byes) into NVRAM to read
 * @param[in] len - number of bytes to read
 * @return - standard @ref t_std_error
 */
t_std_error sdi_nvram_read(sdi_resource_hdl_t resource_hdl, uint8_t *buf, uint_t ofs, uint_t len);

/**
 * @brief Write NVRAM
 * @param[in] resource_hdl - handle of the NVRAM resource that is of interest
 * @param[in] buf - buffer of data to write to NVRAM
 * @param[in] ofs - offset (byes) into NVRAM to write
 * @param[in] len - number of bytes to write
 * @return - standard @ref t_std_error
 */
t_std_error sdi_nvram_write(sdi_resource_hdl_t resource_hdl, uint8_t *buf, uint_t ofs, uint_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif   /* !defined(__SDI_NVRAM_H__) */
