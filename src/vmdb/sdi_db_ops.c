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

/**
 * sdi_db_ops.c
 * SDI DB access implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "db_sql_ops.h"
#include "sdi_db.h"
#include "sdi_db_config.h"
#include "sdi_entity.h"
#include "sdi_entity_info.h"
#include "std_error_codes.h"
#include "std_error_ids.h"
#include "std_assert.h"
#include "event_log.h"
#include "std_utils.h"

static int semid;

/* Extern semaphore functions to expose library functionality
 * to serialize DB accesses.
 */
int sdi_db_sem_get(void)
{
    key_t sem_key;
    char *sem_id_str = getenv(SDI_DB_SEM_ENV);

    if (sem_id_str == NULL) {
        sem_key = SDI_DB_SEM_DEFAULT;
    } else {
        sem_key = strtoul(sem_id_str, NULL, 0);
    }

    return (semid = semget(sem_key, 1, IPC_CREAT | 0777));
}

void sdi_db_sem_take(void)
{
    struct sembuf sb[1];

    sb->sem_num = 0;
    sb->sem_op = -1;
    sb->sem_flg = 0;
    semop(semid, sb, 1);
}

void sdi_db_sem_give(void)
{
    struct sembuf sb[1];

    sb->sem_num = 0;
    sb->sem_op  = 1;
    sb->sem_flg = 0;
    semop(semid, sb, 1);
}

/* Serialized DB attribute get operation */
static t_std_error sdi_db_sql_get_attribute(db_sql_handle_t db_handle,
                                            const char *table_name,
                                            const char *attribute_name,
                                            const char *condition,
                                            char *output_str)
{
    t_std_error result;

    sdi_db_sem_take();

    result = db_sql_get_attribute(db_handle, table_name, attribute_name, condition, output_str);

    sdi_db_sem_give();

    return (result);
}

/* Serialized DB attribute set operation */
static t_std_error sdi_db_sql_set_attribute(db_sql_handle_t db_handle,
                                            const char *table_name,
                                            const char *attribute_name,
                                            const char *value,
                                            const char *condition)
{
    t_std_error result;

    sdi_db_sem_take();

    result = db_sql_set_attribute(db_handle, table_name, attribute_name, value, condition);

    sdi_db_sem_give();

    return (result);
}

/**
 * @brief Construct a path to a SDI DB file
 *
 * @param[out]  buffer  Buffer in which to write the full path. Buffer must be
 *                      NAME_MAX long.
 * @param[in]   file    Name of the file to get the path to
 *
 * @return None
 */
void sdi_db_construct_path(char *buffer, const char *file)
{
    if (file[0] == '/') {
        /* Given file is absolute path => Use directly */

        safestrncpy(buffer, file, NAME_MAX);
        return;
    }

    char *base_dir = getenv(SDI_DB_BASE_ENV);
    if (base_dir == NULL) {
        base_dir = SDI_DB_BASE_DEFAULT;
    }

    snprintf(buffer, NAME_MAX, "%s/%s", base_dir, file);
}

/**
 * @brief Run a SQL script on a database
 *
 * This function reads a SQL script file and runs the commands one by one
 * on the given database handle. The core assumption is that individual
 * SQL commands are separated by semi-colon characters, and there are never
 * any instances of two commands existing on the same line, i.e., the end
 * of one command and the beginning of the next are always separated by at
 * least one new line.
 *
 * @param[in]   db_handle   Handle to the database
 * @param[in]   sql_path    Path to the SQL script
 *
 * @return None
 */
static void sdi_db_run_sql_script(db_sql_handle_t db_handle,
                                  const char *sql_path)
{
    /* Buffer to store the [multi-line] SQL command from the file */
    char sql_cmd[1024];
    char *sql_cmd_ptr;  /* Store the current end of the buffer */
    int len_remaining;
    uint_t bytes_read;
    FILE *sql;

    sql = fopen(sql_path, "r");
    if (!sql) {
        return;
    }

    len_remaining = sizeof(sql_cmd);
    sql_cmd_ptr = sql_cmd;

    while (fgets(sql_cmd_ptr, len_remaining, sql)) {
        bytes_read = strlen(sql_cmd_ptr);
        if (strchr(sql_cmd_ptr, ';')) {
            /* End of command found */
            db_sql_raw_sql_execute(db_handle, sql_cmd, NULL, NULL);
            len_remaining = sizeof(sql_cmd);
            sql_cmd_ptr = sql_cmd;
        } else {
            len_remaining -= bytes_read;
            sql_cmd_ptr += bytes_read;

            if (len_remaining <= 0) {
                /* Error condition, we don't ever expect this to occur */
                /* TODO: Add error logging */
                break;
            }
        }
    }

    fclose(sql);
}

/**
 * @brief Create and initialze a new database
 *
 * @param[in]   db_path     Path to the database
 *
 * @return None
 */
static void sdi_db_init_database(char *db_path)
{
    char file_path[NAME_MAX];
    char *sql_init;
    db_sql_handle_t db_handle;
    t_std_error rc;

    /* Create the database */
    rc = db_sql_open(&db_handle, db_path);

    if (rc != STD_ERR_OK) {
        return;
    }

    /* Initialize the database */
    sdi_db_construct_path(file_path, SDI_DB_CREATE_SQL);
    sdi_db_run_sql_script(db_handle, file_path);

    /* Get the path to the initialization script */
    sql_init = getenv(SDI_DB_INIT_ENV);
    if (sql_init == NULL) {
        /* Use the default initialization script */
        sql_init = SDI_DB_INIT_SQL;
    }

    /* Construct the command to populate the database */
    sdi_db_construct_path(file_path, sql_init);
    sdi_db_run_sql_script(db_handle, file_path);

    db_sql_close(db_handle);
}

