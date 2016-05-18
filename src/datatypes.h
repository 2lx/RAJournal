/*
 * database.h
 *
 *  Created on: 17.11.2011
 *      Author: Aim
 */

#ifndef DATATYPES_H
#define DATATYPES_H

enum RAProfession { prID = 0, prName, prPhone, prColumnCount };
enum RASpecialist { spID = 0, spFIO, spPassword, spProfessionID, spColumnCount };
enum RADepartment { dpID = 0, dpShortName, dpFullName, dpColumnCount };
enum RAMachineModel { mmID = 0, mmName, mmColumnCount };
enum RAMachine { maID = 0, maShortName, maFullName, maImportance, maDepartmentID, maModelID, maColumnCount };
enum RAUnit { unID = 0, unShortName, unFullName, unModelID, unColumnCount };
enum RASensor { seID = 0, seShortName, seFullName, seProgramName, seSchemeName, seTypeID, seUnitID, seColumnCount };
enum RADetailModel { dmID = 0, dmName, dmTypeID, dmColumnCount };
enum RANotification { noID = 0, noSensors, noSms, noMachineID, noProfessionID, noColumnCount };

#endif // DATATYPES_H
