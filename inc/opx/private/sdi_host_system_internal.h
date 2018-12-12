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
 * filename: sdi_host_system_internal.h
 * Internal host_system resource related declarations.
 */

#ifndef __SDI_HOST_SYSTEM_INTERNAL_H_
#define __SDI_HOST_SYSTEM_INTERNAL_H_

#include "std_error_codes.h"
#include "sdi_driver_internal.h"

/*
 * Each host_system resource provides the following callbacks.
 */

typedef struct {
    /* For reading slot from I2C bus */
    t_std_error (*get_slot)(sdi_device_hdl_t resource_hdl, uint8_t *regData);

    /* For reading package notify from I2C bus */
    t_std_error  (*get_pkg_notify)(sdi_device_hdl_t resource_hdl, bool *regData);

    /* For writing booted to I2C bus */
    t_std_error  (*set_booted)(sdi_device_hdl_t resource_hdl, bool regData);

} host_system_ctrl_t;

#endif /* __SDI_HOST_SYSTEM_INTERNAL_H_ */