/**
 * @brief Reinitialize the database with the initial data
 *
 * @return None
 */
void sdi_db_reinit_database(void)
{
    char db_path[NAME_MAX];
    char file_path[NAME_MAX];
    char *db_name = getenv(SDI_DB_NAME_ENV);
    char *sql_init;
    db_sql_handle_t db_handle;
    t_std_error rc;


    if (!db_name) {
        /* Fallback to the default name if the environment variable isn't set */
        db_name = SDI_DB_NAME_DEFAULT;
    }

    sdi_db_construct_path(db_path, db_name);
    rc = db_sql_open(&db_handle, db_path);

    if (rc != STD_ERR_OK) {
        return;
    }

    /* Get the path to the initialization script */
    sql_init = getenv(SDI_DB_INIT_ENV);
    if (sql_init == NULL) {
        /* Use the default initialization script */
        sql_init = SDI_DB_INIT_SQL;
    }

    /* Construct the command to populate the database */
    sdi_db_construct_path(file_path, sql_init);
    sdi_db_run_sql_script(db_handle, file_path);

    db_sql_close(db_handle);
}

/**
 * @brief Check if writes need to be made to the regular field as well
 *
 * @return true to write to regular field
 */
static bool sdi_db_write_regular_field(void)
{
    return (getenv(SDI_DB_NO_SYNC_ENV) == NULL);
}

/**
 * @brief Open a connection to a database
 *
 * @param[out]  db_handle   Database handle to return
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_open(db_sql_handle_t * db_handle, uint_t masterf)
{
    t_std_error rc;
    int access_rc;
    char db_path[NAME_MAX];
    char *db_name = getenv(SDI_DB_NAME_ENV);

    if (!db_name) {
        /* Fallback to the default name if the environment variable isn't set */
        db_name = SDI_DB_NAME_DEFAULT;
    }

    if (sdi_db_sem_get() < 0)  return (STD_ERR(BOARD, FAIL, errno));

    if (masterf)  sdi_db_sem_give();

    sdi_db_construct_path(db_path, db_name);

    /* Check if the database is present, if not we need to initialize it */
    errno = 0;
    access_rc = access(db_path, F_OK);
    if (access_rc == -1 && errno == ENOENT) {
        /* Database does not exist */
        sdi_db_init_database(db_path);
    }
    rc = db_sql_open(db_handle, db_path);

    return rc;
}

/**
 * @brief Close a database connection
 *
 * @param[in]   db_handle   Handle to the database
 *
 * @return None
 */
void sdi_db_close(db_sql_handle_t db_handle)
{
    db_sql_close(db_handle);
}

/**
 * @brief Convert a string to an integer
 *
 * @param[in]   buffer      String buffer to convert
 * @param[out]  int_val     Pointer to the integer value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
static t_std_error sdi_db_strtol(const char *buffer, int *int_val)
{
    char *endptr;

    STD_ASSERT(buffer != NULL);
    STD_ASSERT(int_val != NULL);

    /* Reset errno before calling strtol */
    errno = 0;
    *int_val = strtol(buffer, &endptr, 10);
    if (*endptr == '\0') {
        return STD_ERR_OK;
    }

    return STD_ERR(BOARD, FAIL, errno);
}

/**
 * @brief Convert a string to a 64-bit integer
 *
 * @param[in]   buffer      String buffer to convert
 * @param[out]  int_val     Pointer to the integer value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
static t_std_error sdi_db_strtoll(const char *buffer, int64_t *int_val)
{
    char *endptr;

    STD_ASSERT(buffer != NULL);
    STD_ASSERT(int_val != NULL);

    /* Reset errno before calling strtol */
    errno = 0;
    *int_val = strtoll(buffer, &endptr, 10);
    if (*endptr == '\0') {
        return STD_ERR_OK;
    }

    return STD_ERR(BOARD, FAIL, errno);
}

/**
 * @brief Convert a string to a floating point value
 *
 * @param[in]   buffer      String buffer to convert
 * @param[out]  float_val   Pointer to the floating point value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
static t_std_error sdi_db_strtod(const char *buffer, float *float_val)
{
    char *endptr;

    STD_ASSERT(buffer != NULL);
    STD_ASSERT(float_val != NULL);

    /* Reset errno before calling strtod */
    errno = 0;
    *float_val = strtod(buffer, &endptr);
    if (*endptr == '\0') {
        return STD_ERR_OK;
    }

    return STD_ERR(BOARD, FAIL, errno);
}

