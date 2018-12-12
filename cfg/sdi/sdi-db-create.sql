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
 * \file sdi-sys-entity-vm.sql
 * \brief SQL code to create device database for sdi-sys-vm.
 *
 */

/* Drop all tables */
DROP TABLE IF EXISTS Entity_Info;
DROP TABLE IF EXISTS Entity_Resource;
DROP TABLE IF EXISTS Fan;
DROP TABLE IF EXISTS Thermal;
DROP TABLE IF EXISTS Physical_LED;
DROP TABLE IF EXISTS LED;
DROP TABLE IF EXISTS Digit_Display_LED;
DROP TABLE IF EXISTS PLD;
DROP TABLE IF EXISTS Media;
DROP TABLE IF EXISTS Media_Channel;
DROP TABLE IF EXISTS Media_Parameters;
DROP TABLE IF EXISTS Media_Vendor_Info;
DROP TABLE IF EXISTS Media_Monitor_Thresholds;
DROP TABLE IF EXISTS NVRAM;

CREATE TABLE Entity_Info (
    Entity_Handle       INTEGER,
    Entity_Type         INTEGER,
    Resource_Handle     INTEGER,
    Instance            INTEGER,
    Entity_Power        INTEGER,
    Entity_Reset        INTEGER,
    Entity_Name         TEXT,
    Product_Name        TEXT,
    PPID                TEXT,
    HW_Revision         TEXT,
    Platform_Name       TEXT,
    Vendor_Name         TEXT,
    Service_Tag         TEXT,
    Base_MAC            BLOB,
    MAC_Size            INTEGER,
    Num_Fans            INTEGER,
    Max_Speed           INTEGER,
    Air_Flow_Type       INTEGER,
    Power_Rating        INTEGER,
    Power_Type          INTEGER,
    Presence            BOOL,
    Fault_Status        BOOL,
    TARGET_Entity_Reset INTEGER,
    TARGET_Entity_Power INTEGER,

    PRIMARY KEY ( Entity_Handle )
);

CREATE TABLE Entity_Resource (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    Resource_Type       INTEGER,
    Alias               TEXT,

    PRIMARY KEY ( Resource_Handle ),
    FOREIGN KEY ( Entity_Handle ) REFERENCES Entity_Info(Entity_Handle)
);

CREATE TABLE Fan (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    Alert_On            BOOL,
    Speed               INTEGER,
    TARGET_Speed        INTEGER,

    FOREIGN KEY ( Resource_Handle ) REFERENCES Entity_Resource(Resource_Handle),
    FOREIGN KEY ( Entity_Handle ) REFERENCES Entity_Info(Entity_Handle)
);

CREATE TABLE Thermal (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    Alert_On            BOOL,
    Temp_C              INTEGER,
    Threshold_Low       INTEGER,
    Threshold_High      INTEGER,
    Threshold_Critical  INTEGER,
    TARGET_Threshold_Low        INTEGER,
    TARGET_Threshold_High       INTEGER,
    TARGET_Threshold_Critical   INTEGER,

    FOREIGN KEY ( Resource_Handle ) REFERENCES Entity_Resource(Resource_Handle),
    FOREIGN KEY ( Entity_Handle ) REFERENCES Entity_Info(Entity_Handle)
);

CREATE TABLE Physical_LED (
    Resource_Handle     INTEGER,
    Description         TEXT,
    LED_State           TEXT,

    PRIMARY KEY ( Resource_Handle )
);

CREATE TABLE LED (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    Phy_LED             INTEGER,
    LED_State_On        TEXT,
    LED_State_Off       TEXT,
    LED_On              BOOL,
    TARGET_LED_On       BOOL,

    FOREIGN KEY ( Resource_Handle ) REFERENCES Entity_Resource(Resource_Handle),
    FOREIGN KEY ( Entity_Handle ) REFERENCES Entity_Info(Entity_Handle)
);

CREATE TABLE Digit_Display_LED (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    Display_String      TEXT,
    Display_On          INTEGER,
    TARGET_Display_String   TEXT,
    TARGET_Display_On   INTEGER,

    FOREIGN KEY ( Resource_Handle ) REFERENCES Entity_Resource( Resource_Handle ),
    FOREIGN KEY ( Entity_Handle ) REFERENCES Entity_Info( Entity_Handle )
);

CREATE TABLE PLD (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    Firmware_Version    TEXT,

    PRIMARY KEY (Resource_Handle),
    FOREIGN KEY (Entity_Handle) REFERENCES Entity_Info(Entity_Handle)
);

CREATE TABLE Media (
    Entity_Handle       INTEGER,
    Resource_Handle     INTEGER,
    DELL_Qualified      INTEGER,
    Present             INTEGER,
    Optic_Speed         INTEGER,
    Vendor_OUI          BLOB,
    DELL_Product_Info   BLOB,
    Media_Supported_Features  BLOB,
    XCVR_Code           BLOB,
    Low_Power_Mode      INTEGER,
    Reset               INTEGER,
    TARGET_Low_Power_Mode   INTEGER,
    TARGET_Reset        INTEGER,

    PRIMARY KEY ( Resource_Handle )
);

CREATE TABLE Media_Channel (
    Resource_Handle     INTEGER,
    Channel             INTEGER,
    Channel_Status      INTEGER,
    Monitor_Status      INTEGER,
    TX_Enable           INTEGER,
    Temp_or_RX_Power    FLOAT,
    Volt_or_TX_BIAS     FLOAT,
    TARGET_TX_Enable    INTEGER,

    PRIMARY KEY ( Resource_Handle, Channel )
);

CREATE TABLE Media_Parameters (
    Resource_Handle     INTEGER,
    Parameter           INTEGER,
    Value               INTEGER,

    PRIMARY KEY ( Resource_Handle, Parameter )
);

CREATE TABLE Media_Vendor_Info (
    Resource_Handle     INTEGER,
    Vendor_Info         INTEGER,
    Value               TEXT,

    PRIMARY KEY ( Resource_Handle, Vendor_Info )
);

CREATE TABLE Media_Monitor_Thresholds (
    Resource_Handle     INTEGER,
    Channel             INTEGER,
    Threshold           INTEGER,
    Value               INTEGER,

    PRIMARY KEY ( Resource_Handle, Channel, Threshold )
);

CREATE TABLE NVRAM (
    Resource_Handle     INTEGER,
    Data_Size           INTEGER,
    Data_File           TEXT,
    Offset              INTEGER,

    PRIMARY KEY ( Resource_Handle )
);
