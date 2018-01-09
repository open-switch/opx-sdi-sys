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
 * \file   sdi_sys_vm.h
 * \brief  Dell Networking SDI (System Device Interface) VM
 *         (Virtual Machine) private helper function prototypes and includes
 * \date   09-2014
 */

#ifndef _SDI_SYS_VM_H_
#define _SDI_SYS_VM_H_
#include <stdlib.h>
#include "std_error_codes.h"
#include <std_error_ids.h>
#include "std_utils.h"
#include "sdi_vm_sys_init.h"
#include "sdi_db.h"
#include "std_assert.h"

#define VM_SQl_DEFAULT_BUFFER_LENGTH 256

/** @TODO Replace instances of these errors with new SDI error defines... */
/** Hardware Error **/
#define STD_ERR_HARDWARE (-1)

/**
 * \brief   Get database handle stored in global storage
 * \return  handle to database used by sdi-sys-vm to store object state
 */
db_sql_handle_t  sdi_get_db_handle(void);

#endif