/**
 * @brief Retrieve Entity Handle
 *
 * This is a unique value representing an entity in the system
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   entity_type     Type of the entity
 * @param[in]   entity_instance Instance of the entity (1-based)
 * @param[out]  entity_handle   Pointer to a location to save the handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_entity_handle(db_sql_handle_t db_handle,
                                     sdi_entity_type_t entity_type,
                                     uint_t entity_instance,
                                     sdi_entity_hdl_t *entity_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int e_hdl;

    STD_ASSERT(entity_handle != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u",
             TBL_ENTITY_TYPE, entity_type,
             TBL_ENTITY_INSTANCE, entity_instance);

    rc = sdi_db_sql_get_attribute(db_handle, TABLE_INFO, TBL_ENTITY_HDL,
                              condition, result);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &e_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to an entity handle */
    *entity_handle = (sdi_entity_hdl_t)(uintptr_t)e_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Retrieve entity count
 *
 * Number of entities of the specified type
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   entity_type     Type of the entity
 * @param[out]  count           Pointer to a location to save the count
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_entity_count(db_sql_handle_t db_handle,
                                    sdi_entity_type_t entity_type,
                                    uint_t *count)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(count != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u", TBL_ENTITY_TYPE, entity_type);

    rc = sdi_db_sql_get_attribute(db_handle, TABLE_INFO, "COUNT(*)", condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtol(result, (int *)count);
}

/**
 * @brief Retrieve Resource count
 *
 * Number of resources of the given type within a parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the parent entity
 * @param[in]   resource_type       Type of the resource
 * @param[out]  count               Pointer to a location to save the count
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_resource_count(db_sql_handle_t db_handle,
                                      sdi_entity_hdl_t entity_handle,
                                      sdi_resource_type_t resource_type,
                                      uint_t *count)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(count != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u",
             TBL_ENTITY_HDL, (uint_t)(uintptr_t)entity_handle,
             TBL_RESOURCE_TYPE, resource_type);

    rc = sdi_db_sql_get_attribute(db_handle, TABLE_RESOURCES, "COUNT(*)",
                              condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtol(result, (int *)count);
}

/**
 * @brief Get first entity
 *
 * Get first entity in the list of all entities
 *
 * @param[in]   db_handle       Handle to the database
 * @param[out]  entity_handle   Pointer to a location to save the entity handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_first_entity(db_sql_handle_t db_handle,
                                    sdi_entity_hdl_t *entity_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int e_hdl;

    STD_ASSERT(entity_handle != NULL);

    snprintf(condition, sizeof(condition),
             "ORDER BY %s LIMIT 1", TBL_ENTITY_HDL);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_INFO, TBL_ENTITY_HDL,
                              condition, result);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &e_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to an entity handle */
    *entity_handle = (sdi_entity_hdl_t)(uintptr_t)e_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Get next entity
 *
 * Get the next entity in the list of all entities
 *
 * @param[in]       db_handle       Handle to the database
 * @param[in,out]   entity_handle   Pointer to a location to save the entity handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_next_entity(db_sql_handle_t db_handle,
                                   sdi_entity_hdl_t *entity_handle) {
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int e_hdl;

    STD_ASSERT(entity_handle != NULL);

    snprintf(condition, sizeof(condition),
             "%s > %u ORDER BY %s LIMIT 1",
             TBL_ENTITY_HDL, (uint_t)(uintptr_t)entity_handle, TBL_ENTITY_HDL);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_INFO, TBL_ENTITY_HDL,
                              condition, result);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &e_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to an entity handle */
    *entity_handle = (sdi_entity_hdl_t)(uintptr_t)e_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Get first resource
 *
 * Get first resource of the given type for the parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Parent entity handle
 * @param[in]   resource_type       Resource type to lookup
 * @param[out]  resource_handle     Pointer to a location to save the resource handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_first_resource(db_sql_handle_t db_handle,
                                      sdi_entity_hdl_t entity_handle,
                                      sdi_resource_type_t resource_type,
                                      sdi_resource_hdl_t *resource_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int r_hdl;

    STD_ASSERT(resource_handle != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u ORDER BY %s LIMIT 1",
             TBL_ENTITY_HDL, (uint_t)(uintptr_t)entity_handle,
             TBL_RESOURCE_TYPE, resource_type,
             TBL_RESOURCE_HDL);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_RESOURCES, TBL_RESOURCE_HDL,
                              condition, result);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &r_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to a resource handle */
    *resource_handle = (sdi_entity_hdl_t)(uintptr_t)r_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Get next resource
 *
 * Get the next resource of the given type from the given resource
 *
 * @param[in]       db_handle           Handle to the database
 * @param[in]       entity_handle       Parent entity handle
 * @param[in]       resource_type       Resource type to lookup
 * @param[in,out]   resource_handle     Pointer to a location to save the resource handle
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_next_resource(db_sql_handle_t db_handle,
                                     sdi_entity_hdl_t entity_handle,
                                     sdi_resource_type_t resource_type,
                                     sdi_resource_hdl_t *resource_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int r_hdl;

    STD_ASSERT(resource_handle != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u AND %s > %u ORDER BY %s LIMIT 1",
             TBL_ENTITY_HDL, (uint_t)(uintptr_t)entity_handle,
             TBL_RESOURCE_TYPE, resource_type,
             TBL_RESOURCE_HDL, (uint_t)(uintptr_t)*resource_handle,
             TBL_RESOURCE_HDL);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_RESOURCES, TBL_RESOURCE_HDL,
                              condition, result);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &r_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to a resource handle */
    *resource_handle = (sdi_entity_hdl_t)(uintptr_t)r_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Retrieve Resource Handle by instance
 *
 * This is a unique value representing a resource of the given type within a
 * parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the parent entity
 * @param[in]   resource_type       Type of the resource
 * @param[in]   resource_instance   Instance of the resource (1-based)
 * @param[out]  resource_handle     Pointer to a location to save the handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_resource_handle(db_sql_handle_t db_handle,
                                       sdi_entity_hdl_t entity_handle,
                                       sdi_resource_type_t resource_type,
                                       uint_t resource_instance,
                                       sdi_resource_hdl_t *resource_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int r_hdl;

    STD_ASSERT(resource_handle != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u ORDER BY %s LIMIT 1 OFFSET %u",
             TBL_ENTITY_HDL, (uint_t)(uintptr_t)entity_handle,
             TBL_RESOURCE_TYPE, resource_type,
             TBL_RESOURCE_HDL, (resource_instance - 1));

    rc = sdi_db_sql_get_attribute(db_handle, TABLE_RESOURCES, TBL_RESOURCE_HDL,
                              condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &r_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to a resource handle */
    *resource_handle = (sdi_entity_hdl_t)(uintptr_t)r_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Retrieve Resource Handle by alias
 *
 * This is a unique value representing a resource of the given type within a
 * parent entity
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the parent entity
 * @param[in]   resource_type       Type of the resource
 * @param[in]   alias               Pointer to a string for the alias
 * @param[out]  resource_handle     Pointer to a location to save the handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_get_resource_handle_by_alias(db_sql_handle_t db_handle,
                                                sdi_entity_hdl_t entity_handle,
                                                sdi_resource_type_t resource_type,
                                                const char *alias,
                                                sdi_resource_hdl_t *resource_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int r_hdl;

    STD_ASSERT(alias != NULL);
    STD_ASSERT(resource_handle != NULL);

    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u AND %s = '%s'",
             TBL_ENTITY_HDL, (uint_t)(uintptr_t)entity_handle,
             TBL_RESOURCE_TYPE, resource_type,
             TBL_RESOURCE_ALIAS, alias);

    rc = sdi_db_sql_get_attribute(db_handle, TABLE_RESOURCES, TBL_RESOURCE_HDL,
                              condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &r_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to a resource handle */
    *resource_handle = (sdi_entity_hdl_t)(uintptr_t)r_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Retrieve the entity presence
 *
 * Flag indicating whether the entity is present or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[out]  present             Pointer to a location to save the presence flag
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_presence_get(db_sql_handle_t db_handle,
                                       sdi_entity_hdl_t entity_handle,
                                       bool *present)
{
    int db_value;
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;

    rc = sdi_db_get_first_resource(db_handle, entity_handle,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_int_field_get(db_handle, info_hdl,
                              TABLE_INFO, INFO_PRESENCE, &db_value);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    *present = (bool)db_value;
    return STD_ERR_OK;
}

/**
 * @brief Set the entity presence
 *
 * Flag indicating whether the entity is present or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[in]   present             Presence flag value to set
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_presence_set(db_sql_handle_t db_handle,
                                       sdi_entity_hdl_t entity_handle,
                                       bool present)
{
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;
    int db_value = (int)present;

    rc = sdi_db_get_first_resource(db_handle, entity_handle,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_int_field_set(db_handle, info_hdl,
                                TABLE_INFO, INFO_PRESENCE, &db_value);
}

/**
 * @brief Retrieve the entity fault status
 *
 * Flag indicating whether the entity is faulty or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[out]  fault_status        Pointer to a location to save the fault status flag
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_fault_status_get(db_sql_handle_t db_handle,
                                           sdi_entity_hdl_t entity_handle,
                                           bool *fault_status)
{
    int db_value;
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;

    rc = sdi_db_get_first_resource(db_handle, entity_handle,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_int_field_get(db_handle, info_hdl,
                              TABLE_INFO, INFO_FAULT, (int *)&db_value);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    *fault_status = (bool)db_value;
    return STD_ERR_OK;
}

/**
 * @brief Set the entity fault status
 *
 * Flag indicating whether the entity is faulty or not.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   entity_handle       Handle for the entity
 * @param[in]   fault_status        Fault status flag value to set
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_entity_fault_status_set(db_sql_handle_t db_handle,
                                           sdi_entity_hdl_t entity_handle,
                                           bool fault_status)
{
    t_std_error rc;
    sdi_resource_hdl_t info_hdl;
    int db_value = (int)fault_status;

    rc = sdi_db_get_first_resource(db_handle, entity_handle,
                                   SDI_RESOURCE_ENTITY_INFO, &info_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_int_field_set(db_handle, info_hdl,
                                TABLE_INFO, INFO_FAULT, &db_value);
}

/**
 * @brief Get the parent entity handle for a resource
 *
 * For a given resource and table, retrieve the handle of the parent entity.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   resource_handle     Handle to the resource
 * @param[in]   resource_type       Type of the resource
 * @param[out]  entity_handle       Handle to parent entity
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_resource_get_parent_entity(db_sql_handle_t db_handle,
                                              sdi_resource_hdl_t resource_handle,
                                              sdi_resource_type_t resource_type,
                                              sdi_entity_hdl_t *entity_handle)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int e_hdl;

    STD_ASSERT(entity_handle != NULL);

    /* Get the parent entity handle for the resource */
    snprintf(condition, sizeof(condition),
             "%s = %u AND %s = %u",
             TBL_RESOURCE_HDL, (uint_t)(uintptr_t)resource_handle,
             TBL_RESOURCE_TYPE, resource_type);

    rc = sdi_db_sql_get_attribute(db_handle, TABLE_RESOURCES, TBL_ENTITY_HDL,
                              condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &e_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to an entity handle */
    *entity_handle = (sdi_entity_hdl_t)(uintptr_t)e_hdl;
    return STD_ERR_OK;
}

/**
 * @brief Get associated info handle for a resource
 *
 * For any given resource, retrieve the resource handle of its associated info
 * structure.
 *
 * @param[in]   db_handle           Handle to the database
 * @param[in]   resource_handle     Handle to the resource
 * @param[in]   resource_type       Type of the resource
 * @param[out]  info_handle         Handle to associated info
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_resource_get_associated_info(db_sql_handle_t db_handle,
                                                sdi_resource_hdl_t resource_handle,
                                                sdi_resource_type_t resource_type,
                                                sdi_resource_hdl_t *info_handle)
{
    t_std_error rc;
    sdi_entity_hdl_t parent_entity_hdl;

    STD_ASSERT(info_handle != NULL);

    /* Get the parent entity handle for the resource */
    rc = sdi_db_resource_get_parent_entity(db_handle, resource_handle,
                                           resource_type, &parent_entity_hdl);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    /*
     * Look up the first info resource for this entity. This will be the
     * sole entity_info resource for this entity.
     */
    return sdi_db_get_first_resource(db_handle, parent_entity_hdl,
                                     SDI_RESOURCE_ENTITY_INFO, info_handle);
}

/**
 * @brief Create a condition based on a resource handle
 *
 * @param[out]  buf     Buffer in which to save the condition. It is expected
 *                      that the buffer length is at least \ref
 *                      SDI_DB_SQL_DEFAULT_BUFFER_LENGTH
 * @param[out]  len     Length of the condition string. If this is not NULL,
 *                      the length written is saved in this pointer.
 * @param[in]   r_hdl   Resource handle
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_cond_resource_handle(char * buf, uint_t * len,
                                        sdi_resource_hdl_t r_hdl)
{
    uint_t buf_len;
    STD_ASSERT(buf != NULL);

    buf_len = snprintf(buf, SDI_DB_SQL_DEFAULT_BUFFER_LENGTH,
                       "%s=%u", TBL_RESOURCE_HDL, (unsigned int)(uintptr_t)r_hdl);

    if (len) {
        *len = buf_len;
    }

    return STD_ERR_OK;
}

/**
 * @brief Retrieve an integer variable from the database, given the resource
 * handle, table and field. Booleans may also be retrieved using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int_field_get(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 int *value)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);
    rc = sdi_db_sql_get_attribute(db_handle, table, field, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtol(result, value);
}

/**
 * @brief Set an integer variable into the database, given the resource
 * handle, table and field. Booleans may also be set using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int_field_set(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 int *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);
    snprintf(value_str, sizeof(value_str), "%d", *value);

    return sdi_db_sql_set_attribute(db_handle, table, field, value_str, condition);
}

/**
 * @brief Retrieve a 64-bit integer variable from the database, given the resource
 * handle, table and field. Booleans may also be retrieved using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int64_field_get(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t res_handle,
                                   const char *table,
                                   const char *field,
                                   int64_t *value)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);
    rc = sdi_db_sql_get_attribute(db_handle, table, field, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtoll(result, value);
}

/**
 * @brief Set a 64-bit integer variable into the database, given the resource
 * handle, table and field. Booleans may also be set using this method.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int64_field_set(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t res_handle,
                                   const char *table,
                                   const char *field,
                                   int64_t *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);
    snprintf(value_str, sizeof(value_str), "%ld", *value);

    return sdi_db_sql_set_attribute(db_handle, table, field, value_str, condition);
}

/**
 * @brief Set an integer variable into the database, given the resource
 * handle, table and field. Booleans may also be set using this method.
 * This function will set the target field, and may set the regular field
 * as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_int_field_target_set(db_sql_handle_t db_handle,
                                        sdi_resource_hdl_t res_handle,
                                        const char *table,
                                        const char *field,
                                        int *value)
{
    t_std_error rc;
    char tgt_field[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    /* Write to the target field */
    snprintf(tgt_field, sizeof(tgt_field), "%s%s", TARGET_PREFIX, field);
    rc = sdi_db_int_field_set(db_handle, res_handle, table, tgt_field, value);
    if (rc != STD_ERR_OK || !sdi_db_write_regular_field()) {
        return rc;
    }

    return sdi_db_int_field_set(db_handle, res_handle, table, field, value);
}

/**
 * @brief Retrieve a string variable from the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_str_field_get(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 char *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);
    return sdi_db_sql_get_attribute(db_handle, table, field, condition, value);
}

/**
 * @brief Set a string variable into the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_str_field_set(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 const char *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);
    snprintf(value_str, sizeof(value_str), "'%s'", value);

    return sdi_db_sql_set_attribute(db_handle, table, field, value_str, condition);
}

/**
 * @brief Set a string variable into the database, given the resource
 * handle, table and field.  This function will set the target field, and may
 * set the regular field as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_str_field_target_set(db_sql_handle_t db_handle,
                                        sdi_resource_hdl_t res_handle,
                                        const char *table,
                                        const char *field,
                                        const char *value)
{
    t_std_error rc;
    char tgt_field[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    /* Write to the target field */
    snprintf(tgt_field, sizeof(tgt_field), "%s%s", TARGET_PREFIX, field);
    rc = sdi_db_str_field_set(db_handle, res_handle, table, tgt_field, value);
    if (rc != STD_ERR_OK || !sdi_db_write_regular_field()) {
        return rc;
    }

    return sdi_db_str_field_set(db_handle, res_handle, table, field, value);
}

/**
 * @brief Retrieve a binary variable from the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_bin_field_get(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 uint8_t *value,
                                 uint_t *len)
{
    char sql_stmt[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    size_t length = sizeof(result);
    t_std_error rc;

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);
    STD_ASSERT(len != NULL);

    sdi_db_cond_resource_handle(condition, NULL, res_handle);

    /* sdi_db_sql_get_attribute uses safestrncpy which fails in the presence of
     * embedded NULLs. Therefore, we must construct our own SQL statement
     * and call db_sql_raw_sql_execute
     */
    snprintf(sql_stmt, sizeof(sql_stmt),
             "SELECT %s FROM %s WHERE %s", field, table, condition);

    sdi_db_sem_take();

    rc = db_sql_raw_sql_execute(db_handle, sql_stmt, &length, result);

    sdi_db_sem_give();

    if (rc == STD_ERR_OK) {
        /* Ensure we don't overflow the passed buffer */
        if (*len < length) {
            length = *len;
        }

        memcpy(value, result, length);
        /* Set the returned length */
        *len = length;
    }

    return rc;
}

/**
 * @brief Set a binary variable into the database, given the resource
 * handle, table and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_bin_field_set(db_sql_handle_t db_handle,
                                 sdi_resource_hdl_t res_handle,
                                 const char *table,
                                 const char *field,
                                 const uint8_t *value,
                                 uint_t len)
{
    t_std_error result;
    char sql_stmt[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    uint_t stmt_len;
    uint_t cond_len;
    uint_t i;

    STD_ASSERT(table != NULL);
    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    /* Prefix the WHERE keyword to the condition, since this will eventually
     * be concatenated with the full SQL statement.
     */
    stmt_len = snprintf(condition, sizeof(condition), "' WHERE ");
    sdi_db_cond_resource_handle(&condition[stmt_len], &cond_len, res_handle);
    cond_len += stmt_len;

    /* sdi_db_sql_set_attribute cannot work with blobs with arbitrary bytes,
     * therefore we must construct our own SQL statement and call
     * db_sql_raw_sql_execute
     */
    stmt_len = snprintf(sql_stmt, sizeof(sql_stmt),
                        "UPDATE %s SET %s = x'", table, field);

    /* The SQL statement is "UPDATE <table> SET <field> = x'<value in hex>'
     * WHERE <condition>". This total length cannot exceed SDI_DB_SQL_DEFAULT_BUFFER_LENGTH
     * bytes, so the number of bytes in the buffer cannot exceed this value:
     * max_len = (SDI_DB_SQL_DEFAULT_BUFFER_LENGTH + stmt_len + cond_len) >> 1
     */
    if (len >= ((SDI_DB_SQL_DEFAULT_BUFFER_LENGTH + stmt_len + cond_len) >> 1)) {
        return STD_ERR(BOARD, PARAM, EINVAL);
    }

    /* Build the blob value to write to the database */
    for (i = 0; i < len; i++) {
        stmt_len += snprintf(&sql_stmt[stmt_len],
                             sizeof(sql_stmt) - stmt_len,
                             "%02x", value[i]);
    }

    /* We have already verified that the condition will fit inside sql_stmt,
     * therefore there is no need to use strncat.
     */
    strcat(sql_stmt, condition);

    sdi_db_sem_take();

    result = db_sql_raw_sql_execute(db_handle, sql_stmt, NULL, NULL);

    sdi_db_sem_give();

    return (result);
}

