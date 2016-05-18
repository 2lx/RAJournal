/*
 * database.cpp
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "database.h"

bool RADB::setPSQLConnection( const QString & host, const QString & login, const QString & password )
{
    QString dataBaseName = "redanchor";
    int port = 5432;

    QSqlDatabase m_database = QSqlDatabase::addDatabase( "QPSQL" );
    m_database.setHostName( host );
    m_database.setDatabaseName( dataBaseName );
    m_database.setUserName( login );
    m_database.setPassword( password );
    m_database.setPort( port );
    m_database.setConnectOptions( "requiressl=1" );

    if ( !m_database.open() )
    {
    	m_database.setConnectOptions();
		if ( !m_database.open() )
		{
			QMessageBox::warning( 0, QObject::tr( "Database Error" ), m_database.lastError().databaseText() );
			return false;
		}
    }


    return true;
}

void RADB::discardPSQLConnection()
{
	QSqlDatabase::removeDatabase( "QPSQL" );
}


QString RADB::loginQuery()
{
	return "SELECT * FROM v_specialist WHERE NOT spID = 0";
}
/*********************************************************************************************
 * Symptom
 **********************************************************************************************/
QString RADB::Symptom::sqlSelectAll = "SELECT syID, syText, syUnitID FROM symptom sy ORDER BY syText";
QString RADB::Symptom::strZeroString = QString::fromLocal8Bit( "Проблема не описана" );
int RADB::Symptom::p_needDelete = -1;
QSqlQueryModel RADB::Symptom::m_model;

bool RADB::Symptom::refresh()
{
	m_model.setQuery( RADB::Symptom::sqlSelectAll );
	if ( m_model.lastError().isValid() )
		QMessageBox::about( 0, "", m_model.lastError().databaseText() );

	return !m_model.lastError().isValid();
}

