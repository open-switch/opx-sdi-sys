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
 * @file sdi_db_config.h
 * @brief Configuration for location of DB files
 */

#ifndef __SDI_DB_CONFIG_H
#define __SDI_DB_CONFIG_H

/** Default parameters for the SDI database **/

/** Name of the environment variable which sets the default base path */
#define SDI_DB_BASE_ENV     "DN_SDI_DB_BASE_DIR"

/** Default base path for SDI DB related files **/
#define SDI_DB_BASE_DEFAULT "/etc/opx/sdi"

/** Name of the database **/
#define SDI_DB_NAME_DEFAULT "vm.db"

/** Name of the script to create the tables **/
#define SDI_DB_CREATE_SQL   "sdi-db-create.sql"

/** Name of the script to initialize the data **/
#define SDI_DB_INIT_SQL     "sdi-db-data.sql"

/** Name of the environment variable with the database name **/
#define SDI_DB_NAME_ENV     "DN_SDI_DB_NAME"

/** Name of the environment variable with the initialization script name **/
#define SDI_DB_INIT_ENV     "DN_SDI_DB_INIT"

/** Name of the environment variable with the semaphore key **/
#define SDI_DB_SEM_ENV      "DN_SDI_DB_SEM_KEY"

/** Default semaphore key if the above environment variable is unspecified **/
#define SDI_DB_SEM_DEFAULT  0x53444900

/** Name of the environment variable which if set will prevent writing to the
 * regular database field
 */
#define SDI_DB_NO_SYNC_ENV  "DN_SDI_DB_NO_SYNC"

#endif /* __SDI_DB_CONFIG_H */