/**
 * @brief Set a binary variable into the database, given the resource
 * handle, table and field.  This function will set the target field, and may
 * set the regular field as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_handle      Handle to the resource
 * @param[in]   table           Table name
 * @param[in]   field           Field name
 * @param[in]   value           Pointer to the value
 * @param[in]   len             Length of the buffer
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_bin_field_target_set(db_sql_handle_t db_handle,
                                        sdi_resource_hdl_t res_handle,
                                        const char *table,
                                        const char *field,
                                        const uint8_t *value,
                                        uint_t len)
{
    t_std_error rc;
    char tgt_field[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    /* Write to the target field */
    snprintf(tgt_field, sizeof(tgt_field), "%s%s", TARGET_PREFIX, field);
    rc = sdi_db_bin_field_set(db_handle, res_handle, table, tgt_field, value, len);
    if (rc != STD_ERR_OK || !sdi_db_write_regular_field()) {
        return rc;
    }

    return sdi_db_bin_field_set(db_handle, res_handle, table, field, value, len);
}

/* Build the condition string for media and channel */
static void sdi_db_cond_media_channel(char *buf, size_t len,
                                      sdi_resource_hdl_t media_hdl,
                                      int channel)
{
    snprintf(buf, len, "%s=%u AND %s=%d",
             TBL_RESOURCE_HDL, (uint_t)(uintptr_t)media_hdl,
             MEDIA_CHANNEL, channel);
}

