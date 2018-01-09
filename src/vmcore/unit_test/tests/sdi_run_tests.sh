#!/bin/bash

SDI_CFG_DIR=/etc/opx/sdi
SQL_CREATE=sdi-db-create.sql
DN_SDI_DB_NAME=vm-test.db
DN_SDI_DB_INIT=sdi-db-test-init.sql
DN_SDI_DB_SEM_KEY=0x564d5554    # VMUT
BIN_DIR=$(dirname $0)
DN_SDI_DB_BASE_DIR=$(realpath $BIN_DIR/data/)
TEST_DB="$DN_SDI_DB_BASE_DIR/$DN_SDI_DB_NAME"

# Copy the create script over to the test data folder
cp $SDI_CFG_DIR/$SQL_CREATE $DN_SDI_DB_BASE_DIR

# Export the environment variable that tells SDI-DB to load the test database
export DN_SDI_DB_BASE_DIR
export DN_SDI_DB_NAME
export DN_SDI_DB_INIT
export DN_SDI_DB_SEM_KEY

# Cleanup the semaphore in case we have an old one lying around
ipcrm -S $DN_SDI_DB_SEM_KEY

# Wrapper function to run the tests and abort early if necessary
run_test()
{
    local test_prog=$1

    # Run the test
    $BIN_DIR/$test_prog

    # Save the return value
    local retval=$?

    if [[ "$retval" != "0" ]]
    then
        cleanup
        exit $retval
    fi
}

# Cleanup function
cleanup()
{
    ipcrm -S $DN_SDI_DB_SEM_KEY
    rm -f $TEST_DB
    unset DN_SDI_DB_BASE_DIR
    unset DN_SDI_DB_NAME
}

# Run the individual tests
run_test sdi_vm_entity_info_unittest
run_test sdi_vm_fan_unittest
run_test sdi_vm_led_unittest
run_test sdi_vm_media_unittest
run_test sdi_vm_thermal_unittest

# Cleanup and exit
cleanup
