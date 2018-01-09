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

/*
 * @file sdi_vm_sys_init.h
 * @brief Dell Networking SDI (System Device Interface) VM
 * (Virtual Machine) initialization/deinitialization functions
 */

#ifndef _SDI_VM_SYS_INIT_H
#define _SDI_VM_SYS_INIT_H


/*
 * Initialization function for SDI.
 * return  STD_ERR_OK if okay, or error value if initialization fails
 */
t_std_error sdi_sys_init (void);

/*
 * Deinitialization function for SDI.
 * return  STD_ERR_OK if okay, or error value if Deinitialization fails
 */
t_std_error sdi_sys_close (void);

#endif