/**
 * @brief Retrieve an integer value from the media channel table, given the
 * resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_int_field_get(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               const char *field,
                                               int *value)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_media_channel(condition, sizeof(condition), media_hdl, channel);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_MEDIA_CHANNEL, field, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtol(result, value);
}

/**
 * @brief Set an integer value in the media channel table, given the
 * resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Value to save
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_int_field_set(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               const char *field,
                                               int value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(field != NULL);

    sdi_db_cond_media_channel(condition, sizeof(condition), media_hdl, channel);
    snprintf(value_str, sizeof(value_str), "%d", value);
    return sdi_db_sql_set_attribute(db_handle, TABLE_MEDIA_CHANNEL, field, value_str, condition);
}

/**
 * @brief Set an integer value in the media channel table, given the
 * resource handle, channel and field.  This function will set the target
 * field, and may set the regular field as well.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Value to save
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_int_field_target_set(db_sql_handle_t db_handle,
                                                      sdi_resource_hdl_t media_hdl,
                                                      int channel,
                                                      const char *field,
                                                      int value)
{
    t_std_error rc;
    char tgt_field[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    /* Write to the target field */
    snprintf(tgt_field, sizeof(tgt_field), "%s%s", TARGET_PREFIX, field);
    rc = sdi_db_media_channel_int_field_set(db_handle, media_hdl, channel, tgt_field, value);
    if (rc != STD_ERR_OK || !sdi_db_write_regular_field()) {
        return rc;
    }

    return sdi_db_media_channel_int_field_set(db_handle, media_hdl, channel, field, value);
}

