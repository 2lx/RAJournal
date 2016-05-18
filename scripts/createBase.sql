-- !!!!!
-- login as postgres !       
-- !!!!!

CREATE DATABASE redanchor;

CREATE USER raadmin WITH PASSWORD 'redanchoradmin';
GRANT ALL PRIVILEGES ON DATABASE redanchor TO raadmin WITH GRANT OPTION;

-- !!!!!
-- login as raadmin !       
-- !!!!!

CREATE TABLE profession (
	prID SERIAL PRIMARY KEY,
	prName text,
	prPhone text
);

CREATE TABLE specialist (
	spID SERIAL PRIMARY KEY,
	spName text NOT NULL,
	spSurname text,
	spPatronymic text,
	spPassword text,
	spProfessionID integer REFERENCES profession (prID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE department (
	dpID SERIAL PRIMARY KEY,
	dpShortName text,
	dpFullName text
);

CREATE TABLE machinemodel (
	mmID SERIAL PRIMARY KEY,
	mmName text
);

CREATE TABLE machine (
	maID SERIAL PRIMARY KEY,
	maShortName text,
	maFullName text,
	maImportance SMALLINT DEFAULT 1,
	maDepartmentID integer REFERENCES department (dpID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	maModelID integer REFERENCES machinemodel (mmID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE unit (
	unID SERIAL PRIMARY KEY,
	unShortName text,
	unFullName text,
	unModelID integer REFERENCES machinemodel (mmID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE sensortype (
	stID SERIAL PRIMARY KEY,
	stName text NOT NULL
);

CREATE TABLE sensor (
	seID SERIAL PRIMARY KEY,
	seShortName text NOT NULL,
	seFullName text NOT NULL,
	seSchemeName text,
	seProgramName text,
	seTypeID integer REFERENCES sensortype (stID)
		ON DELETE CASCADE ON UPDATE CASCADE,
-- 1 = sensor, 4 = signal, 2 = button, 3-valve
	seUnitID integer REFERENCES unit (unID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE detailmodeltype (
	dtID SERIAL PRIMARY KEY,
	dtName text NOT NULL
);

CREATE TABLE detailmodel (
	dmID SERIAL PRIMARY KEY,
	dmName text NOT NULL,
	dmTypeID integer REFERENCES detailmodeltype (dtID)
		ON DELETE CASCADE ON UPDATE CASCADE
-- 0 = unspecified, 1 = optical sensor, 2 = inductive sensor, 3 = magn. switch sensor, 
-- 4 = pulse sensor, 5 = position sensor, 6 = drive, 7 = other
);

CREATE TABLE symptom (
	syID SERIAL PRIMARY KEY,
	syText text,
	syUnitID integer REFERENCES unit (unID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE UNIQUE INDEX symptom_unique_index ON symptom (syText, syUnitID);

CREATE TABLE displayalarm (
	daID SERIAL PRIMARY KEY,
	daText text,
	daMachineID integer REFERENCES machine (maID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE reaction (
	reID SERIAL PRIMARY KEY,
	reSymptomID integer REFERENCES symptom (syID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	reAlarmID integer REFERENCES displayalarm (daID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE solution (
	soID SERIAL PRIMARY KEY,
	soText text,
	soSymptomID integer REFERENCES symptom (syID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	soSensorID integer REFERENCES sensor (seID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE journal (
	joID SERIAL PRIMARY KEY,
	joErrorDT TIMESTAMP,
	joSolveDT TIMESTAMP,
	joMachineID integer REFERENCES machine (maID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	joSpecialistID integer REFERENCES specialist (spID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	joSymptomID integer REFERENCES symptom (syID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE solutiongrouptype (
	sgtID SERIAL PRIMARY KEY,
	sgtName text NOT NULL
);

CREATE TABLE solutiongroup (
	sgID SERIAL PRIMARY KEY,
	sgTypeID integer REFERENCES solutiongrouptype (sgtID)
		ON DELETE CASCADE ON UPDATE CASCADE,
-- 0 = slack, 1 = repair, 2 = change, 3 = discard
	sgSolutionID integer REFERENCES solution (soID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	sgJournalID integer REFERENCES journal (joID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	sgDetailModelID integer REFERENCES detailmodel (dmID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE notification (
	noID SERIAL PRIMARY KEY,
	noSensors text,
	noSms text,
	noMachineID integer REFERENCES machine (maID)
		ON DELETE CASCADE ON UPDATE CASCADE,
	noProfessionID integer REFERENCES profession (prID)
		ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE VIEW v_specialist AS
	SELECT spID,
	sp.spSurname || ' '
	|| substring(sp.spName from 1 for 1) || '.'
	|| substring(sp.spPatronymic from 1 for 1) || '.' AS spFio,
	spPassword,
	spProfessionID
	FROM specialist sp
	ORDER BY sp.spSurname;

CREATE VIEW v_journal AS
    SELECT 	jo.joID,
			jo.joErrorDT,
			ma.maShortName || ': ' || un.unShortName,
			sy.syText,
			age( jo.joSolveDT, jo.joErrorDT),
			jo.joSolveDT,
			( SELECT COALESCE( array_to_string(array_agg(so2.soText), '***') , 'Не указаны' )
				FROM solution so2, solutiongroup sg2
				WHERE sg2.sgJournalID = jo.joID
				AND sg2.sgSolutionID = so2.soID ) clSolutions,
			sp.spFio,
			sp.spID,
			un.unID,
			jo.joMachineID ,
			dp.dpID,
			sy.syID
      FROM journal jo, symptom sy, v_specialist sp,
			machinemodel mm, unit un, machine ma, department dp
     WHERE jo.joSymptomID = sy.syID
       AND jo.joSpecialistID = sp.spID
       AND sy.syUnitID = un.unID
       AND un.unModelID = mm.mmID
       AND joMachineID = ma.maID
       AND ma.maDepartmentID = dp.dpID
       ORDER BY jo.joErrorDT;

CREATE VIEW v_notification AS
	SELECT 	no.noID,
			no.noSensors,
			ma.maShortName,
			pr.prName,
			no.noSms,
			no.noMachineID,
			no.noProfessionID
	FROM notification no, machine ma, profession pr
	WHERE no.noMachineID = ma.maID
	AND no.noProfessionID = pr.prID
	ORDER BY no.noSensors;

CREATE VIEW v_solution AS
	SELECT 	sg.sgID,
			sg.sgTypeID,
			sg.sgSolutionID,
			so.soText,
			so.soSensorID,
			se.seShortName,
			so.soSymptomID,
			sg.sgJournalID,
			sg.sgDetailModelID,
			dm.dmName
	FROM solutiongroup sg, solution so, detailmodel dm, sensor se
	WHERE sg.sgSolutionID = so.soID
	  AND sg.sgDetailModelID = dm.dmID
	  AND so.soSensorID = se.seID
	ORDER BY sg.sgID;


-- !!!!!
-- login as postgres !       
-- !!!!!

CREATE USER rauser WITH PASSWORD 'user';
GRANT CONNECT ON DATABASE redanchor TO rauser;

GRANT SELECT, REFERENCES ON profession TO rauser;
GRANT SELECT, REFERENCES ON specialist TO rauser;
GRANT SELECT, REFERENCES ON department TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON machine TO rauser;
GRANT USAGE, SELECT ON SEQUENCE machine_maid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON machinemodel TO rauser;
GRANT USAGE, SELECT ON SEQUENCE machinemodel_mmid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON unit TO rauser;
GRANT USAGE, SELECT ON SEQUENCE unit_unid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON sensortype TO rauser;
GRANT USAGE, SELECT ON SEQUENCE sensortype_stid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON sensor TO rauser;
GRANT USAGE, SELECT ON SEQUENCE sensor_seid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON detailmodeltype TO rauser;
GRANT USAGE, SELECT ON SEQUENCE detailmodeltype_dtid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON detailmodel TO rauser;
GRANT USAGE, SELECT ON SEQUENCE detailmodel_dmid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON symptom TO rauser;
GRANT USAGE, SELECT ON SEQUENCE symptom_syid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON displayalarm TO rauser;
GRANT USAGE, SELECT ON SEQUENCE displayalarm_daid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE ON reaction TO rauser;
GRANT USAGE, SELECT ON SEQUENCE reaction_reid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE, DELETE ON solution TO rauser;
GRANT USAGE, SELECT ON SEQUENCE solution_soid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE, DELETE ON journal TO rauser;
GRANT USAGE, SELECT ON SEQUENCE journal_joid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE, DELETE ON solutiongrouptype TO rauser;
GRANT USAGE, SELECT ON SEQUENCE solutiongrouptype_sgtid_seq TO rauser;

GRANT SELECT, REFERENCES, INSERT, UPDATE, DELETE ON solutiongroup TO rauser;
GRANT USAGE, SELECT ON SEQUENCE solutiongroup_sgid_seq TO rauser;

GRANT SELECT, REFERENCES, UPDATE ON notification TO rauser;
GRANT USAGE, SELECT ON SEQUENCE notification_noid_seq TO rauser;

GRANT SELECT, REFERENCES ON v_specialist TO rauser;
GRANT SELECT, REFERENCES ON v_journal TO rauser;
GRANT SELECT, REFERENCES ON v_notification TO rauser;
GRANT SELECT, REFERENCES ON v_solution TO rauser;
