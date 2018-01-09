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
 * @file sdi_vm_nvram.c
 *
 * @brief NVRAM simulation functionality implements sdi-api headers
 */

#include "sdi_nvram.h"
#include "sdi_sys_vm.h"
#include "sdi_db.h"
#include "event_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * Create the NVRAM file if it does not exist
 */
static int sdi_nvram_find_or_create_nvram_file(const char *path, uint_t size)
{
    char nvram_file[NAME_MAX];
    int access_rc;
    int fd;

    sdi_db_construct_path(nvram_file, path);
    access_rc = access(nvram_file, F_OK);

    if (access_rc == -1) {
        if (errno != ENOENT)  return (-1);

        fd = open(nvram_file, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
        if (fd > 0) {
            if (ftruncate(fd, size) != 0) {
                close(fd);
                return -1;
            }
        }
    } else {
        fd = open(nvram_file, O_RDWR);
    }

    return fd;
}

/*
 * Retrieve size of NVRAM
 */
t_std_error sdi_nvram_size(sdi_resource_hdl_t resource_hdl, uint_t *size)
{
    int db_size = 0;
    t_std_error rc;

    rc = sdi_db_int_field_get(sdi_get_db_handle(), resource_hdl,
                              TABLE_NVRAM, NVRAM_SIZE, &db_size);
    if (rc == STD_ERR_OK)  *size = (uint_t) db_size;

    return rc;
}

/*
 * Read from NVRAM
 */
t_std_error sdi_nvram_read(sdi_resource_hdl_t resource_hdl, uint8_t *buf, uint_t ofs, uint_t len)
{
    uint_t nvram_size;
    t_std_error rc;
    char nvram_filename[NAME_MAX];
    int nvram_fd;

    rc = sdi_nvram_size(resource_hdl, &nvram_size);
    if (STD_IS_ERR(rc)) {
        return rc;
    }

    if ((ofs + len) > nvram_size) {
        return STD_ERR(BOARD, PARAM, E2BIG);
    }

    rc = sdi_db_str_field_get(sdi_get_db_handle(), resource_hdl,
                              TABLE_NVRAM, NVRAM_DATA_FILE, nvram_filename);
    if (STD_IS_ERR(rc)) {
        return rc;
    }

    off_t nvram_ofs;

    rc = sdi_db_int64_field_get(sdi_get_db_handle(), resource_hdl,
                                TABLE_NVRAM, NVRAM_OFFSET, &nvram_ofs);
    if (STD_IS_ERR(rc)) {
        return rc;
    }    
   
    rc = STD_ERR_OK;
    nvram_fd = sdi_nvram_find_or_create_nvram_file(nvram_filename, nvram_size);
    if (nvram_fd < 0
        || lseek(nvram_fd, nvram_ofs + ofs, SEEK_SET) < 0
        || read(nvram_fd, buf, len) != len
        ) {
        rc = STD_ERR(BOARD, FAIL, EFAULT);
    }

    if (nvram_fd >= 0)  close(nvram_fd);
    
    return (rc);
}

/*
 * Write to NVRAM
 */
t_std_error sdi_nvram_write(sdi_resource_hdl_t resource_hdl, uint8_t *buf, uint_t ofs, uint_t len)
{
    uint_t nvram_size;
    t_std_error rc;
    char nvram_filename[NAME_MAX];
    int nvram_fd;

    rc = sdi_nvram_size(resource_hdl, &nvram_size);
    if (STD_IS_ERR(rc)) {
        return rc;
    }

    if ((ofs + len) > nvram_size) {
        return STD_ERR(BOARD, PARAM, E2BIG);
    }

    rc = sdi_db_str_field_get(sdi_get_db_handle(), resource_hdl,
                              TABLE_NVRAM, NVRAM_DATA_FILE, nvram_filename);
    if (STD_IS_ERR(rc)) {
        return rc;
    }
   
    off_t nvram_ofs;

    rc = sdi_db_int64_field_get(sdi_get_db_handle(), resource_hdl,
                                TABLE_NVRAM, NVRAM_OFFSET, &nvram_ofs);
    if (STD_IS_ERR(rc)) {
        return rc;
    }    
   
    rc = STD_ERR_OK;
    nvram_fd = sdi_nvram_find_or_create_nvram_file(nvram_filename, nvram_size);
    if (nvram_fd < 0
        || lseek(nvram_fd, nvram_ofs + ofs, SEEK_SET) < 0
        || write(nvram_fd, buf, len) != len
        ) {
        rc = STD_ERR(BOARD, FAIL, EFAULT);
    }

    if (nvram_fd >= 0)  close(nvram_fd);
    
    return (rc);
}