/**
 * @brief Retrieve a floating point value from the media channel table, given
 * the resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_float_field_get(db_sql_handle_t db_handle,
                                                 sdi_resource_hdl_t media_hdl,
                                                 int channel,
                                                 const char *field,
                                                 float *value)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(field != NULL);
    STD_ASSERT(value != NULL);

    sdi_db_cond_media_channel(condition, sizeof(condition), media_hdl, channel);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_MEDIA_CHANNEL, field, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtod(result, value);
}


/**
 * @brief Set a floating point value in the media channel table, given the
 * resource handle, channel and field.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel index (\ref MEDIA_NO_CHANNEL for the module value)
 * @param[in]   field           Field name
 * @param[out]  value           Value to save
 *
 * @return STD_ERR_OK on success, error code on failure.
 */
t_std_error sdi_db_media_channel_float_field_set(db_sql_handle_t db_handle,
                                                 sdi_resource_hdl_t media_hdl,
                                                 int channel,
                                                 const char *field,
                                                 float value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(field != NULL);

    sdi_db_cond_media_channel(condition, sizeof(condition), media_hdl, channel);
    snprintf(value_str, sizeof(value_str), "%f", value);
    return sdi_db_sql_set_attribute(db_handle, TABLE_MEDIA_CHANNEL, field, value_str, condition);
}

