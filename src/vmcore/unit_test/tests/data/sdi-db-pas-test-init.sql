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
INSERT INTO "Entity_Resource" VALUES(1, 91, 4, 'Entity Info');
INSERT INTO "Entity_Resource" VALUES(2, 92, 4, 'Entity Info');
INSERT INTO "Entity_Resource" VALUES(3, 93, 4, 'Entity Info');
INSERT INTO "Entity_Resource" VALUES(4, 94, 4, 'Entity Info');
INSERT INTO "Entity_Resource" VALUES(5, 95, 4, 'Entity Info');
INSERT INTO "Entity_Resource" VALUES(6, 96, 4, 'Entity Info');
INSERT INTO "Entity_Resource" VALUES(1, 7, 2, 'System Booting LED');
INSERT INTO "Entity_Resource" VALUES(1, 8, 2, 'System Normal LED');
INSERT INTO "Entity_Resource" VALUES(1, 9, 2, 'System Major LED');
INSERT INTO "Entity_Resource" VALUES(1, 10, 2, 'System Minor LED');
INSERT INTO "Entity_Resource" VALUES(1, 11, 2, 'PSU Faulty LED');
INSERT INTO "Entity_Resource" VALUES(1, 12, 2, 'Fantray Faulty LED');
INSERT INTO "Entity_Resource" VALUES(1, 13, 2, 'POST Test LED');
INSERT INTO "Entity_Resource" VALUES(1, 14, 2, 'Location LED');
INSERT INTO "Entity_Resource" VALUES(1, 70, 2, 'Stack Master LED');
INSERT INTO "Entity_Resource" VALUES(1, 15, 3, 'Stack LED');
INSERT INTO "Entity_Resource" VALUES(1, 16, 0, 'Trident Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(1, 17, 0, 'NIC Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(1, 18, 0, 'Ambient Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(1, 19, 0, 'BCM Onboard Thermal Sensor');
INSERT INTO "Entity_Resource" VALUES(2, 20, 1, 'PSU 1 Fan');
INSERT INTO "Entity_Resource" VALUES(3, 21, 1, 'PSU 2 Fan');
INSERT INTO "Entity_Resource" VALUES(4, 22, 1, 'Fantray 1 Fan 1');
INSERT INTO "Entity_Resource" VALUES(4, 23, 1, 'Fantray 1 Fan 2');
INSERT INTO "Entity_Resource" VALUES(5, 24, 1, 'Fantray 2 Fan 1');
INSERT INTO "Entity_Resource" VALUES(5, 25, 1, 'Fantray 2 Fan 2');
INSERT INTO "Entity_Resource" VALUES(6, 26, 1, 'Fantray 3 Fan 1');
INSERT INTO "Entity_Resource" VALUES(6, 27, 1, 'Fantray 3 Fan 2');
INSERT INTO "Entity_Resource" VALUES(1, 28, 5, 'Main CPLD');
INSERT INTO "Entity_Resource" VALUES(1, 30, 6, 'QSFP 1');
INSERT INTO "Entity_Resource" VALUES(1, 31, 6, 'QSFP 2');
INSERT INTO "Entity_Resource" VALUES(1, 32, 6, 'QSFP 3');
INSERT INTO "Entity_Resource" VALUES(1, 33, 6, 'QSFP 4');
INSERT INTO "Entity_Resource" VALUES(1, 34, 6, 'QSFP 5');
INSERT INTO "Entity_Resource" VALUES(1, 35, 6, 'QSFP 6');
INSERT INTO "Entity_Resource" VALUES(1, 36, 6, 'QSFP 7');
INSERT INTO "Entity_Resource" VALUES(1, 37, 6, 'QSFP 8');
INSERT INTO "Entity_Resource" VALUES(1, 38, 6, 'QSFP 9');
INSERT INTO "Entity_Resource" VALUES(1, 39, 6, 'QSFP 10');
INSERT INTO "Entity_Resource" VALUES(1, 40, 6, 'QSFP 11');
INSERT INTO "Entity_Resource" VALUES(1, 41, 6, 'QSFP 12');
INSERT INTO "Entity_Resource" VALUES(1, 42, 6, 'QSFP 13');
INSERT INTO "Entity_Resource" VALUES(1, 43, 6, 'QSFP 14');
INSERT INTO "Entity_Resource" VALUES(1, 44, 6, 'QSFP 15');
INSERT INTO "Entity_Resource" VALUES(1, 45, 6, 'QSFP 16');
INSERT INTO "Entity_Resource" VALUES(1, 46, 6, 'QSFP 17');
INSERT INTO "Entity_Resource" VALUES(1, 47, 6, 'QSFP 18');
INSERT INTO "Entity_Resource" VALUES(1, 48, 6, 'QSFP 19');
INSERT INTO "Entity_Resource" VALUES(1, 49, 6, 'QSFP 20');
INSERT INTO "Entity_Resource" VALUES(1, 50, 6, 'QSFP 21');
INSERT INTO "Entity_Resource" VALUES(1, 51, 6, 'QSFP 22');
INSERT INTO "Entity_Resource" VALUES(1, 52, 6, 'QSFP 23');
INSERT INTO "Entity_Resource" VALUES(1, 53, 6, 'QSFP 24');
INSERT INTO "Entity_Resource" VALUES(1, 54, 6, 'QSFP 25');
INSERT INTO "Entity_Resource" VALUES(1, 55, 6, 'QSFP 26');
INSERT INTO "Entity_Resource" VALUES(1, 56, 6, 'QSFP 27');
INSERT INTO "Entity_Resource" VALUES(1, 57, 6, 'QSFP 28');
INSERT INTO "Entity_Resource" VALUES(1, 58, 6, 'QSFP 29');
INSERT INTO "Entity_Resource" VALUES(1, 59, 6, 'QSFP 30');
INSERT INTO "Entity_Resource" VALUES(1, 60, 6, 'QSFP 31');
INSERT INTO "Entity_Resource" VALUES(1, 61, 6, 'QSFP 32');

DELETE FROM "Fan";
INSERT INTO "Fan" VALUES(2,20,0,0,0);
INSERT INTO "Fan" VALUES(3,21,0,0,0);
INSERT INTO "Fan" VALUES(4,22,0,0,0);
INSERT INTO "Fan" VALUES(4,23,0,0,0);
INSERT INTO "Fan" VALUES(5,24,0,0,0);
INSERT INTO "Fan" VALUES(5,25,0,0,0);
INSERT INTO "Fan" VALUES(6,26,0,0,0);
INSERT INTO "Fan" VALUES(6,27,0,0,0);

DELETE FROM "Thermal";
INSERT INTO "Thermal" VALUES(1,16,0,35,60,40,80,60,40,80);
INSERT INTO "Thermal" VALUES(1,17,1,65,45,55,65,45,55,65);
INSERT INTO "Thermal" VALUES(1,18,0,35,60,40,80,60,40,80);
INSERT INTO "Thermal" VALUES(1,19,0,35,60,40,80,60,40,80);

DELETE FROM "LED";
INSERT INTO "LED" VALUES(1,7,0,0);
INSERT INTO "LED" VALUES(1,8,1,1);
INSERT INTO "LED" VALUES(1,9,0,0);
INSERT INTO "LED" VALUES(1,10,0,0);
INSERT INTO "LED" VALUES(1,11,0,0);
INSERT INTO "LED" VALUES(1,12,0,0);
INSERT INTO "LED" VALUES(1,13,0,0);
INSERT INTO "LED" VALUES(1,14,0,0);
INSERT INTO "LED" VALUES(1,70,1,1);

DELETE FROM "Digit_Display_LED";
INSERT INTO "Digit_Display_LED" VALUES(1, 15, 'Hello', 1, 'Hello', 1);

DELETE FROM "PLD";
INSERT INTO "PLD" VALUES(1, 28, "2");

DELETE FROM "Media";
INSERT INTO "Media" VALUES(1, 30, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 31, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 32, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 33, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 34, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 35, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 36, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 37, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 38, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 39, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 40, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 41, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 42, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 43, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 44, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 45, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 46, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 47, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 48, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 49, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 50, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 51, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 52, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 53, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 54, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 55, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 56, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 57, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 58, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 59, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 60, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);
INSERT INTO "Media" VALUES(1, 61, 1, 1, 3, x'313233', x'0F100011114000', x'2000000700000000', 0, 0, 0, 0);

DELETE FROM "Media_Channel";
INSERT INTO "Media_Channel" VALUES(30, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(30,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(30,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(30,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(30,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(31, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(31,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(31,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(31,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(31,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(32, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(32,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(32,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(32,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(32,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(33, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(33,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(33,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(33,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(33,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(34, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(34,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(34,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(34,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(34,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(35, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(35,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(35,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(35,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(35,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(36, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(36,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(36,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(36,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(36,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(37, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(37,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(37,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(37,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(37,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(38, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(38,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(38,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(38,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(38,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(39, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(39,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(39,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(39,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(39,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(40, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(40,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(40,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(40,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(40,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(41, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(41,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(41,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(41,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(41,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(42, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(42,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(42,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(42,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(42,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(43, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(43,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(43,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(43,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(43,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(44, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(44,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(44,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(44,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(44,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(45, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(45,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(45,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(45,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(45,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(46, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(46,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(46,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(46,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(46,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(47, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(47,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(47,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(47,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(47,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(48, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(48,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(48,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(48,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(48,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(49, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(49,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(49,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(49,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(49,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(50, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(50,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(50,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(50,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(50,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(51, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(51,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(51,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(51,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(51,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(52, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(52,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(52,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(52,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(52,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(53, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(53,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(53,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(53,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(53,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(54, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(54,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(54,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(54,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(54,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(55, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(55,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(55,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(55,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(55,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(56, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(56,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(56,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(56,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(56,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(57, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(57,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(57,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(57,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(57,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(58, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(58,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(58,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(58,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(58,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(59, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(59,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(59,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(59,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(59,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(60, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(60,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(60,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(60,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(60,  3, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(61, 65535, 0, 0, 0, 98.4, 3.023, 0);
INSERT INTO "Media_Channel" VALUES(61,  0, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(61,  1, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(61,  2, 0, 0, 1, 5.4142, 3.14159, 1);
INSERT INTO "Media_Channel" VALUES(61,  3, 0, 0, 1, 5.4142, 3.14159, 1);

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
INSERT INTO "Media_Parameters" VALUES(30, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(31, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(31, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(31, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(31, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(31, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(31, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(31, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(31, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(31, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(31, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(31, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(31, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(31, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(31, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(31, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(31, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(31, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(31, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(31, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(31, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(32, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(32, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(32, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(32, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(32, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(32, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(32, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(32, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(32, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(32, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(32, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(32, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(32, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(32, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(32, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(32, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(32, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(32, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(32, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(32, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(33, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(33, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(33, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(33, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(33, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(33, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(33, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(33, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(33, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(33, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(33, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(33, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(33, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(33, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(33, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(33, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(33, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(33, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(33, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(33, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(34, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(34, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(34, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(34, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(34, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(34, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(34, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(34, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(34, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(34, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(34, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(34, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(34, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(34, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(34, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(34, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(34, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(34, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(34, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(34, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(35, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(35, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(35, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(35, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(35, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(35, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(35, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(35, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(35, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(35, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(35, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(35, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(35, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(35, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(35, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(35, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(35, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(35, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(35, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(35, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(36, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(36, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(36, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(36, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(36, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(36, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(36, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(36, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(36, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(36, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(36, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(36, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(36, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(36, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(36, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(36, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(36, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(36, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(36, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(36, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(37, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(37, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(37, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(37, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(37, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(37, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(37, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(37, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(37, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(37, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(37, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(37, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(37, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(37, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(37, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(37, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(37, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(37, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(37, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(37, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(38, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(38, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(38, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(38, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(38, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(38, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(38, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(38, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(38, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(38, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(38, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(38, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(38, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(38, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(38, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(38, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(38, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(38, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(38, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(39, 18, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(39, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(39, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(39, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(39, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(39, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(39, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(39, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(39, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(39, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(39, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(39, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(39, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(39, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(39, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(39, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(39, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(39, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(39, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(39, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(39, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(40, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(40, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(40, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(40, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(40, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(40, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(40, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(40, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(40, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(40, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(40, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(40, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(40, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(40, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(40, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(40, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(40, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(40, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(40, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(40, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(41, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(41, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(41, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(41, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(41, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(41, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(41, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(41, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(41, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(41, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(41, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(41, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(41, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(41, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(41, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(41, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(41, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(41, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(41, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(41, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(42, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(42, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(42, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(42, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(42, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(42, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(42, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(42, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(42, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(42, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(42, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(42, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(42, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(42, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(42, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(42, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(42, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(42, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(42, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(42, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(43, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(43, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(43, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(43, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(43, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(43, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(43, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(43, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(43, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(43, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(43, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(43, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(43, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(43, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(43, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(43, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(43, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(43, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(43, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(43, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(44, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(44, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(44, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(44, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(44, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(44, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(44, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(44, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(44, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(44, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(44, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(44, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(44, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(44, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(44, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(44, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(44, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(44, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(44, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(44, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(45, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(45, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(45, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(45, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(45, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(45, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(45, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(45, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(45, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(45, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(45, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(45, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(45, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(45, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(45, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(45, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(45, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(45, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(45, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(45, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(46, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(46, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(46, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(46, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(46, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(46, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(46, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(46, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(46, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(46, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(46, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(46, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(46, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(46, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(46, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(46, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(46, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(46, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(46, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(46, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(47, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(47, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(47, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(47, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(47, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(47, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(47, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(47, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(47, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(47, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(47, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(47, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(47, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(47, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(47, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(47, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(47, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(47, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(47, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(47, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(48, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(48, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(48, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(48, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(48, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(48, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(48, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(48, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(48, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(48, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(48, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(48, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(48, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(48, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(48, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(48, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(48, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(48, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(48, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(49, 18, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(49, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(49, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(49, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(49, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(49, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(49, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(49, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(49, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(49, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(49, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(49, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(49, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(49, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(49, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(49, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(49, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(49, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(49, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(49, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(49, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(50, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(50, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(50, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(50, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(50, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(50, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(50, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(50, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(50, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(50, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(50, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(50, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(50, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(50, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(50, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(50, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(50, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(50, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(50, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(50, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(51, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(51, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(51, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(51, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(51, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(51, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(51, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(51, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(51, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(51, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(51, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(51, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(51, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(51, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(51, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(51, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(51, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(51, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(51, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(51, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(52, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(52, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(52, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(52, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(52, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(52, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(52, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(52, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(52, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(52, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(52, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(52, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(52, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(52, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(52, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(52, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(52, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(52, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(52, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(52, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(53, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(53, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(53, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(53, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(53, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(53, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(53, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(53, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(53, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(53, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(53, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(53, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(53, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(53, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(53, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(53, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(53, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(53, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(53, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(53, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(54, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(54, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(54, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(54, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(54, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(54, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(54, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(54, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(54, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(54, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(54, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(54, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(54, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(54, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(54, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(54, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(54, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(54, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(54, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(54, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(55, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(55, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(55, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(55, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(55, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(55, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(55, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(55, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(55, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(55, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(55, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(55, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(55, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(55, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(55, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(55, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(55, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(55, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(55, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(55, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(56, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(56, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(56, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(56, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(56, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(56, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(56, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(56, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(56, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(56, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(56, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(56, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(56, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(56, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(56, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(56, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(56, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(56, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(56, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(56, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(57, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(57, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(57, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(57, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(57, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(57, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(57, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(57, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(57, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(57, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(57, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(57, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(57, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(57, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(57, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(57, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(57, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(57, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(57, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(57, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(58, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(58, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(58, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(58, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(58, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(58, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(58, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(58, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(58, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(58, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(58, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(58, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(58, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(58, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(58, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(58, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(58, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(58, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(58, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(59, 18, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(59, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(59, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(59, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(59, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(59, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(59, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(59, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(59, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(59, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(59, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(59, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(59, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(59, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(59, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(59, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(59, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(59, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(59, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(59, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(59, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(60, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(60, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(60, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(60, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(60, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(60, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(60, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(60, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(60, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(60, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(60, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(60, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(60, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(60, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(60, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(60, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(60, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(60, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(60, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(60, 19, 7);    /* Device tech */
INSERT INTO "Media_Parameters" VALUES(61, 0, 850);   /* Wavelength */
INSERT INTO "Media_Parameters" VALUES(61, 1, 2);     /* Wavelength tolerance */
INSERT INTO "Media_Parameters" VALUES(61, 2, 60);    /* Max Case temp (C) */
INSERT INTO "Media_Parameters" VALUES(61, 3, 123);   /* Check code base */
INSERT INTO "Media_Parameters" VALUES(61, 4, 231);   /* Check code extended */
INSERT INTO "Media_Parameters" VALUES(61, 5, 1);     /* Connector */
INSERT INTO "Media_Parameters" VALUES(61, 6, 0);     /* Encoding type */
INSERT INTO "Media_Parameters" VALUES(61, 7, 400);   /* Nominal bitrate */
INSERT INTO "Media_Parameters" VALUES(61, 8, 3);     /* Identifier */
INSERT INTO "Media_Parameters" VALUES(61, 9, 4);     /* Extended Identifier */
INSERT INTO "Media_Parameters" VALUES(61, 10, 1);    /* Length SMF (km) */
INSERT INTO "Media_Parameters" VALUES(61, 11, 1024); /* Length OM1 (m) */
INSERT INTO "Media_Parameters" VALUES(61, 12, 512);  /* Length OM2 (m) */
INSERT INTO "Media_Parameters" VALUES(61, 13, 128);  /* Length OM3 (2m) */
INSERT INTO "Media_Parameters" VALUES(61, 14, 0);    /* Length cable asembly */
INSERT INTO "Media_Parameters" VALUES(61, 15, 0);    /* Length SMF */
INSERT INTO "Media_Parameters" VALUES(61, 16, 0);    /* Options */
INSERT INTO "Media_Parameters" VALUES(61, 17, 0);    /* Enhanced options */
INSERT INTO "Media_Parameters" VALUES(61, 18, 7);    /* Diag monitoring type */
INSERT INTO "Media_Parameters" VALUES(61, 19, 7);    /* Device tech */

DELETE FROM "Media_Vendor_Info";
INSERT INTO "Media_Vendor_Info" VALUES(30, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(30, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(30, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(30, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(30, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(31, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(31, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(31, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(31, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(31, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(32, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(32, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(32, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(32, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(32, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(33, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(33, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(33, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(33, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(33, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(34, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(34, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(34, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(34, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(34, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(35, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(35, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(35, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(35, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(35, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(36, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(36, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(36, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(36, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(36, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(37, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(37, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(37, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(37, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(37, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(38, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(38, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(38, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(38, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(38, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(39, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(39, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(39, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(39, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(39, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(40, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(40, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(40, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(40, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(40, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(41, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(41, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(41, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(41, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(41, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(42, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(42, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(42, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(42, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(42, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(43, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(43, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(43, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(43, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(43, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(44, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(44, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(44, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(44, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(44, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(45, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(45, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(45, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(45, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(45, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(46, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(46, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(46, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(46, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(46, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(47, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(47, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(47, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(47, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(47, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(48, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(48, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(48, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(48, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(48, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(49, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(49, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(49, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(49, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(49, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(50, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(50, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(50, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(50, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(50, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(51, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(51, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(51, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(51, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(51, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(52, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(52, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(52, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(52, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(52, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(53, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(53, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(53, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(53, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(53, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(54, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(54, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(54, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(54, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(54, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(55, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(55, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(55, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(55, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(55, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(56, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(56, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(56, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(56, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(56, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(57, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(57, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(57, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(57, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(57, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(58, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(58, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(58, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(58, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(58, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(59, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(59, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(59, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(59, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(59, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(60, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(60, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(60, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(60, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(60, 5, "4");
INSERT INTO "Media_Vendor_Info" VALUES(61, 0, "Finisar");
INSERT INTO "Media_Vendor_Info" VALUES(61, 2, "123456");
INSERT INTO "Media_Vendor_Info" VALUES(61, 3, "20150512");
INSERT INTO "Media_Vendor_Info" VALUES(61, 4, "FTL410QE3C-FC");
INSERT INTO "Media_Vendor_Info" VALUES(61, 5, "4");

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
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(31, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(32, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(33, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(34, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(35, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(36, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(37, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(38, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(39, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(40, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(41, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(42, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(43, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(44, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(45, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(46, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(47, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(48, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(49, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(50, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(51, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(52, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(53, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(54, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(55, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(56, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(57, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(58, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(59, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(60, 0, 128, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 1, 120);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 2, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 4, 100);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 8, 50);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 16, 5);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 32, 1);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 64, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 65535, 128, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 1, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 2, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 4, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 8, 4);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 16, 20);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 32, 2);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 64, 18);
INSERT INTO "Media_Monitor_Thresholds" VALUES(61, 0, 128, 4);
COMMIT;
