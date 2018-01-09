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
 * @file sdi_vm_sys.c
 * @brief SDI initialization routines and helper functions for virtualized implmentation
 */

#include "sdi_sys_vm.h"
#include <stdarg.h>
#include <stdio.h>
#include <event_log.h>
#include "std_assert.h"

static db_sql_handle_t db_handle;

static inline void sdi_set_db_handle(db_sql_handle_t temp_db_handle)
{
  db_handle = temp_db_handle;
}

db_sql_handle_t sdi_get_db_handle(void)
{
  return db_handle;
}

t_std_error sdi_sys_init(void)
{
    t_std_error rc = STD_ERR_OK;
    db_sql_handle_t db = NULL;

    /* Get global variable for DB path */
    rc = sdi_db_open(&db, 1);

    if (rc != STD_ERR_OK) {
        EV_LOGGING(SYSTEM, ERR, __func__, "Can't open database");
    } else {
        EV_LOGGING(SYSTEM, INFO, __func__, "Database opened database successfully");
        sdi_set_db_handle(db);
    }

    return rc;
}


t_std_error sdi_sys_close(void)
{
    sdi_db_close(sdi_get_db_handle());
    return STD_ERR_OK;
}