/* Build the condition string for media and parameter */
static void sdi_db_cond_media_param(char *buf, size_t len,
                                    sdi_resource_hdl_t media_hdl,
                                    sdi_media_param_type_t param_type)
{
    snprintf(buf, len, "%s=%u AND %s=%d",
             TBL_RESOURCE_HDL, (uint_t)(uintptr_t)media_hdl,
             MEDIA_PARAM_TYPE, param_type);
}

/**
 * @brief Retrieve the media parameter value, given the resource handle and
 * parameter type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   param_type      Type of parameter
 * @param[out]  value           Pointer to a location to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_param_get(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t media_hdl,
                                   sdi_media_param_type_t param_type,
                                   uint_t *value)
{
    t_std_error rc;
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(value != NULL);

    sdi_db_cond_media_param(condition, sizeof(condition), media_hdl, param_type);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_MEDIA_PARAMS, MEDIA_PARAM_VALUE,
                              condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtol(result, (int *)value);
}

/**
 * @brief Set the media parameter value, given the resource handle and
 * parameter type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   param_type      Type of parameter
 * @param[in]   value           Value to save in the DB
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_param_set(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t media_hdl,
                                   sdi_media_param_type_t param_type,
                                   uint_t value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    sdi_db_cond_media_param(condition, sizeof(condition), media_hdl, param_type);
    snprintf(value_str, sizeof(value_str), "%u", value);
    return sdi_db_sql_set_attribute(db_handle, TABLE_MEDIA_PARAMS, MEDIA_PARAM_VALUE,
                                value_str, condition);
}

/* Build the condition string for media and vendor info */
static void sdi_db_cond_media_vendor(char *buf, size_t len,
                                     sdi_resource_hdl_t media_hdl,
                                     sdi_media_vendor_info_type_t info_type)
{
    snprintf(buf, len, "%s=%u AND %s=%d",
             TBL_RESOURCE_HDL, (uint_t)(uintptr_t)media_hdl,
             MEDIA_VENDOR_INFO_TYPE, info_type);
}

/**
 * @brief Retrieve the media vendor info value, given the resource handle and
 * vendor info type. Vendor OUI cannot be retrieved using this function and
 * must be retrieved using \ref sdi_db_bin_field_get
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   info_type       Type of vendor_info
 * @param[in]   length          Length of buffer
 * @param[out]  value           Pointer to a buffer to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_vendor_info_get(db_sql_handle_t db_handle,
                                         sdi_resource_hdl_t media_hdl,
                                         sdi_media_vendor_info_type_t info_type,
                                         uint_t length,
                                         char *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    t_std_error rc;
    size_t res_len;

    STD_ASSERT(value != NULL);

    sdi_db_cond_media_vendor(condition, sizeof(condition), media_hdl, info_type);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_MEDIA_VENDOR_INFO,
                              MEDIA_VENDOR_INFO_VALUE, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    res_len = strlen(result);
    if (length <= res_len) {
        return STD_ERR(BOARD, PARAM, EINVAL);
    }

    /* We've verified the length of the buffer is sufficient for the result */
    strcpy(value, result);

    return STD_ERR_OK;
}