int RADB::Symptom::insertUnique( const QString & text, int unitID )
{
	QSqlQueryModel modelTemp;
	QString strSql = "SELECT sy.syID FROM symptom sy WHERE sy.syText = '" + text +
			"' AND sy.syUnitID = " + QString::number( unitID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about(0, "", modelTemp.lastError().databaseText() );
	if ( modelTemp.rowCount() > 0 )
		return modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();

	strSql = "INSERT INTO symptom (syUnitID, syText) "
			"VALUES( " + QString::number( unitID ) + ", '" + text + "') RETURNING syID";
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about(0, "", modelTemp.lastError().databaseText() );

	return modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();
}

int RADB::Symptom::updateUnique( int syID, const QString & text, int unitID )
{
	QSqlQueryModel modelTemp;
	QString strSql = "SELECT jo.joID FROM journal jo WHERE jo.joSymptomID = " + QString::number( syID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	bool isAlone = modelTemp.rowCount() < 2;

	strSql = "SELECT sy.syID FROM symptom sy WHERE sy.syText = '" + text +
			"' AND sy.syUnitID = " + QString::number( unitID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about(0, "", modelTemp.lastError().databaseText() );
	int iNewId = 0;
	if ( modelTemp.rowCount() > 0 )
		iNewId = modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();

	p_needDelete = -1;
	if ( isAlone && !iNewId )
	{
		QString strQuery = "UPDATE symptom SET syUnitID = " + QString::number( unitID ) +
				", syText = '" + text + "' WHERE syID = " + QString::number( syID );
		QSqlQuery query( strQuery );
		return syID;
	}
	else if ( isAlone && iNewId && ( iNewId != syID ) )
	{
		p_needDelete = syID;
		return iNewId;
	}
	else if ( !isAlone && !iNewId )
		return insertUnique( text, unitID );
	else if ( !isAlone && iNewId )
		return iNewId;

	return syID;
}

void RADB::Symptom::deleteQuery()
{
	if ( p_needDelete > 0 )
	{
		QString strQuery = QString( "DELETE FROM symptom WHERE syID = %1" ).arg( p_needDelete );
		QSqlQuery query( strQuery );
		p_needDelete = -1;
	}
}

void RADB::Symptom::deleteOne( int symptomID )
{
	QString strQuery = QString( "DELETE FROM symptom WHERE syID = %1" ).arg( symptomID );
	QSqlQuery query( strQuery );
}

/**********************************************************************************************
 * Solution
 **********************************************************************************************/
QString RADB::Solution::sqlSelectAllBySymptom = "SELECT so.soID, so.soText, so.soSensorID FROM solution so WHERE so.soSymptomID = %1";
QString RADB::Solution::strZeroString = QString::fromLocal8Bit( "В процессе ремонта" );
int RADB::Solution::p_needDelete = -1;

int RADB::Solution::insertUnique( const QString & text, int syID, int seID  )
{
	QSqlQueryModel modelTemp;
	QString strSql = "SELECT so.soID FROM solution so WHERE so.soText = '" + text +
			"' AND so.soSymptomID = " + QString::number( syID ) + " AND so.soSensorID = " + QString::number( seID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about(0, "", modelTemp.lastError().databaseText() );
	if ( modelTemp.rowCount() > 0 )
		return modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();

	modelTemp.setQuery( QSqlQuery( "INSERT INTO solution ( soText, soSymptomID, soSensorID ) "
			"VALUES( '" + text + "', " + QString::number( syID ) + ", " + QString::number( seID ) + ") RETURNING soID" ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );

	return modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();
}

int RADB::Solution::updateUnique( int soID, const QString & text, int syID, int seID  )
{
	QSqlQueryModel modelTemp;
	QString strSql = "SELECT sg.sgID FROM solutiongroup sg WHERE sg.sgSolutionID = " + QString::number( soID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	bool isAlone = modelTemp.rowCount() < 2;

	int iNewId = 0;
	strSql = "SELECT so.soID FROM solution so WHERE so.soText = '" + text +
			"' AND so.soSymptomID = " + QString::number( syID ) + " AND so.soSensorID = " + QString::number( seID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about(0, "", modelTemp.lastError().databaseText() );
	if ( modelTemp.rowCount() > 0 )
		iNewId = modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();

	p_needDelete = -1;
	if ( isAlone && !iNewId )
	{
		QString strQuery = "UPDATE solution SET soText = '" + text +
			"', soSymptomID = " + QString::number( syID ) + ", soSensorID = " + QString::number( seID )
			+ " WHERE soID = " + QString::number( soID );
		QSqlQuery query( strQuery );
		return soID;
	}
	else if ( isAlone && iNewId && ( iNewId != soID ) )
	{
		p_needDelete = soID;
		return iNewId;
	}
	else if ( !isAlone && !iNewId )
		return insertUnique( text, syID, seID );
	else if ( !isAlone && iNewId )
		return iNewId;

	return soID;
}

void RADB::Solution::deleteAlone( int soID )
{
	QSqlQueryModel modelTemp;
	modelTemp.setQuery( QSqlQuery( QString( "SELECT sg.sgID FROM solutiongroup sg WHERE sg.sgSolutionID = %1" ).arg( soID ) ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );

	if ( modelTemp.rowCount() < 1 )
	{
		QString strQuery = QString( "DELETE FROM solution WHERE soID = %1" ).arg( soID );
		QSqlQuery query( strQuery );
		if ( query.lastError().isValid() )
			QMessageBox::about( 0, "", query.lastError().databaseText() );
	}
}

void RADB::Solution::deleteQuery()
{
	if ( p_needDelete > 0 )
	{
		QString strQuery = QString( "DELETE FROM solution WHERE soID = %1" ).arg( p_needDelete );
		QSqlQuery query( strQuery );
		p_needDelete = -1;
	}
}

/**********************************************************************************************
 * SolutionGroup
 **********************************************************************************************/
QString RADB::SolutionGroup::sqlSelectAllByJournal = "SELECT * FROM v_solution WHERE sgJournalID = %1";
SolutionModel RADB::SolutionGroup::m_model;

bool RADB::SolutionGroup::refresh( int idJournal )
{
	m_model.setQuery( RADB::SolutionGroup::sqlSelectAllByJournal.arg( idJournal ) );
	if ( m_model.lastError().isValid() )
		QMessageBox::about( 0, "", m_model.lastError().databaseText() );

	return !m_model.lastError().isValid();
}

int RADB::SolutionGroup::insertNew( int joID, int syID )
{
	int soID = Solution::insertUnique( Solution::strZeroString, syID, 0 );

	QSqlQueryModel modelTemp;
	modelTemp.setQuery( QSqlQuery( "INSERT INTO solutiongroup( sgTypeID, sgSolutionID, sgJournalID, sgDetailModelID ) "
			"VALUES( 1, " + QString::number( soID ) + ", " + QString::number( joID ) + ", 0 ) RETURNING sgID" ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );

	return modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();
}

bool RADB::SolutionGroup::updateType( int sgID, int newType )
{
	QString strQuery = QString( "UPDATE solutiongroup SET sgTypeID = %1 WHERE sgID = %2" ).arg( newType ).arg( sgID );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	return !query.lastError().isValid();
}

bool RADB::SolutionGroup::updateDetailModel( int sgID, int newId )
{
	QString strQuery = QString( "UPDATE solutiongroup SET sgDetailModelID = %1 WHERE sgID = %2" ).arg( newId ).arg( sgID );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	return !query.lastError().isValid();
}

bool RADB::SolutionGroup::updateSolution( int sgID, int solID, int senID, int symID, const QString & newText )
{
	int iNewSol = Solution::updateUnique( solID, newText, symID, senID );

	QString strQuery = QString( "UPDATE solutiongroup SET sgSolutionID = %1 WHERE sgID = %2" ).arg( iNewSol ).arg( sgID );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	RADB::Solution::deleteQuery();
	return !query.lastError().isValid();
}

void RADB::SolutionGroup::deleteOne( int sgID, int soID )
{
	QString strQuery = QString( "DELETE FROM solutiongroup sg WHERE sg.sgID = %1" ).arg( sgID );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );
	else RADB::Solution::deleteAlone( soID );
}

/**********************************************************************************************
 * Journal
 **********************************************************************************************/


QString RADB::Journal::sqlSelectAll = "SELECT * FROM v_journal";
QString RADB::Journal::sqlSelectAllAfterDT = "SELECT * FROM v_journal WHERE joErrorDT > '%1'";
JournalModel RADB::Journal::m_model;

bool RADB::Journal::refresh( const QDateTime & dt )
{
	m_model.setQuery( RADB::Journal::sqlSelectAllAfterDT.arg( dt.toString( "yyyy-MM-dd HH:mm:ss" ) ) );
	if ( m_model.lastError().isValid() )
		QMessageBox::about( 0, "", m_model.lastError().databaseText() );

	return !m_model.lastError().isValid();
}

void RADB::Journal::insertOneByUser( int userId )
{
	const QString strDTBegin = QDateTime::currentDateTime().toString( "yyyy-MM-dd HH:mm:ss" );
	const QString strDTEnd = QDateTime::currentDateTime().toString( "yyyy-MM-dd HH:mm:ss" );
	QString sqlInsert = "INSERT INTO journal (joErrorDT, joSolveDT, joSpecialistID, joSymptomID, joMachineID) "
			"VALUES ( '%1', '%2', %3, 0, 0 )";

	QString strQuery = sqlInsert.arg( strDTBegin ).arg( strDTEnd ).arg( userId );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );
}

void RADB::Journal::insertOneByMachine( int machineId )
{	
	QSqlQueryModel modelTemp;
	QString strSql = "SELECT ma.maModelID FROM machine ma WHERE ma.maID = " + QString::number( machineId );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	int modelID = modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();

	strSql = "SELECT un.unID FROM unit un WHERE un.unModelID = " + QString::number( modelID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	int unitID = modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();

	strSql = "SELECT sy.syID FROM symptom sy WHERE sy.syText = '" + RADB::Symptom::strZeroString + "' AND " +
			" sy.syUnitID = " + QString::number( unitID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	int sCount = modelTemp.rowCount();
	int symptomID;
	
	if( sCount > 0 )
		symptomID = modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();
	else symptomID = Symptom::insertUnique( RADB::Symptom::strZeroString, unitID );

	strSql = "SELECT so.soID FROM solution so WHERE so.soText = '" + RADB::Solution::strZeroString + "' AND " +
			" so.soSymptomID = " + QString::number( symptomID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	sCount = modelTemp.rowCount();
	int solutionID;
	
	if( sCount > 0 )
		solutionID = modelTemp.data( modelTemp.index( 0, 0 ) ).toInt();
	else solutionID = Solution::insertUnique( RADB::Solution::strZeroString, symptomID, 0 );

	const QString strDTBegin = QDateTime::currentDateTime().toString( "yyyy-MM-dd HH:mm:ss" );
	const QString strDTEnd = QDateTime::currentDateTime().addSecs( -1 ).toString( "yyyy-MM-dd HH:mm:ss" );
	QString sqlInsert = "INSERT INTO journal (joErrorDT, joSolveDT, joType, joSpecialistID, joSymptomID, joSolutionID, joMachineID) "
			"VALUES ( '%1', '%2', 0, 0, %3, %4, %5 )";

	QString strQuery = sqlInsert.arg( strDTBegin ).arg( strDTEnd ).arg( symptomID ).arg( solutionID ).arg( machineId );
	QSqlQuery query( strQuery );
}

void RADB::Journal::deleteOne( int journalID, int symptomID )
{
	QString strQuery = QString( "DELETE FROM journal WHERE joID = %1" ).arg( journalID );
	QSqlQuery query( strQuery );

	if ( !symptomID ) return;
	QSqlQueryModel modelTemp;
	QString strSql = "SELECT jo.joID FROM journal jo WHERE jo.joSymptomID = " + QString::number( symptomID );
	modelTemp.setQuery( QSqlQuery( strSql ) );
	if ( modelTemp.lastError().isValid() )
		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );
	if ( modelTemp.rowCount() < 1 )
		Symptom::deleteOne( symptomID );
}

bool RADB::Journal::updateErrorDT( int joNumber, const QDateTime & errorDT )
{
	QString strQuery = "UPDATE journal SET joErrorDT = '" + errorDT.toString( "yyyy-MM-dd HH:mm:ss" )
			+ "' WHERE joID = " + QString::number( joNumber );

	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	return !query.lastError().isValid();
}

bool RADB::Journal::updateSolveDT( int joNumber, const QDateTime & solveDT )
{
	QString strQuery = "UPDATE journal SET joSolveDT = '" + solveDT.toString( "yyyy-MM-dd HH:mm:ss" )
			+ "' WHERE joID = " + QString::number( joNumber );

	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	return !query.lastError().isValid();
}

bool RADB::Journal::updateSpecialist( int joNumber, int indexSp )
{
	QString strQuery = "UPDATE journal SET joSpecialistID = " + QString::number( indexSp )
			+ " WHERE joID = " + QString::number( joNumber );

	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	return !query.lastError().isValid();
}

bool RADB::Journal::updateMachine( int joNumber, int indexMach )
{
	QString strQuery = "UPDATE journal SET joMachineID = " + QString::number( indexMach )
			+ " WHERE joID = " + QString::number( joNumber );

	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	return !query.lastError().isValid();
}

bool RADB::Journal::updateSymptom( int joNumber, int symptomID, int unitID, int machineID, const QString & symptom )
{
	int iNewSyID = symptomID;

	if ( !symptomID )
		iNewSyID = RADB::Symptom::insertUnique( symptom, unitID );
	else iNewSyID = RADB::Symptom::updateUnique( symptomID, symptom, unitID );

//	int iNewSolID = solutionID;

//	if ( !solutionID && ( solution.trimmed() != Solution::strZeroString || iNewSyID ) )
//		iNewSolID = RADB::Solution::insertOne( solution.trimmed(), iNewSyID );
//	else
//		iNewSolID = RADB::Solution::updateOne( solutionID, solution.trimmed(), iNewSyID );

	QString strQuery = "UPDATE journal SET joSymptomID = " + QString::number( iNewSyID )
			+ " , joMachineID = " +  QString::number( machineID )
			+ " WHERE joID = " + QString::number( joNumber );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );

	QSqlQueryModel modelTemp;
//	modelTemp.setQuery( QSqlQuery( RADB::SolutionGroup::sqlSelectAllByJournal.arg( joNumber ) ) );
//	if ( modelTemp.lastError().isValid() )
//		QMessageBox::about( 0, "", modelTemp.lastError().databaseText() );

	for ( int i = 0; i < modelTemp.rowCount(); i++ )
	{
	//	const int iSGID = modelTemp.data( modelTemp.index( i, SolutionModel::clID ) ).toInt();
	//	const int iSolID = modelTemp.data( modelTemp.index( i, SolutionModel::clSolutionID ) ).toInt();
	//	const int iSenID = modelTemp.data( modelTemp.index( i, SolutionModel::clSensorID ) ).toInt();
	//	const QString strSolText = modelTemp.data( modelTemp.index( i, SolutionModel::clText ) ).toString();
	//	RADB::SolutionGroup::updateSolution( iSGID, iSolID, iSenID, iNewSyID, strSolText );
	}

	RADB::Symptom::deleteQuery();
	return !query.lastError().isValid();
}

/*

QString RADB::Notification::sqlSelectAll = "SELECT * FROM v_notification";
QString RADB::Notification::sqlSelectBySensor = "SELECT * FROM v_notification WHERE noSensors = '%1'";

void RADB::Notification::updateOne( int noID, const QString & sms, int machineID, int profID )
{
	QString strQuery = "UPDATE notification SET noSms = '" + sms +
			"', noMachineID = " + QString::number( machineID ) + ", noProfessionID = " + QString::number( profID ) +
			" WHERE noID = " + QString::number( noID );
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
		QMessageBox::about( 0, "", query.lastError().databaseText() );
}
*/
