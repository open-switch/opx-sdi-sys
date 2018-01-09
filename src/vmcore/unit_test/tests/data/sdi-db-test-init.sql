/* Test data for running unit tests */
BEGIN TRANSACTION;

DELETE FROM "Entity_Info";
INSERT INTO "Entity_Info" VALUES(1, 0, 91, 1, 1, 2, 'S6000 System Board', 'S6000 System Board', '1', 'REV_1', 'S6000', 'DELL', 'S6KSVC', x'414243444546', 1, 0, 0, 0, 0, 0, 1, 0, 1, 2);
INSERT INTO "Entity_Info" VALUES(2, 2, 92, 1, 1, 2, 'S6000 Power Supply', 'Power Supply', '1', 'REV_1', 'S6000', 'DELL', 'S6KPSU', '', 0, 1, 19000, 0, 48, 2, 1, 0, 1, 2);
INSERT INTO "Entity_Info" VALUES(3, 2, 93, 2, 1, 2, 'S6000 Power Supply', 'Power Supply', '1', 'REV_1', 'S6000', 'DELL', 'S6KPSU', '', 0, 1, 19000, 0, 48, 2, 1, 0, 1, 2);
INSERT INTO "Entity_Info" VALUES(4, 1, 94, 1, 1, 2, 'S6000 Fan Tray', 'Fan Tray', '1', 'REV_1', 'S6000', 'DELL', 'S6KFTY', '', 0, 2, 18000, 0, 0, 0, 1, 0, 1, 2);
INSERT INTO "Entity_Info" VALUES(5, 1, 95, 2, 1, 2, 'S6000 Fan Tray', 'Fan Tray', '1', 'REV_1', 'S6000', 'DELL', 'S6KFTY', '', 0, 2, 18000, 0, 0, 0, 1, 0, 1, 2);
INSERT INTO "Entity_Info" VALUES(6, 1, 96, 3, 1, 2, 'S6000 Fan Tray', 'Fan Tray', '1', 'REV_1', 'S6000', 'DELL', 'S6KFTY', '', 0, 2, 18000, 0, 0, 0, 1, 0, 1, 2);