/**
 * @brief Set the media vendor info value, given the resource handle and
 * vendor info type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   info_type       Type of parameter
 * @param[in]   len             Length of buffer
 * @param[in]   value           Value to save in the DB
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_vendor_info_set(db_sql_handle_t db_handle,
                                         sdi_resource_hdl_t media_hdl,
                                         sdi_media_vendor_info_type_t info_type,
                                         uint_t len,
                                         char *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    STD_ASSERT(value != NULL);

    sdi_db_cond_media_vendor(condition, sizeof(condition), media_hdl, info_type);
    snprintf(value_str, sizeof(value_str), "'%s'", value);

    return sdi_db_sql_set_attribute(db_handle, TABLE_MEDIA_VENDOR_INFO,
                                MEDIA_VENDOR_INFO_VALUE, value_str, condition);
}

/* Build the condition string for media module threshold */
static void sdi_db_cond_media_module_monitor(char *buf, size_t len,
                                             sdi_resource_hdl_t media_hdl,
                                             int channel,
                                             uint_t type)
{
    snprintf(buf, len, "%s=%u AND %s=%d AND %s=%u",
             TBL_RESOURCE_HDL, (uint_t)(uintptr_t)media_hdl,
             MEDIA_CHANNEL, channel,
             MEDIA_THRESHOLD_TYPE, type);
}

/**
 * @brief Retrieve the media monitor threshold value, given the resource handle,
 * channel and monitor type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel
 * @param[in]   type            Type of threshold
 * @param[out]  value           Pointer to a buffer to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_monitor_threshold_get(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               uint_t type,
                                               uint_t *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    t_std_error rc;

    STD_ASSERT(value != NULL);

    sdi_db_cond_media_module_monitor(condition, sizeof(condition), media_hdl,
                                     channel, type);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_MEDIA_THRESHOLD,
                              MEDIA_THRESHOLD_VALUE, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtol(result, (int *) value);
}



/**
 * @brief Set the media monitor threshold value, given the resource handle,
 * channel and monitor type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   channel         Channel
 * @param[in]   type            Type of threshold
 * @param[out]  value           Threshold value to save
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_monitor_threshold_set(db_sql_handle_t db_handle,
                                               sdi_resource_hdl_t media_hdl,
                                               int channel,
                                               uint_t type,
                                               uint_t value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    sdi_db_cond_media_module_monitor(condition, sizeof(condition), media_hdl,
                                     channel, type);
    snprintf(value_str, sizeof(value_str), "%u", value);
    return sdi_db_sql_set_attribute(db_handle, TABLE_MEDIA_THRESHOLD,
                                MEDIA_THRESHOLD_VALUE, condition, value_str);
}

/**
 * @brief Retrieve the media threshold value, given the resource handle,
 * and threshold type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   type            Type of threshold
 * @param[out]  value           Pointer to a buffer to save the value
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_threshold_get(db_sql_handle_t db_handle,
                                       sdi_resource_hdl_t media_hdl,
                                       sdi_media_threshold_type_t type,
                                       float *value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    t_std_error rc;

    STD_ASSERT(value != NULL);

    sdi_db_cond_media_module_monitor(condition, sizeof(condition), media_hdl,
                                     MEDIA_DEFAULT_CHANNEL, type);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_MEDIA_THRESHOLD,
                              MEDIA_THRESHOLD_VALUE, condition, result);
    if (rc != STD_ERR_OK) {
        return rc;
    }

    return sdi_db_strtod(result, value);
}

/**
 * @brief Set the media monitor threshold value, given the resource handle,
 * and threshold type.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   media_hdl       Handle to the media resource
 * @param[in]   type            Type of threshold
 * @param[in]   value           Threshold value to save
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_media_threshold_set(db_sql_handle_t db_handle,
                                       sdi_resource_hdl_t media_hdl,
                                       sdi_media_threshold_type_t type,
                                       float value)
{
    char condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char value_str[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];

    sdi_db_cond_media_module_monitor(condition, sizeof(condition), media_hdl,
                                     MEDIA_DEFAULT_CHANNEL, type);
    snprintf(value_str, sizeof(value_str), "%f", value);
    return sdi_db_sql_set_attribute(db_handle, TABLE_MEDIA_THRESHOLD,
                                MEDIA_THRESHOLD_VALUE, condition, value_str);
}

/**
 * @brief Get the entity_type given the resource handle.
 *
 * @param[in]   db_handle       Handle to the database
 * @param[in]   res_hdl         Handle to the resource
 * @param[out]  entity_type     Type of the parent entity
 *
 * @return STD_ERR_OK on success, error code on failure
 */
t_std_error sdi_db_get_entity_type(db_sql_handle_t db_handle,
                                   sdi_resource_hdl_t res_hdl,
                                   sdi_entity_type_t *entity_type)
{
    t_std_error rc = STD_ERR_OK;
    char        condition[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    char        result[SDI_DB_SQL_DEFAULT_BUFFER_LENGTH];
    int         e_type;

    STD_ASSERT(res_hdl != NULL);
    sdi_db_cond_resource_handle(condition, NULL, res_hdl);
    rc = sdi_db_sql_get_attribute(db_handle, TABLE_INFO, TBL_ENTITY_TYPE,
            condition, result);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    rc = sdi_db_strtol(result, &e_type);

    if (rc != STD_ERR_OK) {
        return rc;
    }

    /* Cast the retrieved value to an entity handle */
    *entity_type = (sdi_entity_type_t) e_type;
    return STD_ERR_OK;
}