DELETE FROM "Entity_Resource";
INSERT INTO "Entity_Resource" VALUES(1, 91, 4, 'Info');
INSERT INTO "Entity_Resource" VALUES(2, 92, 4, 'Info');
INSERT INTO "Entity_Resource" VALUES(3, 93, 4, 'Info');
INSERT INTO "Entity_Resource" VALUES(4, 94, 4, 'Info');
INSERT INTO "Entity_Resource" VALUES(5, 95, 4, 'Info');
INSERT INTO "Entity_Resource" VALUES(6, 96, 4, 'Info');
INSERT INTO "Entity_Resource" VALUES(1, 7, 2, 'Alarm Major');
INSERT INTO "Entity_Resource" VALUES(1, 8, 2, 'Alarm Minor');
INSERT INTO "Entity_Resource" VALUES(1, 9, 2, 'PSU Fault');
INSERT INTO "Entity_Resource" VALUES(1, 10, 2, 'Fan Fault');
INSERT INTO "Entity_Resource" VALUES(1, 11, 2, 'Beacon');
INSERT INTO "Entity_Resource" VALUES(1, 12, 2, 'Stack Master');
INSERT INTO "Entity_Resource" VALUES(4, 13, 2, 'Fantray Fault');
INSERT INTO "Entity_Resource" VALUES(5, 14, 2, 'Fantray Fault');
INSERT INTO "Entity_Resource" VALUES(6, 15, 2, 'Fantray Fault');
INSERT INTO "Entity_Resource" VALUES(1, 16, 0, 'Trident Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(1, 17, 0, 'NIC Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(1, 18, 0, 'Ambient Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(1, 19, 0, 'BCM Onboard Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(2, 20, 1, 'PSU Fan');
INSERT INTO "Entity_Resource" VALUES(3, 21, 1, 'PSU Fan');
INSERT INTO "Entity_Resource" VALUES(4, 22, 1, 'Fantray Fan 1');
INSERT INTO "Entity_Resource" VALUES(4, 23, 1, 'Fantray Fan 2');
INSERT INTO "Entity_Resource" VALUES(5, 24, 1, 'Fantray Fan 1');
INSERT INTO "Entity_Resource" VALUES(5, 25, 1, 'Fantray Fan 2');
INSERT INTO "Entity_Resource" VALUES(6, 26, 1, 'Fantray Fan 1');
INSERT INTO "Entity_Resource" VALUES(6, 27, 1, 'Fantray Fan 2');
INSERT INTO "Entity_Resource" VALUES(1, 28, 5, 'Main CPLD');
INSERT INTO "Entity_Resource" VALUES(1, 29, 5, 'Test CPLD');
INSERT INTO "Entity_Resource" VALUES(1, 30, 6, 'QSFP 1');
INSERT INTO "Entity_Resource" VALUES(1, 31, 3, 'Stack LED');

DELETE FROM "Fan";
INSERT INTO "Fan" VALUES(2,20,0,60,60);
INSERT INTO "Fan" VALUES(3,21,0,60,60);
INSERT INTO "Fan" VALUES(4,22,1,80,80);
INSERT INTO "Fan" VALUES(4,23,0,100,100);
INSERT INTO "Fan" VALUES(5,24,0,60,60);
INSERT INTO "Fan" VALUES(5,25,0,60,60);
INSERT INTO "Fan" VALUES(6,26,0,60,60);
INSERT INTO "Fan" VALUES(6,27,0,60,60);

DELETE FROM "THERMAL";
INSERT INTO "THERMAL" VALUES(1,16,0,35,60,40,80,60,40,80);
INSERT INTO "THERMAL" VALUES(1,17,1,65,45,55,65,45,55,65);
INSERT INTO "THERMAL" VALUES(1,18,0,35,60,40,80,60,40,80);
INSERT INTO "THERMAL" VALUES(1,19,0,35,60,40,80,60,40,80);

DELETE FROM "Physical_LED";
INSERT INTO "Physical_LED" VALUES(1, 'System LED', 'Off');
INSERT INTO "Physical_LED" VALUES(2, 'Power LED', 'Off');
INSERT INTO "Physical_LED" VALUES(3, 'Fan LED', 'Off');
INSERT INTO "Physical_LED" VALUES(4, 'Stack Master LED', 'Off');
INSERT INTO "Physical_LED" VALUES(5, 'Beacon LED', 'Off');
INSERT INTO "Physical_LED" VALUES(6, 'Fantray 1 LED', 'Off');
INSERT INTO "Physical_LED" VALUES(7, 'Fantray 2 LED', 'Off');
INSERT INTO "Physical_LED" VALUES(8, 'Fantray 3 LED', 'Off');

DELETE FROM "LED";
INSERT INTO "LED" VALUES(1,7,1,'Blinking Amber','Solid Green',0,0);
INSERT INTO "LED" VALUES(1,8,1,'Solid Amber','Solid Green',0,0);
INSERT INTO "LED" VALUES(1,9,2,'Solid Amber','Solid Green',0,0);
INSERT INTO "LED" VALUES(1,10,3,'Solid Amber','Solid Green',0,0);
INSERT INTO "LED" VALUES(1,11,5,'Blinking Blue','Solid Blue',0,0);
INSERT INTO "LED" VALUES(1,12,4,'Solid Green','Off',0,0);
INSERT INTO "LED" VALUES(1,13,6,'Solid Amber','Solid Green',0,0);
INSERT INTO "LED" VALUES(1,14,7,'Solid Amber','Solid Green',0,0);
INSERT INTO "LED" VALUES(1,15,8,'Solid Amber','Solid Green',0,0);

DELETE FROM "Digit_Display_LED";
INSERT INTO "Digit_Display_LED" VALUES(1, 31, 'Hello', 1, 'Hello', 1);

DELETE FROM "PLD";
INSERT INTO "PLD" VALUES(1, 28, "2.0");
INSERT INTO "PLD" VALUES(1, 29, "Supercalifragilisticexpialidocious");

DELETE FROM "Media";
INSERT INTO "Media" VALUES(1, 30, 1, 1, 3, x'313233', x'0F100011114000', x'000000', x'0123456789ABCDEF', 0, 0, 0, 0);

DELETE FROM "Media_Channel";
INSERT INTO "Media_Channel" VALUES(30, 65535, 0, 255, 0, 98.4, 6.023, 0);
INSERT INTO "Media_Channel" VALUES(30,  0, 8, 255, 1, 1.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(30,  1, 0, 0, 1, 1.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(30,  2, 0, 0, 1, 1.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(30,  3, 0, 0, 1, 1.4142, 3.14159, 1);

DELETE FROM "Media_Parameters";
INSERT INTO "Media_Parameters" VALUES(30, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(30, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(30, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(30, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(30, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(30, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(30, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(30, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(30, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(30, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(30, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(30, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(30, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(30, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(30, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(30, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(30, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(30, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(30, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(30, 19, 9);    /* Device tech */

DELETE FROM "Media_Vendor_Info";
INSERT INTO "Media_Vendor_Info" VALUES(30, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(30, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(30, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(30, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(30, 5, "4");

DELETE FROM "Media_Monitor_Thresholds";
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(30, 0, 128, 4);
COMMIT;
